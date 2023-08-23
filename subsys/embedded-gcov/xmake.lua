
target("puppy")

if has_config("gcov") then
    add_defines('ENABLE_GCOV')
    add_files("gcov/*.c")
    add_includedirs("gcov")
end