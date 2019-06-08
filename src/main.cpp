#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "CplexSolver.h"

int main(int, char** argv) {
    using namespace tsp_bc;

    // --- 1st argument: instance file ---
    const auto inst = std::filesystem::path{argv[1]};

    if(!std::filesystem::exists(inst)) {
        std::cerr << "Instance not found: " << inst << "\n";
        return 1;
    }

    // --- 2nd parameter: enumeration k ---
    std::stringstream ss;
    ss << argv[2];

    std::size_t subtour_enumeration_k;
    if(!(ss >> subtour_enumeration_k)) {
        std::cerr << "Wrong parameter for k!\n";
        return 1;
    }

    // --- 3rd parameter: output file ---
    auto ofs = std::ofstream{argv[3]};

    // --- Solve! ---
    const auto solver = CplexSolver{inst};
    const auto solution = solver.solve(subtour_enumeration_k);

    // --- Print output ---
    ofs << inst.stem().string() << ","
        << subtour_enumeration_k << ","
        << solution.instance_size << ","
        << solution.num_rows << ","
        << solution.model_creation_time_sec << ","
        << solution.root_node_time_sec << ","
        << solution.solver_time_sec << ","
        << solution.obj_value << ","
        << solution.root_node_gap << ","
        << solution.gap << "\n";

    return 0;
}