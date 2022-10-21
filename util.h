/*
// Copyright (c) 2017-2022 Intel Corporation
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
// Abstract: misc util functions and macros
*/

#ifndef __UTIL_H__
#define __UTIL_H__

#include <algorithm>
#include <gsl/span>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

typedef gsl::span<const uint8_t> cbspan;

#ifndef block_round
#define block_round(ODD, BLK)                                                  \
    ((ODD) + (((BLK) - ((ODD) & ((BLK)-1))) & ((BLK)-1)))
#endif

template <class TContainer>
static inline bool starts_with(const TContainer& haystack,
                               const TContainer& needle)
{
    return haystack.size() >= needle.size() &&
           std::equal(needle.begin(), needle.end(), haystack.begin());
}

static inline void split(const std::string& s, char delim,
                         std::vector<std::string>& result)
{
    std::string::size_type prev_pos = 0, pos = 0;
    do
    {
        pos = s.find(delim, pos);
        if (pos == std::string::npos)
        {
            result.emplace_back(std::move(s.substr(prev_pos)));
        }
        else
        {
            result.emplace_back(std::move(s.substr(prev_pos, pos - prev_pos)));
            prev_pos = ++pos;
        }
    } while (pos != std::string::npos);
}

static inline std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

static constexpr uint64_t prime = 0x100000001B3ull;
static constexpr uint64_t basis = 0xCBF29CE484222325ull;

static constexpr uint64_t const_hash(const char* str,
                                     uint64_t last_value = basis)
{
    return *str ? const_hash(str + 1, (*str ^ last_value) * prime) : last_value;
}

static inline uint64_t hash(const std::string& str)
{
    uint64_t ret{basis};

    for (auto i : str)
    {
        ret ^= static_cast<uint64_t>(i);
        ret *= prime;
    }

    return ret;
}

#endif /* __UTIL_H__ */
