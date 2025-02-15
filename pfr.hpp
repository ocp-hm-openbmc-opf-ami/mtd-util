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
// Abstract: PFR image capabilities
*/

#pragma once

#include <boost/iostreams/device/mapped_file.hpp>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>

#include "debug.h"
#include "exceptions.h"
#include "mtd.h"

constexpr uint32_t pfr_pc_type_cpld_update = 0x00;
constexpr uint32_t pfr_pc_type_pch_pfm = 0x01;
constexpr uint32_t pfr_pc_type_pch_update = 0x02;
constexpr uint32_t pfr_pc_type_bmc_pfm = 0x03;
constexpr uint32_t pfr_pc_type_bmc_update = 0x04;
constexpr uint32_t pfr_pc_type_partial_update = 0x05;
constexpr uint32_t pfr_pc_type_afm_update = 0x06;
constexpr uint32_t pfr_pc_type_combined_cpld_update = 0x07;
constexpr uint32_t pfr_pc_type_per_device_afm= 0x08;
constexpr uint32_t pfr_img_type_add_to_update_afm= 0x0a;
constexpr uint32_t secure_boot_pc_type_bmc = 0xf1;
constexpr uint32_t secure_boot_pc_type_otp = 0xf2;
constexpr uint32_t pfr_pc_type_retimer_update = 0x09;
constexpr uint32_t pfr_pc_type_cancel_cert = 0x100;
constexpr uint32_t pfr_pc_type_pfr_decommission = 0x200;

constexpr uint32_t key_non_cancellable = static_cast<uint32_t>(-1);
constexpr uint32_t pfr_perm_sign_all = static_cast<uint32_t>(-1);
constexpr uint32_t pfr_perm_sign_pch_pfm = 0x01;
constexpr uint32_t pfr_perm_sign_pch_update = 0x02;
constexpr uint32_t pfr_perm_sign_bmc_pfm = 0x04;
constexpr uint32_t pfr_perm_sign_bmc_update = 0x08;
constexpr uint32_t secure_boot_perm_sign_bmc_update = 0x08;
constexpr uint32_t secure_boot_perm_sign_otp_update = 0x08;
constexpr uint32_t pfr_perm_sign_cpld_update = 0x10;
constexpr uint32_t pfr_perm_sign_combined_cpld_update = 0x40;
constexpr uint32_t pfr_perm_sign_afm_update = 0x20;
constexpr uint32_t pfr_perm_sign_retimer_update = 0x04;

constexpr size_t pfr_blk_size = 0x1000;
constexpr size_t pfr_pfm_max_size = 64 * 1024;                // 64 kB
constexpr size_t pfr_cpld_update_size = 1 * 1024 * 1024;      // 1 MB
constexpr size_t pfr_pch_max_size = 24 * 1024 * 1024;         // 24 MB
constexpr size_t pfr_bmc_max_size = 33 * 1024 * 1024;         // 33 MB
constexpr size_t secure_boot_bmc_max_size = 32 * 1024 * 1024; // 32 MB
// TODO: confirm the image size before merging
constexpr size_t secure_boot_otp_max_size = 67 * 1024; // 67 KB
constexpr size_t pfr_afm_max_size = 512 * 1024;        // 512KB
constexpr size_t pfr_cancel_cert_size = 128;
constexpr uint32_t pfr_max_key_id = 127;
constexpr size_t pfr_combined_cpld_max_size = 2 * 1024 * 1024; // 2 MB
constexpr size_t pfr_retimer_max_size = 1 * 1024 * 1024;       // 1 MB

constexpr uint32_t curve_secp256r1 = 0xc7b88c74;
constexpr uint32_t curve_secp384r1 = 0x08f07b47;
constexpr uint32_t sig_magic_secp256r1 = 0xde64437d;
constexpr uint32_t sig_magic_secp384r1 = 0xea2a50e9;

