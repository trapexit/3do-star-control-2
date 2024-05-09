#line 1 ":DECODERS.c"
 



















 







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






#line 30 ":DECODERS.c"
#line 1 "assert.h"

___toplevel 












 extern void __assert(char *, char *, int);











#line 31 ":DECODERS.c"
#line 1 ":decoders.h"
 



















 







 

#line 1 "Mr. Gristle:3do:Interfaces:1p3:Includes:stdio.h"







































































































#line 32 ":decoders.h"
#line 1 ":util.h"
 




















 





 



 
extern unsigned int bitMask[];
extern unsigned int nBitMask[];
extern unsigned int rBitMask[];
extern unsigned int bitTest[];

 
extern unsigned int curBits;
extern int bitOffset;
extern int bufLength;
extern unsigned int *bitBuffer;

 


























































































































































































































































































































#line 33 ":decoders.h"

 






 



 



 

typedef struct {
  unsigned int value;        
  int num_bits;              
} mb_addr_inc_entry;

 
extern mb_addr_inc_entry mb_addr_inc[2048];


 
typedef struct {
  unsigned int mb_quant;               
  unsigned int mb_motion_forward;      
  unsigned int mb_motion_backward;     
  unsigned int mb_pattern;             
  unsigned int mb_intra;               
  int num_bits;                        
} mb_type_entry;

 
extern mb_type_entry mb_type_P[64];

 
extern mb_type_entry mb_type_B[64];


 
typedef struct {
  unsigned int cbp;             
  int num_bits;                 
} coded_block_pattern_entry;

 

extern coded_block_pattern_entry coded_block_pattern[512];



 
typedef struct {
  int code;               




  int num_bits;           
} motion_vectors_entry;


 
extern motion_vectors_entry motion_vectors[2048];


 
typedef struct {
  unsigned int value;     
  int num_bits;           
} dct_dc_size_entry;

 

extern dct_dc_size_entry dct_dc_size_luminance[128];

 

extern dct_dc_size_entry dct_dc_size_chrominance[256];


 







 

extern unsigned int dct_coeff_tbl_0[256];
extern unsigned int dct_coeff_tbl_1[16];
extern unsigned int dct_coeff_tbl_2[4];
extern unsigned int dct_coeff_tbl_3[4];
extern unsigned int dct_coeff_next[256];
extern unsigned int dct_coeff_first[256];



































































































































 


























 




























 


































 




































 



































 


























#line 32 ":DECODERS.c"
#line 1 ":util.h"
 




















 





 



 
extern unsigned int bitMask[];
extern unsigned int nBitMask[];
extern unsigned int rBitMask[];
extern unsigned int bitTest[];

 
extern unsigned int curBits;
extern int bitOffset;
extern int bufLength;
extern unsigned int *bitBuffer;

 


























































































































































































































































































































#line 33 ":DECODERS.c"
#line 1 ":video.h"
 






















#line 1 "Mr. Gristle:3do:Interfaces:1p3:Includes:stdio.h"







































































































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








#line 34 ":DECODERS.c"
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





#line 35 ":DECODERS.c"

 
mb_addr_inc_entry     mb_addr_inc[2048];

 
mb_type_entry         mb_type_P[64];

 
mb_type_entry         mb_type_B[64];

 
motion_vectors_entry  motion_vectors[2048];

 

