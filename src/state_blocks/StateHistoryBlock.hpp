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
#ifndef TIBEE_SRC_STATE_BLOCKS_STATEHISTORYBLOCK_HPP_
#define TIBEE_SRC_STATE_BLOCKS_STATEHISTORYBLOCK_HPP_

#include "build_blocks/AbstractBuildBlock.hpp"

namespace tibee {
namespace state_blocks {

/**
 * Blocks that keeps track of the state history.
 *
 * @author Francois Doray
 */
class StateHistoryBlock : public build_blocks::AbstractBuildBlock
{
public:
    StateHistoryBlock();
    ~StateHistoryBlock();

    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void OnState(const notification::Path& path, const value::Value* value);
};

}  // namespace state_blocks
}  // namespace tibee

#endif  // TIBEE_SRC_STATE_BLOCKS_STATEHISTORYBLOCK_HPP_
