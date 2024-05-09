/*
 * Copyright (c) 1992 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */
#include "Portfolio.h"
#include "Init3DO.h"
#include "Parse3DO.h"
#include "Utils3DO.h"
#include "filefunctions.h"

#include "video.h"
#include "proto.h"

#include "util.h"
#include "dither.h"

/* Define buffer length. */

#define BUF_LENGTH 80000

/* Function return type declarations */
void usage();

/* External declaration of main decoding call. */

extern VidStream *mpegVidRsrc();
extern VidStream *NewVidStream();

/* Declaration of global variable to hold dither info. */

int ditherType;

/* Global file pointer to incoming data. */
FILE *input;

/* End of File flag. */
static int EOF_flag = 0;

/* Loop flag. */
int loopFlag = 0;

/* Shared memory flag. */
int shmemFlag = 0;

/* Quiet flag. */
int quietFlag = 0;

/* Display image on screen? */
int noDisplayFlag = 0;

/* Setjmp/Longjmp env. */
jmp_buf env;

int	display_width = 320, display_height = 240;


/*
 *--------------------------------------------------------------
 *
 * get_more_data --
 *
 *	Called by correct_underflow in bit parsing utilities to
 *      read in more data.
 *
 * Results:
 *	Input buffer updated, buffer length updated.
 *      Returns 1 if data read, 0 if EOF, -1 if error.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

int 
get_more_data(buf_start, max_length, length_ptr, buf_ptr)
     unsigned int *buf_start;
     int max_length;
     int *length_ptr;
     unsigned int **buf_ptr;
{
  
  int length, num_read, request;
  unsigned char *buffer, *mark;

  if (EOF_flag) return 0;

  length = *length_ptr;
  buffer = (unsigned char *) *buf_ptr;

  if (length > 0) {
    memcpy((unsigned char *) buf_start, buffer, (length*4));
    mark = ((unsigned char *) (buf_start + length));
  }
  else {
    mark = (unsigned char *) buf_start;
    length = 0;
  }

  request = (max_length-length)*4;
  
//  num_read = fread( mark, 1, request, input);
  num_read = ReadDiskStream ((Stream *)input, (char *)mark, request);
//printf ("Trying to read %d(%d) bytes\n", request, num_read);

  if   (num_read < 0) {
    return -1;
  }
  else if (num_read == 0) {
    *buf_ptr = buf_start;
    
    /* Make 32 bits after end equal to 0 and 32
       bits after that equal to seq end code
       in order to prevent messy data from infinite
       recursion.
    */

//printf ("Marking EOF\n");
    *(buf_start + length) = 0x0;
    *(buf_start + length+1) = SEQ_END_CODE;
    *length_ptr = length + 2;

    EOF_flag = 1;
    return 0;
  }

  if (num_read & 3)
  {
//printf ("Filling in %u bytes\n", 4 - (num_read & 3));
    memset (&mark[num_read], 0, 4 - (num_read & 3));
    num_read += 4;
  }
  
  num_read = num_read/4;

  *buf_ptr = buf_start;
  *length_ptr = length + num_read;

#ifdef MSDOS
  {
    unsigned char *lmark;

    lmark = mark;
    do
    {
      unsigned char	c;

      c = lmark[0];
      lmark[0] = lmark[3];
      lmark[3] = c;
      c = lmark[2];
      lmark[2] = lmark[1];
      lmark[1] = c;
      lmark += 4;
    } while (--num_read);
  }
#endif /* MSDOS */

  return 1;
}