/*		0B
 *		<------+--------------------------------------+
 *		       |                                      |
 *		       |           Block0 (128B)              |
 *		       |                                      |
 *		       | Contains generic description of      |
 *		       | protected content (type, size) and   |
 *		       | SHA256 hash of protected content     |
 *		       |                                      |
 *		       |                                      |
 *		128B   |                                      |
 *		<---------------------------------+-----------+
 *		       |                          |           |
 *		       |           Block1 (896B)  |  Root Key |
 *		       |                          |           |
 *		       | Contains signature chain +-----------+
 *		       | (root key, code signing  |           |
 *		       | key, block0 signature)   |    CSK    |
 *		       | used for authentication  |           |
 *		       | of Block0                +-----------+
 *		       |                          |           |
 *		       |                          |   Block0  |
 *		1024B  |                          | Signature |
 *		<---------------------------------+-----------+
 *		       |                                      |
 *		       |     Protected Content (PC size)      |
 *		       |                                      |
 *		       |                                      |
 *		       |                                      |
 *		       |                                      |
 *		       |                                      |
 *		       |                                      |
 *		       |                                      |
 *		       |                                      |
 *		       |                                      |
 *		       |                                      |
 *		       |                                      |
 *	    PC size +  |                                      |
 *	    1024B +    |                                      |
 *	    padding 64B|                                      |
 *              <------+--------------------------------------+
 */

constexpr size_t blk0_size = 128;
constexpr uint32_t blk0_magic = 0xb6eafd19;
constexpr size_t blk0_pad_size = 32;
struct blk0
{
    uint32_t magic;
    uint32_t pc_length;
    uint32_t pc_type;
    uint32_t rsvd;
    uint8_t sha256[32];
    uint8_t sha384[48];
    uint8_t pad[blk0_pad_size];
} __attribute__((packed));
static_assert(sizeof(blk0) == 128, "blk0 size is not 128 bytes");

constexpr uint32_t root_key_magic = 0xa757a046;
constexpr size_t key_entry_rsvd_size = 5;
struct key_entry
{
    uint32_t magic;
    uint32_t curve;
    uint32_t permissions;
    uint32_t key_id;
    uint8_t key_x[48];
    uint8_t key_y[48];
    uint32_t rsvd[key_entry_rsvd_size];
} __attribute__((packed));
static_assert(sizeof(key_entry) == 132, "key_entry is not 132 bytes");
static constexpr size_t block1_csk_entry_hash_region_size =
    (sizeof(key_entry) - sizeof(uint32_t));

constexpr uint32_t csk_key_magic = 0x14711c2f;
struct csk_entry
{
    key_entry key;
    uint32_t sig_magic;
    uint8_t sig_r[48];
    uint8_t sig_s[48];
} __attribute__((packed));
static_assert(sizeof(csk_entry) == 232, "csk_entry is not 232 bytes");

constexpr uint32_t block0_sig_entry_magic = 0x15364367;
struct block0_sig_entry
{
    uint32_t magic;
    uint32_t sig_magic;
    uint8_t sig_r[48];
    uint8_t sig_s[48];
};

constexpr size_t blk0blk1_size = 1024;
constexpr uint32_t blk1_magic = 0xf27f28d7;
constexpr size_t blk1_pad_size =
    (blk0blk1_size - sizeof(blk0) - sizeof(uint32_t) * 4 - sizeof(key_entry) -
     sizeof(csk_entry) - sizeof(block0_sig_entry)) /
    sizeof(uint32_t);
struct blk1
{
    uint32_t magic;
    uint32_t pad[3];
    key_entry root_key;
    csk_entry csk;
    block0_sig_entry block0_sig;
    uint32_t rsvd[blk1_pad_size];
} __attribute__((packed));

constexpr size_t cancel_pad_size = 9;
struct cancel_cert
{
    uint32_t magic;
    uint32_t pc_length;
    uint32_t pc_type;
    uint8_t sha256[32];
    uint8_t sha384[48];
    uint32_t rsvd[cancel_pad_size];
} __attribute__((packed));

