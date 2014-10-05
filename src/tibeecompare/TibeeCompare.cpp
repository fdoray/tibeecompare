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
#include "TibeeCompare.hpp"

#include <analysis_blocks/LinuxGraphBuilderBlock.hpp>
#include <base/print.hpp>
#include <block/BlockRunner.hpp>
#include <boost/filesystem.hpp>
#include <sstream>
#include <state_blocks/CurrentStateBlock.hpp>
#include <state_blocks/LinuxSchedStateBlock.hpp>
#include <trace_blocks/TraceBlock.hpp>
#include <value/MakeValue.hpp>
#include <value/Value.hpp>

#include "tibeecompare/ex/InvalidArgument.hpp"

#define THIS_MODULE "tibeecompare"

namespace bfs = boost::filesystem;

namespace tibeecompare
{

using tibee::base::tbmsg;
using tibee::base::tbendl;

TibeeCompare::TibeeCompare(const Arguments& args)
{
    // validate arguments as soon as possible (will throw if anything wrong)
    this->validateSaveArguments(args);
}

void TibeeCompare::validateSaveArguments(const Arguments& args)
{
    // make sure all traces actually exist
    for (const auto& pathStr : args.traces) {
        auto tracePath = bfs::path {pathStr};

        // make sure this trace exists (at least, may still be invalid)
        if (!bfs::exists(tracePath)) {
            std::stringstream ss;

            ss << "trace " << tracePath << " does not exist";

            throw ex::InvalidArgument {ss.str()};
        }

        // append trace path
        _traces.push_back(tracePath);
    }

    // verbose
    _verbose = args.verbose;
}

bool TibeeCompare::run()
{
    if (_verbose) {
        tbmsg(THIS_MODULE) << "starting" << tbendl();
    }

    // Trace block params.
    tibee::value::ArrayValue::UP traces {new tibee::value::ArrayValue};

    for (const auto& tracePath : _traces)
        traces->Append(tibee::value::MakeValue(tracePath.string()));

    tibee::value::StructValue traceBlockParams;
    traceBlockParams.AddField("traces", std::move(traces));

    // Graph builder block params.
    tibee::value::ArrayValue graphBuilderBlockParams;
    graphBuilderBlockParams.Append(tibee::value::MakeValue("wk-tasks"));

    // Instantiate blocks.
    tibee::trace_blocks::TraceBlock traceBlock;
    tibee::state_blocks::CurrentStateBlock currentStateBlock;
    tibee::state_blocks::LinuxSchedStateBlock linuxSchedStateBlock;
    tibee::analysis_blocks::LinuxGraphBuilderBlock linuxGraphBuilderBlock;

    // Run the analysis.
    tibee::block::BlockRunner blockRunner;
    blockRunner.AddBlock(&traceBlock, &traceBlockParams);
    blockRunner.AddBlock(&currentStateBlock, nullptr);
    blockRunner.AddBlock(&linuxSchedStateBlock, nullptr);
    blockRunner.AddBlock(&linuxGraphBuilderBlock, &graphBuilderBlockParams);
    blockRunner.Run();

    return true;
}

}  // namespace tibeecompare
