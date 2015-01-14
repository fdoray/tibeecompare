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
#ifndef _TIBEE_REPORT_TIBEEREPORT_HPP
#define _TIBEE_REPORT_TIBEEREPORT_HPP

#include "report/Arguments.hpp"

namespace tibee
{
namespace report
{

/**
 * Tibee Report.
 *
 * @author Francois Doray
 */
class TibeeReport
{
public:
    /**
     * Instanciates a TibeeReport program.
     *
     * @param args Program arguments
     */
    TibeeReport(const Arguments& args);

    /**
     * Runs the program.
     *
     * @returns True if everything went fine
     */
    bool run();

private:
    void validateSaveArguments(const Arguments& args);

    Arguments _args;
};

}  // namespace report
}  // namespace tibee

#endif // _TIBEE_REPORT_TIBEEBUILD_HPP
