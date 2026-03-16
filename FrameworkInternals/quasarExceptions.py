#!/usr/bin/env python3
# encoding: utf-8
'''
quasarExceptions.py

@author:     Piotr Nikiel <piotr@nikiel.info>

@copyright:  2018 CERN

@license:
Copyright (c) 2018, CERN
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact:    quasar-developers@cern.ch
'''

class WrongArguments(Exception):
    def __init__(self, desc):
        Exception.__init__(self, desc)

class WrongReturnValue(Exception):
    def __init__(self, tool, return_value):
        Exception.__init__(self, 'WrongReturnValue: tool {tool} returned {rv}'.format(
                tool=tool,
                rv=str(return_value)
                ))

class Mistake(Exception):
    def __init__(self, desc):
        Exception.__init__(self, desc)

class DesignFlaw(Exception):
    def __init__(self, desc):
        Exception.__init__(self, desc)
