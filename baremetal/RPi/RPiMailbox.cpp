/*-------------------------------------------------------------------------
This source file is a part of Placid

For the latest info, see http://www.marrin.org/

Copyright (c) 2018, Chris Marrin
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

    - Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer.
    
    - Redistributions in binary form must reproduce the above copyright 
    notice, this list of conditions and the following disclaimer in the 
    documentation and/or other materials provided with the distribution.
    
    - Neither the name of the <ORGANIZATION> nor the names of its 
    contributors may be used to endorse or promote products derived from 
    this software without specific prior written permission.
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.
-------------------------------------------------------------------------*/

#include "bare.h"

#include "bare/Mailbox.h"

#include "bare/Serial.h"

using namespace bare;

static volatile unsigned int *MAILBOX0READ = (unsigned int *) 0x2000b880;
static volatile unsigned int *MAILBOX0STATUS = (unsigned int *) 0x2000b898;
static volatile unsigned int *MAILBOX0WRITE = (unsigned int *) 0x2000b8a0;

#define MAILBOX_FULL 0x80000000
#define MAILBOX_EMPTY 0x40000000

static inline void dmb() { __asm volatile ("mcr p15, #0, %[zero], c7, c10, #5" : : [zero] "r" (0) ); }
static inline void flushcache() { __asm volatile ("mcr p15, #0, %[zero], c7, c14, #0" : : [zero] "r" (0) ); }

// This code is from corn-mainline by Raspberry Alpha Omega

static uint32_t readmailbox(uint32_t channel) {
  uint32_t count = 0;
  uint32_t data;

    if ((channel & 0xFFFFFFF0) != 0) {
        Serial::printf("Channel %d is wrong\n", (int) channel);
        return -1;
    }

    /* Loop until something is received from channel
     * If nothing recieved, it eventually give up and returns 0xffffffff
     */
    while(1) {
        while (*MAILBOX0STATUS & MAILBOX_EMPTY) {
            /* Need to check if this is the right thing to do */
            flushcache();

            /* This is an arbritarily large number */
            if(count++ >(1<<25)) {
                return 0xffffffff;
            }
        }

        /* Read the data
         * Data memory barriers as we've switched peripheral
         */
        dmb();
        data = *MAILBOX0READ;
        dmb();

        if ((data & 15) == channel)
            return data & 0xFFFFFFF0;
    }

    return 0;
}

static void writemailbox(uint32_t channel, uint32_t data)
{
    if ((data & 0x0000000F) != 0) {
        return;
    }
    if ((channel & 0xFFFFFFF0) != 0) {
        return;
    }

    // Wait for mailbox to be not full
    while (*MAILBOX0STATUS & MAILBOX_FULL)     {
        // Need to check if this is the right thing to do
        flushcache();
    }

    dmb();
    *MAILBOX0WRITE = (data | channel);
}

Mailbox::Error Mailbox::getParameter(Param param, uint32_t* result, uint32_t size)
{
    static constexpr uint32_t BufferPadding = 6;
    static constexpr uint32_t MaxSize = 256 - BufferPadding;
    if (size > MaxSize) {
        return Error::SizeTooLarge;
    }
    
    uint32_t buf[MaxSize + BufferPadding] __attribute__((aligned(16)));
    memset(buf, 0, sizeof(buf));

    buf[0] = (size + BufferPadding) * sizeof(uint32_t);
    buf[1] = 0; // Request code: process request
    buf[2] = static_cast<uint32_t>(param);
    buf[3] = size * sizeof(uint32_t); // value buffer size in bytes ((1 entry)*4)
    buf[4] = 0; // 1bit request/response (0/1), 31bit value length in bytes

    uint32_t addr = static_cast<uint32_t>((reinterpret_cast<uintptr_t>(buf)) + 0x40000000);
    writemailbox(8, addr);
    uint32_t mail = readmailbox(8);
    dmb();
    
    uint32_t* responseBuf = reinterpret_cast<uint32_t*>(mail);
    for (uint32_t i = 0; i < size; ++i) {
        result[i] = responseBuf[5 + i];
    }

    return Error::OK;
}

void Mailbox::printBoardParams()
{
    uint32_t responseBuf[2];
    getParameter(Mailbox::Param::FirmwareRev, responseBuf, 1);
    Serial::printf("FirmwareRev: %d\n", responseBuf[0]);
    
    getParameter(Mailbox::Param::BoardModel, responseBuf, 1);
    Serial::printf("BoardModel: %d\n", responseBuf[0]);
    
    getParameter(Mailbox::Param::BoardRev, responseBuf, 1);
    Serial::printf("BoardRev: %d\n", responseBuf[0]);
    
    getParameter(Mailbox::Param::BoardSerialNo, responseBuf, 2);
    Serial::printf("BoardSerialNo: %d, %d\n", responseBuf[0], responseBuf[1]);
    
    getParameter(Mailbox::Param::ARMMemory, responseBuf, 2);
    Serial::printf("ARMMemory: start=0x%08x, size=0x%08x\n", responseBuf[0], responseBuf[1]);
    
    getParameter(Mailbox::Param::VCMemory, responseBuf, 2);
    Serial::printf("VCMemory: start=0x%08x, size=0x%08x\n", responseBuf[0], responseBuf[1]);
    
    getParameter(Mailbox::Param::DMAChannelMask, responseBuf, 1);
    Serial::printf("DMAChannelMask: 0x%08x\n", responseBuf[0]);
    
}
