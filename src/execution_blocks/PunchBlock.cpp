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
#include "execution_blocks/PunchBlock.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

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
const char kUstPrefix[] = "ust/";
const char kKernelPrefix[] = "kernel/";
}  // namespace

PunchBlock::PunchBlock()
    : _stats(false)
{
}

PunchBlock::~PunchBlock()
{
}

void PunchBlock::Start(const value::Value* params)
{
    auto nameValue = params->GetField("name");
    auto execValue = params->GetField("exec");
    auto beginValue = params->GetField("begin");
    auto endValue = params->GetField("end");
    auto statsValue = params->GetField("stats");

    if (nameValue == nullptr || execValue == nullptr ||
        beginValue == nullptr || endValue == nullptr ||
        statsValue == nullptr)
    {
        base::tberror() << "Missing some parameters for punch block." << base::tbendl();
        return;
    }

    _name = nameValue->AsString();
    _exec = execValue->AsString();
    _beginEvent = beginValue->AsString();
    _endEvent = endValue->AsString();
    _stats = value::BoolValue::Cast(statsValue)->GetValue();

    if (_stats)
    {
        std::cout << "duration_us" << std::endl;
    }
}

void PunchBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    AddEventObserver(notificationCenter, _beginEvent, &PunchBlock::onBegin);
    AddEventObserver(notificationCenter, _endEvent, &PunchBlock::onEnd);
}

void PunchBlock::onBegin(const trace::EventValue& event)
{
    auto tid = ThreadForEvent(event);
    if (!TidIsAnalyzed(tid))
        return;

    Executions()->StartExecution(tid, _name, true);
}

void PunchBlock::onEnd(const trace::EventValue& event)
{
    auto tid = ThreadForEvent(event);
    if (!TidIsAnalyzed(tid))
        return;

    timestamp_t duration = Executions()->EndExecution(tid);
    if (_stats && duration != 0 && duration > 5000000)
    {
        std::cout << (duration / 1000) << "," << State()->timestamp() << std::endl;
    }
}

void PunchBlock::AddEventObserver(notification::NotificationCenter* notificationCenter,
                                  const std::string& name,
                                  Observer observer)
{
    if (boost::starts_with(name, kUstPrefix))
    {
        AddUstObserver(notificationCenter,
                       notification::Token(name.substr(strlen(kUstPrefix))),
                       base::BindObject(observer, this));
    }
    else if (boost::starts_with(name, kKernelPrefix))
    {
        AddKernelObserver(notificationCenter,
                          notification::Token(name.substr(strlen(kKernelPrefix))),
                          base::BindObject(observer, this));
    }
    else
    {
        base::tberror() << "Punch block: event name is not prefixed with ust/ or kernel/."
                        << base::tbendl();
    }
}

bool PunchBlock::TidIsAnalyzed(uint32_t tid) const
{
    if (_exec.empty())
        return true;
    auto thread_name = State()->CurrentNameForThread(tid);
    return thread_name == _exec;
}

}  // namespace execution_blocks
}  // namespace tibee
