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
#ifndef _TIBEE_SYMBOLS_SYMBOLLOOKUP_HPP_
#define _TIBEE_SYMBOLS_SYMBOLLOOKUP_HPP_

#include <map>
#include <string>
#include <unordered_map>
#include <set>

#include "symbols/Image.hpp"
#include "symbols/Symbol.hpp"

namespace tibee {
namespace symbols {

class SymbolLookup {
 public:
  typedef std::map<uint64_t, Symbol> ImageSymbolCache;
  typedef std::unordered_map<std::string, ImageSymbolCache> SymbolCache;

  SymbolLookup();
  ~SymbolLookup();

  bool LookupSymbol(uint64_t address,
                    const ImageVector& images,
                    Symbol* symbol,
                    uint64_t* offset);

 private:
  // Symbol cache.
  SymbolCache cache_;

  // Unknown symbols previously encountered.
  std::set<std::pair<std::string, uint64_t>> unknown_symbols_;
};

}  // namespace symbols
}  // namespace tibee

#endif  // _TIBEE_SYMBOLS_SYMBOL_LOOKUP_HPP_