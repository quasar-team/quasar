#!/usr/bin/env python3
# encoding: utf-8
'''
transform_filters.py

@author:     Piotr Nikiel <piotr@nikiel.info>

@copyright:  2020 CERN

@license:
Copyright (c) 2020, CERN
All rights reserved.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS  OR IMPLIED  WARRANTIES, INCLUDING, BUT NOT  LIMITED TO, THE IMPLIED
WARRANTIES  OF  MERCHANTABILITY  AND  FITNESS  FOR  A  PARTICULAR  PURPOSE  ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL, EXEMPLARY, OR  CONSEQUENTIAL DAMAGES
(INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF  SUBSTITUTE GOODS OR  SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS  INTERRUPTION) HOWEVER CAUSED AND ON
ANY  THEORY  OF  LIABILITY,   WHETHER IN  CONTRACT, STRICT  LIABILITY,  OR  TORT
(INCLUDING  NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT OF  THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact:    quasar-developers@cern.ch
'''

import lxml.etree
import re
from colorama import Fore, Style

def cap_first(word):
    """Capitalizes first letter of the string.
    Often used to generate setCapitalCase and other verb methods"""
    return word[0].upper() + word[1:]

def cpp_comments_to_cmake_comments(block):
    """Used to 'translate' C++ style block comments into CMake comments"""
    ## TODO @pnikiel consider sth better ;-)
    return '\n'.join(['# '+line for line in block.split('\n')])

def template_debug(*args):
    """This filter is used to print debug messages from a Jinja transform"""
    print(Fore.MAGENTA + ' '.join([str(x) for x in args]) + Style.RESET_ALL)
    return '' # Important, it's a no-output Jinja filter!!

def sorted_by_objectified_attr(items, attribute):
    """Sorts with the key being chosen attribute of lxml-objectified node"""
    return sorted(items, key=lambda x: x.get(attribute))

def node_text_contents_to_string(node):
    """Returns text equivalent to TEXT of given element, assuming processing was skipped"""
    text = lxml.etree.tostring(node).decode()
    # the following regex will generate two groups named "tag" (less relevant here) and "text" that
    # will match TEXT of given node, like if processing of children elements was skipped
    regex = re.compile(r"<(?P<tag>\S+)[^>]*>(?P<text>.*)</(?P=tag)>", re.DOTALL)
    match = regex.match(text)
    return match.group('text') if match else '' # might not have the text field.

def setup_all_filters(jinja_environment):
    """Decorates jinja2.Environment object attaching custom filters"""
    jinja_environment.filters['capFirst'] = cap_first
    jinja_environment.filters['debug'] = template_debug
    jinja_environment.filters['cppCommentsToCmakeComments'] = cpp_comments_to_cmake_comments
    jinja_environment.filters['sorted_by_objectified_attr'] = sorted_by_objectified_attr
    jinja_environment.filters['node_text_contents_to_string'] = node_text_contents_to_string
    jinja_environment.globals['debug'] = template_debug
