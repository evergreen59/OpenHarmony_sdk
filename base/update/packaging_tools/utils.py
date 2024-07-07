#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (c) 2021 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""
Description : Defining constants, common interface
"""
import argparse
import json
import os
import shutil
import tempfile
import zipfile
from collections import OrderedDict
from copy import copy
from ctypes import cdll
from cryptography.hazmat.primitives import hashes
from log_exception import UPDATE_LOGGER
import xmltodict
from build_pkcs7 import sign_ota_package

operation_path = os.path.dirname(os.path.realpath(__file__))
PRODUCT = 'hi3516'
BUILD_TOOLS_FILE_NAME = 'build_tools.zip'
UPDATE_EXE_FILE_NAME = "updater_binary"

SCRIPT_KEY_LIST = ['prelude', 'verse', 'refrain', 'ending']
TOTAL_SCRIPT_FILE_NAME = "loadScript.us"
REGISTER_SCRIPT_FILE_NAME = "registerCmd.us"
SCRIPT_FILE_NAME = '-script.us'

UPDATER_CONFIG = "updater_config"
XML_FILE_PATH = "updater_specified_config.xml"
SO_PATH = os.path.join(operation_path, 'lib/libpackage.so')
SO_PATH_L1 = os.path.join(operation_path, 'lib/libpackageL1.so')
DIFF_EXE_PATH = os.path.join(operation_path, 'lib/diff')
E2FSDROID_PATH = os.path.join(operation_path, 'lib/e2fsdroid')
MISC_INFO_PATH = "misc_info.txt"
VERSION_MBN_PATH = "VERSION.mbn"
BOARD_LIST_PATH = "BOARD.list"
EXTEND_COMPONENT_LIST = ["version_list", "board_list"]
EXTEND_OPTIONAL_COMPONENT_LIST = ["partitions_file"]
PARTITION_FILE = "partitions_file"
IGNORED_PARTITION_LIST = ['fastboot', 'boot', 'kernel', 'misc',
                          'updater', 'userdata']

HASH_ALGORITHM_DICT = {'sha256': hashes.SHA256, 'sha384': hashes.SHA384}
LINUX_HASH_ALGORITHM_DICT = {'sha256': 'sha256sum', 'sha384': 'sha384sum'}
HASH_CONTENT_LEN_DICT = {'sha256': 64, 'sha384': 96}

COMPONENT_INFO_INNIT = ['', '000', '00', '0', '0o00']

ON_SERVER = "ON_SERVER"

EXTEND_VALUE = 512

FILE_MAP_ZERO_KEY = "__ZERO"
FILE_MAP_NONZERO_KEY = "__NONZERO"
FILE_MAP_COPY_KEY = "__COPY"

MAX_BLOCKS_PER_GROUP = BLOCK_LIMIT = 1024
PER_BLOCK_SIZE = 4096
TWO_STEP = "updater"

VERSE_SCRIPT_EVENT = 0
INC_IMAGE_EVENT = 1
SIGN_PACKAGE_EVENT = 2

# Image file can not support update.
FORBIDEN_UPDATE_IMAGE_LIST = ["updater_boot", "updater_b", "ptable"]

# 1000000: max number of function recursion depth
MAXIMUM_RECURSION_DEPTH = 1000000


def singleton(cls):
    _instance = {}

    def _singleton(*args, **kargs):
        if cls not in _instance:
            _instance[cls] = cls(*args, **kargs)
        return _instance[cls]

    return _singleton


class ExtInit:
    """
    Init event for ext
    """

    def __init__(self):
        self.funs = []

    def reg_event(self, evevt_id, funs):
            self.funs.append([evevt_id, funs])
            UPDATE_LOGGER.print_log(
                'register event %s: %s' % (evevt_id, funs.__name__))

    def invoke_event(self, evevt_id):
        UPDATE_LOGGER.print_log(self.funs)
        for event in self.funs:
            funs = event[1]
            if event[0] == evevt_id and funs is not None:
                UPDATE_LOGGER.print_log(
                    'invoke event %s: %s' % (evevt_id, funs.__name__))
                return funs
        return False


@singleton
class OptionsManager:
    """
    Options management class
    """

    def __init__(self):
        self.init = ExtInit()
        self.parser = argparse.ArgumentParser()

        # Own parameters
        self.product = None

        # Entry parameters
        self.source_package = None
        self.target_package = None
        self.update_package = None
        self.unpack_package_path = None
        self.no_zip = False
        self.partition_file = None
        self.signing_algorithm = None
        self.hash_algorithm = None
        self.private_key = None
        self.not_l2 = False
        self.signing_length = 256
        self.xml_path = None
        self.sd_card = False

        self.make_dir_path = None

        # Parsed package parameters
        self.target_package_dir = None
        self.target_package_config_dir = None
        self.target_package_temp_obj = None
        self.misc_info_dict = {}
        self.version_mbn_file_path = None
        self.version_mbn_content = None
        self.board_list_file_path = None
        self.board_list_content = None

        self.source_package_dir = None
        self.source_package_temp_obj = None

        # XML parsing parameters
        self.head_info_list = []
        self.component_info_dict = {}
        self.full_img_list = []
        self.full_img_name_list = []
        self.incremental_img_list = []
        self.incremental_img_name_list = []
        self.target_package_version = None
        self.source_package_version = None
        self.two_step = False
        self.full_image_path_list = []

        self.partition_file_obj = None

        # Full processing parameters
        self.full_image_content_len_list = []
        self.full_image_file_obj_list = []

        # Incremental processing parameters
        self.incremental_content_len_list = []
        self.incremental_image_file_obj_list = []
        self.incremental_temp_file_obj_list = []
        self.src_image = None
        self.tgt_image = None

        # Script parameters
        self.opera_script_file_name_dict = {}
        for each in SCRIPT_KEY_LIST:
            self.opera_script_file_name_dict[each] = []
        self.total_script_file_obj = None

        self.register_script_file_obj = None

        # Update package parameters
        self.update_bin_obj = None
        self.build_tools_zip_obj = None
        self.update_package_file_path = None
        self.signed_package = None


OPTIONS_MANAGER = OptionsManager()


def unzip_package(package_path, origin='target'):
    """
    Decompress the zip package.
    :param package_path: zip package path
    :param origin: package origin, which indicates
                whether the zip package is a source package or target package
    :return: Temporary directory (tmp_dir) and zip_data package;
            false if an exception occurs.
    """
    try:
        tmp_dir_obj = tempfile.TemporaryDirectory(prefix="%sfiles-" % origin)
        tmp_dir = tmp_dir_obj.name

        zf_obj = zipfile.ZipFile(package_path)
        for name in zf_obj.namelist():
            if name.endswith('/'):
                os.mkdir(os.path.join(tmp_dir, name))
            else:
                ext_filename = os.path.join(
                    tmp_dir, name)
                with open(ext_filename, 'wb') as f_w:
                    f_w.write(zf_obj.read(name))
    except OSError:
        UPDATE_LOGGER.print_log(
            "Unzip package failed! path: %s" % package_path,
            log_type=UPDATE_LOGGER.ERROR_LOG)
        return False, False
    tmp_dir_list = os.listdir(tmp_dir)
    if len(tmp_dir_list) == 1:
        unzip_dir = os.path.join(tmp_dir, tmp_dir_list[0])
        if UPDATER_CONFIG not in \
                os.listdir(unzip_dir):
            UPDATE_LOGGER.print_log(
                'Unsupported zip package structure!', UPDATE_LOGGER.ERROR_LOG)
            return False, False
    elif UPDATER_CONFIG in tmp_dir_list:
        unzip_dir = tmp_dir
    else:
        UPDATE_LOGGER.print_log(
            'Unsupported zip package structure!', UPDATE_LOGGER.ERROR_LOG)
        return False, False
    UPDATE_LOGGER.print_log(
        '%s package unzip complete! path: %s' % (origin.title(), unzip_dir))

    return tmp_dir_obj, unzip_dir


def split_img_name(image_path):
    """
    Split the image name by image path
    :return image name
    """
    tmp_path = image_path
    str_list = tmp_path.split("/")

    return str_list[-1]


def get_update_config_softversion(mbn_dir, head_info_dict):
    soft_version_file = head_info_dict.get('softVersionFile')
    if soft_version_file is not None:
        mbn_path = os.path.join(mbn_dir, soft_version_file)
        if os.path.exists(mbn_path):
            with open(mbn_path, 'r') as mbn_file:
                head_info_dict['info']["@softVersion"] = mbn_file.read()


def parse_update_config(xml_path):
    """
    Parse the XML configuration file.
    :param xml_path: XML configuration file path
    :return head_info_dict: header information dict of the update package
            component_info_dict: component information dict
            full_img_list: full image list
            incremental_img_list: incremental image list
    """
    two_step = False
    if os.path.exists(xml_path):
        with open(xml_path, 'r') as xml_file:
            xml_str = xml_file.read()
    else:
        UPDATE_LOGGER.print_log("XML file does not exist! xml path: %s" %
                                xml_path, UPDATE_LOGGER.ERROR_LOG)
        ret_params = [False, False, False, False, False, False, False]
        return ret_params
    xml_content_dict = xmltodict.parse(xml_str, encoding='utf-8')
    package_dict = xml_content_dict.get('package', {})
    get_update_config_softversion(OPTIONS_MANAGER.target_package_dir, package_dict.get('head', {}))
    head_dict = package_dict.get('head', {}).get('info')
    package_version = head_dict.get("@softVersion")
    # component
    component_info = package_dict.get('group', {}).get('component')
    head_list = list(head_dict.values())
    head_list.pop()
    whole_list = []
    difference_list = []
    component_dict = {}
    full_image_path_list = []

    if not OPTIONS_MANAGER.not_l2:
        expand_component(component_dict)
    if isinstance(component_info, OrderedDict):
        component_info = [component_info]
    if component_info is None:
        ret_params = [[], {}, [], [], '', [], False]
        return ret_params
    for component in component_info:
        component_list = list(component.values())
        component_list.pop()
        component_dict[component['@compAddr']] = component_list

        if component['@compAddr'] in (whole_list + difference_list):
            UPDATE_LOGGER.print_log("This component %s  repeats!" %
                                    component['@compAddr'],
                                    UPDATE_LOGGER.ERROR_LOG)
            ret_params = [False, False, False, False, False, False, False]
            return ret_params

        if component['@compType'] == '0':
            whole_list.append(component['@compAddr'])
            OPTIONS_MANAGER.full_img_name_list.\
                append(split_img_name(component['#text']))
            tem_path = os.path.join(OPTIONS_MANAGER.target_package_dir,
                                    component.get("#text", None))
            full_image_path_list.append(tem_path)
        elif component['@compType'] == '1':
            difference_list.append(component['@compAddr'])
            OPTIONS_MANAGER.incremental_img_name_list.\
                append(split_img_name(component['#text']))

        if component['@compAddr'] == TWO_STEP:
            two_step = True

    UPDATE_LOGGER.print_log('XML file parsing completed!')
    ret_params = [head_list, component_dict,
                  whole_list, difference_list, package_version,
                  full_image_path_list, two_step]
    return ret_params


def partitions_conversion(data):
    """
    Convert the start or length data in the partition table through
    multiply 1024 * 1024 and return the data.
    :param data: start or length data
    :return :
    """
    if data == '0':
        return 0
    elif data.endswith('M'):
        return int(data[0:-1]) * 1024 * 1024 // 512
    else:
        return False


def parse_partition_file_xml(xml_path):
    """
    Parse the XML configuration file.
    :param xml_path: XML configuration file path
    :return part_json: partition table information in JSON format
    """
    if os.path.exists(xml_path):
        with open(xml_path, 'r') as xml_file:
            xml_str = xml_file.read()
    else:
        UPDATE_LOGGER.print_log("XML file does not exist! xml path: %s" %
                                xml_path, UPDATE_LOGGER.ERROR_LOG)
        return False, False, False
    partitions_list = []
    partitions_file_path_list = []
    xml_content_dict = xmltodict.parse(xml_str, encoding='utf-8')
    part_list = xml_content_dict['Partition_Info']['Part']
    new_part_list = []
    for i, part in enumerate(part_list):
        start_value = partitions_conversion(part.get('@Start'))
        length_value = partitions_conversion(part.get('@Length'))
        if start_value is False or length_value is False:
            UPDATE_LOGGER.print_log(
                "Partition file parsing failed! part_name: %s, xml_path: %s" %
                (part.get('@PartitionName'), xml_path),
                UPDATE_LOGGER.ERROR_LOG)
            return False, False, False

        if part.get('@PartitionName') not in IGNORED_PARTITION_LIST:
            partitions_list.append(part.get('@PartitionName'))
            partitions_file_path_list.append(
                os.path.join(OPTIONS_MANAGER.target_package_dir,
                             "%s.img" % part.get('@PartitionName')))
        part_dict = {'start': start_value,
                     'length': length_value,
                     'partName': part.get('@PartitionName'),
                     'fsType': part.get('@FlashType')}
        new_part_list.append(part_dict)
    part_json = json.dumps(new_part_list)
    part_json = '{"Partition": %s}' % part_json
    file_obj = tempfile.NamedTemporaryFile(
        dir=OPTIONS_MANAGER.target_package_dir, prefix="partition_file-", mode='wb')
    file_obj.write(part_json.encode())
    file_obj.seek(0)
    return file_obj, partitions_list, partitions_file_path_list


def expand_component(component_dict):
    """
    Append components such as VERSION.mbn and board list.
    :param component_dict: component information dict
    :return:
    """
    if OPTIONS_MANAGER.partition_file is not None:
        extend_component_list = \
            EXTEND_COMPONENT_LIST + EXTEND_OPTIONAL_COMPONENT_LIST
    else:
        extend_component_list = EXTEND_COMPONENT_LIST
    for each in extend_component_list:
        tmp_info_list = copy(COMPONENT_INFO_INNIT)
        tmp_info_list[0] = each
        component_dict[each] = tmp_info_list


def clear_options():
    """
    Clear OPTIONS_MANAGER.
    """
    OPTIONS_MANAGER.product = None

    # Entry parameters
    OPTIONS_MANAGER.source_package = None
    OPTIONS_MANAGER.target_package = None
    OPTIONS_MANAGER.update_package = None
    OPTIONS_MANAGER.no_zip = False
    OPTIONS_MANAGER.partition_file = None
    OPTIONS_MANAGER.signing_algorithm = None
    OPTIONS_MANAGER.hash_algorithm = None
    OPTIONS_MANAGER.private_key = None
    OPTIONS_MANAGER.not_l2 = False
    OPTIONS_MANAGER.signing_length = 256
    OPTIONS_MANAGER.xml_path = None
    OPTIONS_MANAGER.sd_card = False

    OPTIONS_MANAGER.full_image_path_list = []

    OPTIONS_MANAGER.make_dir_path = None

    # Parsed package parameters
    OPTIONS_MANAGER.target_package_dir = None
    OPTIONS_MANAGER.target_package_config_dir = None
    OPTIONS_MANAGER.target_package_temp_obj = None
    OPTIONS_MANAGER.misc_info_dict = {}
    OPTIONS_MANAGER.version_mbn_file_path = None
    OPTIONS_MANAGER.version_mbn_content = None
    OPTIONS_MANAGER.board_list_file_path = None
    OPTIONS_MANAGER.board_list_content = None

    OPTIONS_MANAGER.source_package_dir = None
    OPTIONS_MANAGER.source_package_temp_obj = None

    # XML parsing parameters
    OPTIONS_MANAGER.head_info_list = []
    OPTIONS_MANAGER.component_info_dict = {}
    OPTIONS_MANAGER.full_img_list = []
    OPTIONS_MANAGER.incremental_img_list = []
    OPTIONS_MANAGER.target_package_version = None
    OPTIONS_MANAGER.source_package_version = None
    OPTIONS_MANAGER.partition_file_obj = None

    # Global processing parameters
    OPTIONS_MANAGER.full_image_content_len_list = []
    OPTIONS_MANAGER.full_image_file_obj_list = []

    # Incremental processing parameters
    OPTIONS_MANAGER.incremental_content_len_list = []
    OPTIONS_MANAGER.incremental_image_file_obj_list = []
    OPTIONS_MANAGER.incremental_temp_file_obj_list = []

    # Script parameters
    OPTIONS_MANAGER.opera_script_file_name_dict = {}
    for each in SCRIPT_KEY_LIST:
        OPTIONS_MANAGER.opera_script_file_name_dict[each] = []
    OPTIONS_MANAGER.total_script_file_obj = None

    OPTIONS_MANAGER.register_script_file_obj = None

    # Update package parameters
    OPTIONS_MANAGER.update_bin_obj = None
    OPTIONS_MANAGER.build_tools_zip_obj = None
    OPTIONS_MANAGER.update_package_file_path = None


def clear_resource(err_clear=False):
    """
    Clear resources, close temporary files, and clear temporary paths.
    :param err_clear: whether to clear errors
    :return:
    """
    target_package_temp_obj = OPTIONS_MANAGER.target_package_temp_obj
    if target_package_temp_obj is not None:
        target_package_temp_obj.cleanup()
    source_package_temp_obj = OPTIONS_MANAGER.source_package_temp_obj
    if source_package_temp_obj is not None:
        source_package_temp_obj.cleanup()

    partition_file_obj = OPTIONS_MANAGER.partition_file_obj
    if partition_file_obj is not None:
        partition_file_obj.close()

    build_tools_zip_obj = OPTIONS_MANAGER.build_tools_zip_obj
    if build_tools_zip_obj is not None:
        build_tools_zip_obj.close()
    update_bin_obj = OPTIONS_MANAGER.update_bin_obj
    if update_bin_obj is not None:
        update_bin_obj.close()
    total_script_file_obj = OPTIONS_MANAGER.total_script_file_obj
    if total_script_file_obj is not None:
        total_script_file_obj.close()

    register_script_file_obj = OPTIONS_MANAGER.register_script_file_obj
    if register_script_file_obj is not None:
        register_script_file_obj.close()

    full_image_file_obj_list = OPTIONS_MANAGER.full_image_file_obj_list
    if len(full_image_file_obj_list) != 0:
        for each_full_obj in full_image_file_obj_list:
            each_full_obj.close()

    clear_file_obj(err_clear)
    clear_options()


def clear_file_obj(err_clear):
    """
    Clear resources and temporary file objects.
    :param err_clear: whether to clear errors
    :return:
    """
    incremental_temp_file_obj_list = \
        OPTIONS_MANAGER.incremental_temp_file_obj_list
    if len(incremental_temp_file_obj_list) != 0:
        for each_incremental_temp_obj in incremental_temp_file_obj_list:
            if each_incremental_temp_obj is not None:
                each_incremental_temp_obj.close()
    incremental_image_file_obj_list = \
        OPTIONS_MANAGER.incremental_image_file_obj_list
    if len(incremental_image_file_obj_list) != 0:
        for each_incremental_obj in incremental_image_file_obj_list:
            if each_incremental_obj is not None:
                each_incremental_obj.close()
    opera_script_file_name_dict = OPTIONS_MANAGER.opera_script_file_name_dict
    for each_value in opera_script_file_name_dict.values():
        for each in each_value:
            each[1].close()
    if err_clear:
        make_dir_path = OPTIONS_MANAGER.make_dir_path
        if make_dir_path is not None and os.path.exists(make_dir_path):
            shutil.rmtree(make_dir_path)
        update_package_file_path = OPTIONS_MANAGER.update_package_file_path
        if update_package_file_path is not None and \
                os.path.exists(update_package_file_path):
            os.remove(update_package_file_path)
        UPDATE_LOGGER.print_log(
            'Exception occurred, Resource cleaning completed!')
    else:
        UPDATE_LOGGER.print_log('Resource cleaning completed!')


def get_file_content(file_path, file_name=None):
    """
    Read the file content.
    :param file_path: file path
    :param file_name: file name
    :return: file content
    """
    if not os.path.exists(file_path):
        UPDATE_LOGGER.print_log(
            "%s is not exist! path: %s" % (file_name, file_path),
            log_type=UPDATE_LOGGER.ERROR_LOG)
        return False
    with open(file_path, 'r') as r_f:
        file_content = r_f.read()
    UPDATE_LOGGER.print_log(
        "%s file parsing complete! path: %s" % (file_name, file_path))
    return file_content


def get_update_info():
    """
    Parse the configuration file to obtain the update information.
    :return: update information if any; false otherwise.
    """
    if not OPTIONS_MANAGER.not_l2:
        OPTIONS_MANAGER.version_mbn_file_path = os.path.join(
            OPTIONS_MANAGER.target_package_config_dir, VERSION_MBN_PATH)
        version_mbn_content = \
            get_file_content(
                OPTIONS_MANAGER.version_mbn_file_path, os.path.basename(
                    os.path.join(OPTIONS_MANAGER.target_package_config_dir,
                                 VERSION_MBN_PATH)))
        if version_mbn_content is False:
            UPDATE_LOGGER.print_log(
                "Get version mbn content failed!",
                log_type=UPDATE_LOGGER.ERROR_LOG)
            return False
        OPTIONS_MANAGER.version_mbn_content = version_mbn_content
        OPTIONS_MANAGER.board_list_file_path = os.path.join(
            OPTIONS_MANAGER.target_package_config_dir, BOARD_LIST_PATH)
        board_list_content = \
            get_file_content(
                OPTIONS_MANAGER.board_list_file_path, os.path.basename(
                    os.path.join(OPTIONS_MANAGER.target_package_config_dir,
                                 BOARD_LIST_PATH)))
        if board_list_content is False:
            UPDATE_LOGGER.print_log(
                "Get board list content failed!",
                log_type=UPDATE_LOGGER.ERROR_LOG)
            return False
        OPTIONS_MANAGER.board_list_content = board_list_content

    if OPTIONS_MANAGER.xml_path is None:
        xml_file_path = os.path.join(
            OPTIONS_MANAGER.target_package_config_dir, XML_FILE_PATH)
    else:
        xml_file_path = OPTIONS_MANAGER.xml_path

    # Parse the XML configuration file.
    head_info_list, component_info_dict, \
        full_img_list, incremental_img_list, \
        OPTIONS_MANAGER.target_package_version, \
        OPTIONS_MANAGER.full_image_path_list, \
        OPTIONS_MANAGER.two_step = \
        parse_update_config(xml_file_path)
    if head_info_list is False or component_info_dict is False or \
            full_img_list is False or incremental_img_list is False:
        UPDATE_LOGGER.print_log(
            "Get parse update config xml failed!",
            log_type=UPDATE_LOGGER.ERROR_LOG)
        return False
    OPTIONS_MANAGER.head_info_list, OPTIONS_MANAGER.component_info_dict, \
        OPTIONS_MANAGER.full_img_list, OPTIONS_MANAGER.incremental_img_list = \
        head_info_list, component_info_dict, \
        full_img_list, incremental_img_list
    return True


def sign_package():
    return sign_ota_package(
        OPTIONS_MANAGER.update_package_file_path,
        OPTIONS_MANAGER.signed_package,
        OPTIONS_MANAGER.private_key)