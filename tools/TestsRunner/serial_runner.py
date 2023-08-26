#!/user/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2020, Armink, <armink.ztl@gmail.com>
#
# SPDX-License-Identifier: Apache-2.0
#
# Change Logs:
# Date           Author       Notes
# 2020-01-26     armink       the first version
# 2023-07-03     flybreak     support serial port
#
import argparse
import os
import sys
import threading
import logging
import time
import asyncio
from serial_asyncio import open_serial_connection

from utest import Utest

LOG_LVL = logging.DEBUG
LOG_TAG = 'serial_runner'
logger = logging.getLogger(LOG_TAG)

class SerialRunner:
    def __init__(self, port, baudrate, delay):
        logging.basicConfig(level=LOG_LVL, format='%(asctime)s %(name)s %(levelname)s: %(message)s', datefmt=None)
        self.port = port
        self.baudrate = baudrate
        self.delay = delay
        self.env_version = ''
        self.sub_proc = None
        self.reader = None
        self.writer = None
        self.log_recorder = None
        self.logs = []
        self.logs_lock = threading.Lock()
        self.is_executing = False

    def run(self):

        event = threading.Event()

        # RT-Thread console log recorder with serial process
        def log_recorder_entry():
            async def run():
                reader, writer = await open_serial_connection(url=self.port, baudrate=self.baudrate)
                self.reader = reader
                self.writer = writer
                with open('console.log', 'w') as log_file:
                    while True:
                        line = await self.reader.readline()
                        if line:
                            line = str(line, encoding="utf8").replace('\n', '').replace('\r', '')
                            log_file.write(line + '\n')
                            log_file.flush()
                            self.logs_lock.acquire()
                            self.logs.append(line)
                            self.logs_lock.release()
                            if self.is_executing and line.find('Test end') != -1:
                                event.set()
            loop =  asyncio.new_event_loop()
            asyncio.set_event_loop(loop)
            loop.run_until_complete(run())

        self.log_recorder = threading.Thread(target=log_recorder_entry)
        self.log_recorder.daemon = True
        self.log_recorder.start()

        time.sleep(0.1)
        if self.writer == None:
            logger.error("Utest init failed!")
            return False

        def exec_utest_cmd(name, timeout):
            self.logs_lock.acquire()
            self.logs.clear()
            self.logs_lock.release()
            self.is_executing = True

            # send command to serial
            cmb_str = name + '\n'
            if self.writer:
                self.writer.write(bytes(cmb_str, encoding='utf8'))
            else:
                self.is_executing = False
                return False, []

            # wait command execute finish
            event.clear()
            signaled = event.wait(timeout=timeout)
            self.is_executing = False

            self.logs_lock.acquire()
            logs = self.logs
            self.logs_lock.release()
            if logs == []:
                logger.error("Command exec failed!")
            return signaled, logs

        time.sleep(self.delay)
        utest = Utest(exec_utest_cmd)
        if utest.init_result == False:
            logger.error("Utest init failed!")
            self.destroy()
            return False

        success = utest.test_all()
        self.destroy()

        return success

    def destroy(self):
        logger.info("serial runner destroy")


def main():
    parser = argparse.ArgumentParser(description='serial runner for Puppy-RTOS tests', prog=os.path.basename(sys.argv[0]))

    parser.add_argument('--port',
                        metavar='port',
                        help='port for serial',
                        default="com16",
                        type=str,
                        required=False)

    parser.add_argument('--baudrate',
                        metavar='bps',
                        help='baudrate for serial',
                        default=115200,
                        type=int,
                        required=False)
    
    parser.add_argument('--delay',
                        metavar='seconds',
                        help='delay some seconds for serial running finish',
                        default=5,
                        type=int,
                        required=False)

    args = parser.parse_args()
    runner = SerialRunner(args.port, args.baudrate, args.delay)
    if not runner.run():
        sys.exit(-1)


if __name__ == '__main__':
    main()
