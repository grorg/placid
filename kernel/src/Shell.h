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

#pragma once

#include <cstdint>
#include <vector>
#include "bare/String.h"

namespace placid {
	
	// Shell - Base class for a console shell
	//

	class Shell {
	public:
	    enum class State { Connect, Disconnect, NeedPrompt, ShowingPrompt, ShowHelp };
		
	    void connected();
	    void disconnected();
	    bool received(uint8_t c);
		
		virtual const char* welcomeString() const = 0;
		virtual const char* helpString() const = 0;
        virtual const char* promptString() const = 0;
	    virtual void shellSend(const char* data, uint32_t size = 0, bool raw = false) = 0;
		virtual bool executeShellCommand(const std::vector<bare::String>&) = 0;

	protected:
        enum class MessageType { Info, Error };
        void showMessage(MessageType type, const char* msg, ...);

	    void sendComplete();

	private:
	    bool executeCommand(const std::vector<bare::String>&);

	    State _state = State::Connect;
		
		static constexpr uint32_t BufferSize = 200;
		char _buffer[BufferSize + 1];
		uint32_t _bufferIndex = 0;
	};

}
