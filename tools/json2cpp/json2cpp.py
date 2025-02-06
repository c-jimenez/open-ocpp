# -*- coding: utf-8 -*-

'''
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
'''

import os
import sys
import argparse
import jinja2
import json

class Parameters(object):
    '''
        Tools parameters
    '''

    def __init__(self):
        '''
            Constructor
        '''

        self.input_dir = ""
        ''' Input directory '''

        self.output_dir = ""
        ''' Output directory '''

        self.templates_dir = ""
        ''' Code templates directory '''

        self.ocpp_version = ""
        ''' OCPP version '''

        self.types_dir = ""
        ''' Directory where the data types will be generated '''

        self.messages_dir = ""
        ''' Directory where the messages will be generated '''

class Type(object):
    '''
        OCPP type
    '''

    def __init__(self):
        '''
            Constructor
        '''

        self.name = ""
        ''' Name of the type '''

        self.basic_type = ""
        ''' Corresponding basic type '''

        self.comment = ""
        ''' Comment of the type definition '''

        self.requires = []
        ''' Dependencies of the type '''

        self.fields = {}
        ''' Fields composing the type '''

    def cpp_dependencies(self) -> list:
        '''
            Get the C++ dependencies corresponding to the type
        '''
        deps = []
        for field in self.fields.values():
            field_deps = field.cpp_dependencies()
            for field_dep in field_deps:
                if not field_dep in deps:
                    deps.append(field_dep)
        return deps

class TypeField(object):
    '''
        Field of an OCPP type
    '''

    def __init__(self):
        '''
            Constructor
        '''

        self.name = ""
        ''' Name of the field '''

        self.type = ""
        ''' Type of the field '''

        self.array_type = None
        ''' Underlying type of the array composing the field '''

        self.is_required = False
        ''' Indicate if the presence of the field is required '''

        self.min = None
        ''' Minimum value of the field '''

        self.max = None
        ''' Maximum value of the field '''

        self.comment = ""
        ''' Comment of the field '''

    def cpp_type(self, add_namespace : str) -> str:
        '''
            Get the C++ type corresponding to the type of the field
        '''
        type = self.get_cpp_type(self.type, add_namespace)
        if not self.is_required and not self.type == "array":
            type = f"ocpp::types::Optional<{type}>"
        return type
    
    def cpp_dependencies(self) -> list:
        '''
            Get the C++ dependencies corresponding to the type of the field
        '''
        deps = self.__cpp_dependencies(self.type)
        if not self.is_required and not self.type == "array":
            deps.append('"Optional.h"')
        return deps
    
    def get_cpp_type(self, type : str, add_namespace : str) -> str:
        '''
            Get the C++ type corresponding to the type of the field
        '''
        if type == "string":
            if self.max:
                return f"ocpp::types::CiStringType<{self.max}u>"
            else:
                return "std::string"
        elif type == "array":
            return f"std::vector<{self.get_cpp_type(self.array_type, add_namespace)}>"
        elif type == "integer":
            return "int"
        elif type == "boolean":
            return "bool"
        elif type == "number":
            return "float"
        elif type == "date-time":
            return "ocpp::types::DateTime"
        else:
            if not (len(add_namespace) == 0):
                return f"ocpp::types::{add_namespace}::{type}"
            else:
                return type

    def __cpp_dependencies(self, type : str) -> list:
        '''
            Get the C++ dependencies corresponding to the type of the field
        '''
        if type == "string":
            if self.max:
                return ['"CiStringType.h"']
            else:
                return ["<string>"]
        elif type == "array":
            deps = ["<vector>"]
            array_deps = self.__cpp_dependencies(self.array_type)
            for array_dep in array_deps:
                if not array_dep in deps:
                    deps.append(array_dep)
            return deps
        elif type == "date-time":
            return ['"DateTime.h"']
        else:
            return []

class Request(Type):
    '''
        OCPP request
    '''

    def __init__(self):
        '''
            Constructor
        '''

        self.associated_types = []
        ''' OCPP types associated with the request '''

