# MTD-Util

## Introduction

The platforms have a SPI flash, exposed as MTD devices. The mtd-util
provides mechanisms to read and write from these MTD partitions.

## Dependencies

- Boost
- OpenSSL
- pthread
- gpiodcxx
- CMake
- C++20
- systemd
- sdbusplus
- GTest

## Overview

The **MTD-Util** repository provides a command line tool for managing and
interacting with memory technology devices (MTD). It includes commands for
reading, writing, erasing, and authenticating firmware images.
It provides the following commands for interacting with MTD devices:

```sh
mtd-util [options] command <arguments...>
```

### Developer Options (Only if DEVELOPER_OPTIONS is enabled)

- ```sh
  mtd-util [-v] [-d <mtd-device>] e[rase] start +len
  mtd-util [-v] [-d <mtd-device>] e[rase] start end
  ```

Erase operations, either from `start` for a given length (`+len`) or from `start` to `end`.

- ```sh
  mtd-util [-v] [-d <mtd-device>] w[rite] offset Xx [Xx ...]
  ```

 Write bytes (hex values) to the flash at the given offset.

### Standard Commands

- ```sh
  mtd-util [-v] [-d <mtd-device>] c[p] file offset
  ```

Copy a file to flash at a specific offset.

- ```sh
  mtd-util [-v] [-d <mtd-device>] [-f] c[p] offset file len
  ```

  Copy from flash to a file, starting at offset for a given length (`len`). The `-f` flag forces overwriting an existing file.

- ```sh
  mtd-util [-v] [-d <mtd-device>] d[ump] offset [len]
  ```
  
  Dump flash contents starting from an offset; optional length (defaults to 256 bytes if not specified).

- ```sh
  mtd-util [-v] [-d <mtd-device>] p[fr] a[uthenticate] file
  ```

  PFR authenticate operation on a file.

- ```sh
  mtd-util [-v] [-d <mtd-device>] p[fr] s[tage] file
  ```

  PFR stage operation on a file.

- ```sh
  mtd-util [-v] [-d <mtd-device>] s[ecure_boot] file offset
  ```

  Secure boot image write (update) operation.

- ```sh
  mtd-util [-v] [-d <mtd-device>] [-r] p[fr] w[rite] file [offset]
  ```

  PFR write operation. The `-r` option resets erase-only regions.

### Additional Notes

- Commands can be abbreviated to their first letter (e.g., `c`, `d`, `p`, etc.).
- `-v` enables verbose output; can be used multiple times.
- `mtd-device` defaults to `/dev/mtd0` if not specified.
- All addresses, offsets, and values must be in hexadecimal.
- Dump length defaults to 256 bytes if not specified.
- "cp to flash" does read/erase/cp/write to preserve flash content integrity.
- Erase rounds to the nearest 4KB boundaries.
- `-f` enables forced overwrite of an existing file.
- `-r` resets erase-only regions for PFR write.
