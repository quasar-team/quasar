'''
quasar_basic_utils.py

@author:     Piotr Nikiel <piotr@nikiel.info>

@copyright:  2019 CERN

@license:
Copyright (c) 2019, CERN
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact: quasar-developers@cern.ch
'''

def extract_argument(inData, key):
    """ If key is present in inData, will remove it and the following element from the list. 
            Returns a tuple of the output list (i.e. after removal of two elements) and the value of the element (i.e. the second of the two) """
    if key in inData:
        pos = inData.index(key)
        output = inData
        output.pop(pos)
        try:
            value = output.pop(pos)
        except IndexError:
            raise Exception ('Argument {0} requires to be followed by a value. Run out of arguments.'.format(key))
        return (output, value)
    else:  # nothing to do, argument not present
        return (inData, None)
