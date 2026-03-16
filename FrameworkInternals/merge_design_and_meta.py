#!/usr/bin/env python3
# encoding: utf-8
'''
merge_design_and_meta.py

@author:     Ben Farnham <ben.farnham@cern.ch>

@copyright:  2022 CERN

@license:
Copyright (c) 2015, CERN.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
   and the following disclaimer in the documentation and/or other materials provided with the
   distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS  OR
IMPLIED  WARRANTIES, INCLUDING, BUT NOT  LIMITED TO, THE IMPLIED WARRANTIES  OF  MERCHANTABILITY
AND  FITNESS  FOR  A  PARTICULAR  PURPOSE  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL, EXEMPLARY, OR  CONSEQUENTIAL
DAMAGES (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF  SUBSTITUTE GOODS OR  SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS  INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY  OF  LIABILITY,
WHETHER IN  CONTRACT, STRICT  LIABILITY,  OR  TORT (INCLUDING  NEGLIGENCE OR OTHERWISE)  ARISING IN
ANY WAY OUT OF  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact:    quasar-developers@cern.ch
'''
# see process_args() function for purpose of script

import sys
import logging
import argparse
from lxml import etree

QUASAR_NAMESPACES = {'d':'http://cern.ch/quasar/Design'}

class MergedDesign():
    """Encapsulates merge of user design + meta design files"""
    def __init__(self, user_file, meta_file):
        self.user_design = etree.parse(user_file)
        self.meta_design =  etree.parse(meta_file)
        self.merged_design = None

    def xpath(self, element, xpath_pattern):
        return element.xpath(xpath_pattern, namespaces=QUASAR_NAMESPACES)

    def merge_designs(self):
        """handles in-memory merge of user + meta designs"""
        logging.info(f"performing merge of user and meta designs...")

        # start with user design as base
        user_design_elements = self.xpath(self.user_design, '/d:design')
        if not len(user_design_elements) == 1:
            sys.exit(f"expected a single <design> element in user design file, found [{len(user_design_elements)}]. Merge failed - exiting")
        self.merged_design = user_design_elements[0]

        # merge meta classes
        meta_classes = self.xpath(self.meta_design, '/d:design/d:class')
        logging.info(f"user design child count [{len(self.merged_design)}] meta classes count [{len(meta_classes)}], merging...")
        for meta_class in meta_classes:
            logging.info(f"merging meta class [{meta_class.attrib['name']}]")
            self.merged_design.append(meta_class)
        logging.info(f"merged design child count [{len(self.merged_design)}]")

        # merge hasobjects - to instantiate StandardMetaData
        roots = self.xpath(self.merged_design, '/d:design/d:root')
        if not len(roots) ==1:
            sys.exit(f"expected a single <root> element in user design file, found [{len(roots)}]. Merge failed - exiting")
        standardmetadata_hasobjects_list = self.xpath(self.meta_design, '/d:design/d:root/d:hasobjects')
        if not len(standardmetadata_hasobjects_list) == 1:
            sys.exit(f"expected a single <design/root/hasobjects> element in meta design file, found [{len(standardmetadata_hasobjects_list)}]. Merge failed - exiting")
        roots[0].append(standardmetadata_hasobjects_list[0])
        logging.debug(f"merged design [\n{etree.tostring(self.merged_design, pretty_print=True)}\n]")

    def write_to_file(self, output_file):
        """writes merged design file (user + meta) to disk"""
        logging.info(f"writing file to [{output_file}]")
        content = etree.ElementTree(element=self.merged_design)
        xml_string = etree.tostring(content, encoding='utf-8', method='xml', pretty_print=True, xml_declaration=True)
        logging.debug(f"content of XML file [\n{xml_string.decode('utf-8')}\n]")
        output_file.write(xml_string.decode('utf-8'))


def merge_user_and_meta_design(user_file, meta_file, merged_file):
    merged = MergedDesign(user_file, meta_file)
    merged.merge_designs()
    merged.write_to_file(merged_file)

def process_args():
    """process command line args if file run as stand alone script"""
    logging.info(f"{__file__} processing command line args...")
    parser = argparse.ArgumentParser(description="""meta_build_info.py merges 
    1. Input: Design/Design.xml - the end-user quasar design file
    2. Input: Meta/design/meta-design.xml - quasar internal file; an XML snippet describing StandardMetaData classes
    and outputs
    3. Output: XML file containing merged contents
    Requires 3 args - absolute paths all 3 input and output files above""", formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('--user_file',   required=True, metavar="FILE", type=argparse.FileType(mode='r', encoding='utf-8'), help='absolute path to user design XML file')
    parser.add_argument('--meta_file',   required=True, metavar="FILE", type=argparse.FileType(mode='r', encoding='utf-8'), help='absolute path to meta design XML file')
    parser.add_argument('--merged_file', required=True, metavar="FILE", type=argparse.FileType(mode='w', encoding='utf-8'), help='absolute path to output XML file (NB: overwrites)')
    args = parser.parse_args()
    logging.info(f"{__file__} merging user design [{args.user_file}] meta design [{args.meta_file}] output to [{args.merged_file}]")
    return args

# logging.getLogger().setLevel(logging.INFO)
# logging.info(f"Running {__file__}")
# args = process_args()

# merge_user_and_meta_design(args.user_file, args.meta_file, args.merged_file)