# Original credits to ghaflims
# cortex-a9
This is my attempt to do baremetal programming on cortex-a9 using qemu and vexpress-a9
so far I got keyboard (pl050) , CLCD display (pl111) , timer (sp804) , uart (pl011) , interrupt (bare minimum Generic Interrupt Controller) (gic) working
the code is a mess at this moment.. this is just a proof of concept
if I have time I'm planning to clean up the code and seperate things like (Makefile, drivers, syscall, startup, interrupt ..etc)

tags: qemu cortex-a9 vexpress-a9 arm gic pl050 pl111 sp804 pl011 kmi clcd timer uart irq

# NEON development
I use this code to try to develop NEON assembler code on windows, using Linaro GCC and QEMU.

# Software installation
I donwload and install:

qemu-w32-setup-20170420

gcc-linaro-6.3.1-2017.05-i686-mingw32_arm-linux-gnueabihf.tar

Then I update the PATH environment var with the gcc and qemu path.

