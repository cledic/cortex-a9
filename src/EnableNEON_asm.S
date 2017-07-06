    .syntax unified
    .text                      			@ place our code in the text-section 
    
    .func EnableNEON_asm
    .global EnableNEON_asm       		@ make EnableNEON_asm symbol global 
    .type EnableNEON_asm,%function   	@ EnableNEON_asm is a function 

@/*
@* void EnableNEON_asm( void);
@*
@* @brief  Enable the NEON funcionality.
@* @return none.
@*/

EnableNEON_asm:
    push    {r4-r9,lr}    @ save r4-r7 and return address 
	
	ldr r4, =(0xF << 20)		@ Questa codice abilita il NEON
	mcr p15, 0, r4, c1, c0, 2   @ 
	mov r3, #0x40000000         @ 
	/* vmsr FPEXC, r3 */        @ 
    .long 0xeee83a10            @ 
		
    pop     {r4-r9,pc}    @ restore saved registers and return 

    .pool                  @ allow the assembler to place a literal pool here
    .size   EnableNEON_asm,.-EnableNEON_asm    @ tell linker how large this function is
    
    .endfunc                                   @(for debugging info)

