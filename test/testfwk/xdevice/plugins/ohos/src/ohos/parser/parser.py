#!/usr/bin/env python3
# coding=utf-8

#
# Copyright (c) 2022 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import copy
import re
import threading
import time
import json
from enum import Enum

from xdevice import LifeCycle
from xdevice import IParser
from xdevice import platform_logger
from xdevice import Plugin
from xdevice import check_pub_key_exist
from xdevice import StateRecorder
from xdevice import TestDescription
from xdevice import ResultCode
from xdevice import CommonParserType
from xdevice import get_cst_time

__all__ = ["CppTestParser", "CppTestListParser", "JunitParser", "JSUnitParser",
           "OHKernelTestParser", "OHJSUnitTestParser",
           "OHJSUnitTestListParser", "_ACE_LOG_MARKER"]

_INFORMATIONAL_MARKER = "[----------]"
_START_TEST_RUN_MARKER = "[==========] Running"
_TEST_RUN_MARKER = "[==========]"
_GTEST_DRYRUN_MARKER = "Running main() "
_START_TEST_MARKER = "[ RUN      ]"
_OK_TEST_MARKER = "[       OK ]"
_SKIPPED_TEST_MARKER = "[  SKIPPED ]"
_FAILED_TEST_MARKER = "[  FAILED  ]"
_ALT_OK_MARKER = "[    OK    ]"
_TIMEOUT_MARKER = "[ TIMEOUT  ]"

_START_JSUNIT_RUN_MARKER = "[start] start run suites"
_START_JSUNIT_SUITE_RUN_MARKER = "[suite start]"
_START_JSUNIT_SUITE_END_MARKER = "[suite end]"
_END_JSUNIT_RUN_MARKER = "[end] run suites end"
_PASS_JSUNIT_MARKER = "[pass]"
_FAIL_JSUNIT_MARKER = "[fail]"
_ERROR_JSUNIT_MARKER = "[error]"
_ACE_LOG_MARKER = "jsapp"

"""
OpenHarmony Kernel Test
"""
RUNTEST_TEST = "runtest test"
START_TO_TEST = "Start to test"
FINISHED_TO_TEST = "Finished to test"
TIMEOUT_TESTCASES = "Timeout testcases"
FAIL_DOT = "FAIL."
PASS_DOT = "PASS."
ERROR_EXCLAMATION = "ERROR!!!"
TIMEOUT_EXCLAMATION = "TIMEOUT!"


LOG = platform_logger("Parser")