constexpr size_t cancel_sig_pad_size =
    (blk0blk1_size - sizeof(cancel_cert) - sizeof(uint32_t) -
     (3 * sizeof(uint32_t)) - sizeof(key_entry) - sizeof(block0_sig_entry)) /
    sizeof(uint32_t);
struct cancel_sig
{
    uint32_t magic;
    uint32_t pad[3];
    key_entry root_key;
    block0_sig_entry block0_sig;
    uint32_t rsvd[cancel_sig_pad_size];
} __attribute__((packed));
static_assert(sizeof(cancel_cert) + sizeof(cancel_sig) == blk0blk1_size,
              "cancel blk0blk1 size is not 1024 bytes");

constexpr size_t cancel_payload_pad_size = 31;
struct cancel_payload
{
    uint32_t csk_id;
    uint32_t padding[cancel_payload_pad_size];
} __attribute__((packed));

union sig_blk1
{
    blk1 b1;
    cancel_sig cncl_b1;
};

struct b0b1_signature
{
    blk0 b0;
    sig_blk1 b1_sig;
} __attribute__((packed));
static_assert(sizeof(b0b1_signature) == 1024,
              "block0 + block1 size is not 1024 bytes");

constexpr uint32_t pfm_magic = 0x02b3ce1d;
constexpr uint32_t afm_magic = 0x8883ce1d;
constexpr size_t pfm_block_size = 128;
constexpr size_t pfm_hdr_size = 32;
constexpr size_t CPLD_addr_ref_hdr_size = 12;

union pfm_reserved
{
    uint32_t rsvd;
    uint16_t platform_type;
};

struct pfm
{
    uint32_t magic;
    uint8_t svn;
    uint8_t bkc;
    uint16_t pfm_revision;
    pfm_reserved plt;
    uint8_t oem_data[16];
    uint32_t length;
    // pfm_data
    // padding to 128-byte boundary
} __attribute__((packed));

enum fw_type
{
    CPUfwType = 0x00,
    SCMfwType = 0x01,
    DebugfwType = 0x02
};

struct cpld_addr_def
{
    uint8_t def_type;
    uint16_t fw_type;
    uint8_t rsvd;
    uint32_t length;
    uint32_t img_strt_offset;
} __attribute__((packed));

constexpr uint32_t cfm_magic = 0xa8e7c2d6;
struct cfm
{
    uint32_t magic;
    uint8_t svn;
    uint8_t rsvd;
    uint16_t cpld_revision;
    uint16_t rsvd2;
    uint16_t fw_type;
    uint8_t oem_data[16];
    uint32_t length;
} __attribute__((packed));

struct afm
{
    uint32_t magic;
    uint8_t svn;
    uint8_t rsvd;
    uint16_t afm_revision;
    uint8_t oem_data[16];
    uint32_t length;
    // pfm_data
    // padding to 128-byte boundary
} __attribute__((packed));

constexpr uint32_t fvm_magic = 0xa8e7c2d4;
constexpr size_t fvm_block_size = 128;
struct fvm
{
    uint32_t magic;
    uint8_t svn;
    uint8_t rsvd1;
    uint16_t fvm_revision;
    uint16_t rsvd2;
    uint16_t fv_type;
    uint8_t oem_data[16];
    uint32_t length;
    // fvm_data
    // padding to 128-byte boundary
} __attribute__((packed));

struct spi_region
{
    uint8_t type;
    uint8_t mask;
    uint16_t hash_info;
    uint32_t rsvd;
    uint32_t start;
    uint32_t end;
    // hash 1 if present
    // hash 2 if present
} __attribute__((packed));
constexpr uint8_t sha256_present = 0x01;
constexpr uint8_t sha384_present = 0x02;
constexpr size_t sha256_size = (256 / 8);
constexpr size_t sha384_size = (384 / 8);

