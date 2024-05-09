#line 1 ":24bit.c"
 




















#line 1 ":video.h"
 






















#line 1 "Mr. Gristle:3do:Interfaces:1p3:Includes:stdio.h"



 































 




 


___toplevel 


#line 1 "Mr. Gristle:3do:Interfaces:1p3:Includes:types.h"



 

















































































 














___toplevel 


 

 
typedef signed char	int8;
typedef signed short	int16;		 
typedef signed long	int32;

typedef char	uchar;			 
typedef uchar	ubyte;
typedef uchar	uint8;

typedef unsigned short	ushort;		 
typedef ushort		uint16;		 

typedef unsigned long	ulong;
typedef ulong		uint32;

typedef volatile long	vlong;
typedef	vlong		vint32;

typedef volatile unsigned long	vulong;
typedef	vulong			vuint32;

typedef	uint8	Boolean;
typedef Boolean	bool;

typedef uint16	unichar;	 

typedef	uint32	size_t;









typedef int32	(*func_t)();	 
typedef uint32	(*uifunc_t)();	 
typedef void	(*vfunc_t)();	 
typedef void	*(*vpfunc_t)();	 

typedef int32	Item;
typedef	int32	Err;

 

typedef void *CodeHandle;

 



 
typedef struct TagArg
{
	uint32 ta_Tag;
	void *ta_Arg;
} TagArg, *TagArgP;








 




 


 
 	 
typedef uint32		pd_mask;




typedef struct pd_set
{
	pd_mask	pds_bits[1];	 
} pd_set;














func_t	make_func(int32);
int32	make_int(func_t);








#line 46 "Mr. Gristle:3do:Interfaces:1p3:Includes:stdio.h"
#line 1 "Mr. Gristle:3do:Interfaces:1p3:Includes:stdarg.h"


 










#line 1 "Mr. Gristle:3do:Interfaces:1p3:Includes:varargs.h"



 






















 






___toplevel 


typedef char *va_list;

 




#line 15 "Mr. Gristle:3do:Interfaces:1p3:Includes:stdarg.h"
#line 47 "Mr. Gristle:3do:Interfaces:1p3:Includes:stdio.h"








typedef struct
{
	int32 fcb_currentpos;
	int32 fcb_filesize;	 
	int32 fcb_bytesleft;	 
	int32 fcb_numinbuf;	 
	uint8 *fcb_cp;	 
	uint8 fcb_mode;
	uint8 pad[3];  
	uint8 *fcb_buffer;
	int32	fcb_buffsize;
	char fcb_filename[128];
} FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

  




int32 system(char *s);
void perror(char *s);

extern FILE	*tmpfile(void);

FILE *fopen(char *filename, char *type);
int32 fread(void *ptr, int32 size, int32 nitems, FILE *stream);
int32 fclose (FILE *stream);
int32 getc(FILE *stream);
int32 putc(char c, FILE *stream);
int32 fputs(char *s, FILE *stream);
int32 fseek(FILE *stream, int32 offset, int32 prtname);
int32 ftell(FILE *stream);
int32 fflush(FILE *stream);
int32 ungetc(char s, FILE *stream);
int32 printf(const char *fmt, ...);
int32 sprintf(char *,const char *fmt, ...);
int32 vprintf(const char *fmt, va_list a);
int32 vsprintf(char *buf, const char *fmt, va_list a);

int32 MacExpect(char *buff, int32 maxchars);






#line 27 ":video.h"
#line 1 "Mr. Gristle:3do:Interfaces:1p3:Includes:stdlib.h"




 







































#line 1 "Mr. Gristle:3do:Interfaces:1p3:Includes:types.h"





















































































































































































































#line 46 "Mr. Gristle:3do:Interfaces:1p3:Includes:stdlib.h"





extern void *malloc(int32);	 
extern void free(void *);
extern void *calloc(size_t, size_t);
extern void *realloc(void *oldBlock, size_t newSize);

extern void exit(int status);

extern int32 rand(void);
extern void srand(int32);
extern uint32 urand(void);

extern int _ANSI_rand(void);
extern int _ANSI_srand(unsigned int seed);

extern int32 atoi(const char *nptr);
extern long int atol(const char *nptr);

extern ulong strtoul(const char *nsptr, char **endptr, int base);
extern long strtol(const char *nsptr, char **endptr, int base);






#line 28 ":video.h"
#line 1 "Mr. Gristle:3do:Interfaces:1p3:Includes:string.h"



 










































































 






