/* Copyright (c) 2015 Francois Doray <francois.pierre-doray@polymtl.ca>
 *
 * This file is part of tigerbeetle.
 *
 * tigerbeetle is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tigerbeetle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _TIBEE_STACKSBLOCKS_PROFILERBLOCK_HPP
#define _TIBEE_STACKSBLOCKS_PROFILERBLOCK_HPP

#include <string>
#include <vector>

#include "base/BasicTypes.hpp"
#include "build_blocks/AbstractBuildBlock.hpp"
#include "symbols/SymbolLookup.hpp"
#include "trace/value/EventValue.hpp"

namespace tibee {
namespace stacks_blocks {

/**
 * Blocks that handles lttng-profile events.
 *
 * @author Francois Doray
 */
class ProfilerBlock : public build_blocks::AbstractBuildBlock
{
public:
    ProfilerBlock();
    ~ProfilerBlock();

    virtual void Start(const value::Value* params) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void OnBaddr(const trace::EventValue& event);
    void OnOnCpuSample(const trace::EventValue& event);
    void OnOffCpuSample(const trace::EventValue& event);
    void OnSyscallEntry(const trace::EventValue& event);
    void OnSyscallExit(const trace::EventValue& event);

    void ReadStack(const trace::EventValue& event, std::vector<std::string>* stack);

    // Dump stacks to std output.
    bool _dumpStacks;

    // Images loaded in each process.
    std::unordered_map<process_t, symbols::ImageVector> _images;

    // Modules that resolves symbols.
    symbols::SymbolLookup _symbols;
};

}  // namespace stacks_blocks
}  // namespace tibee

#endif // _TIBEE_STACKSBLOCKS_PROFILERBLOCK_HPP
