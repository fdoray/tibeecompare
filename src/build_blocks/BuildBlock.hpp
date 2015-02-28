/* Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
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
#ifndef _TIBEE_BUILDBLOCKS_BUILDBLOCK_HPP
#define _TIBEE_BUILDBLOCKS_BUILDBLOCK_HPP

#include <string>

#include "block/AbstractBlock.hpp"
#include "critical/CriticalGraph.hpp"
#include "db/Database.hpp"
#include "execution/ExecutionsBuilder.hpp"
#include "notification/Path.hpp"
#include "quark/StringQuarkDatabase.hpp"
#include "stacks/StacksBuilder.hpp"
#include "state/CurrentState.hpp"
#include "state/StateHistory.hpp"

namespace tibee {
namespace build_blocks {

class BuildBlock : public block::AbstractBlock
{
public:
    BuildBlock(bool stats);
    ~BuildBlock();

private:
    virtual void LoadServices(const block::ServiceList& serviceList) override;
    virtual void RegisterServices(block::ServiceList* serviceList) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

    void onTimestamp(const notification::Path& path, const value::Value* value);
    void onEnd(const notification::Path& path, const value::Value* value);

    void SaveExecutions();

    // Database.
    db::Database _db;

    // The executions builder.
    execution::ExecutionsBuilder _executionsBuilder;

    // The stacks builder.
    stacks::StacksBuilder _stacksBuilder;

    // The critical graph.
    critical::CriticalGraph _criticalGraph;

    // The state history.
    state::StateHistory _stateHistory;

    // The quarks database.
    quark::StringQuarkDatabase* _quarks;

    // The current state.
    state::CurrentState* _currentState;

    // Trace identifier.
    std::string _traceId;

    // Indicates that we are just showing stats.
    bool _stats;

    // Last timestamp at which executions were saved.
    timestamp_t _saveTs;

    // Timestamp of the last cleanup.
    timestamp_t _lastCleanupTs;

    // Number of executions.
    size_t _numExecutions;
};

}  // namespace build_blocks
}  // namespace tibee

#endif // _TIBEE_BUILDBLOCKS_BUILDBLOCK_HPP
