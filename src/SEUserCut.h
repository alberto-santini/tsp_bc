//
// Created by alberto on 05/06/19.
//

#ifndef TSP_BC_SEUSERCUT_H
#define TSP_BC_SEUSERCUT_H

#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/vector_property_map.hpp>

#include "Graph.h"

#ifndef IL_STD
#define IL_STD
#endif
#include <ilcplex/ilocplex.h>
#include <ilcplex/ilocplexi.h>

namespace tsp_bc {
    struct SEUserCut : IloCplex::UserCutCallbackI {
        struct SPVertexProperty { std::size_t id; };
        struct SPEdgeProperty { std::size_t id; };
        using SPGraph = boost::adjacency_list<
                boost::vecS,
                boost::vecS,
                boost::directedS,
                SPVertexProperty,
                SPEdgeProperty>;
        using SPVertex = SPGraph::vertex_descriptor;
        using SPEdge = SPGraph::edge_descriptor;

        const IloEnv& env;
        const IloArray<IloNumVarArray>& x;
        const Graph& graph;
        SPGraph sp_graph;
        std::vector<SPEdge> reverse;

        SEUserCut(const IloEnv& env, const IloArray<IloNumVarArray>& x, const Graph& graph);

        IloCplex::CallbackI* duplicateCallback() const override {
            return new(getEnv()) SEUserCut{*this};
        }

        void main() override;
    };
}



#endif //TSP_BC_SEUSERCUT_H
