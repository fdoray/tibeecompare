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
#include "report/TibeeReport.hpp"

#include <boost/filesystem.hpp>

#include "base/print.hpp"

#define THIS_MODULE "tibeereport"

namespace bfs = boost::filesystem;

namespace tibee
{
namespace report
{

namespace
{

using base::tbendl;
using base::tberror;
using base::tbmsg;

}  // namespace

TibeeReport::TibeeReport(const Arguments& args)
{
    // validate arguments as soon as possible (will throw if anything wrong)
    this->validateSaveArguments(args);
}

void TibeeReport::validateSaveArguments(const Arguments& args)
{
    // name
    _name = args.name;

    // verbose
    _verbose = args.verbose;
}

bool TibeeReport::run()
{
    if (_verbose)
        tbmsg(THIS_MODULE) << "starting" << tbendl();

    return true;
}

}  // namespace report
}  // namespace tibee