___toplevel 


#line 1 "Mr. Gristle:3do:Interfaces:1p3:Includes:types.h"





















































































































































































































#line 88 "Mr. Gristle:3do:Interfaces:1p3:Includes:string.h"
#line 1 "Mr. Gristle:3do:Interfaces:1p3:Includes:stdlib.h"










































































#line 89 "Mr. Gristle:3do:Interfaces:1p3:Includes:string.h"
			 
  




extern void *memcpy(void *  , const void *  , size_t   );
	 
extern void *memmove(void *  , const void *  , size_t   );
	 

extern char *strcpy(char *  , const char *  );
	 
extern char *strncpy(char *  , const char *  , size_t  );
	 

extern char *strcat(char *  , const char *  );
	 
extern char *strncat(char *  , const char *  , size_t  );
	 

extern int strcmp(const char *  , const char *  );
	 
extern int strcasecmp(const char *  , const char *  );
	 
extern int strncmp(const char *  , const char *  , size_t n);
	 
	 
extern int strncasecmp(const char *  , const char *  , size_t n);
	 
	 

extern size_t strlen(const char *  );
	 

extern char *strchr(const char *  , int c  );
	 

extern void *memset(void *  , int  , size_t  );
	 

extern void bzero(void *s, int len);
extern void bcopy(void *s,void *d,int len);




 










 
extern int ffs(unsigned int mask); 	 
extern int FindMSB(uint32 mask); 	 
extern int FindLSB(uint32 mask); 	 

 



extern int CountBits(uint32 mask);	 
  








#line 29 ":video.h"
#line 1 "Mr. Gristle:3do:Interfaces:1p3:Includes:setjmp.h"



 

































 







___toplevel 


typedef int jmp_buf[22];	 





extern int setjmp(jmp_buf  );
    






extern void longjmp(jmp_buf  , int  );
    






















#line 30 ":video.h"


typedef int	Display;
typedef int	Visual;
typedef int	Window;
typedef int	XErrorEvent;






 





 











 



 






 










 



 




 




















 

extern int scan[][8];

 

typedef int TimeStamp;

 

typedef struct pict_image {
  unsigned char *luminance;               
  unsigned char *Cr;                      
  unsigned char *Cb;                      
  int locked;                             
  TimeStamp show_time;                    






} PictImage;

 

typedef struct GoP {
   int  drop_flag;                      
  unsigned int tc_hours;                  
  unsigned int tc_minutes;                
  unsigned int tc_seconds;                
  unsigned int tc_pictures;               
   int  closed_gop;                     

   int  broken_link;                    
  char *ext_data;                         
  char *user_data;                        
} GoP;

 

typedef struct pict {
  unsigned int temp_ref;                  
  unsigned int code_type;                 
  unsigned int vbv_delay;                 
   int  full_pel_forw_vector;           

  unsigned int forw_r_size;               
  unsigned int forw_f;                    
   int  full_pel_back_vector;           

  unsigned int back_r_size;               
  unsigned int back_f;                    
  char *extra_info;                       
  char *ext_data;                         
  char *user_data;                        
} Pict;

 

typedef struct slice {
  unsigned int vert_pos;                  
  unsigned int quant_scale;               
  char *extra_info;                       
} Slice;

 

typedef struct macroblock {
  int mb_row, mb_col;			  
  int mb_address;                         
  int past_mb_addr;                       
  int motion_h_forw_code;                 
  unsigned int motion_h_forw_r;           
  int motion_v_forw_code;                 
  unsigned int motion_v_forw_r;           
  int motion_h_back_code;                 
  unsigned int motion_h_back_r;           
  int motion_v_back_code;                 
  unsigned int motion_v_back_r;           
  unsigned int cbp;                       
   int  mb_intra;                       
   int  bpict_past_forw;                
   int  bpict_past_back;                
  int past_intra_addr;                    
  int recon_right_for_prev;               
  int recon_down_for_prev;                
  int recon_right_back_prev;              
  int recon_down_back_prev;               
} Macroblock;

 


typedef int	DCTELEM;
typedef DCTELEM	DCTBLOCK[ 8  *  8 ];
 
 

typedef struct block
{
    DCTBLOCK	dct_recon;		        
    DCTELEM	dct_dc_y_past;                  
    DCTELEM	dct_dc_cr_past;                 
    DCTELEM	dct_dc_cb_past;                 
} Block;

 

