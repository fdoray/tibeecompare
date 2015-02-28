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
#include "db/Database.hpp"

#include <iostream>
#include <sstream>
#include <vector>

#include "base/ex/FatalError.hpp"
#include "db/ReadBuffer.hpp"
#include "db/Slice.hpp"
#include "db/WriteBuffer.hpp"

namespace tibee
{
namespace db
{

namespace
{

const char kDbFile[] = "prod-db";
const char kTestDbFile[] = "test-db";

const size_t kIdKeySize = sizeof(char) + sizeof(uint32_t);

const char kFunctionNameCount = 0;
const char kStackCount = 1;
const char kFunctionNameIdType = 2;
const char kStackIdType = 3;
const char kExecutionNameReverseIdType = 4;
const char kFunctionNameReverseIdType = 5;
const char kStackReverseIdType = 6;
const char kExecutionKeyType = 7;

struct Key
{
    Key() : type(0), id(0), timestamp(0) {}

    // Record type.
    char type;

    // Identifier.
    uint32_t id;

    // Timestamp (only for executions).
    timestamp_t timestamp;
} __attribute__ ((packed));

class KeyComparator : public leveldb::Comparator
{
public:
    int Compare(const leveldb::Slice& a, const leveldb::Slice& b) const
    {
        const Key* keyA =
                reinterpret_cast<const Key*>(a.data());
        const Key* keyB =
                reinterpret_cast<const Key*>(b.data());

        // Order by key type.
        if (keyA->type < keyB->type)
            return -1;
        if (keyA->type > keyB->type)
            return 1;

        if (keyA->type <= kStackCount)
            return 0;

        if (keyA->type <= kStackReverseIdType)
        {
            if (a.size() < b.size())
                return -1;
            if (a.size() > b.size())
                return 1;

            return memcmp(a.data(), b.data(), a.size());
        }
        else
        {
            // Execution (name + timestamp) -> Execution.
            if (keyA->id < keyB->id)
                return -1;
            else if (keyA->id > keyB->id)
                return 1;

            if (keyA->timestamp < keyB->timestamp)
                return -1;
            else if (keyA->timestamp > keyB->timestamp)
                return 1;

            return 0;
        }
    }

