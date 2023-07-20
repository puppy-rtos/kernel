
target("puppy")

if has_config("p_stdlib_h") then
    add_defines('ENABLE_STDLIB_H')
    if is_config("build_toolchian", "armclang") then
        add_files("*_armclang.c")
    elseif is_config("build_toolchian", "arm-none-eabi-gcc") then
        add_files("*_gcc.c")
    end
end