//
// Created by alberto on 02/06/19.
//

#ifndef TSP_BC_CPLEXSOLVER_H
#define TSP_BC_CPLEXSOLVER_H

#include <as/tsplib.h>
#include <string>
#include <vector>
#include <stdexcept>

#ifndef IL_STD
#define IL_STD
#endif
#include <ilcplex/ilocplex.h>

#include "Graph.h"

namespace tsp_bc {
    struct Solution {
        long num_rows;
        std::size_t instance_size;
        double model_creation_time_sec;
        double root_node_time_sec;
        double solver_time_sec;
        double obj_value;
        double root_node_gap;
        double gap;
    };

    class CplexSolver {
        // Symmetric TSP instance.
        const as::tsplib::TSPInstance instance;

        // Underlying graph.
        Graph graph;

        // Size of the subset whose subtour-elimination constraints we want
        // to enumerate. Use k == 0 to disable enumeration. Otherwise, the
        // smallest k for which it makes sense to consider enumeration in a
        // symmetric TSP problem is k == 3.
        const std::size_t k;

        // Whether or not we should only add the proximal cuts when enumerating (see below).
        const bool use_proximity;

        // Proximity matrix. proximity[i][j] == true if j is one of the
        // proximity_n-closest vertices of i, where proximity_n is a parameter.
        std::vector<std::vector<bool>> proximity;
        const std::size_t proximity_n;

        // Enumerates and adds subtour elimination constraints for subsets of size sz.
        // If use_proximity == false, it adds all such cuts; otherwise, it only adds cuts for proximal vertices.
        void add_subtour_enumeration(IloEnv& env, IloModel& model, IloArray<IloNumVarArray>& x, std::size_t sz) const;

        // Initialises the model with a greedy feasible solution.
        void add_initial_solution(IloEnv& env, IloCplex& cplex, IloArray<IloNumVarArray>& x) const;

        // Creates the proximity matrix.
        void make_proximity();

        // Tells whether, for each vertex i in the indicator set, there is at least one
        // other vertex in the indicator set, which is proximal to i.
        bool is_indicator_proximal(const std::vector<bool>& indicator) const;

        CplexSolver(std::string instance_file, std::size_t k, bool use_proximity, std::size_t proximity_n);

        friend class CplexSolverFactory;

    public:
        Solution solve() const;
    };

    class CplexSolverFactory {
        std::string instance_file = "";
        std::size_t k = 0u;
        bool use_proximity = false;
        std::size_t proximity_n = 5u;

    public:

        CplexSolverFactory() {}

        CplexSolverFactory& with_instance(std::string instance_file_) {
            this->instance_file = instance_file_;
            return *this;
        }

        CplexSolverFactory& with_enumeration_k(std::size_t k_) {
            this->k = k_;
            return *this;
        }

        CplexSolverFactory& with_proximity(bool use_proximity_ = true) {
            this->use_proximity = use_proximity_;
            return *this;
        }

        CplexSolverFactory& with_proximity_n(std::size_t proximity_n_) {
            this->proximity_n = proximity_n_;
            return *this;
        }

        CplexSolver get() const {
            if(instance_file.empty()) {
                std::cerr << "You need to pass an instance file!\n";
                throw std::logic_error{"No instance file, needed to create a solver."};
            }

            return CplexSolver{instance_file, k, use_proximity, proximity_n};
        }
    };
}

#endif //TSP_BC_CPLEXSOLVER_H
