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
#ifndef _TIBEE_BUILD_CONFIGURATION_HPP
#define _TIBEE_BUILD_CONFIGURATION_HPP

#include <boost/filesystem/path.hpp>
#include <unordered_map>
#include <vector>

#include "block/BlockRunner.hpp"

namespace tibee
{
namespace build
{

/**
 * Reads a configuration file.
 *
 * @author Francois Doray
 */
class Configuration
{
public:
    /**
     * Instanciates a configuration.
     */
    Configuration();

    /**
     * Loads a configuration file.
     */
    bool LoadConfiguration(const boost::filesystem::path& configuration);

    /**
     * Add a block.
     */
    bool AddBlock(const std::string& block);

    /**
     * Add a parameter to a block.
     */
    bool AddParameter(const std::string& block, const std::string& parameter, value::Value::UP value);

    /**
     * Returns the block runner associated with this configuration.
     */
    block::BlockRunner& Runner();

private:
    // Block runner.
    block::BlockRunner _runner;

    // Blocks.
    std::vector<block::BlockInterface::UP> _blocks;

    // Parameters.
    typedef std::unordered_map<std::string, value::StructValue::UP> Parameters;
    Parameters _parameters;
};

}  // namespace build
}  // namespace tibee

#endif // _TIBEE_BUILD_CONFIGURATION_HPP