@Plugin(type=Plugin.PARSER, id=CommonParserType.cpptest)
class CppTestParser(IParser):
    def __init__(self):
        self.state_machine = StateRecorder()
        self.suite_name = ""
        self.listeners = []
        self.product_info = {}
        self.is_params = False
        self.result_data = ""

    def get_suite_name(self):
        return self.suite_name

    def get_listeners(self):
        return self.listeners

    def __process__(self, lines):
        if not self.state_machine.suites_is_started():
            self.state_machine.trace_logs.extend(lines)
        for line in lines:
            self.result_data = "{}[{}] {}\n".format(self.result_data, threading.currentThread().ident, line)
            self.parse(line)

    def __done__(self):
        suite_result = self.state_machine.get_suites()
        if not suite_result.suites_name:
            return
        for listener in self.get_listeners():
            suites = copy.copy(suite_result)
            listener.__ended__(LifeCycle.TestSuites, test_result=suites,
                               suites_name=suites.suites_name,
                               product_info=suites.product_info)
        self.state_machine.current_suites = None
        LOG.debug("CppParser data:")
        LOG.debug(self.result_data)
        self.result_data = ""

    def parse(self, line):

        if self.state_machine.suites_is_started() or line.startswith(
                _TEST_RUN_MARKER):
            if line.startswith(_START_TEST_RUN_MARKER):
                message = line[len(_TEST_RUN_MARKER):].strip()
                self.handle_suites_started_tag(message)
            elif line.startswith(_INFORMATIONAL_MARKER):
                pattern = r"(.*) (\(\d+ ms total\))"
                message = line[len(_INFORMATIONAL_MARKER):].strip()
                if re.match(pattern, line.strip()):
                    self.handle_suite_ended_tag(message)
                elif re.match(r'(\d+) test[s]? from (.*)', message):
                    self.handle_suite_started_tag(message)
            elif line.startswith(_TEST_RUN_MARKER):
                if not self.state_machine.suites_is_running():
                    return
                message = line[len(_TEST_RUN_MARKER):].strip()
                self.handle_suites_ended_tag(message)
            elif line.startswith(_START_TEST_MARKER):
                # Individual test started
                message = line[len(_START_TEST_MARKER):].strip()
                self.handle_test_started_tag(message)
            else:
                self.process_test(line)

    def process_test(self, line):
        if _SKIPPED_TEST_MARKER in line:
            message = line[line.index(_SKIPPED_TEST_MARKER) + len(
                _SKIPPED_TEST_MARKER):].strip()
            if not self.state_machine.test_is_running():
                LOG.error(
                    "Found {} without {} before, wrong GTest log format".
                    format(line, _START_TEST_MARKER))
                return
            self.handle_test_ended_tag(message, ResultCode.SKIPPED)
        elif _OK_TEST_MARKER in line:
            message = line[line.index(_OK_TEST_MARKER) + len(
                _OK_TEST_MARKER):].strip()
            if not self.state_machine.test_is_running():
                LOG.error(
                    "Found {} without {} before, wrong GTest log format".
                    format(line, _START_TEST_MARKER))
                return
            self.handle_test_ended_tag(message, ResultCode.PASSED)
        elif _ALT_OK_MARKER in line:
            message = line[line.index(_ALT_OK_MARKER) + len(
                _ALT_OK_MARKER):].strip()
            self.fake_run_marker(message)
            self.handle_test_ended_tag(message, ResultCode.PASSED)
        elif _FAILED_TEST_MARKER in line:
            message = line[line.index(_FAILED_TEST_MARKER) + len(
                _FAILED_TEST_MARKER):].strip()
            if not self.state_machine.suite_is_running():
                return
            if not self.state_machine.test_is_running():
                self.fake_run_marker(message)
            self.handle_test_ended_tag(message, ResultCode.FAILED)
        elif _TIMEOUT_MARKER in line:
            message = line[line.index(_TIMEOUT_MARKER) + len(
                _TIMEOUT_MARKER):].strip()
            self.fake_run_marker(message)
            self.handle_test_ended_tag(message, ResultCode.FAILED)
        elif self.state_machine.test_is_running():
            self.append_test_output(line)

    def handle_test_suite_failed(self, error_msg):
        error_msg = "Unknown error" if error_msg is None else error_msg
        LOG.info("Test run failed: {}".format(error_msg))
        if self.state_machine.test_is_running():
            self.state_machine.test().is_completed = True
            for listener in self.get_listeners():
                test_result = copy.copy(self.currentTestResult)
                listener.__failed__(LifeCycle.TestCase, test_result)
                listener.__ended__(LifeCycle.TestCase, test_result)
        self.state_machine.suite().stacktrace = error_msg
        self.state_machine.suite().is_completed = True
        for listener in self.get_listeners():
            suite_result = copy.copy(self.currentSuiteResult)
            listener.__failed__(LifeCycle.TestSuite, suite_result)
            listener.__ended__(LifeCycle.TestSuite, suite_result)

    def handle_test_started_tag(self, message):
        test_class, test_name, _ = self.parse_test_description(
            message)
        test_result = self.state_machine.test(reset=True)
        test_result.test_class = test_class
        test_result.test_name = test_name
        for listener in self.get_listeners():
            test_result = copy.copy(test_result)
            listener.__started__(LifeCycle.TestCase, test_result)

    @classmethod
    def parse_test_description(cls, message):
        run_time = 0
        matcher = re.match(r'(.*) \((\d+) ms\)', message)
        if matcher:
            test_class, test_name = matcher.group(1).rsplit(".", 1)
            run_time = int(matcher.group(2))
        else:
            test_class, test_name = message.rsplit(".", 1)
        return test_class, test_name, run_time

    def handle_test_ended_tag(self, message, test_status):
        test_class, test_name, run_time = self.parse_test_description(
            message)
        test_result = self.state_machine.test()
        test_result.run_time = int(run_time)
        test_result.code = test_status.value
        test_result.current = self.state_machine.running_test_index + 1
        if not test_result.is_running():
            LOG.error(
                "Test has no start tag when trying to end test: %s", message)
            return
        found_unexpected_test = False
        if test_result.test_class != test_class:
            LOG.error(
                "Expected class: {} but got:{} ".format(test_result.test_class,
                                                        test_class))
            found_unexpected_test = True
        if test_result.test_name != test_name:
            LOG.error(
                "Expected test: {} but got: {}".format(test_result.test_name,
                                                       test_name))
            found_unexpected_test = True

        if found_unexpected_test or ResultCode.FAILED == test_status:
            for listener in self.get_listeners():
                result = copy.copy(test_result)
                listener.__failed__(LifeCycle.TestCase, result)
        elif ResultCode.SKIPPED == test_status:
            for listener in self.get_listeners():
                result = copy.copy(test_result)
                listener.__skipped__(LifeCycle.TestCase, result)

        self.state_machine.test().is_completed = True
        for listener in self.get_listeners():
            result = copy.copy(test_result)
            listener.__ended__(LifeCycle.TestCase, result)
        self.state_machine.running_test_index += 1

    def fake_run_marker(self, message):
        fake_marker = re.compile(" +").split(message)
        self.handle_test_started_tag(fake_marker)

    def handle_suites_started_tag(self, message):
        self.state_machine.get_suites(reset=True)
        matcher = re.match(r'Running (\d+) test[s]? from .*', message)
        expected_test_num = int(matcher.group(1)) if matcher else -1
        if expected_test_num >= 0:
            test_suites = self.state_machine.get_suites()
            test_suites.suites_name = self.get_suite_name()
            test_suites.test_num = expected_test_num
            test_suites.product_info = self.product_info
            for listener in self.get_listeners():
                suite_report = copy.copy(test_suites)
                listener.__started__(LifeCycle.TestSuites, suite_report)

    def handle_suite_started_tag(self, message):
        self.state_machine.suite(reset=True)
        matcher = re.match(r'(\d+) test[s]? from (.*)', message)
        expected_test_num = int(matcher.group(1)) if matcher else -1
        if expected_test_num >= 0:
            test_suite = self.state_machine.suite()
            test_suite.suite_name = matcher.group(2)
            test_suite.test_num = expected_test_num
            for listener in self.get_listeners():
                suite_report = copy.copy(test_suite)
                listener.__started__(LifeCycle.TestSuite, suite_report)

    def handle_suite_ended_tag(self, message):
        self.state_machine.running_test_index = 0
        suite_result = self.state_machine.suite()
        matcher = re.match(r'.*\((\d+) ms total\)', message)
        if matcher:
            suite_result.run_time = int(matcher.group(1))
        suite_result.is_completed = True
        for listener in self.get_listeners():
            suite = copy.copy(suite_result)
            listener.__ended__(LifeCycle.TestSuite, suite, is_clear=True)

    def handle_suites_ended_tag(self, message):
        suites = self.state_machine.get_suites()
        matcher = re.match(r'.*\((\d+) ms total\)', message)
        if matcher:
            suites.run_time = int(matcher.group(1))
        suites.is_completed = True
        for listener in self.get_listeners():
            copy_suites = copy.copy(suites)
            listener.__ended__(LifeCycle.TestSuites, test_result=copy_suites,
                               suites_name=suites.suites_name,
                               product_info=suites.product_info,
                               suite_report=True)

    def append_test_output(self, message):
        if self.state_machine.test().stacktrace:
            self.state_machine.test().stacktrace += "\r\n"
        self.state_machine.test().stacktrace += message

    @staticmethod
    def handle_test_run_failed(error_msg):
        if not error_msg:
            error_msg = "Unknown error"
        if not check_pub_key_exist():
            LOG.debug("Error msg:%s" % error_msg)

    def mark_test_as_blocked(self, test):
        if not self.state_machine.current_suite and not test.class_name:
            return
        suites_result = self.state_machine.get_suites(reset=True)
        suites_result.suites_name = self.get_suite_name()
        suite_name = self.state_machine.current_suite.suite_name if \
            self.state_machine.current_suite else None
        suite_result = self.state_machine.suite(reset=True)
        test_result = self.state_machine.test(reset=True)
        suite_result.suite_name = suite_name or test.class_name
        suite_result.suite_num = 1
        test_result.test_class = test.class_name
        test_result.test_name = test.test_name
        test_result.stacktrace = "error_msg: run crashed"
        test_result.num_tests = 1
        test_result.run_time = 0
        test_result.code = ResultCode.BLOCKED.value
        for listener in self.get_listeners():
            suite_report = copy.copy(suites_result)
            listener.__started__(LifeCycle.TestSuites, suite_report)
        for listener in self.get_listeners():
            suite_report = copy.copy(suite_result)
            listener.__started__(LifeCycle.TestSuite, suite_report)
        for listener in self.get_listeners():
            test_result = copy.copy(test_result)
            listener.__started__(LifeCycle.TestCase, test_result)
        for listener in self.get_listeners():
            test_result = copy.copy(test_result)
            listener.__ended__(LifeCycle.TestCase, test_result)
        for listener in self.get_listeners():
            suite_report = copy.copy(suite_result)
            listener.__ended__(LifeCycle.TestSuite, suite_report,
                               is_clear=True)
        self.__done__()


