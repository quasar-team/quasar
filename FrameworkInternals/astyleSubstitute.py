'''
@author: Piotr Nikiel <piotr@nikiel.info>

@copyright:  2017 CERN

@license:
Copyright (c) 2017, CERN
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

'''
import sys  # for the command line args


def do_indentation(input_file_name):    

    in_file = file(input_file_name, 'r')
    
    current_indent = 0
    
    basic_tab = '    '  # here you put what you'd like to do
    
    input_lines = in_file.readlines()
    in_file.close()
    
    previous_line = ''
    
    out_file = file(input_file_name, 'w')
    out_file.write('/* This file was indented using fall-back indent tool, because we failed to run astyle program */\n')
    out_file.write('/* Please obtain astyle (there are packages for almost any Linux) for highest user satisfaction */\n')
    
    for line in input_lines:
        if line.isspace() and previous_line.isspace():
            continue
        #  closing bracket should shrink indent BEFORE why opening bracket AFTER
        if current_indent>0:
            current_indent -= line.count('}')
        
        line = basic_tab*current_indent + line.lstrip()
        if not 'namespace' in previous_line:  # for compatibility with existing code, don't indent namespace declarations
            num_brackets = line.count('{')
            if num_brackets > 0:
                current_indent += num_brackets    
        if len(line)>0:
            out_file.write(line)
            if line[-1] != '\n':
                out_file.write('\n')
            
        else:
            if len(previous_line)>0:
                out_file.write('\n')
        previous_line = line
        
    out_file.close()

if __name__ == "__main__":
    do_indentation(sys.argv[1])