/*
// Copyright (c) 2017-2025 Intel Corporation
//
// This software and the related documents are Intel copyrighted
// materials, and your use of them is governed by the express license
// under which they were provided to you ("License"). Unless the
// License provides otherwise, you may not use, modify, copy, publish,
// distribute, disclose or transmit this software or the related
// documents without Intel's prior written permission.
//
// This software and the related documents are provided as is, with no
// express or implied warranties, other than those that are expressly
// stated in the License.
//
// Abstract:   simple FwUpdate debugging
*/

#include "debug.h"

#include <cctype>
#include <cstdint>
#include <iomanip>
#include <iostream>

static dbg_level __dbg_level = PRINT_ERROR;
dbg_level fw_update_get_dbg_level(void)
{
    return __dbg_level;
}

void fw_update_set_dbg_level(dbg_level l)
{
    __dbg_level = l;
}

void _dump(dbg_level lvl, const char* fn, int lineno, const char* bname,
           const std::span<const uint8_t>& buf)
{
    unsigned int i = 0, l;
    std::stringstream hex, ascii;
    auto cb = buf.begin();

    /*  0         1         2         3         4         5         6
     *  0123456789012345678901234567890123456789012345678901234567890123456789
     *  0000000: 0000 0000 0000 0000 0000 0000 0000 0000  ................
     */

    std::cerr << '<' << lvl << '>' << fn << ":" << lineno << ": dumping "
              << buf.size() << " bytes from " << (void*)buf.data() << " ("
              << bname << ")" << std::endl;
    hex << std::hex << std::setfill('0');
    while (cb != buf.end())
    {
        hex.str("");
        ascii.str("");
        hex << std::setw(7) << i << ": ";
        for (l = 0; l < 16 && (cb != buf.end()); l++)
        {
            hex << std::setw(2) << (unsigned int)*cb;
            if (l & 1)
                hex << ' ';
            if (isprint(*cb))
                ascii << (char)(*cb);
            else
                ascii << '.';
            cb++;
            i++;
        }
        std::cerr << '<' << lvl << '>' << std::left << std::setw(50)
                  << hex.str() << ascii.str() << std::endl;
    }
    std::cerr << std::setw(0);
}

void _dump(dbg_level lvl, const char* fn, int lineno, const char* bname,
           const void* buf, size_t len)
{
    const uint8_t* ubuf = (const uint8_t*)buf;
    const std::span<const uint8_t> bspan{ubuf, ubuf + len};
    _dump(lvl, fn, lineno, bname, bspan);
}