@Plugin(type=Plugin.PARSER, id=CommonParserType.cpptest_list)
class CppTestListParser(IParser):
    def __init__(self):
        self.last_test_class_name = None
        self.tests = []
        self.result_data = ""
        self.suites = dict()

    def __process__(self, lines):
        for line in lines:
            self.result_data = "{}{}\n".format(self.result_data, line)
            self.parse(line)

    def __done__(self):
        LOG.debug("CppTestListParser data:")
        LOG.debug(self.result_data)
        self.result_data = ""

    def parse(self, line):
        class_matcher = re.match('^([a-zA-Z]+.*)\\.$', line)
        method_matcher = re.match('\\s+([a-zA-Z_]+[\\S]*)(.*)?(\\s+.*)?$',
                                  line)
        if class_matcher:
            self.last_test_class_name = class_matcher.group(1)
            if self.last_test_class_name not in self.suites:
                self.suites.setdefault(self.last_test_class_name, [])
        elif method_matcher:
            if not self.last_test_class_name:
                LOG.error("Parsed new test case name %s but no test class name"
                          " has been set" % line)
            else:
                test_name = method_matcher.group(1)
                if test_name not in self.suites.get(self.last_test_class_name, []):
                    test = TestDescription(self.last_test_class_name,
                                           test_name)
                    self.tests.append(test)
                    self.suites.get(self.last_test_class_name, []).append(test_name)
                else:
                    LOG.debug("[{}.{}] has already collect it, skip it.".format(
                        self.last_test_class_name, test_name))
        else:
            if not check_pub_key_exist():
                LOG.debug("Line ignored: %s" % line)


class StatusCodes(Enum):
    FAILURE = -2
    START = 1
    ERROR = -1
    SUCCESS = 0
    IN_PROGRESS = 2
    IGNORE = -3
    BLOCKED = 3


class Prefixes(Enum):
    STATUS = "INSTRUMENTATION_STATUS: "
    STATUS_CODE = "INSTRUMENTATION_STATUS_CODE: "
    STATUS_FAILED = "INSTRUMENTATION_FAILED: "
    CODE = "INSTRUMENTATION_CODE: "
    RESULT = "INSTRUMENTATION_RESULT: "
    TIME_REPORT = "Time: "


@Plugin(type=Plugin.PARSER, id=CommonParserType.junit)
class JunitParser(IParser):
    def __init__(self):
        self.state_machine = StateRecorder()
        self.suite_name = ""
        self.listeners = []
        self.current_key = None
        self.current_value = None
        self.start_time = get_cst_time()
        self.test_time = 0
        self.test_run_finished = False

    def get_suite_name(self):
        return self.suite_name

    def get_listeners(self):
        return self.listeners

    def __process__(self, lines):
        for line in lines:
            if not check_pub_key_exist():
                LOG.debug(line)
            self.parse(line)

    def __done__(self):
        suite_result = self.state_machine.suite()
        suite_result.run_time = self.test_time
        suite_result.is_completed = True
        for listener in self.get_listeners():
            suite = copy.copy(suite_result)
            listener.__ended__(LifeCycle.TestSuite, suite,
                               suite_report=True)
        self.state_machine.current_suite = None

    def parse(self, line):
        if line.startswith(Prefixes.STATUS_CODE.value):
            self.submit_current_key_value()
            self.parse_status_code(line)
        elif line.startswith(Prefixes.STATUS.value):
            self.submit_current_key_value()
            self.parse_key(line, len(Prefixes.STATUS.value))
        elif line.startswith(Prefixes.RESULT.value):
            self.test_run_finished = True
        elif line.startswith(Prefixes.STATUS_FAILED.value) or \
                line.startswith(Prefixes.CODE.value):
            self.submit_current_key_value()
            self.test_run_finished = True
        elif line.startswith(Prefixes.TIME_REPORT.value):
            self.parse_time(line)
        else:
            if self.current_key == "stack" and self.current_value:
                self.current_value = self.current_value + r"\r\n"
                self.current_value = self.current_value + line
            elif line:
                pass

    def parse_key(self, line, key_start_pos):
        key_value = line[key_start_pos:].split("=", 1)
        if len(key_value) == 2:
            self.current_key = key_value[0]
            self.current_value = key_value[1]

    def parse_time(self, line):
        message = line[len(Prefixes.TIME_REPORT.value):]
        self.test_time = float(message.replace(",", "")) * 1000

    @staticmethod
    def check_legality(name):
        if not name or name == "null":
            return False
        return True

    def parse_status_code(self, line):
        value = line[len(Prefixes.STATUS_CODE.value):]
        test_info = self.state_machine.test()
        test_info.code = int(value)
        if test_info.code != StatusCodes.IN_PROGRESS:
            if self.check_legality(test_info.test_class) and \
                    self.check_legality(test_info.test_name):
                self.report_result(test_info)
                self.clear_current_test_info()

    def clear_current_test_info(self):
        self.state_machine.current_test = None

    def submit_current_key_value(self):
        if self.current_key and self.current_value:
            status_value = self.current_value
            test_info = self.state_machine.test()
            if self.current_key == "class":
                test_info.test_class = status_value
            elif self.current_key == "test":
                test_info.test_name = status_value
            elif self.current_key == "numtests":
                test_info.num_tests = int(status_value)
            elif self.current_key == "Error":
                self.handle_test_run_failed(status_value)
            elif self.current_key == "stack":
                test_info.stacktrace = status_value
            elif self.current_key == "stream":
                pass
            self.current_key = None
            self.current_value = None

    def report_result(self, test_info):
        if not test_info.test_name or not test_info.test_class:
            LOG.info("Invalid instrumentation status bundle")
            return
        test_info.is_completed = True
        self.report_test_run_started(test_info)
        if test_info.code == StatusCodes.START.value:
            self.start_time = get_cst_time()
            for listener in self.get_listeners():
                result = copy.copy(test_info)
                listener.__started__(LifeCycle.TestCase, result)
        elif test_info.code == StatusCodes.FAILURE.value:
            self.state_machine.running_test_index += 1
            test_info.current = self.state_machine.running_test_index
            end_time = get_cst_time()
            run_time = (end_time - self.start_time).total_seconds()
            test_info.run_time = int(run_time * 1000)
            for listener in self.get_listeners():
                result = copy.copy(test_info)
                result.code = ResultCode.FAILED.value
                listener.__ended__(LifeCycle.TestCase, result)
        elif test_info.code == StatusCodes.ERROR.value:
            self.state_machine.running_test_index += 1
            test_info.current = self.state_machine.running_test_index
            end_time = get_cst_time()
            run_time = (end_time - self.start_time).total_seconds()
            test_info.run_time = int(run_time * 1000)
            for listener in self.get_listeners():
                result = copy.copy(test_info)
                result.code = ResultCode.FAILED.value
                listener.__ended__(LifeCycle.TestCase, result)
        elif test_info.code == StatusCodes.SUCCESS.value:
            self.state_machine.running_test_index += 1
            test_info.current = self.state_machine.running_test_index
            end_time = get_cst_time()
            run_time = (end_time - self.start_time).total_seconds()
            test_info.run_time = int(run_time * 1000)
            for listener in self.get_listeners():
                result = copy.copy(test_info)
                result.code = ResultCode.PASSED.value
                listener.__ended__(LifeCycle.TestCase, result)
        elif test_info.code == StatusCodes.IGNORE.value:
            end_time = get_cst_time()
            run_time = (end_time - self.start_time).total_seconds()
            test_info.run_time = int(run_time * 1000)
            for listener in self.get_listeners():
                result = copy.copy(test_info)
                result.code = ResultCode.SKIPPED.value
                listener.__skipped__(LifeCycle.TestCase, result)
        elif test_info.code == StatusCodes.BLOCKED.value:
            test_info.current = self.state_machine.running_test_index
            end_time = get_cst_time()
            run_time = (end_time - self.start_time).total_seconds()
            test_info.run_time = int(run_time * 1000)
            for listener in self.get_listeners():
                result = copy.copy(test_info)
                result.code = ResultCode.BLOCKED.value
                listener.__ended__(LifeCycle.TestCase, result)

        self.output_stack_trace(test_info)

    @classmethod
    def output_stack_trace(cls, test_info):
        if check_pub_key_exist():
            return
        if test_info.stacktrace:
            stack_lines = test_info.stacktrace.split(r"\r\n")
            LOG.error("Stacktrace information is:")
            for line in stack_lines:
                line.strip()
                if line:
                    LOG.error(line)

    def report_test_run_started(self, test_result):
        test_suite = self.state_machine.suite()
        if not self.state_machine.suite().is_started:
            if not test_suite.test_num or not test_suite.suite_name:
                test_suite.suite_name = self.get_suite_name()
                test_suite.test_num = test_result.num_tests
                for listener in self.get_listeners():
                    suite_report = copy.copy(test_suite)
                    listener.__started__(LifeCycle.TestSuite, suite_report)

    @staticmethod
    def handle_test_run_failed(error_msg):
        if not error_msg:
            error_msg = "Unknown error"
        if not check_pub_key_exist():
            LOG.debug("Error msg:%s" % error_msg)

    def mark_test_as_failed(self, test):
        test_info = self.state_machine.test()
        if test_info:
            test_info.test_class = test.class_name
            test_info.test_name = test.test_name
            test_info.code = StatusCodes.START.value
            self.report_result(test_info)
            test_info.code = StatusCodes.FAILURE.value
            self.report_result(test_info)
            self.__done__()

    def mark_test_as_blocked(self, test):
        test_info = self.state_machine.test()
        if test_info:
            test_info.test_class = test.class_name
            test_info.test_name = test.test_name
            test_info.num_tests = 1
            test_info.run_time = 0
            test_info.code = StatusCodes.START.value
            self.report_result(test_info)
            test_info.code = StatusCodes.BLOCKED.value
            self.report_result(test_info)
            self.__done__()


