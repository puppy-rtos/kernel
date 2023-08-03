set_project("puppy")
set_version("0.0.1")

includes("options.lua")

add_rules("mode.debug", "mode.release")

includes("subsys/**/xmake.lua")
includes("tests/**/xmake.lua")
includes("targets/**/xmake.lua")
includes("arch/xmake.lua")

target("puppy")
    add_defines('PUPPY_RTOS')
    add_files("src/*.c", "apps/*c")
    add_includedirs("include")
