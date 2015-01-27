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
#include "execution_blocks/ProcessBlock.hpp"

#include "base/BindObject.hpp"
#include "base/CompareConstants.hpp"
#include "base/Constants.hpp"
#include "base/print.hpp"
#include "notification/Token.hpp"
#include "value/MakeValue.hpp"

namespace tibee {
namespace execution_blocks {

ProcessBlock::ProcessBlock()
{
}

ProcessBlock::~ProcessBlock()
{
}

void ProcessBlock::Start(const value::Value* params)
{
    auto nameValue = params->GetField("name");

    if (nameValue == nullptr)
    {
        base::tberror() << "Missing process name for process block." << base::tbendl();
        return;
    }

    _processName = nameValue->AsString();
}

void ProcessBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    using notification::Token;

    AddThreadStateObserver(notificationCenter, Token(kStateExecName),
                           base::BindObject(&ProcessBlock::OnExecName, this));
    AddKernelObserver(notificationCenter, Token("sched_process_exit"),
                      base::BindObject(&ProcessBlock::OnSchedProcessExit, this));
    AddKernelObserver(notificationCenter, Token("sched_process_fork"),
                      base::BindObject(&ProcessBlock::OnSchedProcessFork, this));
}

void ProcessBlock::OnExecName(uint32_t tid, const notification::Path& path, const value::Value* value)
{
    /*
    auto threadNameValue = value->GetField(kCurrentStateAttributeValueField);
    if (threadNameValue != nullptr && threadNameValue->AsString() == _processName)
    {
        auto look = _analyzedThreads.find(tid);
        if (look == _analyzedThreads.end())
        {
            auto executionIndex =
                Executions()->StartExecution(tid, _processName, false);
            if (executionIndex != execution::kInvalidExecutionIndex)
            {
                _executionRoots[tid] = executionIndex;
                _analyzedThreads.insert(tid);
            }
        }
    }
    */
}

void ProcessBlock::OnSchedProcessFork(const trace::EventValue& event)
{
    /*
    auto parent_tid = event.getEventField("parent_tid")->AsUInteger();
    auto child_tid = event.getEventField("child_tid")->AsUInteger();

    auto look = _analyzedThreads.find(parent_tid);
    if (look != _analyzedThreads.end())
    {
        _analyzedThreads.insert(child_tid);



                // TODO
        // Executions()->AddThreadToExecution(look->second, child_tid);
    }
    */
}

void ProcessBlock::OnSchedProcessExit(const trace::EventValue& event)
{
    /*
    auto tid = ThreadForEvent(event);
    auto look = _executionRoots.find(tid);
    if (look != _executionRoots.end())
    {
        Executions()->EndExecution(tid);
    }
    */
}

}  // namespace execution_blocks
}  // namespace tibee