# Utest 自动测试机器人

![GitHub Action](https://github.com/armink/UtestRunner/workflows/AutoTestCI/badge.svg)
![Travis CI](https://api.travis-ci.com/armink/UtestRunner.svg?branch=master)

## what

utest 是 RT-Thread 自带的单元测试框架，本项目为其提供自动化测试机器人（Runner），方便使用 utest 的项目自动化执行测试或持续集成（CI）。

## why

使用本项目可以让 utest 测试用例在 qemu 或 真实硬件 平台上自动化运行起来，并获取测试结果，提升测试效率。

## how

### 1. 基于QEMU平台运行自动化测试

#### STEP1 准备 BSP

准备一个可以集成测试用例的 BSP 工程，支持 Scons 构建，并保证可以构建成功，并生成 bin 或 elf。

#### STEP2 准备运行环境

运行环境需要支持以下条件：

- python3
- scons
- qemu-system-arm

也可以通过以下 CI 环境，自动化执行测试机器人，比如：

- Github Action
- Travis CI

项目已提供好其配置模板，详见根目录对应文件及文件夹

#### STEP3 运行测试机器人（以 QEMU 为例）

QEMU 测试机器人代码位于 `qemu_runner.py` ，help 信息如下：

```shell
python qemu_runner.py --help
usage: qemu_runner.py [-h] --elf path --sd path [--delay seconds]

QEMU runner for RT-Thread utest

optional arguments:
  -h, --help       show this help message and exit
  --elf path       elf file path for running QEMU
  --sd path        SD filesystem binary file for QEMU
  --delay seconds  delay some seconds for QEMU running finish
```

传入 BSP 编译出来 elf 路径及 QEMU 启动所需的 sd.bin 文件路径，即可启动机器人，以下是示例：

```shell
D:\Program\Python\UtestQemuRunner>python qemu_runner.py --elf D:/Program/RTT/rt-thread/bsp/qemu-vexpress-a9/Debug/rtthread.elf --sd D:/Program/RTT/rt-thread/bsp/qemu-vexpress-a9/sd.bin
2020-02-04 23:07:29,686 qemu_runner INFO: QEMU environment check OK.
2020-02-04 23:07:29,687 qemu_runner DEBUG: QEMU emulator version 3.0.0 (v3.0.0-11723-ge2ddcc5879-dirty)
Copyright (c) 2003-2017 Fabrice Bellard and the QEMU Project developers

WARNING: Image format was not specified for 'D:/Program/RTT/rt-thread/bsp/qemu-vexpress-a9/sd.bin' and probing guessed raw.
         Automatically detecting the format is dangerous for raw images, write operations on block 0 will be restricted.
         Specify the 'raw' format explicitly to remove the restrictions.
2020-02-04 23:07:35,717 utest DEBUG: Get a testcase: packages.tools.easyflash, timeout: 20
2020-02-04 23:07:35,717 utest INFO: Start test: packages.tools.easyflash
2020-02-04 23:07:46,016 utest INFO: Test passed
2020-02-04 23:07:46,519 qemu_runner INFO: QEMU runner destroy

D:\Program\Python\UtestQemuRunner>
```

### 2. 基于真实硬件平台运行自动化测试

#### STEP1 准备 BSP

准备一个可以集成测试用例的 BSP 工程，支持 Scons 构建，并保证可以构建成功，并生成 bin 或 elf。

#### STEP2 准备运行环境

运行环境需要支持以下条件：

- python3
- scons
- pyserial
- pyserial-asyncio

>  注意：目前不支持通过 CI 环境执行。

#### STEP3 运行测试机器人（以 Serial 为例）

Serial 测试机器人代码位于 `serial _runner.py` ，help 信息如下：

```shell
python .\serial_runner.py --help
usage: serial_runner.py [-h] [--port port] [--baudrate bps] [--delay seconds]

serial runner for RT-Thread utest

options:
  -h, --help       show this help message and exit
  --port port      port for serial
  --baudrate bps   baudrate for serial
  --delay seconds  delay some seconds for serial running finish
```

先将 BSP 编译出来 elf 烧录到开发板上，运行起来。

然后传入开发板对应的串口号和波特率即可启动机器人，以下是示例：

```shell
python.exe .\serial_runner.py --port=com16 --baudrate=115200
2023-07-04 12:04:25,289 asyncio DEBUG: Using proactor: IocpProactor
2023-07-04 12:04:31,419 utest DEBUG: Get a testcase: testcases.utest.pass_tc, timeout: 10
2023-07-04 12:04:31,420 utest INFO: Start test: testcases.utest.pass_tc
2023-07-04 12:04:32,552 utest INFO: Test passed
2023-07-04 12:04:32,552 serial_runner INFO: serial runner destroy
```
