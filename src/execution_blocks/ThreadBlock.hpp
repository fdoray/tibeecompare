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
#ifndef _TIBEE_BUILDERBLOCKS_THREADBLOCK_HPP
#define _TIBEE_BUILDERBLOCKS_THREADBLOCK_HPP

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
 * Block that makes an execution for a full thread.
 *
 * @author Francois Doray
 */
class ThreadBlock : public build_blocks::AbstractBuildBlock
{
public:
    ThreadBlock();
    ~ThreadBlock();

    virtual void Start(const value::Value* params) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void onTimestamp(const notification::Path& path, const value::Value* value);
    void onSchedProcessExit(const trace::EventValue& event);

    bool _started;
};

}  // namespace execution_blocks
}  // namespace tibee

#endif // _TIBEE_BUILDERBLOCKS_THREADBLOCK_HPP
