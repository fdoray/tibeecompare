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

#include "base/print.hpp"
#include "base/ex/InvalidArgument.hpp"
#include "block/BlockRunner.hpp"
#include "value/MakeValue.hpp"
#include "value/Value.hpp"

// Blocks
#include "build_blocks/BuildBlock.hpp"
#include "critical_blocks/CriticalBlock.hpp"
#include "execution_blocks/PunchBlock.hpp"
#include "stacks_blocks/ProfilerBlock.hpp"
#include "state_blocks/CurrentStateBlock.hpp"
#include "state_blocks/LinuxSchedStateBlock.hpp"
#include "state_blocks/PerfCountersBlock.hpp"
#include "state_blocks/StateHistoryBlock.hpp"
#include "trace_blocks/TraceBlock.hpp"

#define THIS_MODULE "tibeebuild"

namespace bfs = boost::filesystem;

namespace tibee
{
namespace build
{

namespace
{

using base::tbendl;
using base::tberror;
using base::tbmsg;

void FindTraces(const bfs::path& parent,
                std::vector<bfs::path>* traces)
{
    bfs::path metadataFile = parent / "metadata";
    if (bfs::exists(metadataFile))
    {
        // |parent| is a trace directory
        traces->push_back(parent);
        return;
    }

    // Find traces in subdirectories.
    bfs::directory_iterator end;
    for (bfs::directory_iterator it(parent);
         it != end;
         ++it)
    {
        if (bfs::is_directory(it->status()))
            FindTraces(it->path(), traces);
    }
}

}  // namespace

TibeeBuild::TibeeBuild(const Arguments& args)
{
    // validate arguments as soon as possible (will throw if anything wrong)
    this->validateSaveArguments(args);
}

void TibeeBuild::validateSaveArguments(const Arguments& args)
{
    _args = args;

    // make sure all traces actually exist
    for (const auto& pathStr : args.traces)
    {
        auto tracePath = bfs::path {pathStr};

        // make sure this trace exists (at least, may still be invalid)
        if (!bfs::exists(tracePath))
        {
            std::stringstream ss;
            ss << "trace path " << tracePath << " does not exist";
            throw base::ex::InvalidArgument {ss.str()};
        }
        else if (!bfs::is_directory(tracePath))
        {
            std::stringstream ss;
            ss << "trace path " << tracePath << " is not a directory";
            throw base::ex::InvalidArgument {ss.str()};
        }

        // find directories that contain a metadata file
        FindTraces(tracePath, &_traces);
    }
}

bool TibeeBuild::run()
{
    if (_args.verbose)
        tbmsg(THIS_MODULE) << "starting" << tbendl();

    block::BlockRunner runner;

    // Trace block.
    value::ArrayValue::UP traces {new value::ArrayValue};
    for (const auto& tracePath : _traces)
        traces->Append(value::MakeValue(tracePath.string()));
    value::StructValue::UP traceParams {new value::StructValue};
    traceParams->AddField("traces", std::move(traces));
    block::BlockInterface::UP traceBlock(new trace_blocks::TraceBlock);
    runner.AddBlock(traceBlock.get(), traceParams.get());

    // Punch block.
    value::StructValue::UP punchParams;
    block::BlockInterface::UP punchBlock;
    if (!_args.dumpStacks)
    {
        punchParams.reset(new value::StructValue);
        punchParams->AddField("name", value::MakeValue(_args.name));
        punchParams->AddField("exec", value::MakeValue(_args.exec));
        punchParams->AddField("begin", value::MakeValue(_args.beginEvent));
        punchParams->AddField("end", value::MakeValue(_args.endEvent));
        punchParams->AddField("stats", value::MakeValue(_args.stats));
        punchBlock.reset(new execution_blocks::PunchBlock);
        runner.AddBlock(punchBlock.get(), punchParams.get());
    }

    // Current state block.
    block::BlockInterface::UP currentStateBlock(new state_blocks::CurrentStateBlock);
    runner.AddBlock(currentStateBlock.get(), nullptr);

    // Profiler block.
    value::StructValue::UP profilerParams {new value::StructValue};
    profilerParams->AddField("dump", value::MakeValue(_args.dumpStacks));
    block::BlockInterface::UP profilerBlock(new stacks_blocks::ProfilerBlock);
    runner.AddBlock(profilerBlock.get(), profilerParams.get());

    // Critical block.
    block::BlockInterface::UP criticalBlock;
    if (!_args.dumpStacks && !_args.stats)
    {
        criticalBlock.reset(new critical_blocks::CriticalBlock);
        runner.AddBlock(criticalBlock.get(), nullptr);
    }

    // Linux sched state block.
    block::BlockInterface::UP linuxSchedStateBlock(new state_blocks::LinuxSchedStateBlock);
    runner.AddBlock(linuxSchedStateBlock.get(), nullptr);

    // State history block.
    block::BlockInterface::UP stateHistoryBlock;
    if (!_args.dumpStacks && !_args.stats)
    {
        stateHistoryBlock.reset(new state_blocks::StateHistoryBlock);
        runner.AddBlock(stateHistoryBlock.get(), nullptr);
    }

    // Perf counters block.
    block::BlockInterface::UP perfCountersBlock;
    if (!_args.dumpStacks && !_args.stats)
    {
        perfCountersBlock.reset(new state_blocks::PerfCountersBlock);
        runner.AddBlock(perfCountersBlock.get(), nullptr);
    }

    // Build block.
    block::BlockInterface::UP buildBlock(new build_blocks::BuildBlock(_args.stats));
    runner.AddBlock(buildBlock.get(), nullptr);

    // Run the blocks.
    runner.Run();

    if (_args.verbose)
        tbmsg(THIS_MODULE) << "ending" << tbendl();

    return true;
}

}  // namespace build
}  // namespace tibee
