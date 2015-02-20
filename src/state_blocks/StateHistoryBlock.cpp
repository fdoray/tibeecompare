/* Copyright (c) 2015 Francois Doray <francois.pierre-doray@polymtl.ca>
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
#include "state_blocks/StateHistoryBlock.hpp"

#include "base/BindObject.hpp"
#include "base/Constants.hpp"
#include "notification/NotificationCenter.hpp"

namespace tibee
{
namespace state_blocks
{

StateHistoryBlock::StateHistoryBlock()
{
}

StateHistoryBlock::~StateHistoryBlock()
{
}

void StateHistoryBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    using notification::AnyToken;
    using notification::Token;

    // Notification for the CPU property of a thread.
    notification::Path threadCpuPath {
        Token(kCurrentStateNotificationPrefix),
        Token(kStateLinux),
        Token(kStateThreads),
        AnyToken(),
        Token(kStateCurCpu)};
    notificationCenter->AddObserver(threadCpuPath, base::BindObject(&StateHistoryBlock::OnState, this));

    // Notification for the thread property of a CPU.
    notification::Path cpuThreadPath {
        Token(kCurrentStateNotificationPrefix),
        Token(kStateLinux),
        Token(kStateCpus),
        AnyToken(),
        Token(kStateCurThread)};
    notificationCenter->AddObserver(cpuThreadPath, base::BindObject(&StateHistoryBlock::OnState, this));
}

void StateHistoryBlock::OnState(const notification::Path& path, const value::Value* value)
{
    auto valuePtr = value->GetField(kCurrentStateAttributeValueField);
    if (valuePtr == nullptr)
        return;
    auto key = value->GetField(kCurrentStateAttributeKeyField)->AsUInteger();
    StateHistory()->SetUIntegerValue(state::AttributeKey(key), valuePtr->AsUInteger());
}

}  // namespace state_blocks
}  // namespace tibee