@Plugin(type=Plugin.PARSER, id=CommonParserType.jsunit)
class JSUnitParser(IParser):
    last_line = ""
    pattern = r"(\d{1,2}-\d{1,2}\s\d{1,2}:\d{1,2}:\d{1,2}\.\d{3}) "

    def __init__(self):
        self.state_machine = StateRecorder()
        self.suites_name = ""
        self.listeners = []
        self.expect_tests_dict = dict()
        self.marked_suite_set = set()
        self.exclude_list = list()

    def get_listeners(self):
        return self.listeners

    def __process__(self, lines):
        if not self.state_machine.suites_is_started():
            self.state_machine.trace_logs.extend(lines)
        for line in lines:
            self.parse(line)

    def __done__(self):
        pass

    def parse(self, line):
        if (self.state_machine.suites_is_started() or line.find(
                _START_JSUNIT_RUN_MARKER) != -1) and \
                line.lower().find(_ACE_LOG_MARKER) != -1:
            if line.find(_START_JSUNIT_RUN_MARKER) != -1:
                self.handle_suites_started_tag()
            elif line.endswith(_END_JSUNIT_RUN_MARKER):
                self.handle_suites_ended_tag()
            elif line.find(_START_JSUNIT_SUITE_RUN_MARKER) != -1:
                self.handle_suite_started_tag(line.strip())
            elif line.endswith(_START_JSUNIT_SUITE_END_MARKER):
                self.handle_suite_ended_tag()
            elif _PASS_JSUNIT_MARKER in line or _FAIL_JSUNIT_MARKER \
                    in line or _ERROR_JSUNIT_MARKER in line:
                self.handle_one_test_tag(line.strip())
            self.last_line = line

    def parse_test_description(self, message):
        pattern = r".*\[(pass|fail|error)\]"
        year = time.strftime("%Y")
        match_list = ["app Log:", "JSApp:", "JsApp:"]
        filter_message = ""
        for keyword in match_list:
            if keyword in message:
                filter_message = \
                    message.split(r"{0}".format(keyword))[1].strip()
                break
        end_time = "%s-%s" % \
                   (year, re.match(self.pattern, message).group().strip())
        start_time = "%s-%s" % \
                     (year, re.match(self.pattern,
                                     self.last_line.strip()).group().strip())
        start_timestamp = int(time.mktime(
            time.strptime(start_time, "%Y-%m-%d %H:%M:%S.%f"))) * 1000 + int(
            start_time.split(".")[-1])
        end_timestamp = int(time.mktime(
            time.strptime(end_time, "%Y-%m-%d %H:%M:%S.%f"))) * 1000 + int(
            end_time.split(".")[-1])
        run_time = end_timestamp - start_timestamp
        match = re.match(pattern, filter_message)
        _, status_end_index = match.span()
        if " ;" in filter_message:
            test_name = filter_message[status_end_index:
                                       str(filter_message).find(" ;")]
        else:
            test_name = filter_message[status_end_index:]
        status_dict = {"pass": ResultCode.PASSED, "fail": ResultCode.FAILED,
                       "ignore": ResultCode.SKIPPED,
                       "error": ResultCode.FAILED}
        status = status_dict.get(match.group(1))
        return test_name.strip(), status, run_time

    def handle_suites_started_tag(self):
        self.state_machine.get_suites(reset=True)
        test_suites = self.state_machine.get_suites()
        test_suites.suites_name = self.suites_name
        test_suites.test_num = 0
        for listener in self.get_listeners():
            suite_report = copy.copy(test_suites)
            listener.__started__(LifeCycle.TestSuites, suite_report)

    def handle_suites_ended_tag(self):
        self._mark_all_test_case()
        suites = self.state_machine.get_suites()
        suites.is_completed = True

        for listener in self.get_listeners():
            listener.__ended__(LifeCycle.TestSuites, test_result=suites,
                               suites_name=suites.suites_name)

    def handle_one_test_tag(self, message):
        test_name, status, run_time = \
            self.parse_test_description(message)
        test_suite = self.state_machine.suite()
        if self.exclude_list:
            qualified_name = "{}#{}".format(test_suite.suite_name, test_name)
            if qualified_name in self.exclude_list:
                LOG.debug("{} will be discard!".format(qualified_name))
                test_suite.test_num -= 1
                return
        test_result = self.state_machine.test(reset=True)
        test_result.test_class = test_suite.suite_name
        test_result.test_name = test_name
        test_result.run_time = run_time
        test_result.code = status.value
        test_result.current = self.state_machine.running_test_index + 1
        self.state_machine.suite().run_time += run_time
        for listener in self.get_listeners():
            test_result = copy.copy(test_result)
            listener.__started__(LifeCycle.TestCase, test_result)

        test_suites = self.state_machine.get_suites()
        found_unexpected_test = False

        if found_unexpected_test or ResultCode.FAILED == status:
            for listener in self.get_listeners():
                result = copy.copy(test_result)
                listener.__failed__(LifeCycle.TestCase, result)
        elif ResultCode.SKIPPED == status:
            for listener in self.get_listeners():
                result = copy.copy(test_result)
                listener.__skipped__(LifeCycle.TestCase, result)

        self.state_machine.test().is_completed = True
        if not hasattr(test_suite, "total_cases"):
            test_suite.test_num += 1
        test_suites.test_num += 1
        for listener in self.get_listeners():
            result = copy.copy(test_result)
            listener.__ended__(LifeCycle.TestCase, result)
        self.state_machine.running_test_index += 1

    def fake_run_marker(self, message):
        fake_marker = re.compile(" +").split(message)
        self.processTestStartedTag(fake_marker)

    def handle_suite_started_tag(self, message):
        self.state_machine.suite(reset=True)
        self.state_machine.running_test_index = 0
        test_suite = self.state_machine.suite()
        if "total cases:" in message:
            m_result = re.match(r".*\[suite start](.+), total cases: (\d+)",
                                message)
            if m_result:
                expect_test_num = m_result.group(2)
                test_suite.suite_name = m_result.group(1)
                test_suite.test_num = int(expect_test_num)
                setattr(test_suite, "total_cases", True)

        else:
            if re.match(r".*\[suite start].*", message):
                _, index = re.match(r".*\[suite start]", message).span()
                if message[index:]:
                    test_suite.suite_name = message[index:]
                else:
                    test_suite.suite_name = self.suite_name
                test_suite.test_num = 0
        for listener in self.get_listeners():
            suite_report = copy.copy(test_suite)
            listener.__started__(LifeCycle.TestSuite, suite_report)

    def handle_suite_ended_tag(self):
        suite_result = self.state_machine.suite()
        suites = self.state_machine.get_suites()
        suite_result.run_time = suite_result.run_time
        suites.run_time += suite_result.run_time
        suite_result.is_completed = True
        self._mark_test_case(suite_result, self.get_listeners())
        for listener in self.get_listeners():
            suite = copy.copy(suite_result)
            listener.__ended__(LifeCycle.TestSuite, suite, is_clear=True)

    def append_test_output(self, message):
        if self.state_machine.test().stacktrace:
            self.state_machine.test().stacktrace = \
                "%s\r\n" % self.state_machine.test().stacktrace
        self.state_machine.test().stacktrace = \
            ''.join((self.state_machine.test().stacktrace, message))

    def _mark_test_case(self, suite, listeners):
        if not self.expect_tests_dict:
            return
        tests_list = []
        for listener in listeners:
            if listener.__class__.__name__ == "ReportListener":
                tests_list.extend(listener.tests.values())
                break
        test_name_list = []
        for item_test in tests_list:
            test_name_list.append(item_test.test_name)
        self.marked_suite_set.add(suite.suite_name)
        test_in_cur = self.expect_tests_dict.get(suite.suite_name, [])
        for test in test_in_cur:
            if "{}#{}".format(suite.suite_name, test.test_name) \
                    in self.exclude_list:
                suite.test_num -= 1
                continue
            if test.test_name not in test_name_list:
                self._mock_test_case_life_cycle(listeners, test)

    def _mock_test_case_life_cycle(self, listeners, test):
        test_result = self.state_machine.test(reset=True)
        test_result.test_class = test.class_name
        test_result.test_name = test.test_name
        test_result.stacktrace = "error_msg: mark blocked"
        test_result.num_tests = 1
        test_result.run_time = 0
        test_result.current = self.state_machine.running_test_index + 1
        test_result.code = ResultCode.BLOCKED.value
        test_result = copy.copy(test_result)
        for listener in listeners:
            listener.__started__(LifeCycle.TestCase, test_result)
        test_result = copy.copy(test_result)
        for listener in listeners:
            listener.__ended__(LifeCycle.TestCase, test_result)
        self.state_machine.running_test_index += 1

    def _mark_all_test_case(self):
        if not self.expect_tests_dict:
            return
        all_suite_set = set(self.expect_tests_dict.keys())
        un_suite_set = all_suite_set.difference(self.marked_suite_set)
        for un_suite_name in un_suite_set:
            test_list = self.expect_tests_dict.get(un_suite_name, [])

            self.state_machine.suite(reset=True)
            self.state_machine.running_test_index = 0
            test_suite = self.state_machine.suite()
            test_suite.suite_name = un_suite_name
            test_suite.test_num = len(test_list)
            for listener in self.get_listeners():
                suite_report = copy.copy(test_suite)
                listener.__started__(LifeCycle.TestSuite, suite_report)

            for test in test_list:
                if "{}#{}".format(test_suite.suite_name, test.test_name) \
                        in self.exclude_list:
                    test_suite.test_num -= 1
                    continue
                self._mock_test_case_life_cycle(self.get_listeners(), test)

            test_suite.is_completed = True
            for listener in self.get_listeners():
                suite = copy.copy(test_suite)
                listener.__ended__(LifeCycle.TestSuite, suite, is_clear=True)


