
target("puppy")

if has_config("tlsf") then
    add_defines('ENABLE_TLSF')
    add_files("*.c")
    add_includedirs("./")
end