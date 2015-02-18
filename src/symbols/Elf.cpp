/* Copyright (c) 2015 Francois Doray <francois.pierre-doray@polymtl.ca>
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
#include "symbols/Elf.hpp"

#include <cxxabi.h>
#include <dwarf.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/user.h>

namespace tibee
{
namespace
{

class ScopedFd
{
public:
    ScopedFd(int fd) : _fd(fd) {}
    ~ScopedFd() {
        if (_fd >= 0)
            close(_fd);
    }
    int operator*() const {
        return _fd;
    }

private:
    int _fd;
};

class ScopedElf
{
public:
    ScopedElf(Elf* elf) : _elf(elf) {}
    ~ScopedElf() {
        if (_elf != nullptr)
            elf_end(_elf);
    }
    Elf* operator*() const {
        return _elf;
    }

private:
    Elf* _elf;
};

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

}  // namespace

bool ReadImageSymbols(const symbols::Image& image,
                      symbols::SymbolLookup::ImageSymbolCache* cache)
{
    Elf_Scn *scn = NULL;
    size_t pnum, i;
    uint64_t vaddr = 0;

    // Open image.
    ScopedFd fd(open(image.path().c_str(), O_RDONLY));
    if (*fd < 0)
        return false;

    // Open ELF handle.
    ScopedElf elf(elf_begin(*fd, ELF_C_READ, NULL));
    if (*elf == NULL)
        return false;

    if (elf_getphdrnum (*elf, &pnum))
        return false;

    for (i = 0; i < pnum; ++i) {
        GElf_Phdr phdr;
        if (gelf_getphdr(*elf, i, &phdr) == NULL)
            return false;
        if (phdr.p_type != PT_LOAD)
            continue;
        if (phdr.p_flags != (PF_X | PF_R))
            continue;
        if ((phdr.p_offset & ~(phdr.p_align - 1)) != image.offset())
            continue;
        vaddr = phdr.p_vaddr;
        break;
    }

    /*
     * search symtab or dynsym section
     */
    while ((scn = elf_nextscn(*elf, scn)) != NULL) {
        GElf_Shdr shdr;

        if (gelf_getshdr(scn, &shdr) == NULL) {
            fprintf(stderr, "elf_nextscn: %s\n", elf_errmsg(elf_errno()));
            return false;
        }

        if (shdr.sh_type == SHT_DYNSYM || shdr.sh_type == SHT_SYMTAB) {
            Elf_Data *data = NULL;
            int symbol_count;

            if ((data = elf_getdata(scn, data)) == NULL) {
                fprintf(stderr, "elf_getdata: %s\n", elf_errmsg(elf_errno()));
                return false;
            }

            symbol_count = shdr.sh_size / shdr.sh_entsize;
            for (i = 0; i < (size_t)symbol_count; ++i) {
                GElf_Sym s;

                if (gelf_getsym(data, i, &s) == NULL) {
                    fprintf(stderr, "elf_getsym: %s\n",
                            elf_errmsg(elf_errno()));
                    return false;
                }

                if (ELF64_ST_TYPE(s.st_info) != STT_FUNC)
                    continue;

                /*
                 * adjust symbol value
                 */
                s.st_value -= vaddr;

                /*
                 * add symbol to cache
                 */
                symbols::Symbol symbol;
                char* str = elf_strptr(*elf, shdr.sh_link, s.st_name);
                if (str == NULL)
                {
                    fprintf(stderr, "elf_strptr #1: %s\n",
                            elf_errmsg(elf_errno()));
                    return false;
                }
                symbol.set_name(Demangle(str));
                symbol.set_address(s.st_value);
                symbol.set_size(s.st_size);

                (*cache)[symbol.address()] = symbol;
            }
        }
    }

    return true;
}

}  // namespace tibee

