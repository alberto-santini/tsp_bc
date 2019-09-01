//
// Created by alberto on 01/09/2019.
//

#include "SEIntegerLazyConstraint.h"

#include <cstddef>
#include <vector>
#include <numeric>
#include <algorithm>

namespace tsp_bc {
    void SEIntegerLazyConstraint::main() {
        const std::size_t n = x.getSize();
        std::vector<std::size_t> free_vertices(n);

        std::iota(free_vertices.begin(), free_vertices.end(), 0u);

        while(!free_vertices.empty()) {
            std::size_t prev_v = free_vertices.back();
            std::size_t current_v = prev_v;
            std::vector<std::size_t> current_subtour = {prev_v};

            do {
                const auto current_v_cpy = current_v;

                for(std::size_t i = 0u; i < n; ++i) {
                    if(i == current_v || i == prev_v) {
                        continue;
                    }

                    const auto&[s, t] = std::minmax(current_v, i);

                    if(getValue(x[s][t]) > 0.5) {
                        current_subtour.push_back(i);
                        free_vertices.erase(std::remove(free_vertices.begin(), free_vertices.end(), i));
                        prev_v = current_v;
                        current_v = i;
                        break;
                    }
                }

                if(current_v == current_v_cpy) {
                    current_subtour.push_back(current_subtour[0u]);
                    free_vertices.erase(std::remove(free_vertices.begin(), free_vertices.end(), current_subtour[0u]));
                    current_v = current_subtour[0u];
                }
            } while(current_v != current_subtour[0u]);

            if(current_subtour.size() != n + 1u) {
                IloExpr expr{env};

                for(auto i = 0u; i < current_subtour.size() - 1u; ++i) {
                    const auto&[s, t] = std::minmax(current_subtour[i], current_subtour[i + 1u]);
                    expr += x[s][t];
                }

                try {
                    add(expr <= static_cast<IloNum>(current_subtour.size() - 2u));
                } catch(IloException& e) {
                    std::cerr << "Callback exception: " << e.getMessage() << "\n";
                    throw;
                }
            }
        }
    }
}