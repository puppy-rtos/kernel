
target("puppy")

    add_includedirs("include")
if is_config("build_target", "qemu-virt-riscv") then
    add_files("riscv/**.c")
else
    add_files("arm/**.c")
end
