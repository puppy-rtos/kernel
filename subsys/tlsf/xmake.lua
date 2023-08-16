
target("puppy")

if has_config("tlsf") then
    add_defines('ENABLE_TLSF')

if has_config("kasan") then
    add_files("*.c", {cxflags = "-fno-sanitize=kernel-address"})
else
    add_files("*.c")
end

    add_includedirs("./")
end