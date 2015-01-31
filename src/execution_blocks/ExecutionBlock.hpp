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
#ifndef _TIBEE_EXECUTIONBLOCKS_EXECUTIONBLOCK_HPP
#define _TIBEE_EXECUTIONBLOCKS_EXECUTIONBLOCK_HPP

#include <string>

#include "execution/ExecutionsBuilder.hpp"
#include "execution/LinksBuilder.hpp"
#include "execution/StacksBuilder.hpp"
#include "block/AbstractBlock.hpp"
#include "notification/NotificationSink.hpp"
#include "notification/Path.hpp"
#include "quark/StringQuarkDatabase.hpp"

namespace tibee {
namespace execution_blocks {

class ExecutionBlock : public block::AbstractBlock
{
public:
    ExecutionBlock();
    ~ExecutionBlock();

private:
    virtual void LoadServices(const block::ServiceList& serviceList) override;
    virtual void RegisterServices(block::ServiceList* serviceList) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

    void onTimestamp(const notification::Path& path, const value::Value* value);
    void onEnd(const notification::Path& path, const value::Value* value);

    // The executions builder.
    execution::ExecutionsBuilder _executionsBuilder;

    // The links builder.
    execution::LinksBuilder _linksBuilder;

    // The stacks builder.
    execution::StacksBuilder _stacksBuilder;

    // The quarks database.
    quark::StringQuarkDatabase* _quarks;

    // Trace identifier.
    std::string _traceId;
};

}  // namespace execution_blocks
}  // namespace tibee

#endif // _TIBEE_EXECUTIONBLOCKS_EXECUTIONBLOCK_HPP