@Plugin(type=Plugin.PARSER, id=CommonParserType.oh_kernel_test)
class OHKernelTestParser(IParser):

    def __init__(self):
        self.state_machine = StateRecorder()
        self.suites_name = ""
        self.listeners = []

    def get_listeners(self):
        return self.listeners

    def __process__(self, lines):
        if not self.state_machine.suites_is_started():
            self.state_machine.trace_logs.extend(lines)
        for line in lines:
            self.parse(line)

    def __done__(self):
        pass

    def parse(self, line):
        line = re.sub('\x1b.*?m', '', line)
        if self.state_machine.suites_is_started() or RUNTEST_TEST in line:
            if RUNTEST_TEST in line:
                self.handle_suites_started_tag(line)
            elif START_TO_TEST in line:
                self.handle_suite_start_tag(line)
            elif FINISHED_TO_TEST in line:
                self.handle_suite_end_tag(line)
            elif line.endswith(PASS_DOT) or line.endswith(FAIL_DOT):
                self.handle_one_test_case_tag(line)
            elif line.endswith(ERROR_EXCLAMATION) \
                    or line.endswith(TIMEOUT_EXCLAMATION):
                self.handle_test_case_error(line)
            elif TIMEOUT_TESTCASES in line:
                self.handle_suites_ended_tag(line)

    def handle_suites_started_tag(self, line):
        self.state_machine.get_suites(reset=True)
        test_suites = self.state_machine.get_suites()
        test_suites.suites_name = self.suites_name
        test_suites.test_num = 0
        for listener in self.get_listeners():
            suite_report = copy.copy(test_suites)
            listener.__started__(LifeCycle.TestSuites, suite_report)

    def handle_suites_ended_tag(self, line):
        suites = self.state_machine.get_suites()
        suites.is_completed = True

        for listener in self.get_listeners():
            listener.__ended__(LifeCycle.TestSuites, test_result=suites,
                               suites_name=suites.suites_name)

    def handle_suite_start_tag(self, line):
        pattern = "^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}" \
                  " Start to test (.+)$"
        matcher = re.match(pattern, line)
        if matcher and matcher.group(1):
            self.state_machine.suite(reset=True)
            test_suite = self.state_machine.suite()
            test_suite.suite_name = matcher.group(1)
            for listener in self.get_listeners():
                suite_report = copy.copy(test_suite)
                listener.__started__(LifeCycle.TestSuite, suite_report)

    def handle_suite_end_tag(self, line):
        pattern = "^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}" \
                  " Finished to test (.+)$"
        matcher = re.match(pattern, line)
        if matcher and matcher.group(1):
            suite_result = self.state_machine.suite()
            suites = self.state_machine.get_suites()
            suite_result.run_time = suite_result.run_time
            suites.run_time += suite_result.run_time
            suite_result.is_completed = True

            for listener in self.get_listeners():
                suite = copy.copy(suite_result)
                listener.__ended__(LifeCycle.TestSuite, suite, is_clear=True)

    def handle_one_test_case_tag(self, line):
        pattern = "^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2} (.+) " \
                  "(PASS)\\.$"
        matcher = re.match(pattern, line)
        if not (matcher and matcher.group(1) and matcher.group(2)):
            return
        test_result = self.state_machine.test(reset=True)
        test_suite = self.state_machine.suite()
        test_result.test_class = test_suite.suite_name
        test_result.test_name = matcher.group(1)
        test_result.current = self.state_machine.running_test_index + 1
        for listener in self.get_listeners():
            test_result = copy.copy(test_result)
            listener.__started__(LifeCycle.TestCase, test_result)

        test_suites = self.state_machine.get_suites()
        if PASS_DOT in line:
            test_result.code = ResultCode.PASSED.value
        elif FAIL_DOT in line:
            test_result.code = ResultCode.FAILED.value
            for listener in self.get_listeners():
                result = copy.copy(test_result)
                listener.__failed__(LifeCycle.TestCase, result)
        self.state_machine.test().is_completed = True
        test_suite.test_num += 1
        test_suites.test_num += 1
        for listener in self.get_listeners():
            result = copy.copy(test_result)
            listener.__ended__(LifeCycle.TestCase, result)
        self.state_machine.running_test_index += 1

    def handle_test_case_error(self, line):
        pattern = "^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2} (.+) " \
                  "(ERROR!!!|TIMEOUT!)$"
        matcher = re.match(pattern, line)
        if not (matcher and matcher.group(1) and matcher.group(2)):
            return
        test_result = self.state_machine.test(reset=True)
        test_suite = self.state_machine.suite()
        test_result.test_class = test_suite.suite_name
        test_result.test_name = matcher.group(1)
        test_result.current = self.state_machine.running_test_index + 1
        for listener in self.get_listeners():
            test_result = copy.copy(test_result)
            listener.__started__(LifeCycle.TestCase, test_result)

        test_suites = self.state_machine.get_suites()
        if ERROR_EXCLAMATION in line:
            test_result.code = ResultCode.FAILED.value
        elif TIMEOUT_EXCLAMATION in line:
            test_result.code = ResultCode.BLOCKED.value

        for listener in self.get_listeners():
            result = copy.copy(test_result)
            listener.__failed__(LifeCycle.TestCase, result)
        self.state_machine.test().is_completed = True
        test_suite.test_num += 1
        test_suites.test_num += 1
        for listener in self.get_listeners():
            result = copy.copy(test_result)
            listener.__ended__(LifeCycle.TestCase, result)
        self.state_machine.running_test_index += 1


