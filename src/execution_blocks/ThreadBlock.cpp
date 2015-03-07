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
#include "execution_blocks/ThreadBlock.hpp"

#include "base/BindObject.hpp"
#include "base/CompareConstants.hpp"
#include "base/Constants.hpp"
#include "base/print.hpp"
#include "notification/Token.hpp"
#include "value/MakeValue.hpp"

namespace tibee {
namespace execution_blocks {

namespace
{
const thread_t kAnalyzedThread = 17882;
}  // namespace

ThreadBlock::ThreadBlock()
    : _started(false)
{
}

ThreadBlock::~ThreadBlock()
{
}

void ThreadBlock::Start(const value::Value* params)
{
}

void ThreadBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    using notification::Token;

    notificationCenter->AddObserver(
        {Token(kTraceNotificationPrefix), Token(kTimestampNotificationName)},
        base::BindObject(&ThreadBlock::onTimestamp, this));
}

void ThreadBlock::onTimestamp(const notification::Path& path, const value::Value* value)
{
    if (!_started) {
        _started = true;
        Executions()->StartExecution(kAnalyzedThread, "mongoperfreg", false);
    }
}

void ThreadBlock::onSchedProcessExit(const trace::EventValue& event)
{
    auto tid = ThreadForEvent(event);
    if (tid == kAnalyzedThread)
        Executions()->EndExecution(kAnalyzedThread);
}



}  // namespace execution_blocks
}  // namespace tibee
