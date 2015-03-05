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
#ifndef _TIBEE_BUILDERBLOCKS_SPECIALBLOCK_HPP
#define _TIBEE_BUILDERBLOCKS_SPECIALBLOCK_HPP

#include <string>
#include <unordered_map>

#include "build_blocks/AbstractBuildBlock.hpp"
#include "notification/NotificationCenter.hpp"
#include "notification/Path.hpp"
#include "quark/Quark.hpp"
#include "trace/value/EventValue.hpp"
#include "value/Value.hpp"

namespace tibee {
namespace execution_blocks {

/**
 * Special block!
 *
 * @author Francois Doray
 */
class SpecialBlock : public build_blocks::AbstractBuildBlock
{
public:
    SpecialBlock();
    ~SpecialBlock();

    virtual void Start(const value::Value* params) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void onExecutionBegin(const trace::EventValue& event);
    void onExecutionEnd(const trace::EventValue& event);

    void onStepBegin(const trace::EventValue& event);
    void onStepEnd(const trace::EventValue& event);

    struct ThreadStats
    {
        timestamp_t executionBegin;
        timestamp_t executionEnd;

        std::unordered_map<std::string, timestamp_t> stepBegin;
        std::unordered_map<std::string, timestamp_t> stepEnd;
    };
    std::unordered_map<thread_t, ThreadStats> _threadStats;

    int _numPrinted;
};

}  // namespace execution_blocks
}  // namespace tibee

#endif // _TIBEE_BUILDERBLOCKS_SPECIALBLOCK_HPP
