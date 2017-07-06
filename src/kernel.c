#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <arm_neon.h>
#include "pl011.h"
#include "pl050.h"
#include "pl111.h"
#include "sp804.h"
#include "interrupt.h"
#include "image2_160x120.h"

typedef float Transform3D[4][4];
Transform3D r1, r2, xform;

unsigned long htonl(unsigned long val) ;
void EnableNEON_asm( void);

/* NEON functions */
void SplitRGB_asm( uint8_t*pSrc, uint8_t**pDst, uint32_t size);
void MergeRGB565_asm( uint32_t**pSrc, uint32_t*pDst, uint32_t size);
void MergeRGB888_asm( uint32_t**pSrc, uint32_t*pDst, uint32_t size);
void Color2Gray888_asm( uint8_t*pDst, uint8_t*pSrc, uint32_t size);

/* */
void IdentMat( Transform3D t);
void FormatRotateMat(char axis, int dir, Transform3D t);
void ConcatMat( Transform3D dst, Transform3D src1, Transform3D src2);
void PartialNonHomTransform( void);

/* 360 gradi diviso 32 rotazioni = 11,25 */
#define sinphi  0.195090322f        /* sin( 11,25) = 0.195090322 */
#define cosphi  0.980785280f        /* cos( 11,25) = 0.980785280 */

/*
  qemu-system-arm -s -S -M vexpress-a9 -serial mon:stdio -kernel bin/kernel.elf
   arm-linux-gnueabihf-gdb -se bin/kernel.elf -x gdb_cmd.txt
  
  Contenuto del file gdb_cmd.txt:
   target remote localhost:1234
   set disassemble-next-line on
   
  Visualizza mem in float blocco di 16:
   x/16f &r1
*/

/* Struttura con i buffer di lavoro */
typedef union
{
	uint8_t image_area[IMAGE_SIZE];
	struct
	{
		uint8_t R[(IMAGE_SIZE/3)];
		uint8_t G[(IMAGE_SIZE/3)];
		uint8_t B[(IMAGE_SIZE/3)];
	} chnl;
} IMAGE_AREA;

/* */
IMAGE_AREA img, img2;

typedef struct 
{
	float angle;
	uint16_t m;
	uint16_t n;
	uint16_t rows;
	uint16_t cols;
} ROTATION_DATA;

ROTATION_DATA t;
void ArbritaryRotation( uint8_t**src_P, uint8_t**dst_P, ROTATION_DATA t);

/* Puntatore ai tre array singoli */
uint8_t**dst_P[]={img.chnl.R, img.chnl.G, img.chnl.B};
uint8_t**dst2_P[]={img2.chnl.R, img2.chnl.G, img2.chnl.B};

uint8x8_t rfac;
uint16_t A[]={1,2,3,4};

