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
#ifndef _TIBEE_SYMBOLSBLOCK_DUMPSTACKSBLOCK_HPP
#define _TIBEE_SYMBOLSBLOCK_DUMPSTACKSBLOCK_HPP

#include "base/BasicTypes.hpp"
#include "execution_blocks/AbstractExecutionBlock.hpp"
#include "symbols/SymbolLookup.hpp"
#include "trace/value/EventValue.hpp"

namespace tibee {
namespace symbols_blocks {

/**
 * Blocks that prints stacks in the std output.
 *
 * @author Francois Doray
 */
class DumpStacksBlock : public execution_blocks::AbstractExecutionBlock
{
public:
    DumpStacksBlock();
    ~DumpStacksBlock();

    virtual void AddObservers(
        notification::NotificationCenter* notificationCenter) override;

private:
    void OnBaddr(const trace::EventValue& event);
    void OnSample(const trace::EventValue& event);

    // Images loaded in each process.
    std::unordered_map<process_t, symbols::ImageVector> _images;

    // Modules that resolves symbols.
    symbols::SymbolLookup _symbols;
};

}  // namespace symbols_blocks
}  // namespace tibee

#endif // _TIBEE_SYMBOLSBLOCK_DUMPSTACKSBLOCK_HPP