typedef struct vid_stream {
  unsigned int h_size;                          
  unsigned int v_size;                          
  unsigned int mb_height;                       
  unsigned int mb_width;                        
  unsigned char aspect_ratio;                   
  unsigned char picture_rate;                   
  unsigned int bit_rate;                        
  unsigned int vbv_buffer_size;                 
   int  const_param_flag;                     
  unsigned char intra_quant_matrix[8][8];       

  unsigned char non_intra_quant_matrix[8][8];   

  char *ext_data;                               
  char *user_data;                              
  GoP group;                                    
  Pict picture;                                 
  Slice slice;                                  
  Macroblock mblock;                            
  Block block;                                  
  int state;                                    
  int bit_offset;                               
  unsigned int *buffer;                         

  int buf_length;                               
  unsigned int *buf_start;                      
  int max_buf_length;                           
  PictImage *past;                              
  PictImage *future;                            
  PictImage *current;                           
  PictImage *ring[ 5 ];               
} VidStream;   

 
extern Display *display;

 

extern VidStream *curVidStream;

 
extern int shmemFlag;

 
extern int quietFlag;

 
extern char *ditherFlags;


  
 












extern int ditherType;

extern int totNumFrames;
extern int loopFlag;
extern int noDisplayFlag;
extern jmp_buf env;








#line 23 ":24bit.c"
#line 1 ":dither.h"
 




















extern int LUM_RANGE;
extern int CR_RANGE;
extern int CB_RANGE;






extern unsigned char pixel[256];
extern int *lum_values;
extern int *cr_values;
extern int *cb_values;

#line 24 ":24bit.c"
#line 1 ":proto.h"







 
void correct_underflow   (void )  ;
int next_bits   (int num , unsigned int mask )  ;
char *get_ext_data   (void )  ;
int next_start_code   (void )  ;
char *get_extra_bit_info   (void )  ;

 
VidStream *NewVidStream   (int bufLength )  ;
void DestroyVidStream   (VidStream *astream )  ;
PictImage *NewPictImage   (unsigned int width , unsigned int height )  ;
void DestroyPictImage   (PictImage *apictimage )  ;
VidStream *mpegVidRsrc   (TimeStamp time_stamp , VidStream *vid_stream )  ;
void ToggleBFlag   (void )  ;
void TogglePFlag   (void )  ;

 
void ParseReconBlock   (int n )  ;
void ParseAwayBlock   (int n )  ;

 
void ComputeForwVector   (int *recon_right_for_ptr , int *recon_down_for_ptr )  ;
void ComputeBackVector   (int *recon_right_back_ptr , int *recon_down_back_ptr )  ;

 
void init_tables   (void )  ;
void decodeDCTDCSizeLum   (unsigned int *value )  ;
void decodeDCTDCSizeChrom   (unsigned int *value )  ;
void decodeDCTCoeffFirst   (unsigned int *run , int *level )  ;
void decodeDCTCoeffNext   (unsigned int *run , int *level )  ;

 
int get_more_data   (unsigned int *buf_start , int max_length , int *length_ptr , unsigned int **buf_ptr )  ;
void int_handler   (void )  ;
void main   (int argc , char **argv )  ;
void usage   (char *s )  ;
void DoDitherImage   (unsigned char *l , unsigned char *Cr , unsigned char *Cb , unsigned char *disp , int h , int w )  ;

 
void *MemAlloc   (int size)  ;
void MemFree   (void *p)  ;
void InitColor   (void )  ;
int HandleXError   (Display *dpy , XErrorEvent *event )  ;
void InstallXErrorHandler   (void )  ;
void DeInstallXErrorHandler   (void )  ;
void ResizeDisplay   (int w , int h )  ;
void InitDisplay   (char *name )  ;
void InitGrayDisplay   (char *name )  ;
void InitMonoDisplay   (char *name )  ;
void InitColorDisplay   (char *name )  ;
void ExecuteDisplay   (unsigned char *display, int width, int height)  ;

 
void InitFS2Dither   (void )  ;
void FS2DitherImage   (unsigned char *lum , unsigned char *cr , unsigned char *cb , unsigned char *disp , int rows , int cols )  ;

 
void InitFS2FastDither   (void )  ;
void FS2FastDitherImage   (unsigned char *lum , unsigned char *cr , unsigned char *cb , unsigned char *out , int h , int w )  ;

 
void InitFS4Dither   (void )  ;
void FS4DitherImage   (unsigned char *lum , unsigned char *cr , unsigned char *cb , unsigned char *disp , int rows , int cols )  ;

 
void InitHybridDither   (void )  ;
void HybridDitherImage   (unsigned char *lum , unsigned char *cr , unsigned char *cb , unsigned char *out , int h , int w )  ;

 
void InitHybridErrorDither   (void )  ;
void HybridErrorDitherImage   (unsigned char *lum , unsigned char *cr , unsigned char *cb , unsigned char *out , int h , int w )  ;

 
void Init2x2Dither   (void )  ;
void RandInit   (int h , int w )  ;
void PostInit2x2Dither   (void )  ;
void Twox2DitherImage   (unsigned char *lum , unsigned char *cr , unsigned char *cb , unsigned char *out , int h , int w )  ;

 
void GrayDitherImage   (unsigned char *lum , unsigned char *cr , unsigned char *cb , unsigned char *out , int h , int w )  ;

 

 
void init_pre_idct   (void )  ;
void j_rev_dct_sparse   (DCTBLOCK data , int pos )  ;
void j_rev_dct   (DCTBLOCK data )  ;
void j_rev_dct_sparse   (DCTBLOCK data , int pos )  ;
void j_rev_dct   (DCTBLOCK data )  ;

 
void InitColorDither   (void )  ;
void ColorDitherImage   (unsigned char *lum , unsigned char *cr , unsigned char *cb , unsigned char *out , int rows , int cols )  ;

 
Visual *FindFullColorVisual   (Display *dpy , int *depth )  ;
Window CreateFullColorWindow   (Display *dpy , int x , int y , int w , int h )  ;

 
void InitOrderedDither   (void )  ;
void OrderedDitherImage   (unsigned char *lum , unsigned char *cr , unsigned char *cb , unsigned char *out , int h , int w )  ;

 
void InitOrdered2Dither   (void )  ;
void Ordered2DitherImage   (unsigned char *lum , unsigned char *cr , unsigned char *cb , unsigned char *out , int h , int w )  ;

 
void InitMBOrderedDither   (void )  ;
void MBOrderedDitherImage   (unsigned char *lum , unsigned char *cr , unsigned char *cb , unsigned char *out , int h , int w )  ;
void MBOrderedDitherDisplayCopy   (VidStream *vid_stream , int mb_addr , int motion_forw , int r_right_forw , int r_down_forw , int motion_back , int r_right_back , int r_down_back , unsigned char *past , unsigned char *future )  ;





