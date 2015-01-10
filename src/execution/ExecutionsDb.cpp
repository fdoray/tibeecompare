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
#include "execution/ExecutionsDb.hpp"

#include "base/print.hpp"

namespace tibee
{
namespace execution
{

namespace
{

using base::tberror;
using base::tbendl;

const char kMongoHost[] = "localhost:27017";

const char kExecutionsCollection[] = "tibeecompare.executions";
const char kIdField[] = "_id";
const char kNameField[] = "desc";
const char kTraceField[] = "trace";
const char kStartTsField[] = "startts";
const char kStartThreadField[] = "startthread";
const char kEndTsField[] = "endts";
const char kEndThreadField[] = "endthread";
const char kMetricsField[] = "metrics";

}  // namespace

ExecutionsDb::ExecutionsDb(quark::DiskQuarkDatabase* quarks)
    : _connection(true),
      _isConnected(false),
      _quarks(quarks)
{
}

ExecutionsDb::~ExecutionsDb()
{
}

bool ExecutionsDb::InsertExecution(const execution::Execution& execution,
                                   ExecutionId* executionId)
{
    assert(executionId != nullptr);

    if (!Connect())
        return false;

    // Write metadata.
    mongo::BSONObjBuilder bson_properties;
    bson_properties.genOID();
    bson_properties.append(kNameField, execution.name());
    bson_properties.append(kTraceField, execution.trace());
    bson_properties.append(kStartTsField, static_cast<long long>(execution.startTs()));
    bson_properties.append(kStartThreadField, static_cast<int>(execution.startThread()));
    bson_properties.append(kEndTsField, static_cast<long long>(execution.endTs()));
    bson_properties.append(kEndThreadField, static_cast<int>(execution.endThread()));

    // Write metrics.
    mongo::BSONObjBuilder bson_metrics;
    for (auto it = execution.metrics_begin(); it != execution.metrics_end(); ++it)
    {
        std::string metricName = _quarks->String(it->first);
        bson_metrics.append(metricName, static_cast<long long>(it->second));
    }
    bson_properties.append(kMetricsField, bson_metrics.obj());

    // Insert in database.
    auto bson_properties_obj = bson_properties.obj();
    *executionId = bson_properties_obj.getField(kIdField).OID();
    _connection.insert(kExecutionsCollection, bson_properties_obj);

    return true;
}

bool ExecutionsDb::UpdateAvailableMetrics(const execution::Execution& execution)
{
    return true;
}

bool ExecutionsDb::IsConnected()
{
    return _isConnected && _connection.isStillConnected();
}

bool ExecutionsDb::Connect()
{
    // Open Mongo connection.
    if (IsConnected())
        return true;

    std::string errMsg;
    if (!_connection.connect(kMongoHost, errMsg))
    {
        tberror() << "Error while connecting to mongodb: " << errMsg << tbendl();
        return false;
    }

    _isConnected = true;

    return true;
}

}  // namespace execution
}  // namespace tibee