class Message(Type):
    '''
        OCPP message
    '''

    def __init__(self):
        '''
            Constructor
        '''

        self.name = ""
        ''' Name of the message '''

        self.id = ""
        ''' Id of the message definition '''

        self.comment = ""
        ''' Comment of the message definition '''

        self.request = Request()
        ''' Request '''

        self.response = Request()
        ''' Response '''

        self.types = {}
        ''' Dictionnary of the request and response types '''

def check_dir_exists(dir, path) -> bool:
    ''' 
        Check if a directory exists

        @param dir: Name of the directory to check
        @type dir: string

        @param path: Path to the directory to check
        @type path: string

        @return: if the directory exists : True, if not : False
        @rtype: boolean
    '''
    
    ret = False
    if os.path.exists(path):
        if os.path.exists(path):
            ret = True
        else:
            print(f"{dir} must be a directory")
    else:
        print(f"{dir} does not exists")
    return ret

def check_args(args, params):
    '''
        Check the command line parameters

        @param args: command line parameters
        @type args: string[]

        @param params: parameter storage class
        @type params: Parameters

        @return: if the parameters are valid : True, if not : False
        @rtype: boolean
    '''

    # Retrieve the parameters
    parser_description = "json2cpp"
    parser = argparse.ArgumentParser(description=parser_description)
    parser.add_argument('-i', nargs=1, metavar='input_dir', required=True,
                        help='[string] Input directory')
    parser.add_argument('-o', nargs=1, metavar='output_dir', required=True,
                        help='[string] Output directory (must exists)')
    parser.add_argument('-t', nargs=1, metavar='templates_dir', required=False,
                        help='[string] Code templates directory')
    parser.add_argument('-v', nargs=1, metavar='ocpp_version', required=True,
                        help='[string] OCPP version (ocpp20 or ocpp21)')

    # Display the software inline help
    if (("-h" in args) or ("--help" in args)):
        parser.print_help()
        return False

    # Parse the parameters
    args = parser.parse_args()
    params.input_dir = args.i[0]
    params.output_dir = args.o[0]
    params.ocpp_version = args.v[0]
    if args.t:
        params.templates_dir = args.t[0]
    else:
        params.templates_dir = os.path.join(os.curdir, "templates")

    # Checks params
    return check_dir_exists("Input directory", params.input_dir) and check_dir_exists("Output directory", params.output_dir)

