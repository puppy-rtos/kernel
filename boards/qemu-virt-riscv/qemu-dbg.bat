
:run
start D:\Progrem\qemu\qemu-system-riscv32.exe -nographic -machine virt -net none -chardev stdio,id=con,mux=on -serial chardev:con -mon chardev=con,mode=readline -bios none -smp 3 -kernel puppy.bin -S -s
