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
#ifndef _TIBEE_EXECUTION_STACKSFROMDISK_HPP
#define _TIBEE_EXECUTION_STACKSFROMDISK_HPP

#include "execution/AbstractStacks.hpp"

namespace tibee
{
namespace execution
{

class StacksFromDisk : public AbstractStacks
{
public:
    StacksFromDisk();
    ~StacksFromDisk();

    // Loading data from disk.
    void AddThread(const Thread& thread);
    void AddStackItem(thread_t thread, const StackItem& item);
    void AddLink(const Link& link);
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_STACKSFROMDISK_HPP
