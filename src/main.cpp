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
        std::cerr << "Wrong parameter for k: " << argv[2]<< "!\n";
        return 1;
    }

    // --- 4th parameter: proximity n ---
    ss.str(""); ss.clear();
    ss << argv[3];

    std::size_t proximity_n;
    if(!(ss >> proximity_n)) {
        std::cerr << "Wrong parameter for proximity n: " << argv[3] << "\n";
        return 1;
    }
    const auto use_proximity = (proximity_n > 0u);

    // --- 5th parameter: output file ---
    auto ofs = std::ofstream{argv[4]};

    // --- Solve! ---
    const auto solver = CplexSolverFactory{}
        .with_instance(inst)
        .with_proximity(use_proximity)
        .with_proximity_n(proximity_n)
        .with_enumeration_k(subtour_enumeration_k)
        .get();

    const auto solution = solver.solve();

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