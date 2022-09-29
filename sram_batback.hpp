/*
 * MIT License
 *
 * Copyright (c) 2022 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *
 * Abstract: SRAM Battery Backed Registers capabilities
 */

#pragma once

#include <filesystem>
#include <iostream>
#include <string>

#include "debug.h"
#include "exceptions.h"

const std::string bsramProtKeyPath = "/sys/bus/platform/devices/1e6ef000.sram/1e6ef000.sram";
const std::string bsramBattRegPath = "/sys/bus/platform/devices/1e6ef000.sram/1e6ef100.sram";

constexpr uint32_t bsram_prot_key = 0xdba078e2;

bool bsram_unlock();

bool bsram_lock_verify();

unsigned int bsram_read_reg(unsigned int);

bool bsram_write_reg(unsigned int, uint32_t);
