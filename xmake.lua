
includes("targets.lua")
includes("toolchains.lua")

add_rules("mode.debug", "mode.release")


add_defines('PUPPY_RTOS')
add_files("src/*.c")
add_files("arch/**.c")
add_includedirs("include", "arch/include")

if has_config("nr_micro_shell") then
    add_defines('ENABLE_NR_SHELL')
    add_files("subsys/nr_micro_shell/src/*.c", "subsys/nr_micro_shell/examples/nr_micro_shell_commands.c")
    add_includedirs("subsys/nr_micro_shell/inc")
end

includes("targets/"..target_name.."/xmake.lua")