struct smbus_rule
{
    uint8_t type;
    uint32_t rsvd;
    uint8_t bus;
    uint8_t rule;
    uint8_t addr;
    uint8_t allowlist[32];
} __attribute__((packed));

struct fvm_address
{
    uint8_t type;
    uint16_t fvm_type;
    uint32_t rsvd;
    uint32_t addr;
} __attribute__((packed));
constexpr uint8_t fvm_type_bios = 0;
constexpr uint8_t fvm_type_me = 1;
constexpr uint8_t fvm_type_ucode_reg_1 = 2;
constexpr uint8_t fvm_type_ucode_reg_2 = 3;

struct fvm_capabilities
{
    uint8_t type;
    uint16_t rsvd1;
    uint8_t revision;
    uint16_t length;
    struct
    {
        uint8_t major;
        uint8_t minor;
        uint8_t release;
        uint8_t hotfix;
    } version;
    uint32_t layout;
    uint32_t actions;
    uint8_t rsvd2[26];
    uint8_t description[20];
} __attribute__((packed));
constexpr uint8_t action_reboot_required = 1;

constexpr uint8_t type_spi_region = 1;
constexpr uint8_t type_smbus_rule = 2;
constexpr uint8_t type_fvm_address = 3;
constexpr uint8_t type_fvm_capabilities = 4;

constexpr uint32_t pbc_magic = 0x5f504243;
struct pbc
{
    uint32_t magic;
    uint32_t version;
    uint32_t page_size;
    uint32_t pattern_size;
    uint32_t pattern;
    uint32_t bitmap_size;
    uint32_t payload_length;
    uint8_t rsvd[100];
    // active
    // compression
    // payload
} __attribute__((packed));

bool pfr_authenticate(const std::string& filename, bool check_root_key);

template <typename deviceClassT>
bool pfr_stage(mtd<deviceClassT>& dev, const std::string& filename,
               size_t offset)
{
    if (!pfr_authenticate(filename, true))
    {
        return false;
    }
    boost::iostreams::mapped_file file(filename,
                                       boost::iostreams::mapped_file::readonly);
    auto map_base = reinterpret_cast<const uint8_t*>(file.const_data());
    size_t img_size = file.size();

    dev.erase(offset, img_size);

    cbspan rc_img_data(map_base, map_base + img_size);
    dev.write_raw(offset, rc_img_data);
    return true;
}

