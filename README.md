# Puppy RTOS

Puppy RTOS 项目的目标是实现一个运行在低资源平台上的 POSIX 兼容实时操作系统，系统的原生接口就是 POSIX 接口，没有学习门槛。这是一个全新的项目，所以在实现的时候会多尝试一些新技术，最终目标是希望实现一个支持用户交互，可在线安装应用的实时微内核。

### Roadmap

**M0：一个最小实现的实时内核 [Doing]**

 - 包含原子操作、实时调度、线程管理、线程间同步、中断管理、内存堆管理。
 - 支持一个 Target，可以在 STM32L4 平台上运行。
 - 支持两个工具链，gcc、armclang
 - 充分的内核测试用例

**M1：一个最小实现的实时微内核 [ToDo]**

 - 支持运行用户态应用
 - 用户空间内存隔离
 - 用户空间的应用崩溃不影响内核

**M2：一个支持用户交互的实时微内核 [ToDo]**

 - 支持 apt install 安装应用
 - 支持 apt update 检查更新
 - 支持 apt remove 卸载应用

## 如何体验
### 下载工具链

```
wget -q https://github.com/RT-Thread/toolchains-ci/releases/download/v1.3/gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2
sudo tar xjf gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2 -C /opt
/opt/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-gcc --version
```
### 配置编译

```
xmake f -p cross --sdk=/opt/riscv64-unknown-elf-toolchain-10.2.0-2020.12.8-x86_64-linux-ubuntu14 --build_board=qemu-virt-riscv -m debug
xmake
```
### 运行
```
qemu-system-riscv32.exe -nographic -machine virt -net none -chardev stdio,id=con,mux=on -serial chardev:con -mon chardev=con,mode=readline -bios none -smp 4 -kernel puppy.bin
```

**运行结果**

```
Build Time: Aug 18 2023 08:44:26
                           _
    ____   ____    _____  (_) _  __
   / __ \ / __ \  / ___/ / / | |/_/
  / /_/ // /_/ / (__  ) / /  >  <
 / .___/ \____/ /____/ /_/  /_/|_|
/_/          Powered dy puppy-rtos
[  0][I/drv.board] 1:IRQ:I am core 1!
[  0][I/drv.board] 3:IRQ:I am core 3!
[  0][I/drv.board] 2:IRQ:I am core 2!
puppy:

```

## GCOV

进行代码覆盖率测试。

### 配置编译
通过 xmake f --menu 打开图形配置菜单，开启 subsystem 下的 GCOV，然后编译工程。

```
xmake
```
### 运行
```
qemu-system-riscv32.exe -nographic -machine virt -net none -chardev stdio,id=con,mux=on -serial chardev:con -mon chardev=con,mode=readline -bios none -smp 4 -kernel puppy.bin
```
**运行结果**

系统启动会自动执行 gcov 初始化，然后运行 tc runall 即可运行全部测试用例，系统会在测试用例全部执行完毕后，打印所有的 gcov 数据。

```
Build Time: Aug 24 2023 07:21:04
                           _
    ____   ____    _____  (_) _  __
   / __ \ / __ \  / ___/ / / | |/_/
  / /_/ // /_/ / (__  ) / /  >  <
 / .___/ \____/ /____/ /_/  /_/|_|
/_/          Powered dy puppy-rtos
__gcov_call_constructors start
:__gcov_init called for E:\MyWorkspace\puppy-rtos\build\.objs\puppy\cross\riscv32\debug\kernel\src\cpu.c.gcda
:__gcov_init called for E:\MyWorkspace\puppy-rtos\build\.objs\puppy\cross\riscv32\debug\kernel\src\errno.c.gcda
:__gcov_init called for E:\MyWorkspace\puppy-rtos\build\.objs\puppy\cross\riscv32\debug\kernel\src\init.c.gcda
:__gcov_init called for E:\MyWorkspace\puppy-rtos\build\.objs\puppy\cross\riscv32\debug\kernel\src\mem.c.gcda
:__gcov_init called for E:\MyWorkspace\puppy-rtos\build\.objs\puppy\cross\riscv32\debug\kernel\src\object.c.gcda
:__gcov_init called for E:\MyWorkspace\puppy-rtos\build\.objs\puppy\cross\riscv32\debug\kernel\src\ostick.c.gcda
:__gcov_init called for E:\MyWorkspace\puppy-rtos\build\.objs\puppy\cross\riscv32\debug\kernel\src\pthread.c.gcda
:__gcov_init called for E:\MyWorkspace\puppy-rtos\build\.objs\puppy\cross\riscv32\debug\kernel\src\sched.c.gcda
:__gcov_init called for E:\MyWorkspace\puppy-rtos\build\.objs\puppy\cross\riscv32\debug\kernel\src\sched_policy_fool.c.gcda
:__gcov_init called for E:\MyWorkspace\puppy-rtos\build\.objs\puppy\cross\riscv32\debug\kernel\src\sem.c.gcda
:__gcov_init called for E:\MyWorkspace\puppy-rtos\build\.objs\puppy\cross\riscv32\debug\kernel\src\thread.c.gcda
:__gcov_init called for E:\MyWorkspace\puppy-rtos\build\.objs\puppy\cross\riscv32\debug\kernel\src\util.c.gcda
__gcov_call_constructors finish
[  0][I/drv.board] 3:IRQ:I am core 3!
[  0][I/drv.board] 1:IRQ:I am core 1!
[  0][I/drv.board] 2:IRQ:I am core 2!
puppy:
```

将完整的日志文件复制到项目根路径下，创建一个 log.txt 文件。然后运行下面的命令，解析日志并转化为对应的 gcda 文件

```
python .\subsys\embedded-gcov\scripts\log2gcda.py
```

### 生成网页

推荐使用 gcovr 生成展示网页，安装方法
```
pip install -U gcovr
```
**gcovr 使用方法**
```
mkdir -p CoverageGcovr
gcovr --html --html-details -o CoverageGcovr/index.html  --gcov-executable /opt/riscv64-unknown-elf-toolchain-10.2.0-2020.12.8-x86_64-linux-ubuntu14/bin/riscv64-unknown-elf-gcov
```
双击 CoverageGcovr 目录下的 index.html 即可在浏览器中查看内核的代码覆盖率。
