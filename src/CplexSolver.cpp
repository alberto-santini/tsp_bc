//
// Created by alberto on 02/06/19.
//

#include "CplexSolver.h"
#include "SEUserCut.h"
#include "SELazyConstraint.h"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <chrono>

namespace tsp_bc {
    namespace {
        template<typename Fn>
        void visit_fixed_size_subsets(std::size_t n, std::size_t k, std::size_t start, std::size_t curr_sz,
                                      std::vector<bool>& indicator, Fn& fn)
        {
            assert(std::count(indicator.begin(), indicator.end(), true) == static_cast<long>(curr_sz));

            if(curr_sz > k) { return; }
            if(curr_sz == k) { fn(indicator); return; }
            if(start == n) { return; }

            indicator[start] = true;
            visit_fixed_size_subsets(n, k, start + 1u, curr_sz + 1u, indicator, fn);

            indicator[start] = false;
            visit_fixed_size_subsets(n, k, start + 1u, curr_sz, indicator, fn);
        }
    }

    CplexSolver::CplexSolver(std::string instance_file) : instance{instance_file} {
        graph = Graph(instance.number_of_vertices());

        for(auto i = 0u; i < instance.number_of_vertices(); ++i) {
            for(auto j = i + 1; j < instance.number_of_vertices(); ++j) {
                boost::add_edge(i, j, instance.get_distance(i, j), graph);
            }
        }
    }

    Solution CplexSolver::solve(std::size_t subtour_enumeration_k) const {
        using namespace std::chrono;

        const auto n = instance.number_of_vertices();
        const auto cpx_n = static_cast<IloInt>(n);

        IloEnv env;
        IloModel model{env};
        IloArray<IloNumVarArray> x{env, cpx_n};
        IloExpr expr{env};

        const auto start_time = high_resolution_clock::now();

        for(auto i = 0u; i < n; ++i) {
            x[i] = IloNumVarArray{env, cpx_n, 0, 1, IloNumVar::Bool};

            // x[i] = IloNumVarArray{env, cpx_n};
            // for(auto j = i + 1u; j < n; ++j) {
            //     x[i][j] = IloNumVar{env, 0, 1, IloNumVar::Bool, ("x_" + std::to_string(i) + "_" + std::to_string(j)).c_str()};
            // }
        }

        // Incidence constraints
        for(auto i = 0u; i < n; ++i) {
            for(auto j = 0u; j < n; ++j) {
                // We only have variables [smaller_index][larger_index].
                if(j < i) {
                    expr += x[j][i];
                } else if(i < j) {
                    expr += x[i][j];
                }
            }

            model.add(expr == 2);
            expr.clear();
        }

        // Objective function
        for(auto i = 0u; i < n; ++i) {
            for(auto j = i + 1; j < n; ++j) {
                const auto [e, exists] = boost::edge(i, j, graph);
                assert(exists);

                expr += graph[e] * x[i][j];
            }
        }

        model.add(IloObjective{env, expr, IloObjective::Minimize});
        expr.end();

        if(subtour_enumeration_k > 2u) {
            add_subtour_enumeration(env, model, x, subtour_enumeration_k);
        }

        const auto model_creation_end_time = high_resolution_clock::now();
        const auto model_creation_time = duration_cast<duration<double>>(model_creation_end_time - start_time).count();

        IloCplex cplex{model};
        cplex.use(IloCplex::Callback(new(env) SEUserCut{env, x, graph}));
        cplex.use(IloCplex::Callback(new(env) SELazyConstraint{env, x, graph}));
        cplex.setParam(IloCplex::TiLim, 3600 - model_creation_time);
        cplex.setParam(IloCplex::NodeLim, 0);

        add_initial_solution(env, cplex, x);

        const auto num_rows = cplex.getNrows();

        try {
            cplex.solve();
        } catch(const IloException& e) {
            std::cerr << "Cplex exception when solving the root node:\n" << e << "\n";
            throw;
        }

        const auto root_node_end_time = high_resolution_clock::now();
        auto root_node_gap = 1.0;

        try {
            root_node_gap = cplex.getMIPRelativeGap();
        } catch(const IloException& e) {
            // Throws if no integer solution was found.
            // In this case we arbitrarily set the root gap as 100%.
            std::cerr << "Cplex exception when retrieving the root node gap:\n" << e << "\n";
        }

        if(root_node_gap > 0.0) {
            cplex.setParam(IloCplex::NodeLim, 999999);

            try {
                cplex.solve();
            } catch(const IloException& e) {
                std::cerr << "Cplex exception when solving after the root node:\n" << e << "\n";
                throw;
            }
        }

        const auto solver_end_time = high_resolution_clock::now();

        auto obj_value = 999999.0;
        auto gap = 1.0;

        try {
            obj_value = cplex.getObjValue();
            gap = cplex.getMIPRelativeGap();
        } catch(const IloException& e) {
            // Throws if no integer solution was found.
            // In this case we arbitrarily set the gap as 100%,
            // and the objective value as 999999.
            std::cerr << "Cplex exception when retrieving the final objective value/gap:\n" << e << "\n";
        }

        const auto root_node_time = duration_cast<duration<double>>(root_node_end_time - model_creation_end_time).count();
        const auto solver_time = duration_cast<duration<double>>(solver_end_time - model_creation_end_time).count();

        return {
            num_rows,
            n,
            model_creation_time,
            root_node_time,
            solver_time,
            obj_value,
            root_node_gap,
            gap
        };
    }

    void CplexSolver::add_subtour_enumeration(IloEnv& env, IloModel& model, IloArray<IloNumVarArray>& x, std::size_t k) const {
        const auto n = this->instance.number_of_vertices();

        auto add_cut = [&env, &model, &x, k, n] (const std::vector<bool>& indicator) -> void {
            IloExpr expr{env};

            for(auto i = 0u; i < n; ++i) {
                if(!indicator[i]) { continue; }

                for(auto j = i + 1u; j < n; ++j) {
                    if(!indicator[j]) { continue; }

                    expr += x[i][j];
                }
            }

            try {
                model.add(expr <= static_cast<IloInt>(k) - 1);
            } catch(const IloException& e) {
                std::cerr << "Cplex exception when adding cut lhs " << expr << "\n";
                std::cerr << "Exception: " << e << "\n";
            }

            expr.end();
        };

        auto indicator = std::vector<bool>(n, false);
        visit_fixed_size_subsets(n, k, 0u, 0u, indicator, add_cut);
    }

    void CplexSolver::add_initial_solution(IloEnv& env, IloCplex& cplex, IloArray<IloNumVarArray>& x) const {
        const auto initial = initial_greedy(graph);
        IloNumVarArray ivars{env};
        IloNumArray ivals{env};

        for(auto i = 0u; i < initial.size() - 1u; ++i) {
            const auto s = std::min(initial[i], initial[i + 1u]);
            const auto t = std::max(initial[i], initial[i + 1u]);

            assert(s != t);

            ivars.add(x[s][t]);
            ivals.add(1);
        }

        // Close the tour.
        ivars.add(x[initial.front()][initial.back()]);
        ivals.add(1);

        cplex.addMIPStart(ivars, ivals);
    }
}
