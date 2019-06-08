//
// Created by alberto on 02/06/19.
//

#ifndef TSP_BC_CPLEXSOLVER_H
#define TSP_BC_CPLEXSOLVER_H

#include <as/tsplib.h>
#include <string>

#ifndef IL_STD
#define IL_STD
#endif
#include <ilcplex/ilocplex.h>

#include "Graph.h"

namespace tsp_bc {
    struct Solution {
        long num_rows;
        double model_creation_time_sec;
        double root_node_time_sec;
        double solver_time_sec;
        double obj_value;
        double root_node_gap;
        double gap;
    };

    class CplexSolver {
        as::tsplib::TSPInstance instance;
        Graph graph;

        void add_subtour_enumeration(IloEnv& env, IloModel& model, IloArray<IloNumVarArray>& x, std::size_t k) const;

    public:
        explicit CplexSolver(std::string instance_file);
        Solution solve(std::size_t subtour_enumeration_k) const;
    };
}

#endif //TSP_BC_CPLEXSOLVER_H
