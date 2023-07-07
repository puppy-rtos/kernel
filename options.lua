
option("target")
    set_default("stm32-f407-fkm1")
    set_values("stm32-f412-nucleo", "stm32-f407-fkm1")
    set_description("Build target")
option_end()

if is_config("target", "stm32-f412-nucleo") then
    target_name = "stm32-f412-nucleo"
elseif is_config("target", "stm32-f407-fkm1") then
    target_name = "stm32-f407-fkm1"
else
    target_name = "stm32-f407-fkm1"
end

option("toolchian")
    set_default("arm-none-eabi-gcc")
    set_values("arm-none-eabi-gcc", "armclang")
    set_description("Build Toolchian")
option_end()

option("nr_micro_shell")
    set_default(true)
    set_category("SubSystem")
    set_description("Enable or disable nr_micro_shell")
option("kernel_test")
        set_default(true)
        set_category("Tests")
        set_description("Enable or disable kernrl-tests")
option_end()