template <typename deviceClassT>
bool pfr_write(mtd<deviceClassT>& dev, const std::string& filename,
               size_t dev_offset, bool recovery_reset)
{
    if (!pfr_authenticate(filename, !recovery_reset))
    {
        return false;
    }
    boost::iostreams::mapped_file file(filename,
                                       boost::iostreams::mapped_file::readonly);
    auto map_base = reinterpret_cast<const uint8_t*>(file.const_data());
    auto offset = reinterpret_cast<const uint8_t*>(file.const_data());

    FWDEBUG("file mapped " << file.size() << " bytes at 0x" << std::hex
                           << reinterpret_cast<unsigned long>(offset));

    // walk the bitmap, erase and copy
    offset += blk0blk1_size * 2; // one blk0blk1 for package, one for pfm
    auto pfm_hdr = reinterpret_cast<const pfm*>(offset);
    FWDEBUG("pfm header at " << std::hex << pfm_hdr
                             << " (magic:" << pfm_hdr->magic << ")");
    FWDEBUG("pfm length is 0x" << std::hex << pfm_hdr->length);
    size_t pfm_size = block_round(pfm_hdr->length, pfm_block_size);
    cbspan pfm_data(offset - blk0blk1_size, offset + pfm_size);
    offset += pfm_size;
    auto pbc_hdr = reinterpret_cast<const pbc*>(offset);
    FWDEBUG("pbc header at " << std::hex << pbc_hdr
                             << " (magic:" << pbc_hdr->magic << ")");
    FWDEBUG("pbc bitmap size 0x" << std::hex << pbc_hdr->bitmap_size);
    offset += sizeof(pbc);
    auto act_map = reinterpret_cast<const uint8_t*>(offset);
    FWDEBUG("active map at 0x" << std::hex
                               << reinterpret_cast<unsigned long>(act_map));
    offset += pbc_hdr->bitmap_size / 8;
    auto pbc_map = reinterpret_cast<const uint8_t*>(offset);
    FWDEBUG("pbc map at 0x" << std::hex
                            << reinterpret_cast<unsigned long>(pbc_map));

    // copy the pfm manually (not part of the compression bitmap)
    constexpr size_t pfm_address_old_layout = 0x80000;
    constexpr size_t pfm_address_new_layout = 0x100000;
    constexpr size_t pfm_region_size = 0x20000;

    auto pfm_rev = pfm_hdr->oem_data[4];
    // pfm_rev is 0xFF  for old flash layout and
    // 0x01 for new flash layout
    FWDEBUG("PFM version " << std::to_string(pfm_rev));

    size_t pfm_address = pfm_address_old_layout;

    if (pfm_rev == 0x01)
        pfm_address = pfm_address_new_layout;

    dev.erase(pfm_address + dev_offset, pfm_region_size);
    dev.write_raw(pfm_address + dev_offset, pfm_data);
    // set offset to the beginning of the compressed data
    offset += pbc_hdr->bitmap_size / 8;
    uint32_t wr_count = 1;
    uint32_t er_count = 1;
    uint32_t erase_end_addr = 0;
    uint32_t write_end_addr = 0;
    for (uint32_t blk = 0; blk < pbc_hdr->bitmap_size; blk += wr_count)
    {
        // Every flash partition is 64k aligned. Since driver supports 64k erase
        // and write, a chunk of 64k can be handled at once. mod 16 picks up 2
        // bytes at once (1 bit per page * 4k page size * 2 bytes = 64k)
        if ((blk % 16) == 0)
        {
            wr_count = 1;
            er_count = 1;
            if ((blk + 8) < pbc_hdr->bitmap_size)
            {
                uint32_t b8 = blk / 8;
                // try to do 64k first
                // 64k erase is fine if all erase bits are set and either
                // all copy bits or no copy bits are set

                // Determine erase count and write count
                // Erase count being 1 implies flash partition is not 64k
                // aligned (helps in catching errors)
                er_count =
                    (act_map[b8] == 0xff && act_map[b8 + 1] == 0xff) ? 16 : 1;
                wr_count = ((pbc_map[b8] == 0xff && pbc_map[b8 + 1] == 0xff) ||
                            (pbc_map[b8] == 0 && pbc_map[b8 + 1] == 0))
                               ? 16
                               : 1;
            }
        }
        bool erase = (act_map[blk / 8] >> (7 - blk % 8)) & 1;
        bool copy = (pbc_map[blk / 8] >> (7 - blk % 8)) & 1;

        if (!erase)
        {
            continue;
        }
        if (!copy)
        {
            // skip erase if the block is in an unsigned segment
            auto region_offset = reinterpret_cast<const uint8_t*>(pfm_hdr + 1);
            auto region_end = region_offset + pfm_size;
            bool region_is_unsigned = true;
            while (region_offset < region_end)
            {
                auto region =
                    reinterpret_cast<const spi_region*>(region_offset);
                if (region->type == type_spi_region)
                {
                    // check if the first block is within this region
                    if (region->start <= blk * pfr_blk_size)
                    {
                        // check if the last block is within this region
                        if ((blk + er_count) * pfr_blk_size <= region->end)
                        {
                            region_is_unsigned = (region->hash_info == 0);
                            break;
                        }

                        if ((blk + 1) * pfr_blk_size <= region->end)
                        {
                            // Since flash partitions are 64k aligned, the block
                            // will always be within end region. This check is
                            // just to catch any special case if any
                            FWDEBUG("The block is not 64k aligned !");
                            return false;
                        }
                    }
                    region_offset +=
                        sizeof(spi_region) +
                        (region->hash_info & sha256_present ? sha256_size : 0) +
                        (region->hash_info & sha384_present ? sha384_size : 0);
                }
                else if (region->type == type_smbus_rule)
                {
                    region_offset += sizeof(smbus_rule);
                }
                else if (region->type == type_fvm_address)
                {
                    region_offset += sizeof(fvm_address);
                }
                else
                {
                    break;
                }
            }
            if (region_is_unsigned && !recovery_reset)
            {
                FWDEBUG("skipping erase on unsigned block"
                        << (er_count == 16 ? "s" : "") << " @" << std::hex
                        << pfr_blk_size * blk + dev_offset);
                continue;
            }
        }

        if (erase)
        {
            // Since we support only 64k erase
            if (er_count == 16)
            {
                FWDEBUG("erase(" << std::hex << pfr_blk_size * blk << ", "
                                 << pfr_blk_size * er_count + dev_offset
                                 << ")");
                dev.erase(pfr_blk_size * blk + dev_offset,
                          pfr_blk_size * er_count);
                erase_end_addr = (pfr_blk_size * (blk + er_count)) - 1;
                FWDEBUG("erase_end_addr: " << std::hex << erase_end_addr);
                er_count = 0;
            }

            // Catch any special cases. As long as partition is 64k aligned,
            // this condition will be false.
            else if (er_count == 1)
            {
                FWDEBUG("4k Erase not supported !");
                return false;
            }
        }

        if (copy)
        {
            cbspan data(offset, offset + pfr_blk_size * wr_count);
            write_end_addr = (pfr_blk_size * (blk + wr_count)) - 1;
            FWDEBUG("write_end_addr: " << std::hex << write_end_addr);

            // Check if the region to be written is erased
            if ((write_end_addr > erase_end_addr) ||
                ((pfr_blk_size * blk) > erase_end_addr))
            {
                // Currently 4K erases are not working hence making it always
                // 64K erase.
                // TODO: Fix 4K erase issue and fix the below logic to do
                // incremental 4K erases.
                FWDEBUG("erase(" << std::hex
                                 << (erase_end_addr + 1) + dev_offset << ", "
                                 << pfr_blk_size * 16 << ")");

                dev.erase((erase_end_addr + 1) + dev_offset, pfr_blk_size * 16);
                erase_end_addr += pfr_blk_size * 16;
                FWDEBUG("erase_end_addr: " << std::hex << erase_end_addr);
            }
            // DUMP(PRINT_ERROR, data);
            FWDEBUG("write(" << std::hex << pfr_blk_size * blk << ", "
                             << pfr_blk_size * wr_count << "), offset = 0x"
                             << (offset - map_base) + dev_offset);
            dev.write_raw(pfr_blk_size * blk + dev_offset, data);

            offset += pfr_blk_size * wr_count;
        }
    }
    return true;
}

