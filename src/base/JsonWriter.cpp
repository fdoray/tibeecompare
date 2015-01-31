/* Copyright (c) 2015 Francois Pierre Doray <francois.pierre-doray@polymtl.ca>
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
#include "base/JsonWriter.hpp"

#include <cctype>

#include "base/EscapeString.hpp"
#include "base/ex/FatalError.hpp"

namespace tibee
{
namespace base
{

JsonWriter::JsonWriter()
{
    _isFirstOfSection.push(true);
}

JsonWriter::~JsonWriter()
{
    if (!_sections.empty())
        throw ex::FatalError("Json: Some sections were not closed.");
}

bool JsonWriter::Open(const boost::filesystem::path& path)
{
    _out.open(path.string());
    return _out.is_open();
}

void JsonWriter::BeginArray()
{
    EnsureFile();

    if (!_isFirstOfSection.top())
        _out << ",";
    _isFirstOfSection.top() = false;

    _out << "[";
    _sections.push(SectionType::kArray);
    _isFirstOfSection.push(true);
}

void JsonWriter::EndArray()
{
    EnsureFile();

    if (_sections.empty() || _sections.top() != SectionType::kArray)
        throw ex::FatalError("Json: Cannot end array here.");
    _sections.pop();
    _isFirstOfSection.pop();

    _out << "]";
}

void JsonWriter::Value(const std::string& value)
{
    EnsureFile();
    ValueInternal(std::string("\"") + EscapeString(value) + "\"");
}

void JsonWriter::BeginDict()
{
    EnsureFile();

    if (!_isFirstOfSection.top())
        _out << ",";
    _isFirstOfSection.top() = false;

    _out << "{";
    _sections.push(SectionType::kDict);
    _isFirstOfSection.push(true);
}

void JsonWriter::EndDict()
{
    EnsureFile();

    if (_sections.empty() || _sections.top() != SectionType::kDict)
        throw ex::FatalError("Json: Cannot end dictionary here.");
    _sections.pop();
    _isFirstOfSection.pop();

    _out << "}";
}

void JsonWriter::KeyValue(const std::string& key, const std::string& value)
{
    EnsureFile();
    KeyValueInternal(key, std::string("\"") + EscapeString(value) + "\"");
}

void JsonWriter::KeyDictValue(const std::string& key)
{
    EnsureFile();

    if (_sections.empty() || _sections.top() != SectionType::kDict)
        throw ex::FatalError("Json: Cannot write a key-value outside a dictionary.");

    if (!_isFirstOfSection.top())
        _out << ",";
    _isFirstOfSection.top() = false;

    _sections.push(SectionType::kDict);
    _isFirstOfSection.push(true);

    _out << "\"" << EscapeString(key) << "\":{";
}

void JsonWriter::KeyArrayValue(const std::string& key)
{
    EnsureFile();

    if (_sections.empty() || _sections.top() != SectionType::kDict)
        throw ex::FatalError("Json: Cannot write a key-value outside a dictionary.");

    if (!_isFirstOfSection.top())
        _out << ",";
    _isFirstOfSection.top() = false;

    _sections.push(SectionType::kArray);
    _isFirstOfSection.push(true);

    _out << "\"" << EscapeString(key) << "\":[";
}

void JsonWriter::EnsureFile()
{
    if (!_out.is_open())
        throw ex::FatalError("Json: No active Json file.");
}

void JsonWriter::ValueInternal(const std::string& escapedValue)
{
    if (_sections.empty() || _sections.top() != SectionType::kArray)
        throw ex::FatalError("Json: Cannot write a value outside an array.");

    if (!_isFirstOfSection.top())
        _out << ",";
    _isFirstOfSection.top() = false; 

    _out << escapedValue;
}

void JsonWriter::KeyValueInternal(const std::string& key,
                                  const std::string& escapedValue)
{
    if (_sections.empty() || _sections.top() != SectionType::kDict)
        throw ex::FatalError("Json: Cannot write a key-value outside a dictionary.");

    if (!_isFirstOfSection.top())
        _out << ",";
    _isFirstOfSection.top() = false; 

    _out << "\"" << EscapeString(key) << "\":" << escapedValue;
}

}  // namespace base
}  // namespace tibee
