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
#ifndef _TIBEE_EXECUTION_LINKSBUILDER_HPP
#define _TIBEE_EXECUTION_LINKSBUILDER_HPP

#include <functional>

#include "execution/Link.hpp"

namespace tibee
{
namespace execution
{

class LinksBuilder
{
public:
    typedef std::function<void (const Link&)> EnumerateLinksCallback;

    LinksBuilder();
    ~LinksBuilder();

    // Set current timestamp.
    void SetTimestamp(timestamp_t ts) { _ts = ts; }

    // Add a link.
    void AddLink(const Link& link);
    void AddLink(thread_t source, thread_t target);

    // Enumerate links that start whithin the specified interval.
    void EnumerateLinks(timestamp_t start,
                        timestamp_t end,
                        const EnumerateLinksCallback& callback) const;

private:
    // Current timestamp.
    timestamp_t _ts;

    // Links.
    Links _links;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_LINKSBUILDER_HPP
