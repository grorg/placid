# -------------------------------------------------------------------------
# This source file is a part of Placid
# 
# For the latest info, see http://www.marrin.org/
# 
# Copyright (c) 2018, Chris Marrin
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are met:
# 
#     - Redistributions of source code must retain the above copyright notice, 
#     this list of conditions and the following disclaimer.
#     
#     - Redistributions in binary form must reproduce the above copyright 
#     notice, this list of conditions and the following disclaimer in the 
#     documentation and/or other materials provided with the distribution.
#     
#     - Neither the name of the <ORGANIZATION> nor the names of its 
#     contributors may be used to endorse or promote products derived from 
#     this software without specific prior written permission.
#     
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
# POSSIBILITY OF SUCH DAMAGE.
# -------------------------------------------------------------------------

FLOATTYPE = FLOAT32

PLATFORMDIR = ESP

LIBS = ../baremetal/$(PLATFORMDIR)/build/baremetal-$(FLOATTYPE).a

CHIP = esp8266
BOARD = d1_mini
FLASH_DEF = 4M3M
BUILD_DIR = build
UPLOAD_PORT = /dev/cu.wchusbserial2430
UPLOAD_SPEED = 921600
BUILD_THREADS = 1
BUILD_EXTRA_FLAGS = -std=c++11

VERBOSE = 1

include ../baremetal/makeEspArduino.mk







INCLUDES = -I../baremetal
BUILDDIR = build
PRODUCTDIR = $(BUILDDIR)
PRODUCT = kernel
LOADER = loadmap.ld
SRCDIR = src

SRC =    kernel.S \
        main.cpp \
        Allocator.cpp \
        BootShell.cpp \
        FileSystem.cpp \
        Scanner.cpp \
        Shell.cpp \
        String.cpp \

all: checkdirs $(PRODUCTDIR)/$(PRODUCT).bin

include ../baremetal/Common.mk

clean : cleandir cleanlibs
