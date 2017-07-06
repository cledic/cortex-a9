    .syntax unified
    .text                      @ place our code in the text-section 
    
    .func SplitRGB_asm
    .global SplitRGB_asm       @ make SplitRGB_asm symbol global 
    .type SplitRGB_asm,%function   @ SplitRGB_asm is a function 

@/*
@* void SplitRGB_asm( uint32_t*pSrc, uint32_t**pDst, uint32_t size);
@*
@* @brief  Copy the RGB to single buffer.
@* @param[in]  *pSrc       [r0]    	puntatore all'immagine RGB
@* @param[in]  *pDest      [r1]    	puntatore ad un array di puntatori ai buffer R, G, B
@* @param[in]  array_size  [r2]		size of pSrc.
@* @return none.
@*/

SplitRGB_asm:
    	push    {r4-r9,lr}    @ save r4-r7 and return address 
		
	mov 	r4, r0			@ carico l'indirizzo sorgente	
	mov	r8, r1			@ carico il puntatore ai tre buffer RGB
	ldr	r5, [r8], #4		@ carico i tre buffer
	ldr	r6, [r8], #4
	ldr	r7, [r8], #4	
	mov	r8, r2			@ carico la lunghezza del buffer sorgente

SplitRGB_asm_0:
	vld3.8	{d0,d1,d2},[r4]!
	vst1.8	{d0},[r5]!
	vst1.8	{d1},[r6]!
	vst1.8	{d2},[r7]!
	subs	r8,r8,#1
	bne     SplitRGB_asm_0
	
	pop     {r4-r9,pc}    @ restore saved registers and return 

    .pool                  @ allow the assembler to place a literal pool here
    .size   SplitRGB_asm,.-SplitRGB_asm    @ tell linker how large this function is
    
    .endfunc                                   @(for debugging info)

