
target("puppy")

if has_config("kernel_test") then
    add_defines('ENABLE_KERNEL_TEST')
    add_files("*.c")
end
