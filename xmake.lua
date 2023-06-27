
includes("toolchains/*.lua")

add_rules("mode.debug", "mode.release")
add_files("src/*.c")
add_files("arch/**.c")
add_includedirs("include", "arch/include")

add_files("subsystem/nr_micro_shell/src/*.c", "subsystem/nr_micro_shell/examples/nr_micro_shell_commands.c")
add_includedirs("subsystem/nr_micro_shell/inc")


includes("targets/stm32-f412-nucleo/xmake.lua")