class OHJSUnitPrefixes(Enum):
    SUM = "OHOS_REPORT_SUM: "
    STATUS = "OHOS_REPORT_STATUS: "
    STATUS_CODE = "OHOS_REPORT_STATUS_CODE: "
    RESULT = "OHOS_REPORT_RESULT: "
    CODE = "OHOS_REPORT_CODE: "
    TEST_FINISHED_RESULT_MSG = "TestFinished-ResultMsg: "


class OHJSUnitItemConstants(Enum):
    CLASS = "class"
    TEST = "test"
    NUM_TESTS = "numtests"
    STACK = "stack"
    SUITE_CONSUMING = "suiteconsuming"
    APP_DIED = "App died"


@Plugin(type=Plugin.PARSER, id=CommonParserType.oh_jsunit)
class OHJSUnitTestParser(IParser):

    def __init__(self):
        self.state_machine = StateRecorder()
        self.suites_name = ""
        self.listeners = []
        self.current_key = None
        self.current_value = None
        self.start_time = get_cst_time()
        self.test_time = 0
        self.test_run_finished = False
        self.cur_sum = -1
        self.runner = None
        self.result_data = ""

    def get_suite_name(self):
        return self.suites_name

    def get_listeners(self):
        return self.listeners

    def __process__(self, lines):
        for line in lines:
            self.result_data = "{}[{}] {}\n".format(self.result_data, threading.currentThread().ident, line)
            self.parse(line)

    def parse(self, line):
        if not str(line).strip():
            return
        if line.startswith(OHJSUnitPrefixes.SUM.value):
            self.handle_sum_line(line)
        elif line.startswith(OHJSUnitPrefixes.STATUS.value):
            self.handle_status_line(line)
        elif line.startswith(OHJSUnitPrefixes.STATUS_CODE.value):
            self.submit_current_key_value()
            self.parse_status_code(line)
        elif line.startswith(OHJSUnitPrefixes.TEST_FINISHED_RESULT_MSG.value):
            self._handle_result_msg(line)

    def handle_sum_line(self, line):
        value = line[len(OHJSUnitPrefixes.SUM.value):].split("=", 1)[0]
        self.cur_sum = int(value)

    def handle_status_line(self, line):
        self.parse_key(line, len(OHJSUnitPrefixes.STATUS.value))
        if self.cur_sum > 0 and \
                self.current_key == OHJSUnitItemConstants.CLASS.value:
            if self.current_value not in self.runner.suite_recorder.keys():
                current_suite = self.state_machine.suite(reset=True)
                current_suite.test_num = self.cur_sum
                current_suite.suite_name = self.current_value
                self.runner.suite_recorder.update({
                    self.current_value:
                        [len(self.runner.suite_recorder.keys()),
                         current_suite]})
            else:
                current_suite = self.runner.suite_recorder.get(
                    self.current_value)[1]
                self.state_machine.current_suite = current_suite
            self.cur_sum = -1
            self.current_key = None
            self.current_value = None
            self.state_machine.running_test_index = 0
            for listener in self.get_listeners():
                suite = copy.copy(current_suite)
                listener.__started__(LifeCycle.TestSuite, suite)

        else:
            if self.current_key == OHJSUnitItemConstants.SUITE_CONSUMING.value:
                self.test_time = int(self.current_value)
                self.handle_suite_end()
            else:
                self.submit_current_key_value()
                self.parse_key(line, len(OHJSUnitPrefixes.STATUS.value))

    def submit_current_key_value(self):
        if self.current_key and self.current_value:
            status_value = self.current_value
            test_info = self.state_machine.test()
            if self.current_key == OHJSUnitItemConstants.CLASS.value:
                test_info.test_class = status_value
            elif self.current_key == OHJSUnitItemConstants.TEST.value:
                test_info.test_name = status_value
            elif self.current_key == OHJSUnitItemConstants.NUM_TESTS.value:
                test_info.num_tests = int(status_value)
            elif self.current_key == OHJSUnitItemConstants.STACK.value:
                test_info.stacktrace = status_value
            self.current_key = None
            self.current_value = None

    def parse_key(self, line, key_start_pos):
        key_value = line[key_start_pos:].split("=", 1)
        if len(key_value) == 2:
            self.current_key = key_value[0]
            self.current_value = key_value[1]

    def parse_status_code(self, line):
        value = line[len(OHJSUnitPrefixes.STATUS_CODE.value):]
        test_info = self.state_machine.test()
        test_info.code = int(value)
        if test_info.code != StatusCodes.IN_PROGRESS:
            if self.check_legality(test_info.test_class) and \
                    self.check_legality(test_info.test_name):
                self.report_result(test_info)
                self.clear_current_test_info()

    def clear_current_test_info(self):
        self.state_machine.current_test = None

    def report_result(self, test_info):
        if not test_info.test_name or not test_info.test_class:
            LOG.info("Invalid instrumentation status bundle")
            return
        if test_info.code == StatusCodes.START.value:
            self.start_time = get_cst_time()
            for listener in self.get_listeners():
                result = copy.copy(test_info)
                listener.__started__(LifeCycle.TestCase, result)
            return
        if test_info.code == StatusCodes.FAILURE.value:
            self.state_machine.running_test_index += 1
            test_info.current = self.state_machine.running_test_index
            end_time = get_cst_time()
            run_time = (end_time - self.start_time).total_seconds()
            test_info.run_time = int(run_time * 1000)
            for listener in self.get_listeners():
                result = copy.copy(test_info)
                result.code = ResultCode.FAILED.value
                listener.__ended__(LifeCycle.TestCase, result)
                if listener.__class__.__name__ == "ReportListener" \
                        and self.runner.retry_times > 1:
                    index = list(listener.tests.keys())[-1]
                    listener.tests.pop(index)
            test_info.is_completed = True
        elif test_info.code == StatusCodes.ERROR.value:
            self.state_machine.running_test_index += 1
            test_info.current = self.state_machine.running_test_index
            end_time = get_cst_time()
            run_time = (end_time - self.start_time).total_seconds()
            test_info.run_time = int(run_time * 1000)
            for listener in self.get_listeners():
                result = copy.copy(test_info)
                result.code = ResultCode.FAILED.value
                listener.__ended__(LifeCycle.TestCase, result)
                if listener.__class__.__name__ == "ReportListener" \
                        and self.runner.retry_times > 1:
                    index = list(listener.tests.keys())[-1]
                    listener.tests.pop(index)
            test_info.is_completed = True
        elif test_info.code == StatusCodes.SUCCESS.value:
            self.state_machine.running_test_index += 1
            test_info.current = self.state_machine.running_test_index
            end_time = get_cst_time()
            run_time = (end_time - self.start_time).total_seconds()
            test_info.run_time = int(run_time * 1000)
            for listener in self.get_listeners():
                result = copy.copy(test_info)
                result.code = ResultCode.PASSED.value
                listener.__ended__(LifeCycle.TestCase, result)
            test_info.is_completed = True

    @classmethod
    def output_stack_trace(cls, test_info):
        if check_pub_key_exist():
            return
        if test_info.stacktrace:
            stack_lines = test_info.stacktrace.split(r"\r\n")
            LOG.error("Stacktrace information is:")
            for line in stack_lines:
                line.strip()
                if line:
                    LOG.error(line)

    @staticmethod
    def check_legality(name):
        if not name or name == "null":
            return False
        return True

    def __done__(self):
        LOG.debug("OHJSParser data:")
        LOG.debug(self.result_data)
        self.result_data = ""

    def handle_suite_end(self):
        suite_result = self.state_machine.suite()
        suite_result.run_time = self.test_time
        suite_result.is_completed = True
        for listener in self.get_listeners():
            suite = copy.copy(suite_result)
            listener.__ended__(LifeCycle.TestSuite, suite, is_clear=True)

    def handle_suites_end(self):
        suite_result = self.state_machine.suite()
        if not suite_result.is_completed:
            self.handle_suite_end()
        for listener in self.get_listeners():
            if listener.__class__.__name__ == "ReportListener":
                self._cal_result(listener)
            suite = copy.copy(suite_result)
            listener.__ended__(LifeCycle.TestSuites, suite,
                               suites_name=self.suites_name)
        self.state_machine.current_suite = None

    def _cal_result(self, report_listener):
        result_len = len(report_listener.result)
        suites_len = len(report_listener.suites)
        if result_len > suites_len:
            diff_result_tuple_list = report_listener.result[suites_len:]
            report_listener.result = report_listener.result[:suites_len]
            for diff_result_tuple in diff_result_tuple_list:
                suite, case_result_list = diff_result_tuple
                pos = self.runner.suite_recorder.get(suite.suite_name)[0]
                report_listener.result[pos][1].extend(case_result_list)
        self._handle_lacking_one_testcase(report_listener)
        self._handle_lacking_whole_suite(report_listener)

    def _handle_lacking_one_testcase(self, report_listener):
        for suite in report_listener.suites.values():
            test_des_list = self.runner.expect_tests_dict.get(
                suite.suite_name, [])
            pos  = self.runner.suite_recorder.get(suite.suite_name)[0]
            if len(test_des_list) == len(report_listener.result[pos][1]):
                continue
            interval = len(test_des_list) - len(report_listener.result[pos][1])
            if len(test_des_list) > 0:
                LOG.info("{} tests in {} had missed.".format(
                    interval, suite.suite_name))
            else:
                LOG.info("The count of tests in '{}' is incorrect! {} test "
                         "form dry run and {} tests have run."
                         "".format(suite.suite_name, len(test_des_list),
                                   len(report_listener.result[pos][1])))
            for test_des in test_des_list:
                is_contain = False
                for case in report_listener.result[pos][1]:
                    if case.test_name == test_des.test_name:
                        is_contain = True
                        break
                if not is_contain:
                    test_result = self.state_machine.test(reset=True)
                    test_result.test_class = test_des.class_name
                    test_result.test_name = test_des.test_name
                    test_result.stacktrace = "error_msg:mark blocked"
                    test_result.num_tests = 1
                    test_result.run_time = 0
                    test_result.current = \
                        self.state_machine.running_test_index + 1
                    test_result.code = ResultCode.BLOCKED.value
                    report_listener.result[pos][1].append(test_result)
                    LOG.debug("Add {}#{}".format(test_des.class_name,
                                                 test_des.test_name))

    def _handle_lacking_whole_suite(self, report_listener):
        all_suite_set = set(self.runner.expect_tests_dict.keys())
        un_suite_set = set()
        if len(all_suite_set) > len(report_listener.suites):
            suite_name_set = set()
            for suite in report_listener.suites.values():
                suite_name_set.add(suite.suite_name)
            un_suite_set = all_suite_set.difference(suite_name_set)
            if un_suite_set:
                LOG.info("{} suites have missed.".format(len(un_suite_set)))
        for name in un_suite_set:
            self.state_machine.running_test_index = 0
            test_des_list = self.runner.expect_tests_dict.get(
                name, [])
            current_suite = self.state_machine.suite(reset=True)
            current_suite.test_num = len(test_des_list)
            current_suite.suite_name = name
            for listener in self.get_listeners():
                suite = copy.copy(current_suite)
                listener.__started__(LifeCycle.TestSuite, suite)

            for test in test_des_list:
                test_result = self.state_machine.test(reset=True)
                test_result.test_class = test.class_name
                test_result.test_name = test.test_name
                test_result.stacktrace = "error_msg:mark blocked"
                test_result.num_tests = 1
                test_result.run_time = 0
                test_result.current = self.state_machine.running_test_index + 1
                test_result.code = ResultCode.BLOCKED.value
                test_result = copy.copy(test_result)
                for listener in self.get_listeners():
                    listener.__started__(LifeCycle.TestCase, test_result)
                test_result = copy.copy(test_result)
                for listener in self.get_listeners():
                    listener.__ended__(LifeCycle.TestCase, test_result)
                self.state_machine.running_test_index += 1
            current_suite.run_time = self.test_time
            current_suite.is_completed = True
            for listener in self.get_listeners():
                suite = copy.copy(current_suite)
                listener.__ended__(LifeCycle.TestSuite, suite, is_clear=True)

    def notify_task_finished(self):
        self.handle_suites_end()

    def _handle_result_msg(self, line):
        if OHJSUnitItemConstants.APP_DIED.value in line:
            test_result = self.state_machine.test()
            suite = self.state_machine.suite()
            if not test_result.is_completed:
                if self.check_legality(test_result.test_class) and \
                        self.check_legality(test_result.test_name):
                    self.report_result(test_result)
                    self.clear_current_test_info()
            if not suite.is_completed:
                self.handle_suite_end()


@Plugin(type=Plugin.PARSER, id=CommonParserType.oh_jsunit_list)
class OHJSUnitTestListParser(IParser):

    def __init__(self):
        self.tests = []
        self.json_str = ""
        self.tests_dict = dict()
        self.result_data = ""

    def __process__(self, lines):
        for line in lines:
            self.result_data = "{}{}".format(self.result_data, line)
            self.parse(line)

    def __done__(self):
        LOG.debug("OHJSUnitTestListParser data:")
        LOG.debug(self.result_data)
        self.result_data = ""

    def parse(self, line):
        if "{" in line or "}" in line:
            self.json_str = "%s%s" % (self.json_str, line)
            return
        if "dry run finished" in line:
            suite_dict_list = json.loads(self.json_str).get("suites", [])
            for suite_dict in suite_dict_list:
                for class_name, test_name_dict_list in suite_dict.items():
                    self.tests_dict.update({class_name.strip(): []})
                    for test_name_dict in test_name_dict_list:
                        for test_name in test_name_dict.values():
                            test = TestDescription(class_name.strip(),
                                                   test_name.strip())
                            self.tests_dict.get(
                                class_name.strip()).append(test)
                            self.tests.append(test)
