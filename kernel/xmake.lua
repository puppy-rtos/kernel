
target("puppy")

    add_defines('PUPPY_RTOS')
    add_files("src/*.c", {cxflags = "-fno-sanitize=kernel-address"})
    add_includedirs("include")

if is_config("build_board", "qemu-virt-riscv") then
    add_files("arch/riscv/**.c")
else
    add_files("arch/arm/**.c")
end
