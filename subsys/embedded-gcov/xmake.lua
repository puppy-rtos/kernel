
target("puppy")

if has_config("gcov") then
    add_defines('ENABLE_GCOV')
    add_files("code/*.c")
    add_includedirs("code")
end