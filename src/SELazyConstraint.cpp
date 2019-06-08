//
// Created by alberto on 03/06/19.
//

#include "SELazyConstraint.h"

#include <cassert>
#include <boost/property_map/property_map.hpp>
#include <boost/property_map/transform_value_property_map.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>

namespace tsp_bc {
    SELazyConstraint::SELazyConstraint(const IloEnv &env, const IloArray<IloNumVarArray>& x, const Graph& graph) :
            IloCplex::LazyConstraintCallbackI{env},
            env{env},
            x{x},
            graph{graph}
    {
        const auto n = boost::num_vertices(graph);

        sp_graph = SPGraph{n};

        for(auto i = 0u; i < n; ++i) {
            sp_graph[SPVertex{i}] = { i };
        }

        reverse = std::vector<SPEdge>(n * (n - 1u));

        std::size_t edge_id = 0u;
        for(auto i = 0u; i < n; ++i) {
            for(auto j = i + 1; j < n; ++j) {
                const auto [e, e_added] = boost::add_edge(i, j, { edge_id++ }, sp_graph);
                const auto [f, f_added] = boost::add_edge(j, i, { edge_id++ }, sp_graph);

                reverse[sp_graph[e].id] = f;
                reverse[sp_graph[f].id] = e;
            }
        }

        assert(boost::num_edges(sp_graph) == n * (n - 1u));
    }

    void SELazyConstraint::main() {
        const auto n = boost::num_vertices(sp_graph);
        const auto m = boost::num_edges(sp_graph);

        auto capacity = std::vector<float>(m, 0.0f);

        for(auto i = 0u; i < n; ++i) {
            for(auto j = i + 1u; j < n; ++j) {
                auto cap = getValue(x[i][j]);

                if(cap < 1e-6) { cap = 0; }

                const auto [e, e_found] = boost::edge(i, j, sp_graph);
                const auto [f, f_found] = boost::edge(j, i, sp_graph);

                capacity[sp_graph[e].id] = cap;
                capacity[sp_graph[f].id] = cap;
            }
        }

        const auto source = 0u;
        std::vector<std::size_t> separated;

        for(auto sink = 1u; sink < n; ++sink) {
            if(std::find(separated.begin(), separated.end(), sink) != separated.end()) {
                continue;
            }

            auto residual_capacity = std::vector<float>(m, 0.0f);
            auto colour = std::vector<int>(n, 0);

            const auto flow = boost::boykov_kolmogorov_max_flow(
                    sp_graph,
                    boost::make_iterator_property_map(capacity.begin(), boost::get(&SPEdgeProperty::id, sp_graph)),
                    boost::make_iterator_property_map(residual_capacity.begin(), boost::get(&SPEdgeProperty::id, sp_graph)),
                    boost::make_iterator_property_map(reverse.begin(), boost::get(&SPEdgeProperty::id, sp_graph)),
                    boost::make_iterator_property_map(colour.begin(), boost::get(&SPVertexProperty::id, sp_graph)),
                    boost::get(&SPVertexProperty::id, sp_graph),
                    source,
                    sink
                    );

            if(flow < 1.0f) {
                std::vector<std::size_t> T;

                for(auto i = 0u; i < n; ++i) {
                    if(colour[i] == colour[sink]) {
                        T.push_back(i);
                        separated.push_back(i);
                    }
                }

                IloExpr expr{env};

                for(const auto& i : T) {
                    for(const auto& j : T) {
                        if(i < j) {
                            expr += x[i][j];
                        }
                    }
                }

                add(expr <= static_cast<IloInt>(T.size()) - 1, IloCplex::UseCutForce);

                expr.end();
            }
        }
    }
}