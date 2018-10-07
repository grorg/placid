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

#include "SD.h"

#include "GPIO.h"

using namespace placid;

SD::SD(uint32_t cd, uint32_t clk, uint32_t cmd, uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3)
{
    // Initialize GPIO for all the SD pins
    GPIO::setFunction(cd, GPIO::Function::Input);
    GPIO::setPull(cd, GPIO::Pull::Up);
    uint32_t reg = GPIO::reg(GPIO::Register::GPHEN1);
    reg = reg | 1<<(47-32);
    GPIO::reg(GPIO::Register::GPHEN1) = reg;

    GPIO::setFunction(d3, GPIO::Function::Alt3);
    GPIO::setPull(d3, GPIO::Pull::Up);
    GPIO::setFunction(d2, GPIO::Function::Alt3);
    GPIO::setPull(d2, GPIO::Pull::Up);
    GPIO::setFunction(d1, GPIO::Function::Alt3);
    GPIO::setPull(d1, GPIO::Pull::Up);
    GPIO::setFunction(d0, GPIO::Function::Alt3);
    GPIO::setPull(d0, GPIO::Pull::Up);
    GPIO::setFunction(cmd, GPIO::Function::Alt3);
    GPIO::setPull(cmd, GPIO::Pull::Up);
    GPIO::setFunction(clk, GPIO::Function::Alt3);
    GPIO::setPull(clk, GPIO::Pull::Up);
}