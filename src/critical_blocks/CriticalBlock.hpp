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
#ifndef TIBEE_CRITICAL_BLOCKS_CRITICALBLOCK_HPP_
#define TIBEE_CRITICAL_BLOCKS_CRITICALBLOCK_HPP_

#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "critical/CriticalGraph.hpp"
#include "critical_blocks/PacketKey.hpp"
#include "build_blocks/AbstractBuildBlock.hpp"
#include "quark/Quark.hpp"
#include "state/CurrentState.hpp"

namespace tibee
{
namespace critical_blocks
{

/**
 * Critical block.
 *
 * @author Francois Doray
 */
class CriticalBlock : public build_blocks::AbstractBuildBlock
{
public:
    CriticalBlock();
    ~CriticalBlock();

    virtual void LoadServices(const block::ServiceList& serviceList) override;
    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    struct InterruptContext
    {
        InterruptContext() : type(critical::kUnknown), node(nullptr) {}
        InterruptContext(critical::CriticalEdgeType type,
                         critical::CriticalNode* node)
            : type(type), node(node) {}
        critical::CriticalEdgeType type;
        critical::CriticalNode* node;
    };

    void OnTTWU(const trace::EventValue& event);
    void OnIrqHandlerEntry(const trace::EventValue& event);
    void OnIrqHandlerExit(const trace::EventValue& event);
    void OnSoftIrqEntry(const trace::EventValue& event);
    void OnSoftIrqExit(const trace::EventValue& event);
    void OnHrtimerExpireEntry(const trace::EventValue& event);
    void OnHrtimerExpireExit(const trace::EventValue& event);
    void OnInetSockLocalIn(const trace::EventValue& event);
    void OnInetSockLocalOut(const trace::EventValue& event);
    void OnThreadStatus(uint32_t tid, const notification::Path& path, const value::Value* value);
    void OnThreadName(uint32_t tid, const notification::Path& path, const value::Value* value);

    void OnTTWUBetweenThreads(uint32_t source_tid, uint32_t target_tid);
    void OnWakeupFromInterrupt(InterruptContext* context, uint32_t target_tid);

    critical::CriticalNode* CutThread(thread_t tid, const char* msg);

    // Thread for an event.
    uint32_t ThreadForCPU(uint32_t cpu) const;

    // Stack of interrupt contexts per CPU.
    std::unordered_map<uint32_t, std::stack<InterruptContext>> _context;

    // Last state per thread.
    std::unordered_map<uint32_t, critical::CriticalEdgeType> _lastEdgeTypePerThread;

    // Nodes for network dependencies.
    std::unordered_map<PacketKey, critical::CriticalNode*> _networkNodes;

    // Disk journaling threads.
    std::unordered_set<thread_t> _diskThreads;

    // Constant quarks.
    quark::Quark Q_RUN_USERMODE;
    quark::Quark Q_RUN_SYSCALL;
    quark::Quark Q_WAIT_BLOCKED;
    quark::Quark Q_INTERRUPTED;
    quark::Quark Q_WAIT_FOR_CPU;
    quark::Quark Q_UNKNOWN;
};

}  // namespace tibee
}  // namespace critical_blocks

#endif  // TIBEE_CRITICAL_BLOCKS_CRITICALBLOCK_HPP_
