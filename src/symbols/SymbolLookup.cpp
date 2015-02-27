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
#include "symbols/SymbolLookup.hpp"

#include <assert.h>
#include <libelf.h>
#include <stdlib.h>

#include "base/print.hpp"
#include "symbols/Elf.hpp"

namespace tibee {
namespace symbols {

SymbolLookup::SymbolLookup() {
    // Initialize libelf.
    if (elf_version(EV_CURRENT) == EV_NONE) {
        base::tberror() << "Unable to initialize libelf." << base::tbendl();
    }
}

SymbolLookup::~SymbolLookup() {
}

bool SymbolLookup::LookupSymbol(uint64_t address,
                                const ImageVector& images,
                                Symbol* symbol,
                                uint64_t* offset) {
  assert(symbol);
  assert(offset);

  // Find the image for the symbol.
  for (size_t i = 0; i < images.size(); ++i) {
    const Image& image = images[i];

    if (address >= image.base_address() &&
        address < image.base_address() + image.size()) {

      // Load a symbol cache for the image.
      auto image_cache_it = cache_.find(image.path());
      if (image_cache_it == cache_.end()) {
        if (!ReadImageSymbols(image, &cache_[image.path()])) {
            base::tberror() << "Unable to load symbols for " << image.path()
                            << base::tbendl();
            return false;
        }
        image_cache_it = cache_.find(image.path());
      }

      // Find the symbol in the cache.
      uint64_t relative_address = address - image.base_address() + image.offset();
      const auto& image_cache = image_cache_it->second;

      auto symbol_it = image_cache.upper_bound(relative_address);

      if (symbol_it == image_cache.begin()) {
        symbol->set_name(image.path() + "+" + std::to_string(relative_address));
        return true;
      }

      --symbol_it;
      *symbol = symbol_it->second;
      *offset = address - symbol->address() - image.base_address();

      return true;
    }
  }

  /*
  base::tberror() << "No image at address " << std::hex << address << std::dec
                  << base::tbendl();
  */
  return false;
}

}  // namespace symbols
}  // namespace tibee