int main( void)
{
 	interrupt_init();
	clcd_init();
 	kb_init();
 	timer_init();
	
	/* Abilito le funzionalità NEON. Le istruzioni NEON non sono abilitate a default. */
	EnableNEON_asm();

	uint16x4_t v;
	v = vld1_u16(A);
	v = vadd_u16( v, v);
	vst1_u16( A, v);
	
	/* */
// 	UG_FillCircle(100, 100, 30, C_YELLOW);
// 	UG_FillCircle(200, 100, 10, C_RED);
// 	UG_FillCircle(250, 100, 10, C_BLUE);
// 	UG_FillCircle(350, 100, 20, C_GREEN);
// 	UG_FillCircle(300, 100, 10, 0xff0000);
// 	UG_DrawFrame(400,400,440,440,C_WHITE);
// 	UG_FontSelect(&FONT_16X26);
// 	UG_SetBackcolor(C_BLACK);
//	UG_SetForecolor(C_YELLOW);
// 	UG_PutString (200,200 ,"hello form the other side\nthis is a new line ");

	UG_DrawRGB( 10,10, WIDTH, HEIGHT, image_160x120);

	/* Il numero di operazioni richieste per lo spilt è pari al numero di byte dell'immagine RGB
	 * diviso per il numero di byte a cui l'istruzione NEON si applica. In questo caso vengono
	 * utilizzati 3 registri a 64 bit per un totale di 24 Byte.
	*/
	SplitRGB_asm( (uint8_t*)image_160x120, (uint8_t**)dst_P, (IMAGE_SIZE/24));

	UG_DrawChnl( WIDTH+10, 10, WIDTH, HEIGHT, &img.chnl.R[0]);
	UG_DrawChnl( 10, HEIGHT+10, WIDTH, HEIGHT, &img.chnl.G[0]);
	UG_DrawChnl( WIDTH+10, HEIGHT+10, WIDTH, HEIGHT, &img.chnl.B[0]);

	/* Azzero il contenuto dell'immagine originale */
	//for ( int i=0; i<IMAGE_SIZE; i++) 
	//{
	//	img.image_area[i]=0;
	//}
	
	/* Riprendo i tre canali R, G e B e li ricompongo in RGB888 per la visualizzazione. */
	MergeRGB888_asm( (uint8_t**)dst_P, (uint8_t*)img2.image_area, (IMAGE_SIZE/24));
	UG_DrawRGB( 330,250, WIDTH, HEIGHT, img2.image_area);

	/* Azzero il contenuto dell'immagine originale */
	for ( int i=0; i<IMAGE_SIZE; i++) 
	{
		img2.image_area[i]=0;
	}

#if 1
	/* */
	IdentMat( r1);
	IdentMat( r2);
	
	FormatRotateMat( 'x', 1, r1);
	FormatRotateMat( 'y', 1, r2);
	ConcatMat( xform, r1, r2);
	PartialNonHomTransform();
#endif

	MergeRGB888_asm( (uint8_t**)dst2_P, (uint8_t*)img.image_area, (IMAGE_SIZE/24));
	UG_DrawRGB( 330,250+HEIGHT, WIDTH, HEIGHT, img.image_area);
	
	Color2Gray888_asm( img.image_area, image_160x120, (IMAGE_SIZE/24));
	UG_DrawRGB( 330+WIDTH,250+HEIGHT, WIDTH, HEIGHT, img.image_area);

	/* Azzero il contenuto dell'immagine originale */
	for ( int i=0; i<IMAGE_SIZE; i++) 
	{
		img2.image_area[i]=0;
	}
	
	SplitRGB_asm( (uint8_t*)image_160x120, (uint8_t**)dst_P, (IMAGE_SIZE/24));
	
	t.m = WIDTH/2;
	t.n = HEIGHT/2;
	t.rows = WIDTH;
	t.cols = HEIGHT;
	
	ArbritaryRotation( (uint8_t**)dst_P, (uint8_t**)dst2_P, t);
	MergeRGB888_asm( (uint8_t**)dst2_P, (uint8_t*)img.image_area, (IMAGE_SIZE/24));
	UG_DrawRGB( 10,250+HEIGHT, WIDTH, HEIGHT, img.image_area);	

 	for(;;);
	
	return 0;
}

 /*******************************************
 *
 *   arotate(..
 *
 *   This routine performs rotation about
 *   any point m,n.
 *
 *   The basic equations are:
 *
 *   new x = x.cos(a) - y.sin(a)
 *           -m.cos(a) + m + n.sin(a)
 *
 *   new y = y.cos(a) + x.sin(a)
 *           -m.sin(a) - n.cos(a) + n
 *
 *******************************************/

