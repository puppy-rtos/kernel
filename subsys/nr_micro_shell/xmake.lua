

target("puppy")

if has_config("nr_micro_shell") then
    add_defines('ENABLE_NR_SHELL')
    add_files("src/*.c", 
              "examples/nr_micro_shell_commands.c",
              "examples/nr_micro_shell_thread.c")
    add_includedirs("inc")
end
