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
#include "execution/LinksBuilder.hpp"

#include <algorithm>

namespace tibee
{
namespace execution
{

namespace
{

class LinksComparator
{
public:
    bool operator() (const Link& link, timestamp_t ts) const
    {
        return link.sourceTs() < ts;
    }
};

}  // namespace

LinksBuilder::LinksBuilder()
{
}

LinksBuilder::~LinksBuilder()
{
}

void LinksBuilder::AddLink(const Link& link)
{
    _links.push_back(link);
}

void LinksBuilder::AddLink(thread_t source, thread_t target)
{
    AddLink(Link(source, _ts, target, _ts));
}

void LinksBuilder::EnumerateLinks(timestamp_t start,
                                  timestamp_t end,
                                  const EnumerateLinksCallback& callback) const
{
    LinksComparator comparator;
    auto it = std::lower_bound(
        _links.begin(), _links.end(), start, comparator);

    for (; it != _links.end(); ++it)
    {
        if (it->sourceTs() > end)
            return;
        callback(*it);
    }
}

}  // namespace execution
}  // namespace tibee
