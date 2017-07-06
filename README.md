# Original credits to ghaflims
# cortex-a9
This is my attempt to do baremetal programming on cortex-a9 using qemu and vexpress-a9
so far I got keyboard (pl050) , CLCD display (pl111) , timer (sp804) , uart (pl011) , interrupt (bare minimum Generic Interrupt Controller) (gic) working
the code is a mess at this moment.. this is just a proof of concept
if I have time I'm planning to clean up the code and seperate things like (Makefile, drivers, syscall, startup, interrupt ..etc)

tags: qemu cortex-a9 vexpress-a9 arm gic pl050 pl111 sp804 pl011 kmi clcd timer uart irq

# NEON development on an emulated cortex-a9
I use this code to try to develop NEON assembler code on cortex-a9 emulated processor on windows. I use Linaro GCC, and QEMU.
To debug the software I use gdb. See the file gdb_cmd.txt for the run time gdb options I use.

# Software installation
I used: qemu-w32-setup-20170420 and gcc-linaro-6.3.1-2017.05-i686-mingw32_arm-linux-gnueabihf.tar

After the installation I update the PATH environment var with the gcc and qemu path.

To compile and run qemu I use the BASH shell that Octave have installed on my PC. I modify the Makefile accordirly.

# RUN the software
To run the software I use the command "make qemu". The make run "qemu-system-arm -M vexpress-a9 -serial mon:stdio -kernel bin/kernel.elf".
![alt text](https://github.com/cledic/cortex-a9/blob/master/cortex-a9_qemu.PNG "How run qemu")

# DEBUG the software
To debug I use "make dqemu" and on a second bash shell I run: "arm-linux-gnueabihf-gdb -se bin/kernel.elf -x gdb_cmd.txt".
At this point, I set a breakpoint and issue the "c" command. Then I use the "si" command to "step-into". The GDB is configured to show the assembler line.

