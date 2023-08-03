set_project("puppy")
set_version("0.0.1")

includes("options.lua")

add_rules("mode.debug", "mode.release")

includes("subsys/**/xmake.lua")
includes("tests/**/xmake.lua")
includes("boards/**/xmake.lua")
includes("kernel/xmake.lua")

target("puppy")
    add_files("apps/*c")
