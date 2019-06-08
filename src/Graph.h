//
// Created by alberto on 03/06/19.
//

#ifndef TSP_BC_GRAPH_H
#define TSP_BC_GRAPH_H

#include <boost/graph/adjacency_list.hpp>

namespace tsp_bc {
    using Graph = boost::adjacency_list<
            boost::vecS,            // Out-edge list
            boost::vecS,            // Vertex list
            boost::undirectedS,     // Directed type
            boost::no_property,     // Vertex property
            float>;                 // Edge property (cost)
    using Vertex = Graph::vertex_descriptor;
    using Edge = Graph::edge_descriptor;
}

#endif //TSP_BC_GRAPH_H
