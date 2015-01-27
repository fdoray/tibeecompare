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
#ifndef _TIBEE_EXECUTION_EXECUTIONSDB_HPP
#define _TIBEE_EXECUTION_EXECUTIONSDB_HPP

#include <functional>
#include <mongo/client/dbclient.h>
#include <mongo/client/dbclientinterface.h>
#include <string>
#include <vector>

#include "execution/Execution.hpp"
#include "execution/ExecutionId.hpp"
#include "quark/DiskQuarkDatabase.hpp"
#include "quark/Quark.hpp"

namespace tibee
{
namespace execution
{

class ExecutionsDb
{
public:
    typedef std::function<void (const execution::Execution&)>
        EnumerateExecutionsCallback;

    ExecutionsDb(quark::DiskQuarkDatabase* quarks);
    ~ExecutionsDb();

    bool InsertExecution(const execution::Execution& execution,
                         ExecutionId* executionId);

    bool ReadExecution(const ExecutionId& executionId,
                       execution::Execution* execution);

    bool EnumerateExecutions(const std::string& name,
                             const EnumerateExecutionsCallback& callback);

    bool GetAvailableMetrics(const std::string& name,
                             std::vector<quark::Quark>* metrics);

private:
    bool UpdateAvailableMetrics(const execution::Execution& execution);

    bool IsConnected();
    bool Connect();

    // MongoDB connection.
    mongo::DBClientConnection _connection;

    // Indicates whether the Mongo DB connection is active.
    bool _isConnected;

    // Quarks.
    quark::DiskQuarkDatabase* _quarks;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_EXECUTIONSDB_HPP
