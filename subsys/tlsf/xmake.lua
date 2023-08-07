
target("puppy")

if has_config("tlsf") then
    add_defines('ENABLE_TLSF')
    add_files("*.c", {cxflags = "-fno-sanitize=kernel-address"})
    add_includedirs("./")
end