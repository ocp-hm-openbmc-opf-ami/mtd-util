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

#include <fstream>
#include "sram_batback.hpp"

bool bsram_unlock() {
    std::fstream rwBsramProtKey {bsramProtKeyPath, std::ios::binary | 
	    std::ios::in | std::ios::out};
    // Write Protection Key to unlock SRAM
    rwBsramProtKey.write((const char*)&bsram_prot_key, sizeof(bsram_prot_key));
    rwBsramProtKey.close();
    return true;
}

bool bsram_lock_verify() {
    uint32_t keyVal = 0x0;
    std::fstream rwBsramProtKey {bsramProtKeyPath, std::ios::binary | 
	    std::ios::in | std::ios::out};
    rwBsramProtKey.read((char*)&keyVal, 4);
    std::cout << "Protection Reg = " << keyVal << std::endl;
    // To Verify if SRAM unlocked or not

    rwBsramProtKey.close();
    return true;
}

unsigned int bsram_read_reg(unsigned int regNum) {
    uint32_t regVal = 0x0;
    std::fstream rwBatBackRegs {bsramBattRegPath, std::ios::binary | 
	    std::ios::in | std::ios::out};
    // Read to verify Nth Reg.
    rwBatBackRegs.seekp(regNum, std::ios::beg);
    rwBatBackRegs.read((char*)&regVal, 4);
    std::cout << "Batt Reg Val= " << std::hex << regVal << std::dec << std::endl;
    rwBatBackRegs.close();
    return regVal;
}

bool bsram_write_reg(unsigned int regNum, uint32_t val) {
    std::fstream rwBatBackRegs {bsramBattRegPath, std::ios::binary | 
	    std::ios::in | std::ios::out};
    // Write to Nth Reg
    rwBatBackRegs.seekp(regNum, std::ios::beg);
    rwBatBackRegs.write((const char*)&val, sizeof(val));
    rwBatBackRegs.close();
    return true;
}
