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
#ifndef TIBEE_SRC_STATE_BLOCKS_PERFCOUNTERBLOCK_HPP_
#define TIBEE_SRC_STATE_BLOCKS_PERFCOUNTERBLOCK_HPP_

#include <string>
#include <vector>

#include "build_blocks/AbstractBuildBlock.hpp"
#include "quark/Quark.hpp"
#include "state/AttributeKey.hpp"

namespace tibee {
namespace state_blocks {

/**
 * Block to keep track of performance counters.
 *
 * @author Francois Doray
 */
class PerfCountersBlock : public build_blocks::AbstractBuildBlock
{
public:
    PerfCountersBlock();
    ~PerfCountersBlock();

    virtual void LoadServices(const block::ServiceList& serviceList) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void InitializeKernel(const trace::EventValue& event);
    void InitializeUst(const trace::EventValue& event);

    void OnSchedSwitchEvent(const trace::EventValue& event);
    void OnTTWUEvent(const trace::EventValue& event);
    void OnUstEvent(const trace::EventValue& event);

    struct PerfCounter
    {
        std::string name;
        quark::Quark quark;
    };

    // Path for thread states.
    state::AttributeKey _threadStateKey;

    // Indicates whether ust perf counters have been initialized.
    bool _ustInitialized;

    // Enabled ust perf counters.
    std::vector<PerfCounter> _ustCounters;

    // Indicates whether kernel perf counters have been initialized.
    bool _kernelInitialized;

    // Enabled ust perf counters.
    std::vector<PerfCounter> _kernelCounters;
};

}  // namespace state_blocks
}  // namespace tibee

#endif  // TIBEE_SRC_STATE_BLOCKS_PERFCOUNTERBLOCK_HPP_