def read_code_templates(params) -> dict:
    '''
        Read the code templates

        @param params: Command line parameters
        @type params: Parameters

        @return: Content of the code template files if they exists, None otherwise
        @rtype: {string, string}
    '''

    templates = {}

    try:
        template_file_path = os.path.join(params.templates_dir, "types", "enum_header.template.j2")
        template_file = open(template_file_path, "rt")
        templates["enum_header"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "types", "enum_impl.template.j2")
        template_file = open(template_file_path, "rt")
        templates["enum_impl"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "types", "type_header.template.j2")
        template_file = open(template_file_path, "rt")
        templates["type_header"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "types", "type_impl.template.j2")
        template_file = open(template_file_path, "rt")
        templates["type_impl"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "messages", "msg_header.template.j2")
        template_file = open(template_file_path, "rt")
        templates["msg_header"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "messages", "msg_impl.template.j2")
        template_file = open(template_file_path, "rt")
        templates["msg_impl"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "messages", "MessagesConverter.cpp.j2")
        template_file = open(template_file_path, "rt")
        templates["msg_converter_impl"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "messages", "MessagesValidator.cpp.j2")
        template_file = open(template_file_path, "rt")
        templates["msg_validator_impl"] = template_file.read()


        template_file_path = os.path.join(params.templates_dir, "chargepoint", "IChargePoint.h.j2")
        template_file = open(template_file_path, "rt")
        templates["cs_ichargepoint"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "chargepoint", "IChargePointEventsHandler.h.j2")
        template_file = open(template_file_path, "rt")
        templates["cs_ichargepointeventshandler"] = template_file.read()

        
        template_file_path = os.path.join(params.templates_dir, "centralsystem", "ICentralSystem.h.j2")
        template_file = open(template_file_path, "rt")
        templates["csms_icentralsystem"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "centralsystem", "IChargePointRequestHandler.h.j2")
        template_file = open(template_file_path, "rt")
        templates["csms_ichargepointrequesthandler"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "centralsystem", "ChargePointProxy.h.j2")
        template_file = open(template_file_path, "rt")
        templates["csms_chargepointproxy_header"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "centralsystem", "ChargePointProxy.cpp.j2")
        template_file = open(template_file_path, "rt")
        templates["csms_chargepointproxy_impl"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "centralsystem", "ChargePointHandler.h.j2")
        template_file = open(template_file_path, "rt")
        templates["csms_chargepointhandler_header"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "centralsystem", "ChargePointHandler.cpp.j2")
        template_file = open(template_file_path, "rt")
        templates["csms_chargepointhandler_impl"] = template_file.read()


        template_file_path = os.path.join(params.templates_dir, "localcontroller", "ICentralSystemProxy.h.j2")
        template_file = open(template_file_path, "rt")
        templates["lc_icentralsystemproxy"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "localcontroller", "IChargePointProxy.h.j2")
        template_file = open(template_file_path, "rt")
        templates["lc_ichargepointproxy"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "localcontroller", "CentralSystemProxy.h.j2")
        template_file = open(template_file_path, "rt")
        templates["lc_centralsystemproxy_header"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "localcontroller", "CentralSystemProxy.cpp.j2")
        template_file = open(template_file_path, "rt")
        templates["lc_centralsystemproxy_impl"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "localcontroller", "CentralSystemHandler.h.j2")
        template_file = open(template_file_path, "rt")
        templates["lc_centralsystemhandler_header"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "localcontroller", "CentralSystemHandler.cpp.j2")
        template_file = open(template_file_path, "rt")
        templates["lc_centralsystemhandler_impl"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "localcontroller", "ChargePointProxy.h.j2")
        template_file = open(template_file_path, "rt")
        templates["lc_chargepointproxy_header"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "localcontroller", "ChargePointProxy.cpp.j2")
        template_file = open(template_file_path, "rt")
        templates["lc_chargepointproxy_impl"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "localcontroller", "ChargePointHandler.h.j2")
        template_file = open(template_file_path, "rt")
        templates["lc_chargepointhandler_header"] = template_file.read()

        template_file_path = os.path.join(params.templates_dir, "localcontroller", "ChargePointHandler.cpp.j2")
        template_file = open(template_file_path, "rt")
        templates["lc_chargepointhandler_impl"] = template_file.read()

    except IOError:
        print(f"Unable to open template file : {template_file_path}")
        templates = None

    return templates

def parse_type_contents(name, contents, ocpp_version_suffix) -> Type:
    '''
        Parse the contents of an OCPP type message

        @params name: Name of the OCPP type
        @type name: string

        @param contents: JSON definition of the OCPP type
        @type contents: json

        @param ocpp_version_suffix: Suffix of the OCPP version
        @type ocpp_version_suffix: string

        @return message: OCPP message type
        @rtype message: Message
    '''

    # Generic type data
    type = Type()
    type.name = f"{name}"
    type.basic_type = contents["type"]
    if "description" in contents:
        type.comment = contents["description"]    

    # Specific data
    if type.basic_type == "object":
        for property in contents["properties"]:
            property_definition = contents["properties"][property]

            field = TypeField()
            field.name = property
            if "description" in property_definition:
                field.comment = property_definition["description"]
            if "$ref" in property_definition:
                external_type = property_definition["$ref"]
                field.type = external_type[external_type.rfind("/") + 1:]
                type.requires.append(field.type)
            else:
                if "type" in property_definition:
                    field.type = property_definition["type"]
                else:
                    field.type = "string"
                if field.type == "string":
                    if "maxLength" in property_definition:
                        field.max = property_definition["maxLength"]
                    if "format" in property_definition:
                        if property_definition["format"] == "date-time":
                            field.type = "date-time"
                elif field.type == "array":
                    if "$ref" in property_definition["items"]:
                        external_type = property_definition["items"]["$ref"]
                        field.array_type = external_type[external_type.rfind("/") + 1:]
                        type.requires.append(field.array_type)
                    else:
                        field.array_type = property_definition["items"]["type"]
                else:
                    pass
            
            type.fields[property] = field

        if "required" in contents:
            for required_field in contents["required"]:
                type.fields[required_field].is_required = True

    elif type.basic_type == "string" and "enum" in contents:
        type.basic_type = "enum"
        for enum_value in contents["enum"]:
            type.fields[enum_value] = enum_value

    else:
        print(f"Unknown type : {type.basic_type}")
        pass

    return type



