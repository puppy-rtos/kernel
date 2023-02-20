
includes("toolchains/*.lua")

add_rules("mode.debug", "mode.release")

target("kernel")
    add_files("src/*.c")
    add_files("arch/**.c")
    add_includedirs("include", "arch/include")
    -- if is_plat("gcc") then
        set_toolchains("arm-none-eabi")
        set_extension(".elf")
        set_arch("cortex-m4")
        add_links("c", "m", "nosys");
        add_ldflags(' -T link.lds')
    -- end
