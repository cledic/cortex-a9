    .syntax unified
    .text                      @ place our code in the text-section 
    
    .func MergeRGB888_asm
    .global MergeRGB888_asm       @ make MergeRGB888_asm symbol global 
    .type MergeRGB888_asm,%function   @ MergeRGB888_asm is a function 

@/*
@* void MergeRGB888_asm( uint32_t**pSrc, uint32_t*pDst, uint32_t size);
@*
@* @brief  Merge the three R, G, B channel to single RGB888 buffer.
@* @param[in]  **pSrc      [r0]    	puntatore ad un array di puntatori ai buffer R, G, B
@* @param[in]  *pDst       [r1]    	puntatore all'immagine RGB
@* @param[in]  array_size  [r2]		size of pSrc.
@* @return none.
@*/

MergeRGB888_asm:
    push    {r4-r9,lr}    @ save r4-r7 and return address 
		
	mov 	r4, r1		@ carico l'indirizzo al buffer RGB 	
	mov	r8, r0			@ carico il puntatore ai tre buffer RGB
	ldr	r5, [r8], #4	@ carico i puntatori ai tre buffer RGB
	ldr	r6, [r8], #4
	ldr	r7, [r8], #4	
	mov	r8, r2			@ carico la lunghezza del buffer sorgente

MergeRGB888_asm_0:
	vld1.8	{d0},[r5]!
	vld1.8	{d1},[r6]!
	vld1.8	{d2},[r7]!

	vst3.8	{d0,d1,d2},[r4]!		@ salva i colori RGB888

	subs	r8,r8,#1
	bne     MergeRGB888_asm_0
	
	pop     {r4-r9,pc}    @ restore saved registers and return 

    .pool                  @ allow the assembler to place a literal pool here
    .size   MergeRGB888_asm,.-MergeRGB888_asm    @ tell linker how large this function is
    
    .endfunc                                   @(for debugging info)

