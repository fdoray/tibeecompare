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
#include "execution_blocks/SpecialBlock.hpp"

#include <iostream>

#include "base/BindObject.hpp"
#include "base/CompareConstants.hpp"
#include "base/Constants.hpp"
#include "base/print.hpp"
#include "notification/Token.hpp"
#include "value/MakeValue.hpp"

namespace tibee {
namespace execution_blocks {

SpecialBlock::SpecialBlock()
    : _numPrinted(0)
{
}

SpecialBlock::~SpecialBlock()
{
}

void SpecialBlock::Start(const value::Value* params)
{
}

void SpecialBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    using notification::Token;

    AddUstObserver(notificationCenter, Token("tracekit:begin"), base::BindObject(&SpecialBlock::onExecutionBegin, this));
    AddUstObserver(notificationCenter, Token("tracekit:end"), base::BindObject(&SpecialBlock::onExecutionEnd, this));

    AddUstObserver(notificationCenter, Token("tracekit:stepBegin"), base::BindObject(&SpecialBlock::onStepBegin, this));
    AddUstObserver(notificationCenter, Token("tracekit:stepEnd"), base::BindObject(&SpecialBlock::onStepEnd, this));
}

void SpecialBlock::onExecutionBegin(const trace::EventValue& event)
{
    auto thread = ThreadForEvent(event);

    std::string type = event.getEventField("type")->AsString();

    ThreadStats stats;
    stats.executionBegin = 0;
    stats.executionEnd = 0;

    if (type == "update")
        stats.executionBegin = State()->timestamp();

    _threadStats[thread] = stats;
}

void SpecialBlock::onExecutionEnd(const trace::EventValue& event)
{
    auto thread = ThreadForEvent(event);
    auto& stats = _threadStats[thread];

    if (stats.executionBegin == 0)
        return;

    ++_numPrinted;
    if (_numPrinted != 10)
        return;

    stats.executionEnd = State()->timestamp();

    auto execution = stats.executionEnd - stats.executionBegin;
    auto lock = stats.stepEnd["lock"] - stats.stepBegin["lock"];
    auto plan = stats.stepEnd["plan"] - stats.stepBegin["plan"];
    std::cout << execution << "," << lock << "," << plan << "," << std::endl;

    stats.executionBegin = 0;
    _numPrinted = 0;
}

void SpecialBlock::onStepBegin(const trace::EventValue& event) {
    auto thread = ThreadForEvent(event);
    auto& stats = _threadStats[thread];
    stats.stepBegin[event.getEventField("step")->AsString()] = State()->timestamp();
}

void SpecialBlock::onStepEnd(const trace::EventValue& event) {
    auto thread = ThreadForEvent(event);
    auto& stats = _threadStats[thread];
    stats.stepEnd[event.getEventField("step")->AsString()] = State()->timestamp();
}

}  // namespace execution_blocks
}  // namespace tibee
