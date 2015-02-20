/* Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
 *
 * This file is part of tigerbeetle.
 *
 * tigerbeetle is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tigerbeetle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _TIBEE_BUILDBLOCKS_ABSTRACTBUILDBLOCK_HPP
#define _TIBEE_BUILDBLOCKS_ABSTRACTBUILDBLOCK_HPP

#include "block/AbstractBlock.hpp"
#include "critical/CriticalGraph.hpp"
#include "execution/ExecutionsBuilder.hpp"
#include "stacks/StacksBuilder.hpp"
#include "state/CurrentState.hpp"
#include "state/StateHistory.hpp"

namespace tibee {
namespace build_blocks {

/**
 * Abstract build block.
 *
 * @author Francois Doray
 */
class AbstractBuildBlock : public block::AbstractBlock
{
public:
    AbstractBuildBlock();

    virtual void LoadServices(const block::ServiceList& serviceList) override;

protected:
    // Current state.
    state::CurrentState* State() const { return _currentState; }

    // Quarks.
    quark::StringQuarkDatabase* Quarks() const { return _quarks; }

    // Executions builder.
    execution::ExecutionsBuilder* Executions() const { return _executionsBuilder; }

    // Stacks builder.
    stacks::StacksBuilder* Stacks() const { return _stacksBuilder; }

    // Critical graph.
    critical::CriticalGraph* CriticalGraph() const { return _criticalGraph; }

    // State history.
    state::StateHistory* StateHistory() const { return _stateHistory; }

    // CPU for an event.
    uint32_t CpuForEvent(const trace::EventValue& event) const;

    // Thread for an event.
    thread_t ThreadForEvent(const trace::EventValue& event) const;

    // Process for an event.
    process_t ProcessForEvent(const trace::EventValue& event) const;

private:
    // Current state.
    state::CurrentState* _currentState;

    // Quarks.
    quark::StringQuarkDatabase* _quarks;

    // Executions builder.
    execution::ExecutionsBuilder* _executionsBuilder;

    // Stacks builder.
    stacks::StacksBuilder* _stacksBuilder;

    // Critical graph.
    critical::CriticalGraph* _criticalGraph;

    // State history.
    state::StateHistory* _stateHistory;
};

}  // namespace build_blocks
}  // namespace tibee

#endif // _TIBEE_BUILDBLOCKS_ABSTRACTBUILDBLOCK_HPP
