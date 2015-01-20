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
#ifndef _TIBEE_SYMBOLS_SYMBOL_HPP_
#define _TIBEE_SYMBOLS_SYMBOL_HPP_

#include <stdint.h>
#include <string>

namespace tibee {
namespace symbols {

class Symbol {
 public:
  Symbol() : address_(0), size_(0), type_(0) {}
  ~Symbol() {}

  uint64_t address() const { return address_; }
  void set_address(uint64_t address) { address_ = address; }

  uint64_t size() const { return size_; }
  void set_size(uint64_t size) { size_ = size; }

  char type() const { return type_; }
  void set_type(char type) { type_ = type; }

  const std::string& name() const { return name_; }
  void set_name(const std::string& name) { name_ = name; }

 private:
  uint64_t address_;
  uint64_t size_;
  char type_;
  std::string name_;
};

}  // namespace symbols
}  // namespace tibee

#endif  // _TIBEE_SYMBOLS_SYMBOL_HPP_
