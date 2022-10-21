/*
// Copyright (c) 2015-2022 Intel Corporation
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
// Workfile:   FwUpdateDebug.c
// Abstract:   macros to wrap dbglog for simple FwUpdate debugging
*/

#ifndef __FW_UPDATE_DEBUG_H__
#define __FW_UPDATE_DEBUG_H__

#include <gsl/span>
#include <iostream>

typedef enum
{
    PRINT_NONE = 0,
    PRINT_CRITICAL = 1,
    PRINT_ERROR,
    PRINT_WARNING,
    PRINT_INFO,
    PRINT_DEBUG,
    PRINT_DEBUG2,
    PRINT_ALL,
} dbg_level;

#define FWCRITICAL(MSG) PRINT(PRINT_CRITICAL, MSG)
#define FWERROR(MSG) PRINT(PRINT_ERROR, MSG)
#define FWWARN(MSG) PRINT(PRINT_WARNING, MSG)
#define FWINFO(MSG) PRINT(PRINT_INFO, MSG)
#define FWDEBUG(MSG) PRINT(PRINT_DEBUG, MSG)
#define FWDEBUG2(MSG) PRINT(PRINT_DEBUG2, MSG)

#define FWDUMP(D, L) DUMP(PRINT_DEBUG, D, L)

#define FW_UPDATE_DEBUG 1

#ifdef FW_UPDATE_DEBUG

extern dbg_level fw_update_get_dbg_level(void);
extern void fw_update_set_dbg_level(dbg_level l);

#define PRINT(LEVEL, MSG)                                                      \
    do                                                                         \
    {                                                                          \
        if ((LEVEL) <= fw_update_get_dbg_level())                              \
        {                                                                      \
            std::stringstream ss;                                              \
            ss << '<' << LEVEL << '>' << __FUNCTION__ << ":" << __LINE__       \
               << ": " << MSG;                                                 \
            std::cerr << ss.str() << std::endl;                                \
        }                                                                      \
    } while (0)

void _dump(dbg_level lvl, const char* fn, int lineno, const char* bname,
           const gsl::span<const uint8_t>& buf);

void _dump(dbg_level lvl, const char* fn, int lineno, const char* bname,
           const void* buf, size_t len);

#define DUMP(LEVEL, BUF, ...)                                                  \
    do                                                                         \
    {                                                                          \
        if ((LEVEL) <= fw_update_get_dbg_level())                              \
        {                                                                      \
            _dump(LEVEL, __FUNCTION__, __LINE__, #BUF, BUF, ##__VA_ARGS__);    \
        }                                                                      \
    } while (0)

#else /* !FW_UPDATE_DEBUG */

#define PRINT(...)
#define DUMP(...)

#endif /* FW_UPDATE_DEBUG */

#endif /* __FW_UPDATE_DEBUG_H__ */
