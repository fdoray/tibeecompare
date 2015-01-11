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
#ifndef _TIBEE_EXECUTION_STACKITEM_HPP
#define _TIBEE_EXECUTION_STACKITEM_HPP

#include <memory>
#include <unordered_map>

#include "base/BasicTypes.hpp"
#include "quark/Quark.hpp"

namespace tibee
{
namespace execution
{

class StackItem
{
public:
    typedef std::unique_ptr<StackItem> UP;
    typedef std::unordered_map<quark::Quark, uint64_t> NumericProperties;
    typedef std::unordered_map<quark::Quark, std::string> StringProperties;

    StackItem();
    ~StackItem();

    // Name.
    const quark::Quark& name() const { return _name; }
    void set_name(const quark::Quark& name) { _name = name; }

    // Depth.
    size_t depth() const { return _depth; }
    void set_depth(size_t depth) { _depth = depth; }

    // Start time.
    timestamp_t start() const { return _start; }
    void set_start(timestamp_t start) { _start = start; }

    // End time.
    timestamp_t end() const { return _end; }
    void set_end(timestamp_t end) { _end = end; }

    bool operator==(const StackItem& other) const;

private:
    // Name.
    quark::Quark _name;

    // Depth.
    size_t _depth;

    // Start time.
    timestamp_t _start;

    // End time.
    timestamp_t _end;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_STACKITEM_HPP