    const char* Name() const { return "KeyComparator"; }
    void FindShortestSeparator(std::string*, const leveldb::Slice&) const { }
    void FindShortSuccessor(std::string*) const { }
};

uint32_t StringToUint32(const std::string str)
{
    return *reinterpret_cast<const uint32_t*>(str.c_str());
}

void WriteExecutionMetadataToBuffer(
        uint32_t nameId,
        uint32_t traceId,
        const execution::Execution& execution,
        std::vector<char>* buffer)
{
    WriteBuffer(nameId, buffer);
    WriteBuffer(traceId, buffer);
    WriteBuffer(execution.startTs(), buffer);
    WriteBuffer(execution.startThread(), buffer);
    WriteBuffer(execution.endTs(), buffer);
    WriteBuffer(execution.endThread(), buffer);
}

void WriteExecutionMetricsToBuffer(
        const execution::Execution& execution,
        std::vector<char>* buffer)
{
    WriteBuffer(execution.metrics_size(), buffer);

    for (auto it = execution.metrics_begin();
            it != execution.metrics_end();
            ++it)
    {
        WriteBuffer(it->first, buffer);
        WriteBuffer(it->second, buffer);
    }
}

void WriteExecutionSamplesToBuffer(
        const execution::Execution& execution,
        std::vector<char>* buffer)
{
    WriteBuffer(execution.samples_size(), buffer);

    for (auto it = execution.samples_begin();
            it != execution.samples_end();
            ++it)
    {
        WriteBuffer(it->first, buffer);
        WriteBuffer(it->second, buffer);
    }
}

bool ReadExecutionMetadataFromBuffer(
        const leveldb::Slice& buffer,
        size_t* pos,
        uint32_t* nameId,
        uint32_t* traceId,
        execution::Execution* execution)
{
    timestamp_t startTs = 0;
    thread_t startThread = 0;
    timestamp_t endTs = 0;
    thread_t endThread = 0;

    if (!ReadBuffer(buffer, pos, nameId) ||
            !ReadBuffer(buffer, pos, traceId) ||
            !ReadBuffer(buffer, pos, &startTs) ||
            !ReadBuffer(buffer, pos, &startThread) ||
            !ReadBuffer(buffer, pos, &endTs) ||
            !ReadBuffer(buffer, pos, &endThread))
    {
        return false;
    }

    execution->set_startTs(startTs);
    execution->set_startThread(startThread);
    execution->set_endTs(endTs);
    execution->set_endThread(endThread);

    return true;
}

bool ReadExecutionMetricsFromBuffer(
        const leveldb::Slice& buffer,
        size_t* pos,
        execution::Execution* execution)
{
    size_t metrics_size = 0;
    if (!ReadBuffer(buffer, pos, &metrics_size))
        return false;

    for (size_t i = 0; i < metrics_size; ++i)
    {
        MetricId metricId = 0;
        uint64_t metricValue = 0;

        if (!ReadBuffer(buffer, pos, &metricId) ||
                !ReadBuffer(buffer, pos, &metricValue))
        {
            return false;
        }

        execution->SetMetric(metricId, metricValue);
    }

    return true;
}

bool ReadExecutionSamplesFromBuffer(
        const leveldb::Slice& buffer,
        size_t* pos,
        execution::Execution* execution)
{
    size_t samples_size = 0;
    if (!ReadBuffer(buffer, pos, &samples_size))
        return false;

    for (size_t i = 0; i < samples_size; ++i)
    {
        stacks::StackId stackId = 0;
        uint64_t stackValue = 0;

        if (!ReadBuffer(buffer, pos, &stackId) ||
                !ReadBuffer(buffer, pos, &stackValue))
        {
            return false;
        }

        execution->IncrementSample(stackId, stackValue);
    }

    return true;
}

}  // namespace

Database::Database()
: _isTest(false)
{
    _comparator.reset(new KeyComparator);
}

Database::Database(bool isTest)
: _isTest(isTest)
{
    _comparator.reset(new KeyComparator);
}

Database::~Database()
{
    // Delete db before comparator.
    _db.reset(nullptr);
    _comparator.reset(nullptr);
}

const std::string& Database::GetFunctionName(stacks::FunctionNameId id) const
{
    OpenDatabase();

    auto look = _functionNamesCache.find(id);
    if (look != _functionNamesCache.end())
        return look->second;

    Key key;
    key.type = kFunctionNameIdType;
    key.id = id;
    auto keySlice = Slice(&key, kIdKeySize);

    std::string name;
    auto status = _db->Get(
            leveldb::ReadOptions(), keySlice, &name);

    if (!status.ok())
    {
        std::stringstream message;
        message << "Unable to retrieve a function name for the provided id ("
                << id << ").";
        throw base::ex::FatalError(message.str());
    }

    const_cast<Database*>(this)->_functionNamesCache[id] = name;

    return const_cast<Database*>(this)->_functionNamesCache[id];
}

stacks::FunctionNameId Database::AddFunctionName(const std::string& name)
{
    OpenDatabase();

    // Check whether this function name is already in the database.
    std::vector<char> reverseKeyBuffer;
    WriteBuffer(kFunctionNameReverseIdType, &reverseKeyBuffer);
    WriteStringToBuffer(name, &reverseKeyBuffer);
    leveldb::Slice reverseKey(
            reverseKeyBuffer.data(), reverseKeyBuffer.size());

    std::string idStr;
    auto status = _db->Get(leveldb::ReadOptions(), reverseKey, &idStr);

    if (status.ok())
    {
        // The function name is already in the database.
        return StringToUint32(idStr);
    }

    // Start a batch of writes to insert the function name in the database.
    leveldb::WriteBatch batch;

    // Generate identifier for this function name.
    char type = kFunctionNameCount;
    uint32_t nextIdentifier = 0;
    auto id = GetIdentifier(&type, &nextIdentifier, &batch);

    // Write id -> function name.
    Key key;
    key.type = kFunctionNameIdType;
    key.id = id;

    leveldb::Slice keySlice = Slice(&key, kIdKeySize);
    batch.Put(keySlice, name);

    // Write function name -> id.
    batch.Put(reverseKey, Slice(&id, sizeof(id)));

    // Execute the batch of writes.
    status = _db->Write(leveldb::WriteOptions(), &batch);
    if (!status.ok())
        throw base::ex::FatalError("Unable to add function name in database.");

    _functionNamesCache[id] = name;

    return id;
}

const stacks::Stack& Database::GetStack(stacks::StackId id) const
{
    OpenDatabase();

    auto look = _stacksCache.find(id);
    if (look != _stacksCache.end())
        return look->second;

    Key key;
    key.type = kStackIdType;
    key.id = id;
    auto keySlice = Slice(&key, kIdKeySize);

    std::string stackStr;
    auto status = _db->Get(
            leveldb::ReadOptions(), keySlice, &stackStr);

    if (!status.ok())
    {
        std::stringstream message;
        message << "Unable to retrieve a stack for the provided id ("
                << id << ").";
        throw base::ex::FatalError(message.str());
    }

    if (stackStr.size() != sizeof(stacks::Stack))
    {
        throw base::ex::FatalError(
                "Read a stack with an incorrect size.");
    }

    stacks::Stack stack;
    memcpy(&stack, stackStr.c_str(), sizeof(stack));

    const_cast<Database*>(this)->_stacksCache[id] = stack;

    return const_cast<Database*>(this)->_stacksCache[id];
}

stacks::StackId Database::AddStack(const stacks::Stack& stack)
{
    OpenDatabase();

    // Check whether this stack is already in the database.
    std::vector<char> reverseKeyBuffer;
    WriteBuffer(kStackReverseIdType, &reverseKeyBuffer);
    WriteBuffer(stack, &reverseKeyBuffer);
    leveldb::Slice reverseKey(
            reverseKeyBuffer.data(), reverseKeyBuffer.size());

    std::string idStr;
    auto status = _db->Get(leveldb::ReadOptions(), reverseKey, &idStr);

    if (status.ok())
    {
        // The stack is already in the database.
        return StringToUint32(idStr);
    }

    // Start a batch of writes to insert the function name in the database.
    leveldb::WriteBatch batch;

    // Generate identifier for this function name.
    char type = kStackCount;
    uint32_t nextIdentifier = 0;
    auto id = GetIdentifier(&type, &nextIdentifier, &batch);

    // Write id -> stack.
    Key key;
    key.type = kStackIdType;
    key.id = id;

    leveldb::Slice keySlice = Slice(&key, kIdKeySize);
    leveldb::Slice stackSlice = Slice(&stack, sizeof(stack));
    batch.Put(keySlice, stackSlice);

    // Write stack -> id.
    batch.Put(reverseKey, Slice(&id, sizeof(id)));

    // Execute the batch of writes.
    status = _db->Write(leveldb::WriteOptions(), &batch);
    if (!status.ok())
        throw base::ex::FatalError("Unable to add stack in database.");

    _stacksCache[id] = stack;

    return id;
}

void Database::EnumerateExecutions(
        const std::string& name,
        const EnumerateExecutionsCallback& callback) const
{
    EnumerateExecutions(name, 0, callback);
}

void Database::EnumerateExecutions(
        const std::string& name,
        uint64_t numDesired,
        const EnumerateExecutionsCallback& callback) const
{
    OpenDatabase();

    auto executionNameId =
            const_cast<Database*>(this)->AddString(name);

    Key startKey;
    startKey.type = kExecutionKeyType;
    startKey.id = executionNameId;
    startKey.timestamp = 0;
    auto startKeySlice = Slice(&startKey, sizeof(startKey));

    Key endKey;
    endKey.type = kExecutionKeyType;
    endKey.id = executionNameId;
    endKey.timestamp = -1;
    auto endKeySlice = Slice(&endKey, sizeof(endKey));

    // Get the approximative number of bytes for all executions.
    size_t numSkip = 0;

    if (numDesired != 0)
    {
        leveldb::Range range(startKeySlice, endKeySlice);
        uint64_t approximativeTotalSize;
        _db->GetApproximateSizes(&range, 1, &approximativeTotalSize);
        uint64_t approximativeExecSize = ApproxExecutionSize(executionNameId);

        if (approximativeTotalSize != 0 && approximativeExecSize != 0)
        {
            auto approximativeExecCount =
                    approximativeTotalSize / approximativeExecSize;
            auto desiredProportion = approximativeExecCount / numDesired;
            if (desiredProportion >= 2)
                numSkip = desiredProportion;
        }
    }

    // Iterate executions.
    std::unique_ptr<leveldb::Iterator> it(
            _db->NewIterator(leveldb::ReadOptions()));

    for (it->Seek(startKeySlice);
            it->Valid() && _comparator->Compare(it->key(), endKeySlice) <= 0;
            it->Next())
    {
        execution::Execution execution;
        size_t pos = 0;

        uint32_t nameId = 0;
        uint32_t traceId = 0;
        if (!ReadExecutionMetadataFromBuffer(
                it->value(), &pos, &nameId, &traceId, &execution))
        {
            throw base::ex::FatalError("Unable to read execution metadata.");
        }

        execution.set_name(GetString(nameId));
        execution.set_trace(GetString(traceId));

        if (!ReadExecutionMetricsFromBuffer(
                it->value(), &pos, &execution))
        {
            throw base::ex::FatalError("Unable to read execution metrics.");
        }

        if (!ReadExecutionSamplesFromBuffer(
                it->value(), &pos, &execution))
        {
            throw base::ex::FatalError("Unable to read execution samples.");
        }

        callback(execution);

        // Skip records.
        for (size_t i = 1; i < numSkip; ++i)
        {
            it->Next();
            if (!it->Valid())
                return;
        }
    }
}

void Database::AddExecution(const execution::Execution& execution)
{
    OpenDatabase();

    auto executionNameId = AddString(execution.name());

    // Generate a key for the execution.
    Key key;
    key.type = kExecutionKeyType;
    key.id = executionNameId,
            key.timestamp = execution.startTs();

    // Write execution to a buffer.
    std::vector<char> buffer;
    WriteExecutionMetadataToBuffer(
            executionNameId,
            AddString(execution.trace()),
            execution,
            &buffer);
    WriteExecutionMetricsToBuffer(execution, &buffer);
    WriteExecutionSamplesToBuffer(execution, &buffer);

    // Insert execution in database.
    auto status = _db->Put(leveldb::WriteOptions(),
            Slice(&key, sizeof(key)),
            Slice(buffer.data(), buffer.size()));

    if (!status.ok())
        throw base::ex::FatalError("Unable to insert execution in database.");
}

void Database::DestroyTestDb()
{
    auto status = leveldb::DestroyDB(kTestDbFile, leveldb::Options());
    if (!status.ok())
        throw base::ex::FatalError("Unable to destroy test database.");
}

void Database::PrintStack(stacks::StackId id)
{
    OpenDatabase();

    if (id == stacks::kEmptyStackId)
    {
        std::cout << "<Empty>" << std::endl;
        return;
    }

    // Get the functions from the top stack.
    while (id != stacks::kEmptyStackId) {
        auto step = GetStack(id);
        std::cout << GetFunctionName(step.function()) << std::endl;
        id = step.bottom();
    }
}

void Database::OpenDatabase() const
{
    if (_db.get() != nullptr)
        return;

    leveldb::DB* db_ptr = nullptr;
    leveldb::Options options;
    options.create_if_missing = true;
    options.comparator = _comparator.get();
    const char* file = _isTest ? kTestDbFile : kDbFile;
    leveldb::Status status = leveldb::DB::Open(options, file, &db_ptr);

    std::unique_ptr<leveldb::DB>* db =
            const_cast<std::unique_ptr<leveldb::DB>*>(&_db);
    db->reset(db_ptr);

    if (!status.ok())
    {
        db->reset(nullptr);
        std::stringstream ss;
        ss << "unable to open level db: " << status.ToString();
        throw base::ex::FatalError(ss.str());
    }
}

uint32_t Database::GetIdentifier(
        char* type, uint32_t* nextIdentifier, leveldb::WriteBatch* batch)
{
    leveldb::Slice key(type, sizeof(*type));
    std::string identifierStr;
    auto status = _db->Get(leveldb::ReadOptions(), key, &identifierStr);

    // The identifier 0 is not used.
    uint32_t identifier = 1;

    if (status.ok())
    {
        identifier = *reinterpret_cast<const uint32_t*>(identifierStr.c_str());
    }
    else if (!status.IsNotFound())
    {
        std::stringstream ss;
        ss << "unable to generate an identifier: " << status.ToString();
        throw base::ex::FatalError(ss.str());
    }

    *nextIdentifier = identifier + 1;
    batch->Put(key,
            leveldb::Slice(reinterpret_cast<const char*>(nextIdentifier),
                    sizeof(*nextIdentifier)));

    return identifier;
}

std::string Database::GetString(uint32_t id) const
{
    return GetFunctionName(id);
}

stacks::FunctionNameId Database::AddString(const std::string& str)
{
    return AddFunctionName(str);
}

uint64_t Database::ApproxExecutionSize(stacks::FunctionNameId executionNameId) const
{
    Key startKey;
    startKey.type = kExecutionKeyType;
    startKey.id = executionNameId;
    startKey.timestamp = 0;
    auto startKeySlice = Slice(&startKey, sizeof(startKey));

    Key endKey;
    endKey.type = kExecutionKeyType;
    endKey.id = executionNameId;
    endKey.timestamp = -1;
    auto endKeySlice = Slice(&endKey, sizeof(endKey));

    // Find the key of the 100th execution.
    std::unique_ptr<leveldb::Iterator> it(
            _db->NewIterator(leveldb::ReadOptions()));

    size_t i = 0;
    for (it->Seek(startKeySlice);
            it->Valid() && _comparator->Compare(it->key(), endKeySlice) <= 0;
            it->Next())
    {
        ++i;
        if (i == 100)
            break;
    }

    // There is less than 100 executions: size cannot be approximated.
    if (!it->Valid())
        return 0;

    leveldb::Range range(startKeySlice, it->key());
    uint64_t approximativeSize = 0;
    _db->GetApproximateSizes(&range, 1, &approximativeSize);

    if (approximativeSize == 0)
        return 0;
    return approximativeSize / i;

}

}  // namespace db
}  // namespace tibee
