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
#ifndef _TIBEE_SYMBOLS_IMAGE_HPP_
#define _TIBEE_SYMBOLS_IMAGE_HPP_

#include <stdint.h>
#include <string>
#include <vector>

namespace tibee {
namespace symbols {

class Image {
 public:
  Image() : base_address_(0), size_(0) {}
  ~Image() {}

  const std::string& path() const { return path_; }
  void set_path(const std::string& path) { path_ = path; }

  uint64_t base_address() const { return base_address_; }
  void set_base_address(uint64_t base_address) { base_address_ = base_address; }

  uint64_t size() const { return size_; }
  void set_size(uint64_t size) { size_ = size; }

 private:
  std::string path_;
  uint64_t base_address_;
  uint64_t size_;
};

typedef std::vector<Image> ImageVector;

}  // namespace symbols
}  // namespace tibee

#endif  // _TIBEE_SYMBOLS_IMAGE_HPP_
