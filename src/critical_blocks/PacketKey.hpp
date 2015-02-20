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
#ifndef TIBEE_CRITICAL_BLOCKS_PACKETKEY_HPP_
#define TIBEE_CRITICAL_BLOCKS_PACKETKEY_HPP_

#include <boost/functional/hash.hpp>
#include <functional>

#include "base/BasicTypes.hpp"

namespace tibee
{
namespace critical_blocks
{

class PacketKey
{
public:
    PacketKey() : _seq(0), _ack_seq(0), _flags(0) {}
    PacketKey(uint64_t seq, uint64_t ack_seq, uint32_t flags)
        : _seq(seq), _ack_seq(ack_seq), _flags(flags) {}

    size_t hash() const {
        std::size_t seed = 0;
        boost::hash_combine(seed, _seq);
        boost::hash_combine(seed, _ack_seq);
        boost::hash_combine(seed, _flags);
        return seed;
    }

    bool operator==(const PacketKey& other) const {
        return _seq == other._seq &&
               _ack_seq == other._ack_seq &&
               _flags == other._flags;
    }

    bool operator!=(const PacketKey& other) const {
        return _seq != other._seq ||
               _ack_seq != other._ack_seq ||
               _flags != other._flags;
    }

private:
    uint64_t _seq;
    uint64_t _ack_seq;
    uint32_t _flags;
};

}  // namespace tibee
}  // namespace critical_blocks

namespace std {

template <>
struct hash<tibee::critical_blocks::PacketKey> {
  size_t operator()(const tibee::critical_blocks::PacketKey& key) const {
    return key.hash();
  }
};

}  // namespace std

#endif  // TIBEE_CRITICAL_BLOCKS_PACKETKEY_HPP_
