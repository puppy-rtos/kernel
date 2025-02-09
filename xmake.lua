set_project("puppy")
set_version("0.0.1")

includes("options.lua")

add_rules("mode.debug", "mode.release")

includes("boards/**/xmake.lua")

target("puppy")
    add_files("apps/*.c")
    add_files("core/*.c")
    add_files("core/**/*.c")
    add_includedirs("core/.")
