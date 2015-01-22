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
#include "symbols_blocks/FinstrumentSymbolsBlock.hpp"

#include "base/BindObject.hpp"
#include "notification/Token.hpp"
#include "trace/value/StructEventValue.hpp"

namespace tibee {
namespace symbols_blocks {

FinstrumentSymbolsBlock::FinstrumentSymbolsBlock()
{
}

FinstrumentSymbolsBlock::~FinstrumentSymbolsBlock()
{
}

void FinstrumentSymbolsBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    AddUstObserver(notificationCenter, notification::Token("ust_baddr_statedump:soinfo"),
                   base::BindObject(&FinstrumentSymbolsBlock::OnBaddr, this));
    AddUstObserver(notificationCenter, notification::Token("lttng_ust_cyg_profile:func_entry"),
                   base::BindObject(&FinstrumentSymbolsBlock::OnFuncEntry, this));
    AddUstObserver(notificationCenter, notification::Token("lttng_ust_cyg_profile:func_exit"),
                   base::BindObject(&FinstrumentSymbolsBlock::OnFuncExit, this));
    AddUstObserver(notificationCenter, notification::Token("chrome:func_entry"),
                   base::BindObject(&FinstrumentSymbolsBlock::OnFuncEntry, this));
    AddUstObserver(notificationCenter, notification::Token("chrome:func_exit"),
                   base::BindObject(&FinstrumentSymbolsBlock::OnFuncExit, this));
}

void FinstrumentSymbolsBlock::OnBaddr(const trace::EventValue& event)
{
    uint32_t pid = event.getStreamEventContext()->GetField("vpid")->AsUInteger();
    uint64_t baddr = event.getEventField("baddr")->AsULong();
    std::string sopath = event.getEventField("sopath")->AsString();
    uint64_t size = event.getEventField("size")->AsULong();

    if (baddr == 0x400000)
    {
        baddr = 0;
        size += 0x400000;
    }

    symbols::Image image;
    image.set_path(sopath);
    image.set_base_address(baddr);
    image.set_size(size);

    _imagesPerProcess[pid].push_back(image);
}

void FinstrumentSymbolsBlock::OnFuncEntry(const trace::EventValue& event)
{
    uint32_t tid = event.getStreamEventContext()->GetField("vtid")->AsUInteger();
    uint32_t pid = event.getStreamEventContext()->GetField("vpid")->AsUInteger();
    uint64_t addr = event.getEventField("addr")->AsULong();

    symbols::Symbol symbol;
    uint64_t offset = 0;
    if (!_symbols.LookupSymbol(addr, _imagesPerProcess[pid], &symbol, &offset))
        symbol.set_name("unknown");

    Stacks()->PushStack(tid, Quarks()->StrQuark(symbol.name()));
}

void FinstrumentSymbolsBlock::OnFuncExit(const trace::EventValue& event)
{
    uint32_t tid = event.getStreamEventContext()->GetField("vtid")->AsUInteger();
    Stacks()->PopStack(tid);
}

}  // namespace symbols_blocks
}  // namespace tibee