def parse_message_contents(message, contents, ocpp_version_suffix) -> None:
    '''
        Parse the contents of an OCPP message

        @param message: OCPP message data
        @type message: Message

        @param contents: JSON definition of the OCPP message
        @type contents: json

        @param ocpp_version_suffix: Suffix of the OCPP version
        @type ocpp_version_suffix: string
    '''

    # Types definition
    for type_name in contents["definitions"]:
        type_definition = contents["definitions"][type_name]
        type = parse_type_contents(type_name, type_definition, ocpp_version_suffix)
        message.associated_types.append(type)
    
    # Message
    message_type = parse_type_contents("", contents, ocpp_version_suffix)
    message.fields = message_type.fields
    message.requires = message_type.requires

    return

def gen_ocpp_enum(message, enum, templates, params, ocpp_version_suffix) -> None:
    ''' 
        Generate the code corresponding to an OCPP enum

        @param message: Associated OCPP message
        @type message: Message

        @param enum: OCPP enum
        @type enum: Type

        @param templates: Code templates
        @type templates: {string,string}

        @param params: Command line parameters
        @type params: Parameters

        @param ocpp_version_suffix: Suffix of the OCPP version
        @type ocpp_version_suffix: string
    '''

    # Generate header file
    enum_header_path = os.path.join(params.types_dir, enum.name + ocpp_version_suffix + ".h")
    enum_header = open(enum_header_path, "wt")

    env = jinja2.Environment()
    template = env.from_string(templates["enum_header"])
    rendered_template = template.render(message = message, enum = enum, ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)

    enum_header.write(rendered_template)
    enum_header.close()

    # Generate implementation file
    enum_impl_path = os.path.join(params.types_dir, enum.name + ocpp_version_suffix + ".cpp")
    enum_impl = open(enum_impl_path, "wt")

    env = jinja2.Environment()
    template = env.from_string(templates["enum_impl"])
    rendered_template = template.render(message = message, enum = enum, ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)

    enum_impl.write(rendered_template)
    enum_impl.close()

def gen_ocpp_type(message, type, other_types, templates, params, ocpp_version_suffix) -> None:
    ''' 
        Generate the code corresponding to an OCPP type

        @param message: Associated OCPP message
        @type message: Message

        @param type: OCPP type
        @type type: Type

        @param other_types: OCPP types
        @type other_types: {string,Type}

        @param templates: Code templates
        @type templates: {string,string}

        @param params: Command line parameters
        @type params: Parameters

        @param ocpp_version_suffix: Suffix of the OCPP version
        @type ocpp_version_suffix: string
    '''

    # Generate header file
    type_header_path = os.path.join(params.types_dir, type.name + ocpp_version_suffix + ".h")
    type_header = open(type_header_path, "wt")

    env = jinja2.Environment()
    template = env.from_string(templates["type_header"])
    rendered_template = template.render(message = message, type = type, ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)

    type_header.write(rendered_template)
    type_header.close()

    # Generate implementation file
    type_impl_path = os.path.join(params.types_dir, type.name + ocpp_version_suffix + ".cpp")
    type_impl = open(type_impl_path, "wt")

    env = jinja2.Environment()
    template = env.from_string(templates["type_impl"])
    rendered_template = template.render(message = message, type = type, other_types=other_types, ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)

    type_impl.write(rendered_template)
    type_impl.close()

