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
#ifndef _TIBEE_BUILDERBLOCKS_PROCESSBLOCK_HPP
#define _TIBEE_BUILDERBLOCKS_PROCESSBLOCK_HPP

#include <unordered_set>

#include "execution_blocks/AbstractExecutionBlock.hpp"
#include "trace/value/EventValue.hpp"

namespace tibee {
namespace execution_blocks {

/**
 * Block that generates an execution for a process and all
 * its children.
 *
 * @author Francois Doray
 */
class ProcessBlock : public execution_blocks::AbstractExecutionBlock
{
public:
    ProcessBlock();
    ~ProcessBlock();

    virtual void Start(const value::Value* params) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void OnExecName(uint32_t tid, const notification::Path& path, const value::Value* value);
    void OnSchedProcessFork(const trace::EventValue& event);
    void OnSchedProcessExit(const trace::EventValue& event);

    // Roots of executions.
    typedef std::unordered_set<thread_t> ExecutionRoots;
    ExecutionRoots _executionRoots;

    // Threads that are currently analyzed mapped to the root of the execution.
    typedef std::unordered_map<thread_t, thread_t> AnalyzedThreads;
    AnalyzedThreads _analyzedThreads;

    // Name of the analyzed process.
    std::string _processName;
};

}  // namespace execution_blocks
}  // namespace tibee

#endif // _TIBEE_BUILDERBLOCKS_PROCESSBLOCK_HPP
