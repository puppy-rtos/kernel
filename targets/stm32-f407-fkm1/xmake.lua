

target("puppy")
    add_files("Core/Src/*.c", "/Drivers/**.c")
    add_defines("USE_HAL_DRIVER", "STM32F407xx", "_POSIX_C_SOURCE=1")
    add_includedirs("Drivers/STM32F4xx_HAL_Driver/Inc", 
                "Drivers/STM32F4xx_HAL_Driver/Inc/Legacy", 
                "Core/Inc", 
                "Drivers/CMSIS/Device/ST/STM32F4xx/Include",
                "Drivers/CMSIS/Include"
                )

    if is_config("toolchian", "armclang") then
        add_files("Drivers/CMSIS/Device/ST/STM32F4xx/arm/*.s")
        set_toolchains("armclang")
        set_arch("cortex-m4")
        add_rules("mdk.binary")
        set_runtimes("microlib")
        add_cflags('-Wa,-mimplicit-it=thumb')
        add_ldflags('--strict --scatter ' .. os.scriptdir() .. '/link.sct')
        after_build(function (package)
            if is_mode('debug') then
                os.exec("D:/Progrem/Keil_v5/ARM/ARMCLANG/bin/fromelf.exe --bin $(buildir)/cross/cortex-m4/debug/puppy.axf --output puppy.bin")
            else
                os.exec("D:/Progrem/Keil_v5/ARM/ARMCLANG/bin/fromelf.exe --bin $(buildir)/cross/cortex-m4/release/puppy.axf --output puppy.bin")
            end
        end)
    elseif is_config("toolchian", "arm-none-eabi-gcc") then
        add_files("Drivers/CMSIS/Device/ST/STM32F4xx/gcc/startup_stm32f407xx.s")
        set_toolchains("arm-none-eabi-gcc")
        set_extension(".elf")
        set_arch("cortex-m4")
        add_links("c", "m", "nosys");
        add_ldflags(' -T ' .. os.scriptdir() .. '/link.lds')
        after_build(function (package)
            if is_mode('debug') then
                os.exec("D:/Progrem/env-windows/tools/gnu_gcc/arm_gcc/mingw/bin/arm-none-eabi-objcopy.exe -O binary $(buildir)/cross/cortex-m4/debug/puppy.elf puppy.bin")
            else
                os.exec("D:/Progrem/env-windows/tools/gnu_gcc/arm_gcc/mingw/bin/arm-none-eabi-objcopy.exe -O binary $(buildir)/cross/cortex-m4/release/puppy.elf puppy.bin")
            end
        end)       
    end
