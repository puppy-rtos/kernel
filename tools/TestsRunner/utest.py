#!/user/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2020, Armink, <armink.ztl@gmail.com>
#
# SPDX-License-Identifier: Apache-2.0
#
# Change Logs:
# Date           Author       Notes
# 2020-01-27     armink       the first version
#

import collections
import logging
import re

LOG_LVL = logging.DEBUG
LOG_TAG = 'utest'
logger = logging.getLogger(LOG_TAG)

class Utest:

    def __init__(self, exec_cmd):
        self.exec_cmd = exec_cmd
        self.testcases = collections.OrderedDict()
        signaled, tc_list_log = self.exec_cmd('tc list', 1)

        if tc_list_log == []:
            self.init_result = False
        else:
            self.init_result = True

        for line in tc_list_log:
            if line.find('Get a testcase:') != -1:
                # get the testcase name
                reg1 = re.compile(r'Get a testcase: (.*?tc)', re.S | re.M)
                tc_name = re.findall(reg1, line)[0]
                # get the testcase timeout
                tc_timeout = 10
                # add to list
                self.testcases[tc_name] = tc_timeout
                # self.testcases.append(testcase)
                logger.debug('Get a testcase: ' + tc_name + ', timeout: ' + str(tc_timeout))

    def test(self, name):
        logger.info('Start test: ' + name)
        signaled, tc_list_log = self.exec_cmd('tc run ' + name, self.testcases[name])
        if tc_list_log == []:
            return False
        for line in tc_list_log:
            if line.find('Test Passed!') != -1:
                if (line.find('Passed')) != -1:
                    logger.info('Test passed')
                    return True
                else:
                    logger.error('Test failed')
                    return False

    def test_all(self):
        success = True
        for k, v in self.testcases.items():
            if not self.test(k):
                success = False
        return success
