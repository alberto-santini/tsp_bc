//
// Created by alberto on 09/06/19.
//

#include "Graph.h"

#include <numeric>
#include <limits>
#include <cassert>

namespace tsp_bc {
    std::vector<Vertex> initial_greedy(const Graph& g) {
        std::vector<Vertex> tour = { 0u };
        tour.reserve(boost::num_vertices(g));

        while(tour.size() < boost::num_vertices(g)) {
            const auto cur = tour.back();
            Vertex next_vertex = 0u;
            float next_cost = std::numeric_limits<float>::max();

            for(auto [it, it_end] = boost::out_edges(cur, g); it != it_end; ++it) {
                const auto t = boost::target(*it, g);

                assert(t != cur);

                if(std::find(tour.begin(), tour.end(), t) != tour.end()) {
                    continue;
                }

                if(g[*it] > next_cost) {
                    continue;
                }

                next_vertex = t;
                next_cost = g[*it];
            }


            assert(next_vertex != 0u);
            assert(next_vertex != cur);

            tour.push_back(next_vertex);
        }

        return tour;
    }
}