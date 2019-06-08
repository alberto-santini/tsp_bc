#include <iostream>
#include <sstream>

#include "CplexSolver.h"

int main(int, char** argv) {
    using namespace tsp_bc;

    CplexSolver solver{argv[1]};

    std::stringstream ss;
    ss << argv[2];

    std::size_t subtour_enumeration_k;
    if(!(ss >> subtour_enumeration_k)) {
        std::cerr << "Wrong parameter for k!\n";
        return 1;
    }

    const auto solution = solver.solve(subtour_enumeration_k);

    std::cout << argv[1] << ","
              << subtour_enumeration_k << ","
              << solution.num_rows << ","
              << solution.model_creation_time_sec << ","
              << solution.root_node_time_sec << ","
              << solution.solver_time_sec << ","
              << solution.obj_value << ","
              << solution.root_node_gap << ","
              << solution.gap << "\n";

    return 0;
}