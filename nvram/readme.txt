NVRAM interface

Overview

Pretty simple interface to NVRAM -- in other words, you can read, write,
and delete files in NVRAM and not do much else!
In the src directory, our code (to do the reading and writing and auxiliary
tasks) is in nvram.c. Sample program code is in main.c.  Their (3DO's) code is
everything else.

What's here

src -- source code to our nvram manager and to 3DO's
	(nvram.make for sample program, makefile for 3DO's stuff)
StorageTuner -- data for 3DO's manager -- this directory should go into $boot
	This contains icons, etc. for 3DO's NVRAM manager

To build

Type "make" and execute the MPW commands to build 3DO's manager.
Type "make -f nvram.make" and execute the MPW commands to build our manager.

To use

This is pretty straightforward -- there are 3 main calls:

1. FILE *OpenNVRAMFile (char *file, char *mode, char *buf, int bufsize)
	file is name of file to read/write
	mode is "w" for write or "r" for read (only one character is
		checked, so don't bother trying to pass "wb", etc.)
	buf and bufsize describe user supplied buffer for I/O
		(0 and function will allocate a default buffer)
2. int WriteNVRAMFile (char *data, int size, int count, FILE *fp)
	Same as fwrite
3. int ReadNVRAMFile (char *data, int size, int count, FILE *fp)
	Same as fread
	
There are other functions which you can use, delete, and/or modify at
	your leisure (and at your own risk!)

Limitations/Things to note

1. You can only have one NVRAM file open at a time (whether reading or
	writing).  Not too difficult to change.
2. 3DO's manager will use your screen group if you pass it in.
3. I've modified 3DO's code to special case Horde and Star Control II saved
	games.
4. I used to resize existing NVRAM files when I would write over them.  After
	experiencing aberrant behavior in Star Control II, we decided simply
	to delete files before we Open for writing.  I would suggest you do
	the same (using DeleteNVRAMFile).
5. All the routines will prepend "/nvram"  Don't add this yourself without
	changing the NVRAM routines.
	
This is a vague and sketchy outline of this NVRAM stuff.  If you have problems
figuring it out, give me a call.

Ken Ford
Toys for Bob
898-1060
