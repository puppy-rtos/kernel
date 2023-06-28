toolchain("arm-none-eabi-gcc")
    set_kind("standalone")
    set_description("arm-none-eabi-gcc")
    local sdk_dir = "D:/Progrem/arm-gnu-toolchain-12.2.rel1"
    set_sdkdir(sdk_dir)

    on_load(function(toolchain)
      toolchain:load_cross_toolchain()
      toolchain:add("cxflags", "-mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Dgcc -Wall --specs=nosys.specs", {force = true})
      toolchain:add("ldflags", "-mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Wl,--gc-sections,-Map=puppy.map,-cref,-u,Reset_Handler --specs=nosys.specs", {force = true})
      toolchain:add("asflags", "-c -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -x assembler-with-cpp -Wa,-mimplicit-it=thumb", {force = true})
  end)

toolchain_end()