/*
 *--------------------------------------------------------------
 *
 * main --
 *
 *	Parses command line, starts decoding and displaying.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

void
main(argc, argv)
     int argc;
     char **argv;
{

  char *name;
  static VidStream *theStream;
  int mark;
  int i;

  ChangeDirectory ("/remote");
  
  mark = 1;
  argc--;

  name = "";
  ditherType = FULL_COLOR_DITHER; //ORDERED2_DITHER;
  LUM_RANGE = 8;
  CR_RANGE = CB_RANGE = 4;
  noDisplayFlag = 0;

#ifdef SH_MEM
  shmemFlag = 1;
#endif

  while (argc) {
    if (strcmp(argv[mark], "-nop") == 0) {
      TogglePFlag();
      argc--; mark++;
    } else if (strcmp(argv[mark], "-nob") == 0) {
      ToggleBFlag();
      argc--; mark++;
    } else if (strcmp(argv[mark], "-display") == 0) {
      name = argv[++mark];
      argc -= 2; mark++;
    } else if (strcmp(argv[mark], "-eachstat") == 0) {
      argc--; mark++;
#ifdef ANALYSIS
      showEachFlag = 1;
#else
      kprintf ( "To use -eachstat, recompile with -DANALYSIS in CFLAGS\n");
      exit(1);
#endif
    }
    else if (strcmp(argv[mark], "-shmem_off") == 0) {
      argc--; mark++;
      shmemFlag = 0;
    }
    else if (strcmp(argv[mark], "-quiet") == 0) {
      argc--; mark++;
      quietFlag = 1;
    }
    else if (strcmp(argv[mark], "-loop") == 0) {
      argc--; mark++;
      loopFlag = 1;
    }
    else if (strcmp(argv[mark], "-no_display") == 0) {
      argc--; mark++;
      noDisplayFlag = 1;
    }
    else if (strcmp(argv[mark], "-l_range") == 0) {
      argc--; mark++;
      LUM_RANGE = atoi(argv[mark]);
      if (LUM_RANGE < 1) {
	kprintf ( "Illegal luminance range value: %d\n", LUM_RANGE);
	exit(1);
      }
      argc--; mark++;
    }
    else if (strcmp(argv[mark], "-cr_range") == 0) {
      argc--; mark++;
      CR_RANGE = atoi(argv[mark]);
      if (CR_RANGE < 1) {
	kprintf ( "Illegal cr range value: %d\n", CR_RANGE);
	exit(1);
      }
      argc--; mark++;
    }
    else if (strcmp(argv[mark], "-cb_range") == 0) {
      argc--; mark++;
      CB_RANGE = atoi(argv[mark]);
      if (CB_RANGE < 1) {
	kprintf ( "Illegal cb range value: %d\n", CB_RANGE);
	exit(1);
      }
      argc--; mark++;
    }
    else if (argv[mark][0] == '-') {
      kprintf ( "Un-recognized flag %s\n",argv[mark]);
      usage(argv[0]);
    }
    else {	
    	char	file[80];
	
	sprintf (file, "video.mpg/%s", argv[mark]);
      input = (FILE *)OpenDiskStream (file, 0);
      if (input == NULL) {
	kprintf ( "Could not open file %s\n", file);
	usage(argv[0]);
      }
      argc--; mark++;
    }
  }

  lum_values = (int *) MemAlloc(LUM_RANGE*sizeof(int));
  cr_values = (int *) MemAlloc(CR_RANGE*sizeof(int));
  cb_values = (int *) MemAlloc(CB_RANGE*sizeof(int));

  init_tables();
  
    InitDisplay(name);
    InitColorDither();
    InitColorDisplay(name);

#ifdef SH_MEM
    if (shmemFlag && (display != NULL)) {
      if (!XShmQueryExtension(display)) {
	shmemFlag = 0;
	if (!quietFlag) {
	  kprintf ( "Shared memory not supported\n");
	  kprintf ( "Reverting to normal Xlib.\n");
	}
      }
    }
#endif

  if (setjmp(env) != 0) {

    DestroyVidStream(theStream);

    EOF_flag = 0;
    curBits = 0;
    bitOffset = 0;
    bufLength = 0;
    bitBuffer = NULL;
    totNumFrames = 0;
#ifdef ANALYSIS 
    init_stats();
#endif

  }

  theStream = NewVidStream(BUF_LENGTH);


  mpegVidRsrc(0, theStream);

  while (1) mpegVidRsrc(0, theStream);
}
 

/*
 *--------------------------------------------------------------
 *
 * usage --
 *
 *	Print mpeg_play usage
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	exits with a return value -1
 *
 *--------------------------------------------------------------
 */

void
usage(s)
char *s;	/* program name */
{
    kprintf ( "Usage:\n");
    kprintf ( "mpeg_play\n");
    kprintf ( "          [-nob]\n");
    kprintf ( "          [-nop]\n");
    kprintf ( "          [-loop]\n");
    kprintf ( "          [-eachstat]\n");
    kprintf ( "          [-no_display]\n");
    kprintf ( "          [-quiet]\n");
    kprintf ( "          file_name\n");
    exit (-1);
}
