'''
quasar_utils.py

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


import sys
import argparse
import io

def amalgamate (args):
    print('Amalgamating {0} files as {1}'.format(len(args.inputs), args.output))
    line_counter = 0
    f_output = io.open(args.output, 'w', encoding='utf-8')
    for path in args.inputs:
        f_in = io.open(path, 'r', encoding='utf-8')
        for line in f_in:
            f_output.write(line)
            line_counter += 1
    f_output.close()
    print('Wrote {0} amalgamated lines'.format(line_counter))

parser = argparse.ArgumentParser(description="Quasar BusyBox")

subparsers = parser.add_subparsers()

parser_amalgamate = subparsers.add_parser('amalgamate')
parser_amalgamate.add_argument('output', type=str, help='path to the amalgamated file to be created')
parser_amalgamate.add_argument('inputs', type=str, nargs='+', help='paths of input files')
parser_amalgamate.set_defaults(func = amalgamate)

args = parser.parse_args()
args.func(args)
