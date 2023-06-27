
option("target")
    set_default("stm32-f412-nucleo")
    set_showmenu(true)
option_end()

if is_config("target", "stm32-f412-nucleo") then
    target_name = "stm32-f412-nucleo"
else 
    target_name = "stm32-f412-nucleo"
end
