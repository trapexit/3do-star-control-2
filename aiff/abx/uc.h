/*****************************************************************************/
/* C prototype header file for UC.OBJ.	Provides ability to decompress audio */
/* data compressed using either ACOMP or BCOMP utilities. 									 */
/*****************************************************************************/
// ABX file format:
//
// Bytes 0-1: int  TotalFrames;     Total number of ACOMP frames in file.
// Bytes 2-5: long int TotalSize;   Total size of original source file.
// Bytes 6-7: unsigned int bufsize; Frame buffer size used to compress in.
// Bytes 8-9: unsigned int freq;    Playback frequency of audio file.
//     ....   ABH HEADERS[TotalFrames] Array of headers indicating all
//				    audio frame data.

typedef struct
{
  DWORD	fileaddress;	  // Address in file of this audio section.
  COUNT	fsize;		  // compressed file size.
  COUNT usize;		  // uncompressed file size.
} ABH;