template <typename deviceClassT>
bool secure_boot_image_update(mtd<deviceClassT>& dev,
                              const std::string& filename, size_t dev_offset)
{
    if (!pfr_authenticate(filename, true))
    {
        return false;
    }
    boost::iostreams::mapped_file file(filename,
                                       boost::iostreams::mapped_file::readonly);
    auto map_base = reinterpret_cast<const uint8_t*>(file.const_data());
    auto offset = reinterpret_cast<const uint8_t*>(file.const_data());

    FWDEBUG("file mapped " << file.size() << " bytes at 0x" << std::hex
                           << reinterpret_cast<unsigned long>(offset));

    auto block0 = reinterpret_cast<const blk0*>(offset);
    // walk the bitmap, erase and copy
    offset += blk0blk1_size * 2; // one blk0blk1 for package, one for pfm
    auto pfm_hdr = reinterpret_cast<const pfm*>(offset);
    FWDEBUG("pfm header at " << std::hex << pfm_hdr
                             << " (magic:" << pfm_hdr->magic << ")");
    FWDEBUG("pfm length is 0x" << std::hex << pfm_hdr->length);
    size_t pfm_size = block_round(pfm_hdr->length, pfm_block_size);
    cbspan pfm_data(offset - blk0blk1_size, offset + pfm_size);
    offset += pfm_size;
    auto pbc_hdr = reinterpret_cast<const pbc*>(offset);
    FWDEBUG("pbc header at " << std::hex << pbc_hdr
                             << " (magic:" << pbc_hdr->magic << ")");
    FWDEBUG("pbc bitmap size 0x" << std::hex << pbc_hdr->bitmap_size);
    offset += sizeof(pbc);
    auto act_map = reinterpret_cast<const uint8_t*>(offset);
    FWDEBUG("active map at 0x" << std::hex
                               << reinterpret_cast<unsigned long>(act_map));
    offset += pbc_hdr->bitmap_size / 8;
    auto pbc_map = reinterpret_cast<const uint8_t*>(offset);
    FWDEBUG("pbc map at 0x" << std::hex
                            << reinterpret_cast<unsigned long>(pbc_map));

    // copy the pfm manually (not part of the compression bitmap)
    constexpr size_t pfm_address = 0x80000;
    constexpr size_t pfm_region_size = 0x20000;
    constexpr uint32_t secondary_image_offset = 0x04000000;
    constexpr uint32_t fit_image_block = 0xb00;
    // As per secure boot image layout the difference in the start address of
    // Image-A kernel fit image and Image-B kernel fit image is 0xA60000 (0xA60
    // 4K blocks)
    constexpr uint32_t blocks_skip = 0xA60;
    constexpr uint32_t uboot_block = 0x10;
    dev.erase(pfm_address + dev_offset, pfm_region_size);
    dev.write_raw(pfm_address + dev_offset, pfm_data);
    // set offset to the beginning of the compressed data
    offset += pbc_hdr->bitmap_size / 8;
    uint32_t wr_count = 1;
    uint32_t er_count = 1;
    uint32_t erase_end_addr = 0;
    uint32_t write_end_addr = 0;
    uint32_t blocks_to_skip = 0;
    uint32_t uboot_offset = dev_offset;
    bool rot_a_update_done = false;
    for (uint32_t blk = 0; blk < pbc_hdr->bitmap_size; blk += wr_count)
    {
        if ((dev_offset == secondary_image_offset) && (blk >= fit_image_block))
        {
            blocks_to_skip = blocks_skip;
        }
        /* When secondary update is initiated for image type 0xf1 the ROT SPL is
        updated in the primary region and uboot, pfm, fit image is updated in
        secondary region */
        if (!rot_a_update_done)
        {
            if ((block0->pc_type == secure_boot_pc_type_bmc) &&
                (uboot_offset == secondary_image_offset))
            {
                if ((blk == 0))
                {
                    dev_offset = 0;
                }
                else if (blk == uboot_block)
                {
                    dev_offset = uboot_offset;
                    rot_a_update_done = true;
                }
            }
        }

        if ((blk % 8) == 0)
        {
            wr_count = 1;
            er_count = 1;
            if ((blk + 8) < pbc_hdr->bitmap_size)
            {
                uint32_t b8 = blk / 8;
                // try to do 64k first
                // 64k erase is fine if all erase bits are set and either
                // all copy bits or no copy bits are set
                er_count =
                    (act_map[b8] == 0xff && act_map[b8 + 1] == 0xff) ? 16 : 1;
                wr_count = ((pbc_map[b8] == 0xff && pbc_map[b8 + 1] == 0xff) ||
                            (pbc_map[b8] == 0 && pbc_map[b8 + 1] == 0))
                               ? 16
                               : 1;
            }
        }
        bool erase = (act_map[blk / 8] >> (7 - blk % 8)) & 1;
        bool copy = (pbc_map[blk / 8] >> (7 - blk % 8)) & 1;
        if (!erase)
        {
            continue;
        }
        if (!copy)
        {
            // skip erase if the block is in an unsigned segment
            auto region_offset = reinterpret_cast<const uint8_t*>(pfm_hdr + 1);
            auto region_end = region_offset + pfm_size;
            bool region_is_unsigned = true;
            while (region_offset < region_end)
            {
                auto region =
                    reinterpret_cast<const spi_region*>(region_offset);
                if (region->type == type_spi_region)
                {
                    // check if the first block is within this region
                    if (region->start <= (blk * pfr_blk_size))
                    {
                        // check if the last block is within this region
                        if ((blk + er_count) * pfr_blk_size <= region->end)
                        {
                            region_is_unsigned = (region->hash_info == 0);
                            break;
                        }
                        // check if a single block is within this region
                        if ((blk + 1) * pfr_blk_size <= region->end)
                        {
                            er_count = 1;
                            region_is_unsigned = (region->hash_info == 0);
                            break;
                        }
                    }
                    region_offset +=
                        sizeof(spi_region) +
                        (region->hash_info & sha256_present ? sha256_size : 0) +
                        (region->hash_info & sha384_present ? sha384_size : 0);
                }
                else if (region->type == type_smbus_rule)
                {
                    region_offset += sizeof(smbus_rule);
                }
                else
                {
                    break;
                }
            }
            if (region_is_unsigned)
            {
                FWDEBUG("skipping erase on unsigned block"
                        << (er_count == 16 ? "s" : "") << " @" << std::hex
                        << pfr_blk_size * (blk - blocks_to_skip) + dev_offset);
                continue;
            }
        }
        if (blk % 16 == 0)
        {
            if (er_count == 1)
            {
                FWDEBUG("block " << std::hex
                                 << pfr_blk_size * (blk - blocks_to_skip)
                                 << " has erase size 1; erasing 64k");
                er_count = 16;
            }
            FWDEBUG("erase("
                    << std::hex
                    << pfr_blk_size * (blk - blocks_to_skip) + dev_offset
                    << ", " << pfr_blk_size * er_count << ")");

            dev.erase((pfr_blk_size * (blk - blocks_to_skip)) + dev_offset,
                      pfr_blk_size * er_count);
            erase_end_addr =
                (pfr_blk_size * ((blk - blocks_to_skip) + er_count)) - 1;
            FWDEBUG("erase_end_addr: " << std::hex << erase_end_addr);
        }

        if (copy)
        {
            cbspan data(offset, offset + pfr_blk_size * wr_count);
            write_end_addr =
                (pfr_blk_size * ((blk - blocks_to_skip) + wr_count)) - 1;
            FWDEBUG("write_end_addr: " << std::hex << write_end_addr);

            // Check if current write address wasn't part of previous 64K sector
            // erase. and erase it here.
            if ((write_end_addr > erase_end_addr) ||
                ((pfr_blk_size * (blk - blocks_to_skip)) > erase_end_addr))
            {
                // Currently 4K erases are not working hence making it always
                // 64K erase.
                // TODO: Fix 4K erase issue and fix the below logic to do
                // incremental 4K erases.
                FWDEBUG("erase(" << std::hex
                                 << (erase_end_addr + 1) + dev_offset << ", "
                                 << pfr_blk_size * 16 << ")");

                dev.erase((erase_end_addr + 1) + dev_offset, pfr_blk_size * 16);
                erase_end_addr += pfr_blk_size * 16;
                FWDEBUG("erase_end_addr: " << std::hex << erase_end_addr);
            }
            // DUMP(PRINT_ERROR, data);
            FWDEBUG("write("
                    << std::hex
                    << pfr_blk_size * (blk - blocks_to_skip) + dev_offset
                    << ", " << pfr_blk_size * wr_count << "), offset = 0x"
                    << (offset - map_base));
            dev.write_raw(pfr_blk_size * (blk - blocks_to_skip) + dev_offset,
                          data);

            offset += pfr_blk_size * wr_count;
        }
    }
    return true;
}
