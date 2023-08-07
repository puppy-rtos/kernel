

target("puppy")

-- if has_config("nr_micro_shell") then
    add_defines('ENABLE_KASAN')
    add_files("*.c", {cxflags = "-fno-sanitize=kernel-address"})
    add_includedirs(".")
-- end
