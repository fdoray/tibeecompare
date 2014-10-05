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
    StackItem();
    ~StackItem();

    // Name.
    const std::string& name() const { return _name; }
    void set_name(const std::string& name) { _name = name; }

    // Numeric properties.
    bool GetNumericProperty(quark::Quark key, uint64_t* value) const;
    void SetNumericProperty(quark::Quark key, uint64_t value);

    // String properties.
    bool GetStringProperty(quark::Quark key, std::string* value) const;
    void SetStringProperty(quark::Quark key, std::string value);

private:
    // Name.
    std::string _name;

    // Numeric properties.
    typedef std::unordered_map<quark::Quark, uint64_t> NumericProperties;
    NumericProperties _numericProperties;

    // String properties.
    typedef std::unordered_map<quark::Quark, std::string> StringProperties;
    StringProperties _stringProperties;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_STACKITEM_HPP
