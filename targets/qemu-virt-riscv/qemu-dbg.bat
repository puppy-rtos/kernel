
:run
start D:\Progrem\env-windows\tools\qemu\qemu64\qemu-system-riscv32.exe -nographic -machine virt -net none -chardev stdio,id=con,mux=on -serial chardev:con -mon chardev=con,mode=readline -bios none -smp 4 -kernel puppy.bin -S -s
