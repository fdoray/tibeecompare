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
#ifndef _TIBEECOMPARE_TIBEECOMPARE_HPP
#define _TIBEECOMPARE_TIBEECOMPARE_HPP

#include <boost/filesystem/path.hpp>
#include <string>
#include <vector>

#include "Arguments.hpp"

namespace tibeecompare
{

/**
 * Tibee Compare.
 *
 * @author Francois Doray
 */
class TibeeCompare
{
public:
    /**
     * Instanciates a TibeeCompare program.
     *
     * @param args Program arguments
     */
    TibeeCompare(const Arguments& args);

    /**
     * Runs the comparison.
     *
     * @returns True if everything went fine
     */
    bool run();

private:
    void validateSaveArguments(const Arguments& args);

    std::vector<boost::filesystem::path> _traces;
    bool _verbose;
};

}  // namespace tibeecompare

#endif // _TIBEECOMPARE_TIBEECOMPARE_HPP
