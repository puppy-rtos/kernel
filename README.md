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
