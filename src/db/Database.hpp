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
#ifndef _TIBEE_DB_DATABASE_HPP
#define _TIBEE_DB_DATABASE_HPP

#include <functional>
#include <leveldb/comparator.h>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <memory>
#include <unordered_map>

#include "base/BasicTypes.hpp"
#include "execution/Execution.hpp"
#include "stacks/Identifiers.hpp"
#include "stacks/Stack.hpp"

namespace tibee
{
namespace db
{

class Database
{
public:
    typedef std::function<void (const execution::Execution&)>
        EnumerateExecutionsCallback;

    Database();
    Database(bool isTest);
    ~Database();

    // Function names.
    const std::string& GetFunctionName(stacks::FunctionNameId id) const;
    stacks::FunctionNameId AddFunctionName(const std::string& name);

    // Stacks.
    const stacks::Stack& GetStack(stacks::StackId id) const;
    stacks::StackId AddStack(const stacks::Stack& stack);

    // Executions.
    void EnumerateExecutions(
        const std::string& name,
        const EnumerateExecutionsCallback& callback) const;
    void EnumerateExecutions(
        const std::string& name,
        uint64_t numDesired,
        const EnumerateExecutionsCallback& callback) const;
    void AddExecution(const execution::Execution& execution);

    // Destroy test database.
    static void DestroyTestDb();

    // Print a stack.
    void PrintStack(stacks::StackId id);

private:
    // Open database.
    void OpenDatabase() const;

    // Get a new identifier.
    uint32_t GetIdentifier(
        char* type, uint32_t* nextIdentifier, leveldb::WriteBatch* batch);

    // Strings. Uses the same data as function names.
    std::string GetString(uint32_t id) const;
    stacks::FunctionNameId AddString(const std::string& str);

    // Find the approximative size of an execution.
    uint64_t ApproxExecutionSize(stacks::FunctionNameId executionNameId) const;

    // Indicates whether we use the test database.
    bool _isTest;

    // Leveldb
    std::unique_ptr<leveldb::DB> _db;

    // Comparator for leveldb keys.
    std::unique_ptr<leveldb::Comparator> _comparator;

    // Cache for function names.
    std::unordered_map<stacks::FunctionNameId, std::string> _functionNamesCache;

    // Cache for stacks.
    std::unordered_map<stacks::StackId, stacks::Stack> _stacksCache;

};

}  // namespace db
}  // namespace tibee

#endif // _TIBEE_DB_DATABASE_HPP
