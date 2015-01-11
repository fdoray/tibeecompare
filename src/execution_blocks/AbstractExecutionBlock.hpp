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
#ifndef _TIBEE_EXECUTIONBLOCKS_ABSTRACTEXECUTIONBLOCK_HPP
#define _TIBEE_EXECUTIONBLOCKS_ABSTRACTEXECUTIONBLOCK_HPP

#include "block/AbstractBlock.hpp"
#include "execution/ExecutionsBuilder.hpp"
#include "execution/StacksBuilder.hpp"
#include "state/CurrentState.hpp"

namespace tibee {
namespace execution_blocks {

/**
 * Abstract execution block.
 *
 * @author Francois Doray
 */
class AbstractExecutionBlock : public block::AbstractBlock
{
public:
    AbstractExecutionBlock();

    virtual void LoadServices(const block::ServiceList& serviceList) override;

protected:
    // Current state.
    state::CurrentState* State() const { return _currentState; }

    // Executions builder.
    execution::ExecutionsBuilder* Executions() const { return _executionsBuilder; }

    // Stacks builder.
    execution::StacksBuilder* Stacks() const { return _stacksBuilder; }

    // CPU for an event.
    uint32_t CpuForEvent(const trace::EventValue& event) const;

    // Thread for an event.
    thread_t ThreadForEvent(const trace::EventValue& event) const;

private:
    // Current state.
    state::CurrentState* _currentState;

    // Executions builder.
    execution::ExecutionsBuilder* _executionsBuilder;

    // Stacks builder.
    execution::StacksBuilder* _stacksBuilder;
};

}  // namespace execution_blocks
}  // namespace tibee

#endif // _TIBEE_EXECUTIONBLOCKS_ABSTRACTEXECUTIONBLOCK_HPP