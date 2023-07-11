set_project("puppy")
set_version("0.0.1")

includes("options.lua")

add_rules("mode.debug", "mode.release")


add_defines('PUPPY_RTOS')
add_files("src/*.c")
add_files("arch/**.c")
add_includedirs("include", "arch/include")

if has_config("nr_micro_shell") then
    add_defines('ENABLE_NR_SHELL')
    add_files("subsys/nr_micro_shell/src/*.c", 
              "subsys/nr_micro_shell/examples/nr_micro_shell_commands.c",
              "subsys/nr_micro_shell/examples/nr_micro_shell_thread.c")
    add_includedirs("subsys/nr_micro_shell/inc")
end

if has_config("tlsf") then
    add_defines('ENABLE_TLSF')
    add_files("subsys/tlsf/*.c")
    add_includedirs("subsys/tlsf")
end

if has_config("p_stdlib_h") then
    add_defines('ENABLE_STDLIB_H')
    if is_config("build_toolchian", "armclang") then
        add_files("subsys/posix/stdlib/*_armclang.c")
    elseif is_config("build_toolchian", "arm-none-eabi-gcc") then
        add_files("subsys/posix/stdlib/*_gcc.c")
    end
end

if has_config("kernel_test") then
    add_defines('ENABLE_KERNEL_TEST')
    add_files("tests/kernel-tests/*.c")
end

includes("targets/"..target_name.."/xmake.lua")
