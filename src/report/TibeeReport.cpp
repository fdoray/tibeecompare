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
#include "report/TibeeReport.hpp"

#include <fstream>

#include "base/CompareConstants.hpp"
#include "base/Constants.hpp"
#include "base/print.hpp"
#include "execution/ExecutionsDb.hpp"

#define THIS_MODULE "tibeereport"

namespace tibee
{
namespace report
{

namespace
{

using base::tbendl;
using base::tberror;
using base::tbmsg;

const char kIdHeader[] = "id";

void PrintHeader(
    std::ofstream& out,
    const std::vector<quark::Quark>& availableMetrics,
    quark::DiskQuarkDatabase* quarks)
{
    out << kIdHeader;
    if (!availableMetrics.empty())
        out << ",";

    for (size_t i = 0; i < availableMetrics.size(); ++i)
    {
        out << quarks->String(availableMetrics[i]);
        if (i != availableMetrics.size() - 1)
            out << ",";
    }
    out << std::endl;
}

void PrintExecution(
    std::ofstream& out,
    const std::vector<quark::Quark>& availableMetrics,
    const execution::Execution& execution)
{
    out << execution.id();
    if (!availableMetrics.empty())
        out << ",";

    for (size_t i = 0; i < availableMetrics.size(); ++i)
    {
        uint64_t value = 0;
        execution.GetMetric(availableMetrics[i], &value);
        out << value;
        if (i != availableMetrics.size() - 1)
            out << ",";
    }
    out << std::endl;   
}

}  // namespace

TibeeReport::TibeeReport(const Arguments& args)
{
    // validate arguments as soon as possible (will throw if anything wrong)
    this->validateSaveArguments(args);
}

void TibeeReport::validateSaveArguments(const Arguments& args)
{
    _args = args;
}

bool TibeeReport::run()
{
    if (_args.verbose)
        tbmsg(THIS_MODULE) << "starting" << tbendl();

    // Open output file.
    std::ofstream out(_args.file);

    // Connect to database.
    quark::DiskQuarkDatabase quarks(kDiskQuarkDatabaseFile);
    execution::ExecutionsDb executionsDb(&quarks);

    // Get available metrics.
    std::vector<quark::Quark> availableMetrics;
    if (!executionsDb.GetAvailableMetrics(_args.name, &availableMetrics))
    {
        tberror() << "Unable to get available metrics." << tbendl();
        return false;
    }

    // Print header.
    PrintHeader(out, availableMetrics, &quarks);

    // Read and print executions.
    executionsDb.EnumerateExecutions(
        _args.name,
        std::bind(&PrintExecution, std::ref(out), std::ref(availableMetrics),
                  std::placeholders::_1));

    return true;
}

}  // namespace report
}  // namespace tibee
