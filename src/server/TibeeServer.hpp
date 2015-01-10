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
#ifndef _TIBEE_SERVER_TIBEESERVER_HPP
#define _TIBEE_SERVER_TIBEESERVER_HPP

#include "server/Arguments.hpp"

namespace tibee
{
namespace server
{

/**
 * Tibee Server.
 *
 * @author Francois Doray
 */
class TibeeServer
{
public:
    /**
     * Instanciates a TibeeServer program.
     *
     * @param args Program arguments
     */
    TibeeServer(const Arguments& args);

    /**
     * Runs the server.
     *
     * @returns True if everything went fine
     */
    bool run();

private:
    void validateSaveArguments(const Arguments& args);

    bool _verbose;
};

}  // namespace server
}  // namespace tibee

#endif // _TIBEE_SERVER_TIBEESERVER_HPP
