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
#include "stacks_blocks/ProfilerBlock.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "base/BindObject.hpp"
#include "value/Value.hpp"

namespace tibee {
namespace stacks_blocks {

namespace
{

using notification::RegexToken;
using notification::Token;

const char kSyscallWithParamsPrefix[] = "syscall_entry_";

}  // namespace

ProfilerBlock::ProfilerBlock()
    : _dumpStacks(false)
{
}

ProfilerBlock::~ProfilerBlock()
{
}

void ProfilerBlock::Start(const value::Value* params)
{
    _dumpStacks = value::BoolValue::GetValue(params->GetField("dump"));
}

void ProfilerBlock::AddObservers(
    notification::NotificationCenter* notificationCenter)
{
    AddUstObserver(notificationCenter, 
                   Token("ust_baddr_statedump:soinfo"),
                   base::BindObject(&ProfilerBlock::OnBaddr, this));
    AddUstObserver(notificationCenter,
                   Token("lttng_profile:on_cpu_sample"),
                   base::BindObject(&ProfilerBlock::OnOnCpuSample, this));
    AddUstObserver(notificationCenter,
                   Token("lttng_profile:off_cpu_sample"),
                   base::BindObject(&ProfilerBlock::OnOffCpuSample, this));

    AddKernelObserver(notificationCenter,
                      RegexToken("^sys_"),
                      base::BindObject(&ProfilerBlock::OnSyscallEntry, this));
    AddKernelObserver(notificationCenter,
                      RegexToken("^compat_sys_"),
                      base::BindObject(&ProfilerBlock::OnSyscallEntry, this));
    AddKernelObserver(notificationCenter,
                      RegexToken("^syscall_entry_"),
                      base::BindObject(&ProfilerBlock::OnSyscallEntry, this));
    AddKernelObserver(notificationCenter,
                      RegexToken("^syscall_exit_"),
                      base::BindObject(&ProfilerBlock::OnSyscallExit, this));
    AddKernelObserver(notificationCenter,
                      RegexToken("^compat_syscall_exit_"),
                      base::BindObject(&ProfilerBlock::OnSyscallExit, this));
    AddKernelObserver(notificationCenter,
                      Token("exit_syscall"),
                      base::BindObject(&ProfilerBlock::OnSyscallExit, this));
}

void ProfilerBlock::OnBaddr(const trace::EventValue& event)
{
    process_t pid = ProcessForEvent(event);
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

    _images[pid].push_back(image);
}

void ProfilerBlock::OnOnCpuSample(const trace::EventValue& event)
{
    thread_t tid = ThreadForEvent(event);
    std::vector<std::string> stack;
    ReadStack(event, &stack);

    Stacks()->SetStack(tid, stack);
}

void ProfilerBlock::OnOffCpuSample(const trace::EventValue& event)
{
    thread_t tid = ThreadForEvent(event);
    std::vector<std::string> stack;
    ReadStack(event, &stack);

    Stacks()->SetLastSystemCallStack(tid, stack);
}

void ProfilerBlock::OnSyscallEntry(const trace::EventValue& event)
{
    thread_t tid = ThreadForEvent(event);

    std::string syscall = event.getName();
    if (syscall.find(kSyscallWithParamsPrefix) == 0)
        syscall = syscall.substr(strlen(kSyscallWithParamsPrefix));

    Stacks()->StartSystemCall(tid, syscall);
}

void ProfilerBlock::OnSyscallExit(const trace::EventValue& event)
{
    thread_t tid = ThreadForEvent(event);
    Stacks()->EndSytemCall(tid);
}

void ProfilerBlock::ReadStack(const trace::EventValue& event,
                              std::vector<std::string>* stack)
{
    process_t pid = ProcessForEvent(event);

    std::vector<std::string> symbolizedStack;
    const auto* stackField = value::ArrayValue::Cast(event.getEventField("stack"));

    for (const auto& addressValue : *stackField)
    {
        uint64_t address = addressValue.AsULong();

        symbols::Symbol symbol;
        uint64_t offset = 0;
        if (!_symbols.LookupSymbol(address, _images[pid], &symbol, &offset))
            symbol.set_name("Unknown Symbol");
        if (boost::starts_with(symbol.name(), "lttng_profile"))
            continue;

        stack->push_back(symbol.name());

        if (_dumpStacks)
            std::cout << symbol.name() << std::endl;
    }

    if (_dumpStacks)
        std::cout << std::endl;
}

}  // namespace stacks_blocks
}  // namespace tibee
