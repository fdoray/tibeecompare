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
#include "build/TibeeBuild.hpp"

#include <boost/filesystem.hpp>
#include <sstream>

#include "base/CompareConstants.hpp"
#include "base/print.hpp"
#include "base/ex/InvalidArgument.hpp"
#include "block/BlockRunner.hpp"
#include "build/Configuration.hpp"
#include "value/MakeValue.hpp"
#include "value/Value.hpp"

#define THIS_MODULE "tibeebuild"

namespace bfs = boost::filesystem;

namespace tibee
{
namespace build
{

using base::tbendl;
using base::tberror;
using base::tbmsg;

TibeeBuild::TibeeBuild(const Arguments& args)
{
    // validate arguments as soon as possible (will throw if anything wrong)
    this->validateSaveArguments(args);
}

void TibeeBuild::validateSaveArguments(const Arguments& args)
{
    // configuration
    _configuration = args.configuration;

    // make sure all traces actually exist
    for (const auto& pathStr : args.traces) {
        auto tracePath = bfs::path {pathStr};

        // make sure this trace exists (at least, may still be invalid)
        if (!bfs::exists(tracePath)) {
            std::stringstream ss;

            ss << "trace " << tracePath << " does not exist";

            throw base::ex::InvalidArgument {ss.str()};
        }

        // append trace path
        _traces.push_back(tracePath);
    }

    // verbose
    _verbose = args.verbose;
}

bool TibeeBuild::run()
{
    if (_verbose)
        tbmsg(THIS_MODULE) << "starting" << tbendl();

    // Instantiate the required modules.
    Configuration configuration;
    if (!configuration.LoadConfiguration(_configuration)) {
        tberror() << "Unable to load configuration file." << tbendl();
        return false;
    }

    // Add the traces.
    value::ArrayValue::UP traces {new value::ArrayValue};

    for (const auto& tracePath : _traces)
        traces->Append(value::MakeValue(tracePath.string()));

    configuration.AddParameter(kTraceBlockName, "traces", std::move(traces));

    // Run the blocks.
    configuration.Runner().Run();

    return true;
}

}  // namespace build
}  // namespace tibee
