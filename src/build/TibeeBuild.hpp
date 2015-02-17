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
#ifndef _TIBEE_BUILD_TIBEEBUILD_HPP
#define _TIBEE_BUILD_TIBEEBUILD_HPP

#include <boost/filesystem/path.hpp>
#include <string>
#include <vector>

#include "build/Arguments.hpp"

namespace tibee
{
namespace build
{

/**
 * Tibee Build.
 *
 * @author Francois Doray
 */
class TibeeBuild
{
public:
    /**
     * Instanciates a TibeeBuild program.
     *
     * @param args Program arguments
     */
    TibeeBuild(const Arguments& args);

    /**
     * Runs the builder.
     *
     * @returns True if everything went fine
     */
    bool run();

private:
    void validateSaveArguments(const Arguments& args);

    // Arguments.
    Arguments _args;
    std::vector<boost::filesystem::path> _traces;
};

}  // namespace build
}  // namespace tibee

#endif // _TIBEE_BUILD_TIBEEBUILD_HPP
