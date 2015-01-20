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
#ifndef _TIBEE_SYMBOLS_SYMBOLSBLOCK_HPP
#define _TIBEE_SYMBOLS_SYMBOLSBLOCK_HPP

#include <unordered_map>

#include "execution_blocks/AbstractExecutionBlock.hpp"
#include "symbols/SymbolLookup.hpp"

namespace tibee {
namespace symbols_blocks {

class FinstrumentSymbolsBlock : public execution_blocks::AbstractExecutionBlock
{
public:
    FinstrumentSymbolsBlock();
    ~FinstrumentSymbolsBlock();

private:
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

    void OnBaddr(const trace::EventValue& event);
    void OnFuncEntry(const trace::EventValue& event);
    void OnFuncExit(const trace::EventValue& event);

    // Images per process.
    std::unordered_map<uint32_t, symbols::ImageVector> _imagesPerProcess;

    // Symbols.
    symbols::SymbolLookup _symbols;
};

}  // namespace execution_blocks
}  // namespace tibee

#endif // _TIBEE_SYMBOLS_SYMBOLSBLOCK_HPP