void ArbritaryRotation( uint8_t**src_P, uint8_t**dst_P, ROTATION_DATA t)
// float angle, uint32_t m, uint32_t n, uint32_t rows, uint32_t cols)
{
   float cosa, sina, radian_angle, tmpx, tmpy;
   int   i, j, new_i, new_j;

   uint8_t*rs=src_P[0];
   uint8_t*gs=src_P[1];
   uint8_t*bs=src_P[2];
   
   uint8_t*rd=dst_P[0];
   uint8_t*gd=dst_P[1];
   uint8_t*bd=dst_P[2];
   
   /* the following magic number is from
      180 degrees divided by pi */
   //radian_angle = t.angle/57.29577951;
   //cosa  = cos(radian_angle);
   //sina  = sin(radian_angle);
   cosa = cosphi;
   sina = sinphi;
   
   /**************************
   *
   *   Loop over image array
   *
   **************************/

   for(i=0; i<t.rows; i++)
   {   
      for(j=0; j<t.cols; j++)
	  {

     /******************************************
     *
     *   new x = x.cos(a) - y.sin(a)
     *           -m.cos(a) + m + n.sin(a)
     *
     *   new y = y.cos(a) + x.sin(a)
     *           -m.sin(a) - n.cos(a) + n
     *
     *******************************************/

         tmpx = (float)(j)*cosa - (float)(i)*sina - (float)(t.m)*cosa + (float)(t.m) + (float)(t.n)*sina;
         tmpy = (float)(i)*cosa + (float)(j)*sina - (float)(t.m)*sina - (float)(t.n)*cosa + (float)(t.n);

         new_j = tmpx;
         new_i = tmpy;

		if ( new_j < 0 || new_j >= t.cols || new_i < 0 || new_i >= t.rows)
		{
		   // out_image[i][j] = 0;
		   rd[i+(j*t.rows)] = 0;
		   gd[i+(j*t.rows)] = 0;
		   bd[i+(j*t.rows)] = 0;
        } else {
           // out_image[i][j] = the_image[new_i][new_j];
		   rd[i+(j*t.rows)] = rs[new_i+(new_j*t.rows)];
		   gd[i+(j*t.rows)] = gs[new_i+(new_j*t.rows)];
		   bd[i+(j*t.rows)] = bs[new_i+(new_j*t.rows)];		   
        }
      }  /* ends loop over j */
   }  /* ends loop over i */
}  /* ends arotate */

void PartialNonHomTransform( void)
{
	int32_t i=0, x, y, xt, yt;
	int32_t xs, ys;
	int32_t hwidth = WIDTH / 2;
	int32_t hheight = HEIGHT / 2;
	float sinma=(sinphi), cosma=(cosphi);

	for ( x=0; x<WIDTH; x++)
	{
		for ( y=0; y<HEIGHT; y++)
		{
			xt = x - hwidth;
			yt = y - hheight;
						
			xs = (int32_t)((cosma * xt - sinma * yt) + hwidth);
			ys = (int32_t)((sinma * xt + cosma * yt) + hheight);
			
			if(xs >= 0 && xs < WIDTH && ys >= 0 && ys < HEIGHT) 
			{
				i=xs+(ys*WIDTH);
				/* set target pixel (x,y) to color at (xs,ys) */
				img2.chnl.R[i] = img.chnl.R[x+(y*WIDTH)];
				img2.chnl.G[i] = img.chnl.G[x+(y*WIDTH)];
				img2.chnl.B[i] = img.chnl.B[x+(y*WIDTH)];				
			}
		}
	}
}

void ConcatMat( Transform3D dst, Transform3D src1, Transform3D src2)
{
	int i;
	int j;

	/* Riga [r][c] */
	for (i = 0; i < 4; ++i) 
	{
		/* Colonna */
		for (j = 0; j < 4; ++j)
		{
			dst[i][j] = src1[i][0] * src2[0][j] + 
			             src1[i][1] * src2[1][j] + 
						 src1[i][2] * src2[2][j] + 
						 src1[i][3] * src2[3][j];
		}
	}
}

void FormatRotateMat(char axis, int dir, Transform3D t)
{
	float s, c;

	IdentMat(t);

	s = sinphi;
	c = cosphi;

	switch(axis)
	{
		case 'x':
			t[1][1] = t[2][2] = c;
			t[1][2] = (float)(s*dir);
			t[2][1] = (float)(-s*dir);
			break;
		case 'y':
			t[0][0] = t[2][2] = c;
			t[2][0] = (float)(s*dir);
			t[0][2] = (float)(-s*dir);
			break;
		case 'z':
			t[0][0] = t[1][1] = c;
			t[0][1] = (float)(s*dir);
			t[1][0] = (float)(-s*dir);
			break;
	}
}

void IdentMat( Transform3D t)
{
 int i;
 int j;

 for (i = 3; i >= 0; --i)
	{
	for (j = 3; j >= 0; --j)
		t[i][j] = 0.0;
	t[i][i] = 1.0;
	}
}

void IdentMat2( void)
{
 int i;
 int j;

 for (i = 3; i >= 0; --i)
	{
	for (j = 3; j >= 0; --j)
		r2[i][j] = 0.0;
	r2[i][i] = 1.0;
	}
}

