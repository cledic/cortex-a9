    .syntax unified
    .text                      @ place our code in the text-section 
    
    .func Color2Gray888_asm
    .global Color2Gray888_asm       	@ make Color2Gray888_asm symbol global 
    .type Color2Gray888_asm,%function   @ Color2Gray888_asm is a function 

@/*
@* void Color2Gray888_asm( uint32_t*pDst, uint32_t*pSrc, uint32_t size);
@*
@* @brief  Merge the three R, G, B channel to single RGB888 buffer.
@* @param[in]  *pDst       [r0]    	puntatore all'array RGB di destinazione
@* @param[in]  *pSrc       [r1]    	puntatore all'array RGB sorgente
@* @param[in]  array_size  [r2]		size of pSrc.
@* @return none.
@*/

Color2Gray888_asm:

	push   	    {r4-r5,lr}
	@ lsr         r2, r2, #3

	@ build the three constants:
	mov         r3, #77
	mov         r4, #151
	mov         r5, #28
	vdup.8      d3, r3
	vdup.8      d4, r4
	vdup.8      d5, r5

Color2Gray888_asm_0:
	@ load 8 pixels:
	vld3.8      {d0-d2}, [r1]!

	@ do the weight average:
	vmull.u8    q3, d0, d3			@ R * 77
	vmlal.u8    q3, d1, d4			@ G * 151
	vmlal.u8    q3, d2, d5			@ B * 28

	@ shift and store:
	vshrn.u16   d6, q3, #8
	
	vmov.8  d7,d6					@ copio il canale B/N su altri due registri per
	vmov.8  d8,d6					@ creare i tre array RGB888 
	
	vst3.8	{d6,d7,d8},[r0]!		@ salva il valore di grigio in RGB888
	
	subs        r2, r2, #1
	bne         Color2Gray888_asm_0

	pop         {r4-r5,pc}

	.pool            @ allow the assembler to place a literal pool here
	.size   Color2Gray888_asm,.-Color2Gray888_asm    @ tell linker how large this function is

	.endfunc         @(for debugging info)
