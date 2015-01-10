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
#ifndef _TIBEE_EXECUTION_THREAD_HPP
#define _TIBEE_EXECUTION_THREAD_HPP

#include <string>

#include "base/BasicTypes.hpp"

namespace tibee
{
namespace execution
{

class Thread
{
public:
    Thread() : _thread(-1) {};
    Thread(thread_t thread) : _thread(thread) {}
    ~Thread() {};

    // Thread id.
    thread_t thread() const { return _thread; }
    void set_thread(thread_t thread) { _thread = thread; }

    // Name.
    const std::string& name() const { return _name; }
    void set_name(const std::string& name) { _name = name; }

    bool operator==(const Thread& other) const {
        return _thread == other._thread &&
            _name == other._name;
    }

    bool operator<(const Thread& other) const {
        if (_thread < other._thread)
            return true;
        if (_thread > other._thread)
            return false;
        return _name < other._name;
    }

private:
    thread_t _thread;
    std::string _name;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_THREAD_HPP