coded_block_pattern_entry coded_block_pattern[512] = 
{ {(unsigned int) -1 , 0}, {(unsigned int) -1 , 0}, {39, 9}, {27, 9}, {59, 9}, {55, 9}, {47, 9}, {31, 9},
    {58, 8}, {58, 8}, {54, 8}, {54, 8}, {46, 8}, {46, 8}, {30, 8}, {30, 8},
    {57, 8}, {57, 8}, {53, 8}, {53, 8}, {45, 8}, {45, 8}, {29, 8}, {29, 8},
    {38, 8}, {38, 8}, {26, 8}, {26, 8}, {37, 8}, {37, 8}, {25, 8}, {25, 8},
    {43, 8}, {43, 8}, {23, 8}, {23, 8}, {51, 8}, {51, 8}, {15, 8}, {15, 8},
    {42, 8}, {42, 8}, {22, 8}, {22, 8}, {50, 8}, {50, 8}, {14, 8}, {14, 8},
    {41, 8}, {41, 8}, {21, 8}, {21, 8}, {49, 8}, {49, 8}, {13, 8}, {13, 8},
    {35, 8}, {35, 8}, {19, 8}, {19, 8}, {11, 8}, {11, 8}, {7, 8}, {7, 8},
    {34, 7}, {34, 7}, {34, 7}, {34, 7}, {18, 7}, {18, 7}, {18, 7}, {18, 7},
    {10, 7}, {10, 7}, {10, 7}, {10, 7}, {6, 7}, {6, 7}, {6, 7}, {6, 7}, 
    {33, 7}, {33, 7}, {33, 7}, {33, 7}, {17, 7}, {17, 7}, {17, 7}, {17, 7}, 
    {9, 7}, {9, 7}, {9, 7}, {9, 7}, {5, 7}, {5, 7}, {5, 7}, {5, 7}, 
    {63, 6}, {63, 6}, {63, 6}, {63, 6}, {63, 6}, {63, 6}, {63, 6}, {63, 6}, 
    {3, 6}, {3, 6}, {3, 6}, {3, 6}, {3, 6}, {3, 6}, {3, 6}, {3, 6}, 
    {36, 6}, {36, 6}, {36, 6}, {36, 6}, {36, 6}, {36, 6}, {36, 6}, {36, 6}, 
    {24, 6}, {24, 6}, {24, 6}, {24, 6}, {24, 6}, {24, 6}, {24, 6}, {24, 6}, 
    {62, 5}, {62, 5}, {62, 5}, {62, 5}, {62, 5}, {62, 5}, {62, 5}, {62, 5},
    {62, 5}, {62, 5}, {62, 5}, {62, 5}, {62, 5}, {62, 5}, {62, 5}, {62, 5},
    {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, 
    {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, {2, 5}, 
    {61, 5}, {61, 5}, {61, 5}, {61, 5}, {61, 5}, {61, 5}, {61, 5}, {61, 5}, 
    {61, 5}, {61, 5}, {61, 5}, {61, 5}, {61, 5}, {61, 5}, {61, 5}, {61, 5}, 
    {1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 5}, 
    {1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 5}, 
    {56, 5}, {56, 5}, {56, 5}, {56, 5}, {56, 5}, {56, 5}, {56, 5}, {56, 5}, 
    {56, 5}, {56, 5}, {56, 5}, {56, 5}, {56, 5}, {56, 5}, {56, 5}, {56, 5}, 
    {52, 5}, {52, 5}, {52, 5}, {52, 5}, {52, 5}, {52, 5}, {52, 5}, {52, 5}, 
    {52, 5}, {52, 5}, {52, 5}, {52, 5}, {52, 5}, {52, 5}, {52, 5}, {52, 5}, 
    {44, 5}, {44, 5}, {44, 5}, {44, 5}, {44, 5}, {44, 5}, {44, 5}, {44, 5}, 
    {44, 5}, {44, 5}, {44, 5}, {44, 5}, {44, 5}, {44, 5}, {44, 5}, {44, 5}, 
    {28, 5}, {28, 5}, {28, 5}, {28, 5}, {28, 5}, {28, 5}, {28, 5}, {28, 5}, 
    {28, 5}, {28, 5}, {28, 5}, {28, 5}, {28, 5}, {28, 5}, {28, 5}, {28, 5}, 
    {40, 5}, {40, 5}, {40, 5}, {40, 5}, {40, 5}, {40, 5}, {40, 5}, {40, 5}, 
    {40, 5}, {40, 5}, {40, 5}, {40, 5}, {40, 5}, {40, 5}, {40, 5}, {40, 5}, 
    {20, 5}, {20, 5}, {20, 5}, {20, 5}, {20, 5}, {20, 5}, {20, 5}, {20, 5}, 
    {20, 5}, {20, 5}, {20, 5}, {20, 5}, {20, 5}, {20, 5}, {20, 5}, {20, 5}, 
    {48, 5}, {48, 5}, {48, 5}, {48, 5}, {48, 5}, {48, 5}, {48, 5}, {48, 5}, 
    {48, 5}, {48, 5}, {48, 5}, {48, 5}, {48, 5}, {48, 5}, {48, 5}, {48, 5}, 
    {12, 5}, {12, 5}, {12, 5}, {12, 5}, {12, 5}, {12, 5}, {12, 5}, {12, 5}, 
    {12, 5}, {12, 5}, {12, 5}, {12, 5}, {12, 5}, {12, 5}, {12, 5}, {12, 5}, 
    {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, 
    {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, 
    {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, 
    {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, 
    {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, 
    {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, 
    {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, 
    {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, 
    {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, 
    {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, 
    {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, 
    {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4}, {8, 4},
    {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4},
    {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, 
    {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, 
    {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4},
    {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, 
    {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, 
    {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, 
    {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, 
    {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, 
    {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, 
    {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, 
    {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}
};

 
dct_dc_size_entry dct_dc_size_luminance[128] =
{   {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, 
    {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, 
    {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, 
    {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, 
    {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, 
    {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, 
    {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, 
    {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, 
    {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, 
    {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, 
    {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, 
    {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, 
    {4, 3}, {4, 3}, {4, 3}, {4, 3}, {4, 3}, {4, 3}, {4, 3}, {4, 3}, 
    {4, 3}, {4, 3}, {4, 3}, {4, 3}, {4, 3}, {4, 3}, {4, 3}, {4, 3}, 
    {5, 4}, {5, 4}, {5, 4}, {5, 4}, {5, 4}, {5, 4}, {5, 4}, {5, 4}, 
    {6, 5}, {6, 5}, {6, 5}, {6, 5}, {7, 6}, {7, 6}, {8, 7}, {(unsigned int) -1 , 0}
};

 
dct_dc_size_entry dct_dc_size_chrominance[256] =
{ {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
    {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
    {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
    {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
    {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
    {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
    {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
    {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
    {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, 
    {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, 
    {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, 
    {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, 
    {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, 
    {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, 
    {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, 
    {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, 
    {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, 
    {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, 
    {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, 
    {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, 
    {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, 
    {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, 
    {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, 
    {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, 
    {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, 
    {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, 
    {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, 
    {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, 
    {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, 
    {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, 
    {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, 
    {6, 6}, {6, 6}, {6, 6}, {6, 6}, {7, 7}, {7, 7}, {8, 8}, {(unsigned int) -1 , 0}
};

 

unsigned int dct_coeff_tbl_0[256] =
{
0xffff, 0xffff, 0xffff, 0xffff, 
0xffff, 0xffff, 0xffff, 0xffff, 
0xffff, 0xffff, 0xffff, 0xffff, 
0xffff, 0xffff, 0xffff, 0xffff, 
0x052f, 0x051f, 0x050f, 0x04ff, 
0x183f, 0x402f, 0x3c2f, 0x382f, 
0x342f, 0x302f, 0x2c2f, 0x7c1f, 
0x781f, 0x741f, 0x701f, 0x6c1f, 
0x028e, 0x028e, 0x027e, 0x027e, 
0x026e, 0x026e, 0x025e, 0x025e, 
0x024e, 0x024e, 0x023e, 0x023e, 
0x022e, 0x022e, 0x021e, 0x021e, 
0x020e, 0x020e, 0x04ee, 0x04ee, 
0x04de, 0x04de, 0x04ce, 0x04ce, 
0x04be, 0x04be, 0x04ae, 0x04ae, 
0x049e, 0x049e, 0x048e, 0x048e, 
0x01fd, 0x01fd, 0x01fd, 0x01fd, 
0x01ed, 0x01ed, 0x01ed, 0x01ed, 
0x01dd, 0x01dd, 0x01dd, 0x01dd, 
0x01cd, 0x01cd, 0x01cd, 0x01cd, 
0x01bd, 0x01bd, 0x01bd, 0x01bd, 
0x01ad, 0x01ad, 0x01ad, 0x01ad, 
0x019d, 0x019d, 0x019d, 0x019d, 
0x018d, 0x018d, 0x018d, 0x018d, 
0x017d, 0x017d, 0x017d, 0x017d, 
0x016d, 0x016d, 0x016d, 0x016d, 
0x015d, 0x015d, 0x015d, 0x015d, 
0x014d, 0x014d, 0x014d, 0x014d, 
0x013d, 0x013d, 0x013d, 0x013d, 
0x012d, 0x012d, 0x012d, 0x012d, 
0x011d, 0x011d, 0x011d, 0x011d, 
0x010d, 0x010d, 0x010d, 0x010d, 
0x282c, 0x282c, 0x282c, 0x282c, 
0x282c, 0x282c, 0x282c, 0x282c, 
0x242c, 0x242c, 0x242c, 0x242c, 
0x242c, 0x242c, 0x242c, 0x242c, 
0x143c, 0x143c, 0x143c, 0x143c, 
0x143c, 0x143c, 0x143c, 0x143c, 
0x0c4c, 0x0c4c, 0x0c4c, 0x0c4c, 
0x0c4c, 0x0c4c, 0x0c4c, 0x0c4c, 
0x085c, 0x085c, 0x085c, 0x085c, 
0x085c, 0x085c, 0x085c, 0x085c, 
0x047c, 0x047c, 0x047c, 0x047c, 
0x047c, 0x047c, 0x047c, 0x047c, 
0x046c, 0x046c, 0x046c, 0x046c, 
0x046c, 0x046c, 0x046c, 0x046c, 
0x00fc, 0x00fc, 0x00fc, 0x00fc, 
0x00fc, 0x00fc, 0x00fc, 0x00fc, 
0x00ec, 0x00ec, 0x00ec, 0x00ec, 
0x00ec, 0x00ec, 0x00ec, 0x00ec, 
0x00dc, 0x00dc, 0x00dc, 0x00dc, 
0x00dc, 0x00dc, 0x00dc, 0x00dc, 
0x00cc, 0x00cc, 0x00cc, 0x00cc, 
0x00cc, 0x00cc, 0x00cc, 0x00cc, 
0x681c, 0x681c, 0x681c, 0x681c, 
0x681c, 0x681c, 0x681c, 0x681c, 
0x641c, 0x641c, 0x641c, 0x641c, 
0x641c, 0x641c, 0x641c, 0x641c, 
0x601c, 0x601c, 0x601c, 0x601c, 
0x601c, 0x601c, 0x601c, 0x601c, 
0x5c1c, 0x5c1c, 0x5c1c, 0x5c1c, 
0x5c1c, 0x5c1c, 0x5c1c, 0x5c1c, 
0x581c, 0x581c, 0x581c, 0x581c, 
0x581c, 0x581c, 0x581c, 0x581c, 
};

unsigned int dct_coeff_tbl_1[16] = 
{
0x00bb, 0x202b, 0x103b, 0x00ab, 
0x084b, 0x1c2b, 0x541b, 0x501b, 
0x009b, 0x4c1b, 0x481b, 0x045b, 
0x0c3b, 0x008b, 0x182b, 0x441b, 
};

unsigned int dct_coeff_tbl_2[4] =
{
0x4019, 0x1429, 0x0079, 0x0839, 
};

unsigned int dct_coeff_tbl_3[4] = 
{
0x0449, 0x3c19, 0x3819, 0x1029, 
};

unsigned int dct_coeff_next[256] = 
{
0xffff, 0xffff, 0xffff, 0xffff, 
0xf7d5, 0xf7d5, 0xf7d5, 0xf7d5, 
0x0826, 0x0826, 0x2416, 0x2416, 
0x0046, 0x0046, 0x2016, 0x2016, 
0x1c15, 0x1c15, 0x1c15, 0x1c15, 
0x1815, 0x1815, 0x1815, 0x1815, 
0x0425, 0x0425, 0x0425, 0x0425, 
0x1415, 0x1415, 0x1415, 0x1415, 
0x3417, 0x0067, 0x3017, 0x2c17, 
0x0c27, 0x0437, 0x0057, 0x2817, 
0x0034, 0x0034, 0x0034, 0x0034, 
0x0034, 0x0034, 0x0034, 0x0034, 
0x1014, 0x1014, 0x1014, 0x1014, 
0x1014, 0x1014, 0x1014, 0x1014, 
0x0c14, 0x0c14, 0x0c14, 0x0c14, 
0x0c14, 0x0c14, 0x0c14, 0x0c14, 
0x0023, 0x0023, 0x0023, 0x0023, 
0x0023, 0x0023, 0x0023, 0x0023, 
0x0023, 0x0023, 0x0023, 0x0023, 
0x0023, 0x0023, 0x0023, 0x0023, 
0x0813, 0x0813, 0x0813, 0x0813, 
0x0813, 0x0813, 0x0813, 0x0813, 
0x0813, 0x0813, 0x0813, 0x0813, 
0x0813, 0x0813, 0x0813, 0x0813, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0xfbe1, 0xfbe1, 0xfbe1, 0xfbe1, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 
};

unsigned int dct_coeff_first[256] = 
{
0xffff, 0xffff, 0xffff, 0xffff, 
0xf7d5, 0xf7d5, 0xf7d5, 0xf7d5, 
0x0826, 0x0826, 0x2416, 0x2416, 
0x0046, 0x0046, 0x2016, 0x2016, 
0x1c15, 0x1c15, 0x1c15, 0x1c15, 
0x1815, 0x1815, 0x1815, 0x1815, 
0x0425, 0x0425, 0x0425, 0x0425, 
0x1415, 0x1415, 0x1415, 0x1415, 
0x3417, 0x0067, 0x3017, 0x2c17, 
0x0c27, 0x0437, 0x0057, 0x2817, 
0x0034, 0x0034, 0x0034, 0x0034, 
0x0034, 0x0034, 0x0034, 0x0034, 
0x1014, 0x1014, 0x1014, 0x1014, 
0x1014, 0x1014, 0x1014, 0x1014, 
0x0c14, 0x0c14, 0x0c14, 0x0c14, 
0x0c14, 0x0c14, 0x0c14, 0x0c14, 
0x0023, 0x0023, 0x0023, 0x0023, 
0x0023, 0x0023, 0x0023, 0x0023, 
0x0023, 0x0023, 0x0023, 0x0023, 
0x0023, 0x0023, 0x0023, 0x0023, 
0x0813, 0x0813, 0x0813, 0x0813, 
0x0813, 0x0813, 0x0813, 0x0813, 
0x0813, 0x0813, 0x0813, 0x0813, 
0x0813, 0x0813, 0x0813, 0x0813, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0412, 0x0412, 0x0412, 0x0412, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 
};

 












 















static void
init_mb_addr_inc()
{
  int i, j, val;

  for (i = 0; i < 8; i++) {
    mb_addr_inc[i].value =  -1 ;
    mb_addr_inc[i].num_bits = 0;
  }

  mb_addr_inc[8].value =  35 ;
  mb_addr_inc[8].num_bits = 11;

  for (i = 9; i < 15; i++) {
    mb_addr_inc[i].value =  -1 ;
    mb_addr_inc[i].num_bits = 0;
  }

  mb_addr_inc[15].value =  34 ;
  mb_addr_inc[15].num_bits = 11;

  for (i = 16; i < 24; i++) {
    mb_addr_inc[i].value =  -1 ;
    mb_addr_inc[i].num_bits = 0;
  }

  val = 33;

   for (i =  24 ; i <  36 ; i+=  1 ) { for (j = 0; j <  1 ; j++) { mb_addr_inc[i+j].value =  val ; mb_addr_inc[i+j].num_bits =  11 ; }  val --; } ;
   for (i =  36 ; i <  48 ; i+=  2 ) { for (j = 0; j <  2 ; j++) { mb_addr_inc[i+j].value =  val ; mb_addr_inc[i+j].num_bits =  10 ; }  val --; } ;
   for (i =  48 ; i <  96 ; i+=  8 ) { for (j = 0; j <  8 ; j++) { mb_addr_inc[i+j].value =  val ; mb_addr_inc[i+j].num_bits =  8 ; }  val --; } ;
   for (i =  96 ; i <  128 ; i+=  16 ) { for (j = 0; j <  16 ; j++) { mb_addr_inc[i+j].value =  val ; mb_addr_inc[i+j].num_bits =  7 ; }  val --; } ;
   for (i =  128 ; i <  256 ; i+=  64 ) { for (j = 0; j <  64 ; j++) { mb_addr_inc[i+j].value =  val ; mb_addr_inc[i+j].num_bits =  5 ; }  val --; } ;
   for (i =  256 ; i <  512 ; i+=  128 ) { for (j = 0; j <  128 ; j++) { mb_addr_inc[i+j].value =  val ; mb_addr_inc[i+j].num_bits =  4 ; }  val --; } ;
   for (i =  512 ; i <  1024 ; i+=  256 ) { for (j = 0; j <  256 ; j++) { mb_addr_inc[i+j].value =  val ; mb_addr_inc[i+j].num_bits =  3 ; }  val --; } ;
   for (i =  1024 ; i <  2048 ; i+=  1024 ) { for (j = 0; j <  1024 ; j++) { mb_addr_inc[i+j].value =  val ; mb_addr_inc[i+j].num_bits =  1 ; }  val --; } ;
}


 









	 



 
















static void
init_mb_type_P()
{
  int i;

  mb_type_P[0].mb_quant = mb_type_P[0].mb_motion_forward 
    = mb_type_P[0].mb_motion_backward = mb_type_P[0].mb_pattern 
      = mb_type_P[0].mb_intra =  -1 ;
  mb_type_P[0].num_bits = 0;

   for (i =  1 ; i <  2 ; i ++) {  mb_type_P [i].mb_quant =  1 ;  mb_type_P [i].mb_motion_forward =  0 ;  mb_type_P [i].mb_motion_backward =  0 ;  mb_type_P [i].mb_pattern =  0 ;  mb_type_P [i].mb_intra =  1 ;  mb_type_P [i].num_bits =  6 ; } 
   for (i =  2 ; i <  4 ; i ++) {  mb_type_P [i].mb_quant =  1 ;  mb_type_P [i].mb_motion_forward =  0 ;  mb_type_P [i].mb_motion_backward =  0 ;  mb_type_P [i].mb_pattern =  1 ;  mb_type_P [i].mb_intra =  0 ;  mb_type_P [i].num_bits =  5 ; } 
   for (i =  4 ; i <  6 ; i ++) {  mb_type_P [i].mb_quant =  1 ;  mb_type_P [i].mb_motion_forward =  1 ;  mb_type_P [i].mb_motion_backward =  0 ;  mb_type_P [i].mb_pattern =  1 ;  mb_type_P [i].mb_intra =  0 ;  mb_type_P [i].num_bits =  5 ; } ;
   for (i =  6 ; i <  8 ; i ++) {  mb_type_P [i].mb_quant =  0 ;  mb_type_P [i].mb_motion_forward =  0 ;  mb_type_P [i].mb_motion_backward =  0 ;  mb_type_P [i].mb_pattern =  0 ;  mb_type_P [i].mb_intra =  1 ;  mb_type_P [i].num_bits =  5 ; } ;
   for (i =  8 ; i <  16 ; i ++) {  mb_type_P [i].mb_quant =  0 ;  mb_type_P [i].mb_motion_forward =  1 ;  mb_type_P [i].mb_motion_backward =  0 ;  mb_type_P [i].mb_pattern =  0 ;  mb_type_P [i].mb_intra =  0 ;  mb_type_P [i].num_bits =  3 ; } ;
   for (i =  16 ; i <  32 ; i ++) {  mb_type_P [i].mb_quant =  0 ;  mb_type_P [i].mb_motion_forward =  0 ;  mb_type_P [i].mb_motion_backward =  0 ;  mb_type_P [i].mb_pattern =  1 ;  mb_type_P [i].mb_intra =  0 ;  mb_type_P [i].num_bits =  2 ; } ;
   for (i =  32 ; i <  64 ; i ++) {  mb_type_P [i].mb_quant =  0 ;  mb_type_P [i].mb_motion_forward =  1 ;  mb_type_P [i].mb_motion_backward =  0 ;  mb_type_P [i].mb_pattern =  1 ;  mb_type_P [i].mb_intra =  0 ;  mb_type_P [i].num_bits =  1 ; } ;
}





 
















static void
init_mb_type_B()
{
  int i;

  mb_type_B[0].mb_quant = mb_type_B[0].mb_motion_forward 
    = mb_type_B[0].mb_motion_backward = mb_type_B[0].mb_pattern 
      = mb_type_B[0].mb_intra =  -1 ;
  mb_type_B[0].num_bits = 0;

   for (i =  1 ; i <  2 ; i ++) {  mb_type_B [i].mb_quant =  1 ;  mb_type_B [i].mb_motion_forward =  0 ;  mb_type_B [i].mb_motion_backward =  0 ;  mb_type_B [i].mb_pattern =  0 ;  mb_type_B [i].mb_intra =  1 ;  mb_type_B [i].num_bits =  6 ; } ;
   for (i =  2 ; i <  3 ; i ++) {  mb_type_B [i].mb_quant =  1 ;  mb_type_B [i].mb_motion_forward =  0 ;  mb_type_B [i].mb_motion_backward =  1 ;  mb_type_B [i].mb_pattern =  1 ;  mb_type_B [i].mb_intra =  0 ;  mb_type_B [i].num_bits =  6 ; } ;
   for (i =  3 ; i <  4 ; i ++) {  mb_type_B [i].mb_quant =  1 ;  mb_type_B [i].mb_motion_forward =  1 ;  mb_type_B [i].mb_motion_backward =  0 ;  mb_type_B [i].mb_pattern =  1 ;  mb_type_B [i].mb_intra =  0 ;  mb_type_B [i].num_bits =  6 ; } ;
   for (i =  4 ; i <  6 ; i ++) {  mb_type_B [i].mb_quant =  1 ;  mb_type_B [i].mb_motion_forward =  1 ;  mb_type_B [i].mb_motion_backward =  1 ;  mb_type_B [i].mb_pattern =  1 ;  mb_type_B [i].mb_intra =  0 ;  mb_type_B [i].num_bits =  5 ; } ;
   for (i =  6 ; i <  8 ; i ++) {  mb_type_B [i].mb_quant =  0 ;  mb_type_B [i].mb_motion_forward =  0 ;  mb_type_B [i].mb_motion_backward =  0 ;  mb_type_B [i].mb_pattern =  0 ;  mb_type_B [i].mb_intra =  1 ;  mb_type_B [i].num_bits =  5 ; } ;
   for (i =  8 ; i <  12 ; i ++) {  mb_type_B [i].mb_quant =  0 ;  mb_type_B [i].mb_motion_forward =  1 ;  mb_type_B [i].mb_motion_backward =  0 ;  mb_type_B [i].mb_pattern =  0 ;  mb_type_B [i].mb_intra =  0 ;  mb_type_B [i].num_bits =  4 ; } ;
   for (i =  12 ; i <  16 ; i ++) {  mb_type_B [i].mb_quant =  0 ;  mb_type_B [i].mb_motion_forward =  1 ;  mb_type_B [i].mb_motion_backward =  0 ;  mb_type_B [i].mb_pattern =  1 ;  mb_type_B [i].mb_intra =  0 ;  mb_type_B [i].num_bits =  4 ; } ;
   for (i =  16 ; i <  24 ; i ++) {  mb_type_B [i].mb_quant =  0 ;  mb_type_B [i].mb_motion_forward =  0 ;  mb_type_B [i].mb_motion_backward =  1 ;  mb_type_B [i].mb_pattern =  0 ;  mb_type_B [i].mb_intra =  0 ;  mb_type_B [i].num_bits =  3 ; } ;
   for (i =  24 ; i <  32 ; i ++) {  mb_type_B [i].mb_quant =  0 ;  mb_type_B [i].mb_motion_forward =  0 ;  mb_type_B [i].mb_motion_backward =  1 ;  mb_type_B [i].mb_pattern =  1 ;  mb_type_B [i].mb_intra =  0 ;  mb_type_B [i].num_bits =  3 ; } ;
   for (i =  32 ; i <  48 ; i ++) {  mb_type_B [i].mb_quant =  0 ;  mb_type_B [i].mb_motion_forward =  1 ;  mb_type_B [i].mb_motion_backward =  1 ;  mb_type_B [i].mb_pattern =  0 ;  mb_type_B [i].mb_intra =  0 ;  mb_type_B [i].num_bits =  2 ; } ;
   for (i =  48 ; i <  64 ; i ++) {  mb_type_B [i].mb_quant =  0 ;  mb_type_B [i].mb_motion_forward =  1 ;  mb_type_B [i].mb_motion_backward =  1 ;  mb_type_B [i].mb_pattern =  1 ;  mb_type_B [i].mb_intra =  0 ;  mb_type_B [i].num_bits =  2 ; } ;
}


 
















 


















static void
init_motion_vectors()
{
  int i, j, val = 16;

  for (i = 0; i < 24; i++) {
    motion_vectors[i].code =  -1 ;
    motion_vectors[i].num_bits = 0;
  }

   for (i =  24 ; i <  36 ; i+=  2 ) { for (j = 0; j <  2  / 2; j++) { motion_vectors[i+j].code =  val ; motion_vectors[i+j].num_bits =  11 ; } for (j =  2  / 2; j <  2 ; j++) { motion_vectors[i+j].code = - val ; motion_vectors[i+j].num_bits =  11 ; }  val --; } ;
   for (i =  36 ; i <  48 ; i+=  4 ) { for (j = 0; j <  4  / 2; j++) { motion_vectors[i+j].code =  val ; motion_vectors[i+j].num_bits =  10 ; } for (j =  4  / 2; j <  4 ; j++) { motion_vectors[i+j].code = - val ; motion_vectors[i+j].num_bits =  10 ; }  val --; } ;
   for (i =  48 ; i <  96 ; i+=  16 ) { for (j = 0; j <  16  / 2; j++) { motion_vectors[i+j].code =  val ; motion_vectors[i+j].num_bits =  8 ; } for (j =  16  / 2; j <  16 ; j++) { motion_vectors[i+j].code = - val ; motion_vectors[i+j].num_bits =  8 ; }  val --; } ;
   for (i =  96 ; i <  128 ; i+=  32 ) { for (j = 0; j <  32  / 2; j++) { motion_vectors[i+j].code =  val ; motion_vectors[i+j].num_bits =  7 ; } for (j =  32  / 2; j <  32 ; j++) { motion_vectors[i+j].code = - val ; motion_vectors[i+j].num_bits =  7 ; }  val --; } ;
   for (i =  128 ; i <  256 ; i+=  128 ) { for (j = 0; j <  128  / 2; j++) { motion_vectors[i+j].code =  val ; motion_vectors[i+j].num_bits =  5 ; } for (j =  128  / 2; j <  128 ; j++) { motion_vectors[i+j].code = - val ; motion_vectors[i+j].num_bits =  5 ; }  val --; } ;
   for (i =  256 ; i <  512 ; i+=  256 ) { for (j = 0; j <  256  / 2; j++) { motion_vectors[i+j].code =  val ; motion_vectors[i+j].num_bits =  4 ; } for (j =  256  / 2; j <  256 ; j++) { motion_vectors[i+j].code = - val ; motion_vectors[i+j].num_bits =  4 ; }  val --; } ;
   for (i =  512 ; i <  1024 ; i+=  512 ) { for (j = 0; j <  512  / 2; j++) { motion_vectors[i+j].code =  val ; motion_vectors[i+j].num_bits =  3 ; } for (j =  512  / 2; j <  512 ; j++) { motion_vectors[i+j].code = - val ; motion_vectors[i+j].num_bits =  3 ; }  val --; } ;
   for (i =  1024 ; i <  2048 ; i+=  1024 ) { for (j = 0; j <  1024  / 2; j++) { motion_vectors[i+j].code =  val ; motion_vectors[i+j].num_bits =  1 ; } for (j =  1024  / 2; j <  1024 ; j++) { motion_vectors[i+j].code = - val ; motion_vectors[i+j].num_bits =  1 ; }  val --; } ;
}





 
















    
void
init_tables()
{
  extern void init_pre_idct();

  init_mb_addr_inc();
  init_mb_type_P();
  init_mb_type_B();
  init_motion_vectors();
  init_pre_idct();
}



















































































































































































































































































































