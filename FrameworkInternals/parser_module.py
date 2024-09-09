# encoding: utf-8
'''
parser_module.py

@author:     Paris Moschovakos <paris.moschovakos@cern.ch>

@copyright:  2023 CERN

@license:
Copyright (c) 2015, CERN.
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

import argparse
import sys

class QuasarArgsHelpFormatter(argparse.HelpFormatter):
    """
    Corrected _max_action_length for the indenting of subactions
    """

    def add_argument(self, action):
        if action.help is not argparse.SUPPRESS:

            # find all invocations
            get_invocation = self._format_action_invocation
            invocations = [get_invocation(action)]
            current_indent = self._current_indent
            for subaction in self._iter_indented_subactions(action):
                # compensate for the indent that will be added
                indent_chg = self._current_indent - current_indent
                added_indent = 'x'*indent_chg
                invocations.append(added_indent+get_invocation(subaction))

            # update the maximum item length
            invocation_length = max([len(s) for s in invocations])
            action_length = invocation_length + self._current_indent
            self._action_max_length = max(self._action_max_length,
                                          action_length)

            # add the item to the list
            self._add_item(self._format_action, [action])

class ArgumentParser(argparse.ArgumentParser):

    def error(self, message):
        pass

def quasar_parser():
    """
    Returns the parser for the quasar command line interface
    """

    def formatter_class(prog): return QuasarArgsHelpFormatter(
        prog, max_help_position=30, width=150)

    parser = ArgumentParser(
        prog='quasar.py',
        description='The quick opcua server generation framework',
        epilog="For more information visit https://quasar.docs.cern.ch/quasarCommands.html",
        # exit_on_error=False, # Doesn't work for Python 3.6 but works for Python 3.9
        formatter_class=formatter_class)
    subparsers = parser.add_subparsers(
        title='quasar commands', metavar="<command>")

    # create the parser for the "build_config" command
    prepare_build = subparsers.add_parser(
        'prepare_build', help='Warning: used to be user\'s command but its future is debatable - don’t use')
    # create the parser for the "generate" command
    generate_parser = subparsers.add_parser(
        'generate', help='Run with -h to see more options')
    generate_subparsers = generate_parser.add_subparsers(
        title='quasar generate commands', metavar="<generate command>")
    # create the parser for the "generate device" command
    generate_device_subparser = generate_subparsers.add_parser(
        'device', help='Run with -h to see more options')
    generate_device_subparser.add_argument('--all', type=str, nargs='?',
                                            help='Equal to “generate device” (above) run with all classes with device logic from the Design')
    generate_device_subparser.add_argument(
        'quasar class', type=str, nargs='*', help='Generates the Device Logic files D<classname>.h and D<classname>.cpp')
    # create the parser for the "generate config_doc" command
    generate_config_doc_subparser = generate_subparsers.add_parser(
        'config_doc', help='Generates documentation of the Configuration Schema. Such a documentation is a HTML document with description of all fields that might appear in the configuration')
    generate_config_doc_subparser = generate_subparsers.add_parser(
        'config_doc_md', help='Generates documentation of the Configuration Schema. Such a documentation is a Markdown document with description of all fields that might appear in the configuration')
    # create the parser for the "generate as_doc" command
    generate_as_doc_subparser = generate_subparsers.add_parser(
        'as_doc', help='Generates address space documentation in a HTML document (the documentation of all OPCUA ObjectTypes, Variables, etc… ) and places it in Documentation/AddressSpaceDoc.html')
    generate_as_doc_subparser = generate_subparsers.add_parser(
        'as_doc_md', help='Generates address space documentation in a markdown document (the documentation of all OPCUA ObjectTypes, Variables, etc… ) and places it in Documentation/AddressSpaceDoc.md')
    # create the parser for the "generate diagram" command
    generate_diagram_subparser = generate_subparsers.add_parser(
        'diagram', help='Creates a quasar design diagram for this project')
    generate_diagram_subparser.add_argument('detailLevel', type=int, nargs='?', choices=range(
        0, 4), default=0, help='When the detailLevel is increased you see more and more information (default: %(default)s)')
    generate_diagram_subparser.add_argument('mode', type=str, nargs='?', choices=[
                                            'dot', 'fdp', 'circo'], default='dot', help='mode is the hierarchical representation used by graphviz (default: %(default)s)')
    # create the parser for the "setup_svn_ignore" command
    setup_svn_ignore_parser = subparsers.add_parser(
        'setup_svn_ignore', help='Setups the .svnignore file, so the generated files will be ignored in your svn repository')
    # create the parser for the "build" command
    build_parser = subparsers.add_parser(
        'build', help='This is the main command to build the project (a quasar-based OPC UA server)')
    build_parser.add_argument('--builder', type=str, nargs=1, choices=[
                              'Ninja'], default='', help='builder to use (default: Platform specific)')
    build_parser.add_argument('buildType', type=str, nargs="?", choices=[
                              'Debug', 'Release'], default='Release', help='build type (default: %(default)s)')
    # create the parser for the "clean" command
    clean_parser = subparsers.add_parser(
        'clean', help='Deprecated because quasar does out-of-source builds by default for long time. Simply remove your build directory')
    # create the parser for the "create_project" command
    create_project_subparser = subparsers.add_parser(
        'create_project', help='Creates a new quasar project at path given as destination')
    create_project_subparser.add_argument(
        'destination', type=str, nargs=1, help='Destination path')
    # create the parser for the "upgrade_project" command
    upgrade_project_subparser = subparsers.add_parser(
        'upgrade_project', help='Upgrades the framework in a given target directory (where a quasar project already is)')
    upgrade_project_subparser.add_argument(
        'destination', type=str, nargs=1, help='Destination path')
    # create the parser for the "upgrade_design" command
    upgrade_design_subparser = subparsers.add_parser(
        'upgrade_design', help='upgrade_design is used to cover backwards-incompatible Design changes between different versions of quasar')
    upgrade_design_subparser.add_argument('additionalParam', type=str, nargs=1, choices=['convert_to_hasDeviceLogic=yes', 'add_nullPolicy=nullAllowed', 'add_nullPolicy=nullForbidden'],
                                          help='convert_to_hasDeviceLogic=yes when you are upgrading from quasar older than 1.1.0, add_nullPolicy=nullAllowed or add_nullPolicy=nullForbidden when you are upgrading from quasar older than 0.96')
    # create the parser for the "format_design" command
    format_design_parser = subparsers.add_parser(
        'format_design', help='Formats/styles Design.xml. The backup is left as Design.xml.backup')
    # create the parser for the "validate_design" command
    validate_design_parser = subparsers.add_parser(
        'validate_design', help='Validates your Design using built-in Design Validator')
    # create the parser for the "doxygen" command
    doxygen_parser = subparsers.add_parser(
        'doxygen', help='Runs doxygen in the documentation folder, by default profiting from quasar-supplied Doxyfile')
    # create the parser for the "external_tool_check" command
    external_tool_check_parser = subparsers.add_parser(
        'external_tool_check', help='Checks dependencies of quasar')
    # create the parser for the "enable_module" command
    enable_module_parser = subparsers.add_parser(
        'enable_module', help='Enables optional module from quasar’s optional modules catalogue')
    enable_module_parser.add_argument(
        'moduleName', type=str, help='The name of the module to enable. You can list available modules by running "list_modules"')
    enable_module_parser.add_argument('tag', type=str, nargs='?', default='master',
                                      help='The tag or branch to checkout. If not specified, the master branch will be used')
    enable_module_parser.add_argument('serverString', type=str, nargs='?', default='https://github.com',
                                      help='The git server to use. The default is "https://github.com"')
    # create the parser for the "disable_module" command
    disable_module_parser = subparsers.add_parser(
        'disable_module', help='Disables optional module. Module files will be deleted')
    disable_module_parser.add_argument(
        'moduleName', type=str, help='The name of the module to disable')
    # create the parser for the "list_modules" command
    list_modules_parser = subparsers.add_parser(
        'list_modules', help='List all officially registered quasar optional modules')
    # create the parser for the "list_enabled_modules" command
    list_enabled_modules_parser = subparsers.add_parser(
        'list_enabled_modules', help='List all optional modules that were enabled for your project')
    # create the parser for the "build_config" command
    build_config_parser = subparsers.add_parser(
        'build_config', help='Print currently chosen build configuration file, see also set_build_config')
    # create the parser for the "set_build_config" command
    set_build_config_parser = subparsers.add_parser(
        'set_build_config', help='Sets the build configuration file to be used for the build')
    set_build_config_parser.add_argument(
        'build_config', type=str, nargs=1, help='Build config file path')
    # create the parser for the "symlink_runtime_deps" command
    symlink_runtime_deps_parser = subparsers.add_parser(
        'symlink_runtime_deps', help='Symlinks the runtime dependencies of the project to the binary directory')
    symlink_runtime_deps_parser.add_argument('wildcard', type=str, nargs='?', default='',
                                             help='Symlinks ServerConfig.xml and config*.xml or user specific wildcard-matches from bin/ to build/bin/ (NOTE: don’t forget to escape asterisk if you use it by backslash!)')
    # create the parser for the "quasar_version" command
    quasar_version_parser = subparsers.add_parser(
        'quasar_version', help='Prints currently deployed quasar version')
    # create the parser for the "style_it" command
    style_it_parser = subparsers.add_parser(
        'style_it', help='Runs source code styler for listed files (or --device)')
    style_it_parser.add_argument(
        'files', type=str, nargs='*', help='File paths to run source code styler on')
    style_it_parser.add_argument(
        '--device', type=str, nargs='?', help='Style all device logic files')

    return parser
