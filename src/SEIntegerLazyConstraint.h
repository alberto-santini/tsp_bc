#ifndef TSP_BC_SEINTEGERLAZYCONSTRAINT_H
#define TSP_BC_SEINTEGERLAZYCONSTRAINT_H

#ifndef IL_STD
#define IL_STD
#endif
#include <ilcplex/ilocplex.h>
#include <ilcplex/ilocplexi.h>

namespace tsp_bc {
    struct SEIntegerLazyConstraint : IloCplex::LazyConstraintCallbackI {
        const IloEnv& env;
        const IloArray<IloNumVarArray>& x;

        SEIntegerLazyConstraint(const IloEnv& env, const IloArray<IloNumVarArray>& x) :
            LazyConstraintCallbackI{env}, env{env}, x{x} {}

        [[nodiscard]]
        IloCplex::CallbackI* duplicateCallback() const override {
            return new(getEnv()) SEIntegerLazyConstraint{*this};
        }

        void main() override;
    };
}

#endif