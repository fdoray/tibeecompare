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
#include "execution_blocks/ThreadNameBlock.hpp"

#include "base/BindObject.hpp"
#include "base/Constants.hpp"
#include "notification/Token.hpp"

namespace tibee {
namespace execution_blocks {

ThreadNameBlock::ThreadNameBlock()
{
}

void ThreadNameBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    AddThreadStateObserver(notificationCenter, notification::Token(kStateExecName),
                           base::BindObject(&ThreadNameBlock::onExecName, this));
}

void ThreadNameBlock::onExecName(uint32_t tid, const notification::Path& path, const value::Value* value)
{
    auto threadNameValue = value->GetField(kCurrentStateAttributeValueField);
    if (threadNameValue != nullptr)
    {
        Stacks()->SetThreadName(tid, threadNameValue->AsString());
    }
}


}  // namespace execution_blocks
}  // namespace tibee
