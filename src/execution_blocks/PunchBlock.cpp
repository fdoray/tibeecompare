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

#include "base/BindObject.hpp"
#include "base/CompareConstants.hpp"
#include "base/Constants.hpp"
#include "base/print.hpp"
#include "notification/Token.hpp"
#include "value/MakeValue.hpp"

namespace tibee {
namespace execution_blocks {

PunchBlock::PunchBlock()
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
    auto typeValue = params->GetField("type");

    if (nameValue == nullptr || execValue == nullptr ||
        beginValue == nullptr || endValue == nullptr ||
        typeValue == nullptr)
    {
        base::tberror() << "Missing some parameters for punch block." << base::tbendl();
        return;
    }

    _name = execValue->AsString();
    _exec = execValue->AsString();
    _beginEvent = beginValue->AsString();
    _endEvent = endValue->AsString();
    _isUST = typeValue->AsString() == "ust";
}

void PunchBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    using notification::Token;

    if (_isUST)
    {
        AddUstObserver(notificationCenter, Token(_beginEvent),
                       base::BindObject(&PunchBlock::onBegin, this));
        AddUstObserver(notificationCenter, Token(_endEvent),
                       base::BindObject(&PunchBlock::onEnd, this));
    }
    else
    {
        AddKernelObserver(notificationCenter, Token(_beginEvent),
                          base::BindObject(&PunchBlock::onBegin, this));
        AddKernelObserver(notificationCenter, Token(_endEvent),
                          base::BindObject(&PunchBlock::onEnd, this));
    }
}

void PunchBlock::onBegin(const trace::EventValue& event)
{
    auto tid = ThreadForEvent(event);
    if (!TidIsAnalyzed(tid))
        return;

    Executions()->StartThreadExecution(tid, _name, true);
}

void PunchBlock::onEnd(const trace::EventValue& event)
{
    auto tid = ThreadForEvent(event);
    if (!TidIsAnalyzed(tid))
        return;

    Executions()->EndThreadExecution(tid);
}

bool PunchBlock::TidIsAnalyzed(uint32_t tid) const
{
    auto thread_name = State()->CurrentNameForThread(tid);
    return thread_name == _exec;
}

}  // namespace execution_blocks
}  // namespace tibee