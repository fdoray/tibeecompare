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
#ifndef _TIBEE_BUILDERBLOCKS_PUNCHBLOCK_HPP
#define _TIBEE_BUILDERBLOCKS_PUNCHBLOCK_HPP

#include <string>

#include "build_blocks/AbstractBuildBlock.hpp"
#include "notification/NotificationCenter.hpp"
#include "notification/Path.hpp"
#include "quark/Quark.hpp"
#include "trace/value/EventValue.hpp"
#include "value/Value.hpp"

namespace tibee {
namespace execution_blocks {

/**
 * Block for tasks that are delimited by specified
 * start and end events.
 *
 * @author Francois Doray
 */
class PunchBlock : public build_blocks::AbstractBuildBlock
{
public:
    PunchBlock();
    ~PunchBlock();

    virtual void Start(const value::Value* params) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void onBegin(const trace::EventValue& event);
    void onEnd(const trace::EventValue& event);

    bool TidIsAnalyzed(uint32_t tid) const;

    // The name of the generated executions.
    std::string _name;

    // The name of the analyzed executable.
    std::string _exec;

    // The name of the begin event.
    std::string _beginEvent;

    // The name of the end event;
    std::string _endEvent;

    // Indicates whether the begin/end events are UST events.
    bool _isUST;
};

}  // namespace execution_blocks
}  // namespace tibee

#endif // _TIBEE_BUILDERBLOCKS_PUNCHBLOCK_HPP