def gen_ocpp_message(message, templates, params) -> None:
    ''' 
        Generate the code corresponding to an OCPP message

        @param message: Name of the OCPP message
        @type message: string

        @param templates: Code templates
        @type templates: {string,string}

        @param params: Command line parameters
        @type params: Parameters
    '''

    # Message object
    ocpp_version_suffix = params.ocpp_version[4:]

    msg = Message()
    msg.name = f"{message}"
    
    # Parse input files
    request = json.load(open(os.path.join(params.input_dir, f"{ocpp_message}Request.json")))
    parse_message_contents(msg.request, request, ocpp_version_suffix)
    response = json.load(open(os.path.join(params.input_dir, f"{ocpp_message}Response.json")))
    parse_message_contents(msg.response, response, ocpp_version_suffix)

    msg.id = request["$id"]
    msg.comment = request["comment"]
    msg.types = {f"{ocpp_message}Req": msg.request, f"{ocpp_message}Conf": msg.response}

    # Required types
    msg.requires = msg.request.requires
    for required_type in msg.response.requires:
        if not required_type in msg.requires:
            msg.requires.append(required_type)

    # Generate associated type
    associated_types = {}
    for type in msg.request.associated_types:
        associated_types[type.name] = type
    for type in msg.response.associated_types:
        associated_types[type.name] = type
    for associated_type in associated_types.values():
        if associated_type.basic_type == "enum":
            gen_ocpp_enum(msg, associated_type, templates, params, ocpp_version_suffix)
        else:
            gen_ocpp_type(msg, associated_type, associated_types, templates, params, ocpp_version_suffix)

    # Generate message header file
    msg_header_path = os.path.join(params.messages_dir, ocpp_message + ocpp_version_suffix + ".h")
    msg_header = open(msg_header_path, "wt")

    env = jinja2.Environment()
    template = env.from_string(templates["msg_header"])
    rendered_template = template.render(msg = msg, ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)

    msg_header.write(rendered_template)
    msg_header.close()

    # Generate implementation file
    msg_impl_path = os.path.join(params.messages_dir, ocpp_message + ocpp_version_suffix + ".cpp")
    msg_impl = open(msg_impl_path, "wt")

    env = jinja2.Environment()
    template = env.from_string(templates["msg_impl"])
    rendered_template = template.render(msg = msg, other_types = associated_types, ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)

    msg_impl.write(rendered_template)
    msg_impl.close()

    return

