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

#include "bare/Timer.h"

#include "bare/InterruptManager.h"
#include "bare/Serial.h"

using namespace bare;

struct ARMTimer
{
	uint32_t load;
	uint32_t value;
	uint32_t control;
	uint32_t clearIRQ;
	uint32_t rawIRQ;
	uint32_t maskedIRQ;
	uint32_t reload;
	uint32_t divider;
	uint32_t freeRunningCounter;
};

struct SystemTimer
{
    uint32_t control;
    uint32_t counter0;
    uint32_t counter1;
    uint32_t compare0;
    uint32_t compare1;
    uint32_t compare2;
    uint32_t compare3;
};

static constexpr uint32_t ARMTimerBase = 0x2000B400;
static constexpr uint32_t SystemTimerBase = 0x20003000;

inline volatile ARMTimer& armTimer()
{
    return *(reinterpret_cast<volatile ARMTimer*>(ARMTimerBase));
}

inline volatile SystemTimer& systemTimer()
{
    return *(reinterpret_cast<volatile SystemTimer*>(SystemTimerBase));
}

void  Timer::init()
{
    // We want a 1MHz tick. System clock is 250MHz, so we divide by 250
    // Set the prescaler (bits 16-23) to one less, or 0xf9
    armTimer().control = 0x00F90000; // Set the prescaler, but keep the timer stopped
    armTimer().control = 0x00F90200; // Now start the timer with the same prescaler value
}

void Timer::updateTimers()
{
    // FIXME: For now we only support a single timer at the head of the list
    if (!_head) {
        return;
    }
    
    disableIRQ();
    InterruptManager::enableBasicIRQ(0, false);

    // Reset Free running prescaler
    armTimer().control = 0x003E0000;
    
    armTimer().load = _head->_timeout - 1;
    armTimer().reload = _head->_timeout - 1;
    
    InterruptManager::enableBasicIRQ(0, true);
    armTimer().clearIRQ = 0;
    
    enableIRQ();

    // 32 bit counter, timer enabled, timer interrupt enabled
    armTimer().control = 0x003E00A2;
    armTimer().clearIRQ = 0;
}

void Timer::handleInterrupt()
{
    if (!interruptsSupported()) {
        return;
    }
    
    for (Timer* timer = _head; timer; timer = timer->_next) {
        timer->handleTimerEvent();
	}
    
	armTimer().clearIRQ = 0;
}

int64_t Timer::systemTime()
{
    return (static_cast<int64_t>(systemTimer().counter1) << 32) | static_cast<int64_t>(systemTimer().counter0);
}
