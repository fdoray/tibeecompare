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
#ifndef _TIBEE_BUILDERBLOCKS_SCHEDWAKEUPBLOCK_HPP
#define _TIBEE_BUILDERBLOCKS_SCHEDWAKEUPBLOCK_HPP

#include <unordered_map>

#include "base/BasicTypes.hpp"
#include "execution_blocks/AbstractExecutionBlock.hpp"
#include "notification/NotificationCenter.hpp"
#include "quark/Quark.hpp"
#include "trace/value/EventValue.hpp"

namespace tibee {
namespace execution_blocks {

/**
 * Block that generates links for sched wakeup events.
 *
 * @author Francois Doray
 */
class SchedWakeupBlock : public execution_blocks::AbstractExecutionBlock
{
public:
    SchedWakeupBlock();
    ~SchedWakeupBlock();

    virtual void Start(const value::Value* params) override;
    virtual void LoadServices(const block::ServiceList& serviceList) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void OnTTWU(const trace::EventValue& event);
    void OnInterruptEntry(const trace::EventValue& event);
    void OnInterruptExit(const trace::EventValue& event);

    // Number of nested interrupts per CPU.
    typedef std::unordered_map<uint32_t, uint32_t> NestedInterrupts;
    NestedInterrupts _nestedInterrupts;

    bool _withIO;
    bool _withFutex;

    // Constant quarks.
    quark::Quark Q_LINUX;
    quark::Quark Q_THREADS;
    quark::Quark Q_SYSCALL;
};

}  // namespace execution_blocks
}  // namespace tibee

#endif // _TIBEE_BUILDERBLOCKS_SCHEDWAKEUPBLOCK_HPP
