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
// Abstract: MTD interface
*/

#ifndef __CPP_MTD_H__
#define __CPP_MTD_H__

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "util.h" // cbspan type

#define BIG_BLOCK_SIZE (64 * 1024)
#define SMALL_BLOCK_SIZE (4 * 1024)
#define BIG_BLOCK_MASK (BIG_BLOCK_SIZE - 1)
#define SMALL_BLOCK_MASK (SMALL_BLOCK_SIZE - 1)

static constexpr bool mtd_use_4k_sectors = false;

class hw_mtd
{
  protected:
    size_t _size;
    int _erase_size;
    bool _is_4k;
    int _fd;

  public:
    hw_mtd() : _size(0), _erase_size(0), _is_4k(mtd_use_4k_sectors), _fd(-1)
    {
    }
    ~hw_mtd()
    {
        if (_fd >= 0)
            ::close(_fd);
    }

    int open(const std::string& path);
    void erase(uint32_t addr, size_t len);
    int write_raw(uint32_t addr, const cbspan& in_buf);

    int erase_size() const
    {
        return _erase_size;
    }
    size_t size() const
    {
        return _size;
    }
    bool is_4k() const
    {
        return _is_4k;
    }
};

#define DEFAULT_MTD_EMU_SZ (128 * 1024 * 1024)
class file_mtd_emulation
{
  protected:
    size_t _size;
    bool _is_4k;
    int _fd;

  public:
    file_mtd_emulation() : _size(0), _is_4k(mtd_use_4k_sectors), _fd(-1)
    {
    }
    ~file_mtd_emulation()
    {
        if (_fd >= 0)
            ::close(_fd);
    }

    int open(const std::string& path);
    void erase(uint32_t addr, size_t len);
    int write_raw(uint32_t addr, const cbspan& in_buf);

    int erase_size() const
    {
        return (64 * 1024);
    }
    size_t size() const
    {
        return _size;
    }
    int is_4k() const
    {
        return _is_4k;
    }
};

template <typename deviceClassT>
class mtd
{
  protected:
    deviceClassT _impl;
    std::string _path;
    int _fd;

  public:
    typedef std::shared_ptr<mtd> ptr;
    mtd();
    ~mtd();

    /* after creating, one must call open, which may throw things */
    void open(const std::string& path);

    /* read into a buffer out_buf.size() bytes */
    int read(uint32_t addr, std::vector<uint8_t>& out_buf);
    /* write with an implied erase */
    int write(uint32_t addr, const cbspan& in_buf);
    /* write without an erase */
    int write_raw(uint32_t addr, const cbspan& in_buf);
    void erase(uint32_t addr, size_t len);
    size_t erase_size(void) const
    {
        return _impl.erase_size();
    }
    size_t size(void) const
    {
        return _impl.size();
    }
    size_t is_4k(void) const
    {
        return _impl.is_4k();
    }
};

#ifdef MTD_EMULATION
typedef mtd<file_mtd_emulation> mtd_type;
#define PROC_MTD_FILE "proc/mtd"
#define MTD_DEV_BASE "dev/"
#else /* !MTD_EMULATION */
typedef mtd<hw_mtd> mtd_type;
#define PROC_MTD_FILE "/proc/mtd"
#define MTD_DEV_BASE "/dev/"
#endif /* MTD_EMULATION */
#define MTD_DEV_MTD0 MTD_DEV_BASE "mtd0"

#endif /* __MTD_H__ */
