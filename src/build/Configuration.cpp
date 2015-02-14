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
#include "build/Configuration.hpp"

#include <fstream>
#include <iostream>
#include <yaml-cpp/eventhandler.h>
#include <yaml-cpp/yaml.h>

#include "base/print.hpp"
#include "build/BlockLoader.hpp"
#include "value/MakeValue.hpp"

namespace tibee
{
namespace build
{

namespace
{

class ConfigurationDocumentHandler : public YAML::EventHandler
{
public:
    ConfigurationDocumentHandler(Configuration* configuration)
        : _configuration(configuration),
          _blockListDepth(0),
          _error(false)
    {
    }

    virtual void OnDocumentStart(const YAML::Mark& mark) override
    {
    }

    virtual void OnDocumentEnd() override
    {
    }

    virtual void OnNull(const YAML::Mark& mark, YAML::anchor_t anchor) override
    {
    }

    virtual void OnAlias(const YAML::Mark& mark, YAML::anchor_t anchor) override
    {
    }

    virtual void OnScalar(const YAML::Mark& mark,
                          const std::string& tag,
                          YAML::anchor_t anchor,
                          const std::string& value) override
    {
        _lastScalar = value;

        if (_blockListDepth == 1 || _blockListDepth == 2)
        {
            if (!_configuration->AddBlock(value)) {
                base::tberror() << "Unable to load block " << value << "."
                                << base::tbendl();
                _error = true;
            } else {
                _lastBlock = value;
            }
        }
        else if (_blockListDepth == 3)
        {
            if (_lastParam.empty()) {
                _lastParam = value;
            } else {
                _configuration->AddParameter(
                        _lastBlock, _lastParam, value::MakeValue(value));
                _lastParam.clear();
            }
        }
    }

    virtual void OnSequenceStart(const YAML::Mark& mark, const std::string& tag,
                                 YAML::anchor_t anchor) override
    {
        if (_lastScalar == "blocks")
            _blockListDepth = 1;
        else if (_blockListDepth != 0)
            ++_blockListDepth;
    }

    virtual void OnSequenceEnd() override
    {
        if (_blockListDepth != 0)
            --_blockListDepth;
    }

    virtual void OnMapStart(const YAML::Mark& mark, const std::string& tag,
                            YAML::anchor_t anchor) override
    {
        if (_blockListDepth != 0)
            ++_blockListDepth;
    }

    virtual void OnMapEnd() override
    {
        if (_blockListDepth != 0)
            --_blockListDepth;
    }

    bool HasError() const {
        return _error;
    }

private:
    // Configuration to build.
    Configuration* _configuration;

    // Value of the last encountered scalar.
    std::string _lastScalar;

    // Name of the last block created.
    std::string _lastBlock;

    // Name of the last parameter encoutered.
    std::string _lastParam;

    // Indicates whether we are in a list named "blocks".
    size_t _blockListDepth;

    // Indicates whether an error has been encountered.
    bool _error;
};

}  // namespace


Configuration::Configuration()
{
}

bool Configuration::LoadConfiguration(
        const boost::filesystem::path& configuration)
{
    std::ifstream file(configuration.string());

    YAML::Parser parser(file);
    ConfigurationDocumentHandler handler(this);
    parser.HandleNextDocument(handler);
    if (handler.HasError())
        return false;

    return true;
}

bool Configuration::AddBlock(const std::string& block)
{
    auto blockInst = LoadBlock(block);
    if (blockInst.get() == nullptr)
        return false;
    _parameters[block].reset(new value::StructValue);
    Runner().AddBlock(blockInst.get(), _parameters[block].get());
    _blocks.push_back(std::move(blockInst));
    return true;
}

bool Configuration::AddParameter(const std::string& block,
                                 const std::string& parameter,
                                 value::Value::UP value)
{
    auto it = _parameters.find(block);
    if (it == _parameters.end())
        return false;
    it->second->AddField(parameter, std::move(value));
    return true;
}

block::BlockRunner& Configuration::Runner()
{
    return _runner;
}

}  // namespace build
}  // namespace tibee
