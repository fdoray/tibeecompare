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

#include <bfd.h>
#include <cxxabi.h>

#include <assert.h>
#include <iostream>
#include <stdlib.h>

namespace tibee {
namespace symbols {

namespace {

const size_t kElfSymbolSizeOffset = 7;

std::string Demangle(const char* name)
{
  int status = 0;    
  char* ret = abi::__cxa_demangle(name, 0, 0, &status); 
  std::string demangled;
  if (ret != nullptr)
    demangled = ret;
  else
    demangled = name;
  free(ret);
  return demangled;
}

bool AddSymbolToCache(bfd* file,
                      asymbol* symbol,
                      SymbolLookup::ImageSymbolCache* cache) {
  symbol_info symbol_info;
  bfd_get_symbol_info(file, symbol, &symbol_info);

  Symbol new_symbol;
  new_symbol.set_address(symbol_info.value);
  new_symbol.set_size(0);
  new_symbol.set_type(symbol_info.type);
  new_symbol.set_name(Demangle(symbol_info.name));

  if (bfd_get_flavour(file) == bfd_target_elf_flavour) {
    new_symbol.set_size(
        reinterpret_cast<uint64_t*>(symbol)[kElfSymbolSizeOffset]);
  }

  (*cache)[new_symbol.address()] = new_symbol;

  return true;
}

bool AddSymbolsToCache(bfd* file,
                       void* minisyms,
                       size_t num_symbols,
                       unsigned int size,
                       SymbolLookup::ImageSymbolCache* cache) {
  asymbol* store = bfd_make_empty_symbol(file);
  if (store == NULL) {
    return false;
  }

  bfd_byte* current_minisym = reinterpret_cast<bfd_byte*>(minisyms);
  for (size_t i = 0; i < num_symbols; ++i, current_minisym += size) {
    asymbol* symbol = bfd_minisymbol_to_symbol(
        file, false, current_minisym, store);
    if (symbol == NULL) {
      return false;
    }

    if (!AddSymbolToCache(file, symbol, cache)) {
      return false;
    }
  }

  return true;
}

bool AddRelFileToCache(bfd* file,
                       SymbolLookup::ImageSymbolCache* cache) {
  assert(file);

  unsigned int size = 0;
  void* minisyms = NULL;
  long num_symbols = bfd_read_minisymbols(file,
                                          false,  // dynamic
                                          &minisyms,
                                          &size);
  if (num_symbols < 0 && bfd_get_error() != bfd_error_no_symbols) {
    if (bfd_get_error() == bfd_error_no_symbols) {
      return true;
    } else {
      return false;
    }
  }

  bool res = AddSymbolsToCache(file,
                               minisyms,
                               static_cast<size_t>(num_symbols),
                               size,
                               cache);

  free(minisyms);

  return res;
}

bool AddFileToCache(const std::string& filename,
                    SymbolLookup::ImageSymbolCache* cache) {
  assert(cache);

  bfd *file = bfd_openr(filename.c_str(), NULL);
  if (file == NULL) {
    return false;
  }

  // Decompress sections to get line numbers.
  file->flags |= BFD_DECOMPRESS;

  char **matching = NULL;
  if (!bfd_check_format (file, bfd_archive) &&
      bfd_check_format_matches (file, bfd_object, &matching)) {
    AddRelFileToCache(file, cache);
  } else {
    // TODO(fdoray): Support archive files.
    return false;
  }

  if (!bfd_close(file))
    return false;

  return true;
}

}  // namespace

SymbolLookup::SymbolLookup() {
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
        if (!AddFileToCache(image.path(), &cache_[image.path()])) {
          std::cerr << "Unable to load symbols for " << image.path()
                    << std::endl;
          return false;
        }
        image_cache_it = cache_.find(image.path());
      }

      // Find the symbol in the cache.
      uint64_t relative_address = address - image.base_address();
      const auto& image_cache = image_cache_it->second;

      auto symbol_it = image_cache.upper_bound(relative_address);

      if (symbol_it == image_cache.begin()) {
        auto unknown_symbol_key = std::make_pair(image.path(), relative_address);
        auto look_unknown = unknown_symbols_.find(unknown_symbol_key);
        if (look_unknown == unknown_symbols_.end())
        {
          std::cerr << "No symbol for offset " << relative_address
                    << " in image " << image.path() << std::endl;
          unknown_symbols_.insert(unknown_symbol_key);
        }
        return false;
      }

      --symbol_it;
      *symbol = symbol_it->second;
      *offset = address - symbol->address() - image.base_address();

      return true;
    }
  }

  std::cerr << "No image at address " << std::hex << address << std::endl;
  return false;
}

}  // namespace symbols
}  // namespace tibee