#line 25 ":24bit.c"

 














 


 


 




static int *Cb_r_tab, *Cr_g_tab, *Cb_g_tab, *Cr_b_tab;


 
















void
InitColorDither()
{
    int CR, CB, i;

    Cr_b_tab = (int *)MemAlloc(256*sizeof(int));
    Cr_g_tab = (int *)MemAlloc(256*sizeof(int));
    Cb_g_tab = (int *)MemAlloc(256*sizeof(int));
    Cb_r_tab = (int *)MemAlloc(256*sizeof(int));

    for (i=0; i<256; i++) {
	CB = CR = i;

	CB -= 128; CR -= 128;

	Cb_r_tab[i] =  ((int) (( 1.40200 )* ( ((int) 1)  <<  8 )  + 0.5))  * CB;
	Cr_g_tab[i] = - ((int) (( 0.34414 )* ( ((int) 1)  <<  8 )  + 0.5))  * CR;
	Cb_g_tab[i] = - ((int) (( 0.71414 )* ( ((int) 1)  <<  8 )  + 0.5))  * CB;   
	Cr_b_tab[i] =  ((int) (( 1.77200 )* ( ((int) 1)  <<  8 )  + 0.5))  * CR;
    }
}



 















void
DoDitherImage(lum, cr, cb, out, rows, cols)
  unsigned char *lum;
  unsigned char *cr;
  unsigned char *cb;
  unsigned char *out;
  int cols, rows;

