/* Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
 *
 * This file is part of tibeecompare.
 *
 * tibeecompare is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tibeecompare is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tibeecompare.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "execution_blocks/SyscallOnlyBlock.hpp"

#include "base/BindObject.hpp"
#include "base/Constants.hpp"
#include "notification/Token.hpp"

namespace tibee {
namespace execution_blocks {

SyscallOnlyBlock::SyscallOnlyBlock()
{
}

void SyscallOnlyBlock::LoadServices(const block::ServiceList& serviceList)
{
    AbstractExecutionBlock::LoadServices(serviceList);

    Q_EMPTY_STRING = Quarks()->StrQuark("");
}

void SyscallOnlyBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    AddThreadStateObserver(notificationCenter, notification::Token(kStateSyscall),
                           base::BindObject(&SyscallOnlyBlock::onSyscall, this));
    AddKernelObserver(notificationCenter, notification::RegexToken("^syscall_exit_"),
                      base::BindObject(&SyscallOnlyBlock::onExitSyscall, this));
}

void SyscallOnlyBlock::onSyscall(uint32_t tid, const notification::Path& path, const value::Value* value)
{
    auto syscallValue = value->GetField(kCurrentStateAttributeValueField);
    if (syscallValue != nullptr)
    {
        Stacks()->PopStack(tid);
        Stacks()->PushStack(tid, Quarks()->StrQuark(syscallValue->AsString()));
    }
}

void SyscallOnlyBlock::onExitSyscall(const trace::EventValue& event)
{
    thread_t tid = ThreadForEvent(event);

    Stacks()->PopStack(tid);
    Stacks()->PushStack(tid, Q_EMPTY_STRING);
}

}  // namespace execution_blocks
}  // namespace tibee
