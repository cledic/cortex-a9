# NEON development on an emulated cortex-a9
Serching for an easy and everywhere usable tool to study and develop with ARM NEON, I found the **ghaflims** work to do baremetal programming on Cortex-A9 using QEMU.

I'm using his effort to start my study and development with ARM NEON. In my support came the wonderfull on-line tool: [NEVADA](http://szeged.github.io/nevada/) a NEON Visualizer Tool. This tool help me a lot, I use it to test instruction and syntax, before write down something and compile it. 

Still a work in progess, here I write some of the steps I follow. Hope to help.

# Software I use
I use this code to try to develop NEON assembler code on **cortex-a9** emulated processor on windows. I use **Linaro GCC**, and **QEMU**.
To debug the software I use **GDB**. See the file `gdb_cmd.txt` for the run time **GDB** options I use.

# Software installation
I use: **qemu-w32-setup-20170420** and **gcc-linaro-6.3.1-2017.05-i686-mingw32_arm-linux-gnueabihf.tar**.

After the installation I update the PATH environment var with the **GCC** and **QEMU** path.

To compile and run **QEMU** I use the BASH shell that Octave have installed on my PC. I modify the Makefile accordingly.

# RUN the software
To run the software I use the command `make qemu`. The `make` command execute: `qemu-system-arm -M vexpress-a9 -serial mon:stdio -kernel bin/kernel.elf`.

![alt text](https://github.com/cledic/cortex-a9/blob/master/neon_docs/cortex-a9_qemu.PNG "How run qemu")

# DEBUG the software
To debug I use `make dqemu` and, on a second bash shell, I run: `arm-linux-gnueabihf-gdb -se bin/kernel.elf -x gdb_cmd.txt`.
At this point, I set a breakpoint with the `b 77` comamnd, and issue the `c` command to `continue`. Then I use the `si` command to `step-into`. The GDB is configured to show the assembler line. See the content of the file: `gdb_cmd.txt`.

![alt text](https://github.com/cledic/cortex-a9/blob/master/neon_docs/cortex-a9_qemu_gdb.PNG "How run gdb with qemu")

# My code
Nothing special! I made a function to enable the NEON capabilities: `EnableNEON_asm`, and some function to: split the RGB channel: `SplitRGB_asm`, merge it to RGB again: `MergeRGB888_asm`. This code came from ARM example. I find on Internet a color to gray that reuse modified as: `Color2Gray888_asm`

![alt text](https://github.com/cledic/cortex-a9/blob/master/neon_docs/cortex-a9_qemu_comments.PNG "How run qemu")

I update the file ugui.c with a couple of functions to draw RGB.

Inside the file `kernel.c` I write some code to do geometrics modification to images. Some code came from a [my project](https://www.youtube.com/watch?v=LL79iNhs-dI) the other, working very well, from: `Image Processing in C by Dwayne Phillips`.

Mostly it's a work in progress. I'm planning to recode the geometrics functions in NEON Intrinsics.

The images I use are 160x120, and RGB888.

# Internet Link

## NEON

[ARM Coding for NEON Part 1: Load and Stores](https://community.arm.com/processors/b/blog/posts/coding-for-neon---part-1-load-and-stores)

[ARM Coding for NEON Part 2: Dealing With Leftovers](https://community.arm.com/processors/b/blog/posts/coding-for-neon---part-2-dealing-with-leftovers)

[ARM Coding for NEON Part 3: Matrix Multiplication](https://community.arm.com/processors/b/blog/posts/coding-for-neon---part-3-matrix-multiplication)

[ARM Coding for NEON Part 4: Shifting Left and Right](https://community.arm.com/processors/b/blog/posts/coding-for-neon---part-4-shifting-left-and-right)

[ARM Coding for NEON Part 5: Rearranging Vectors](https://community.arm.com/processors/b/blog/posts/coding-for-neon---part-5-rearranging-vectors)

[ARM NEON 1.0 Programmer's Guide](https://developer.arm.com/docs/den0018/latest/neontm-version-10-programmers-guide)

[Online NEON Simulator](http://szeged.github.io/nevada/)

[ARM NEON Optimization. An Example](http://hilbert-space.de/?p=22)

## GDB 

[Source and Machine Code](https://www-zeuthen.desy.de/unix/unixguide/infohtml/gdb/Machine-Code.html)

[Using gdb for Assembly Language Debugging](https://www.csee.umbc.edu/~cpatel2/links/310/nasm/gdb_help.shtml)

## GCC Inline Assembler

[ARM GCC Inline Assembler Cookbook](http://www.ethernut.de/en/documents/arm-inline-asm.html)

[GCC Inline Assembler PDF](http://www.ic.unicamp.br/~celio/mc404-s2-2015/docs/ARM-GCC-Inline-Assembler-Cookbook.pdf)