{




    int L;
    int CR, CB;
    register unsigned int *row1;
    register unsigned char *lum1, *lum2;
    int x, y;
    unsigned int r, b, g;
    int	dstadd, srcadd, cadd;
    int cb_r;
    int cr_g;
    int cb_g;
    int cr_b;

    totNumFrames++;

    row1 = (unsigned int *)GetScreenBitmap (); 
    lum1 = lum;
    lum2 = lum1 + cols;
    
    if (cols >   320  )
    {
	cadd = cols -   320  ;
	srcadd = cols + cadd;
	cadd >>= 1;
	dstadd =  320  -   320  ;
        cols =   320  ;
    }
    else
    {
        srcadd = cols;
	dstadd =  320  - cols;
	cadd = 0;
    }
    
    if (rows >   240  )
        rows =   240  ;
    
    y = rows;
    do
    {
	x = cols;
	do
	{
	    int R, G, B;

	    CR = *cr++;
	    CB = *cb++;
	    cb_r = Cb_r_tab[CB];
	    cr_g = Cr_g_tab[CR];
	    cb_g = Cb_g_tab[CB];
	    cr_b = Cr_b_tab[CR];

	    L = *lum1++;
	    L =  (((int)( L )) <<  8 ) ;
	    R = L + cb_r;
	    G = L + cr_g + cb_g;
	    B = L + cr_b;
	    b =  ( (( B )<( 0 )) ?( 0 ):( (( B )>(  (((int)( 255 )) <<  8 )  )) ?(  (((int)( 255 )) <<  8 )  ):( B )))  >> 11;
	    g = ( ( (( G )<( 0 )) ?( 0 ):( (( G )>(  (((int)( 255 )) <<  8 )  )) ?(  (((int)( 255 )) <<  8 )  ):( G )))  & 0xf800) >> 6;
	    r = ( ( (( R )<( 0 )) ?( 0 ):( (( R )>(  (((int)( 255 )) <<  8 )  )) ?(  (((int)( 255 )) <<  8 )  ):( R )))  & 0xf800) >> 1;
	    *row1 = (r | g | b) << 16;

	    L = *lum2++;
	    L =  (((int)( L )) <<  8 ) ;
	    R = L + cb_r;
	    G = L + cr_g + cb_g;
	    B = L + cr_b;
	    b =  ( (( B )<( 0 )) ?( 0 ):( (( B )>(  (((int)( 255 )) <<  8 )  )) ?(  (((int)( 255 )) <<  8 )  ):( B )))  >> 11;
	    g = ( ( (( G )<( 0 )) ?( 0 ):( (( G )>(  (((int)( 255 )) <<  8 )  )) ?(  (((int)( 255 )) <<  8 )  ):( G )))  & 0xf800) >> 6;
	    r = ( ( (( R )<( 0 )) ?( 0 ):( (( R )>(  (((int)( 255 )) <<  8 )  )) ?(  (((int)( 255 )) <<  8 )  ):( R )))  & 0xf800) >> 1;
	    *row1++ |= (r | g | b);

	     


	    L = *lum1++;
	    L =  (((int)( L )) <<  8 ) ;
	    R = L + cb_r;
	    G = L + cr_g + cb_g;
	    B = L + cr_b;
	    b =  ( (( B )<( 0 )) ?( 0 ):( (( B )>(  (((int)( 255 )) <<  8 )  )) ?(  (((int)( 255 )) <<  8 )  ):( B )))  >> 11;
	    g = ( ( (( G )<( 0 )) ?( 0 ):( (( G )>(  (((int)( 255 )) <<  8 )  )) ?(  (((int)( 255 )) <<  8 )  ):( G )))  & 0xf800) >> 6;
	    r = ( ( (( R )<( 0 )) ?( 0 ):( (( R )>(  (((int)( 255 )) <<  8 )  )) ?(  (((int)( 255 )) <<  8 )  ):( R )))  & 0xf800) >> 1;
	    *row1 = (r | g | b) << 16;

	    L = *lum2++;
	    L =  (((int)( L )) <<  8 ) ;
	    R = L + cb_r;
	    G = L + cr_g + cb_g;
	    B = L + cr_b;
	    b =  ( (( B )<( 0 )) ?( 0 ):( (( B )>(  (((int)( 255 )) <<  8 )  )) ?(  (((int)( 255 )) <<  8 )  ):( B )))  >> 11;
	    g = ( ( (( G )<( 0 )) ?( 0 ):( (( G )>(  (((int)( 255 )) <<  8 )  )) ?(  (((int)( 255 )) <<  8 )  ):( G )))  & 0xf800) >> 6;
	    r = ( ( (( R )<( 0 )) ?( 0 ):( (( R )>(  (((int)( 255 )) <<  8 )  )) ?(  (((int)( 255 )) <<  8 )  ):( R )))  & 0xf800) >> 1;
	    *row1++ |= r | g | b;
	} while ((x -= 2) > 0);
	
	cr += cadd;
	cb += cadd;
	
	lum1 += srcadd; 
	lum2 += srcadd; 
	row1 += dstadd; 
    }
    while ((y -= 2) > 0);
    
    FlipScreens ();
}



