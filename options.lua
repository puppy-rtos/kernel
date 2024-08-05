
option("build_board")
    set_default("qemu-virt-riscv")
    set_values("stm32-f407-fkm1", "raspberry-pico", "qemu-virt-riscv")
    set_description("Build board")

option("build_toolchian")
    set_default("arm-none-eabi-gcc")
    set_values("arm-none-eabi-gcc", "armclang")
    set_description("Build Toolchian")