def gen_converters(templates, params, msg_list):
    ''' 
        Generate the code for the message converters

        @param templates: Code templates
        @type templates: {string,string}

        @param params: Command line parameters
        @type params: Parameters

        @param msg_list: List of OCPP messages per roles
        @type msg_list: {string, [string]}
    '''
  
    # Generate implementations
    ocpp_version_suffix = params.ocpp_version[4:]

    gen_file_path = os.path.join(params.messages_dir, f"MessagesConverter{ocpp_version_suffix}.cpp")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["msg_converter_impl"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    gen_file_path = os.path.join(params.messages_dir, f"MessagesValidator{ocpp_version_suffix}.cpp")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["msg_validator_impl"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

def gen_chargepoint(templates, params, msg_list):
    ''' 
        Generate the code for the charge point features

        @param templates: Code templates
        @type templates: {string,string}

        @param params: Command line parameters
        @type params: Parameters

        @param msg_list: List of OCPP messages per roles
        @type msg_list: {string, [string]}
    '''
    
    # Create output directories
    cs_dir = os.path.join(params.output_dir, "chargepoint")
    cs_dirs = [cs_dir]
    cs_dirs.append(os.path.join(cs_dir, "interface"))
    for dir in cs_dirs:
        if not os.path.exists(dir):
            os.mkdir(dir)

    # Generate interfaces
    cs_interface_dir = cs_dirs[1]
    ocpp_version_suffix = params.ocpp_version[4:]

    gen_file_path = os.path.join(cs_interface_dir, f"IChargePoint{ocpp_version_suffix}.h")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["cs_ichargepoint"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    gen_file_path = os.path.join(cs_interface_dir, f"IChargePointEventsHandler{ocpp_version_suffix}.h")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["cs_ichargepointeventshandler"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    return


def gen_centralsystem(templates, params, msg_list):
    ''' 
        Generate the code for the central system features

        @param templates: Code templates
        @type templates: {string,string}

        @param params: Command line parameters
        @type params: Parameters

        @param msg_list: List of OCPP messages per roles
        @type msg_list: {string, [string]}
    '''
    
    # Create output directories
    csms_dir = os.path.join(params.output_dir, "centralsystem")
    csms_dirs = [csms_dir]
    csms_dirs.append(os.path.join(csms_dir, "chargepoint"))
    csms_dirs.append(os.path.join(csms_dir, "interface"))
    for dir in csms_dirs:
        if not os.path.exists(dir):
            os.mkdir(dir)

    # Generate interfaces
    csms_interface_dir = csms_dirs[2]
    ocpp_version_suffix = params.ocpp_version[4:]

    gen_file_path = os.path.join(csms_interface_dir, f"ICentralSystem{ocpp_version_suffix}.h")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["csms_icentralsystem"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    gen_file_path = os.path.join(csms_interface_dir, f"IChargePointRequestHandler{ocpp_version_suffix}.h")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["csms_ichargepointrequesthandler"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    # Generate central system classes
    chargepoint_dir = csms_dirs[1]
    gen_file_path = os.path.join(chargepoint_dir, f"ChargePointProxy{ocpp_version_suffix}.h")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["csms_chargepointproxy_header"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    gen_file_path = os.path.join(chargepoint_dir, f"ChargePointProxy{ocpp_version_suffix}.cpp")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["csms_chargepointproxy_impl"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    gen_file_path = os.path.join(chargepoint_dir, f"ChargePointHandler{ocpp_version_suffix}.h")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["csms_chargepointhandler_header"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    gen_file_path = os.path.join(chargepoint_dir, f"ChargePointHandler{ocpp_version_suffix}.cpp")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["csms_chargepointhandler_impl"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    return


def gen_localcontroller(templates, params, msg_list):
    ''' 
        Generate the code for the local controller features

        @param templates: Code templates
        @type templates: {string,string}

        @param params: Command line parameters
        @type params: Parameters

        @param msg_list: List of OCPP messages per roles
        @type msg_list: {string, [string]}
    '''
    
    # Create output directories
    lc_dir = os.path.join(params.output_dir, "localcontroller")
    lc_dirs = [lc_dir]
    lc_dirs.append(os.path.join(lc_dir, "centralsystem"))
    lc_dirs.append(os.path.join(lc_dir, "chargepoint"))
    lc_dirs.append(os.path.join(lc_dir, "interface"))
    for dir in lc_dirs:
        if not os.path.exists(dir):
            os.mkdir(dir)

    # Generate interfaces
    lc_interface_dir = lc_dirs[3]
    ocpp_version_suffix = params.ocpp_version[4:]
    if params.ocpp_version == "ocpp20":
        ocpp_ws_protocol = "ocpp2.0.1"
    elif params.ocpp_version == "ocpp21":
        ocpp_ws_protocol = "ocpp2.0.1"
    else:
        ocpp_ws_protocol = "ocpp1.6"

    gen_file_path = os.path.join(lc_interface_dir, f"IChargePointProxy{ocpp_version_suffix}.h")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["lc_ichargepointproxy"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    gen_file_path = os.path.join(lc_interface_dir, f"ICentralSystemProxy{ocpp_version_suffix}.h")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["lc_icentralsystemproxy"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    # Generate central system classes
    centralsystem_dir = lc_dirs[1]
    gen_file_path = os.path.join(centralsystem_dir, f"CentralSystemProxy{ocpp_version_suffix}.h")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["lc_centralsystemproxy_header"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    gen_file_path = os.path.join(centralsystem_dir, f"CentralSystemProxy{ocpp_version_suffix}.cpp")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["lc_centralsystemproxy_impl"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix, ocpp_ws_protocol = ocpp_ws_protocol)
    gen_file.write(rendered_template)
    gen_file.close()

    gen_file_path = os.path.join(centralsystem_dir, f"CentralSystemHandler{ocpp_version_suffix}.h")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["lc_centralsystemhandler_header"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    gen_file_path = os.path.join(centralsystem_dir, f"CentralSystemHandler{ocpp_version_suffix}.cpp")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["lc_centralsystemhandler_impl"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    # Generate charge point classes
    chargepoint_dir = lc_dirs[2]
    gen_file_path = os.path.join(chargepoint_dir, f"ChargePointProxy{ocpp_version_suffix}.h")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["lc_chargepointproxy_header"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    gen_file_path = os.path.join(chargepoint_dir, f"ChargePointProxy{ocpp_version_suffix}.cpp")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["lc_chargepointproxy_impl"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    gen_file_path = os.path.join(chargepoint_dir, f"ChargePointHandler{ocpp_version_suffix}.h")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["lc_chargepointhandler_header"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    gen_file_path = os.path.join(chargepoint_dir, f"ChargePointHandler{ocpp_version_suffix}.cpp")
    gen_file = open(gen_file_path, "wt")
    env = jinja2.Environment()
    template = env.from_string(templates["lc_chargepointhandler_impl"])
    rendered_template = template.render(csms_msgs = msg_list["from_csms"], cs_msgs = msg_list["from_cs"], ocpp_version_namespace = params.ocpp_version, ocpp_version_suffix = ocpp_version_suffix)
    gen_file.write(rendered_template)
    gen_file.close()

    return



# Entry point
if __name__ == '__main__':

    ret = False

    try:

        # Check the command line parameters
        params = Parameters()
        if not check_args(sys.argv, params):
            sys.exit(-1)

        print(f"Creating output directories...")

        # Create output directories
        params.types_dir = os.path.join(params.output_dir, "types")
        params.messages_dir = os.path.join(params.output_dir, "messages")
        if not os.path.exists(params.types_dir):
            os.mkdir(params.types_dir)
        if not os.path.exists(params.messages_dir):
            os.mkdir(params.messages_dir)

        print(f"Generating files from {params.input_dir}")

        # Read messages list
        ocpp_version_suffix = params.ocpp_version[4:]
        msg_list_file_path = os.path.join(params.input_dir, "list.json")
        try:
            msg_list = json.load(open(msg_list_file_path, "rt"))
        except:
            print(f"Unable to load messages list file : {msg_list_file_path}")

        # Code templates
        templates = read_code_templates(params)
        if templates:

            # List OCPP messages
            ocpp_messages = [f[:f.find("Request.json")] for f in os.listdir(params.input_dir) if os.path.isfile(os.path.join(params.input_dir, f)) and f.endswith("Request.json")]
            print(f"{len(ocpp_messages)} messages found")

            # Generate messages
            gen_converters(templates, params, msg_list)
            for ocpp_message in ocpp_messages:
                print(f"Generating {ocpp_message} message...")
                gen_ocpp_message(ocpp_message,templates, params)

            # Generate charge point interfaces and classes
            gen_chargepoint(templates, params, msg_list)

            # Generate central system interfaces and classes
            gen_centralsystem(templates, params, msg_list)

            # Generate localcontroller interfaces and classes
            gen_localcontroller(templates, params, msg_list)

            ret = True

    except Exception as e:
        print(f"Unexpected error : {e}")
        ret = False

    if ret:
        sys.exit(0)
    else:
        sys.exit(-1)
