#include "types.h"
#include "item.h"
#include "msgport.h"
#include "kernel.h"
#include "kernelnodes.h"
#include "event.h"
#include "mem.h"
#include "debug.h"
#include "event.h"
#include "Portfolio.h"
#include "Init3DO.h"
#include "Parse3DO.h"
#include "Utils3DO.h"
#include "filefunctions.h"
#include "BlockFile.h"

int
main (int argc, char **argv)
{
    int		read, write, manage;
    char	file[80];
    char	buf[1024];
    
    --argc;
    ++argv;

    read = write = manage = 0;    
    while (argc)
    {
	char	c;

	if ((*argv)[0] == '-')
	{
	    switch ((*argv)[1])
	    {
		case 'r':
		    read = 1;
		    break;
		case 'w':
		    write = 1;
		    break;
		case 'm':
		    manage = 1;
		    break;
	    }
	}
	else
	    strcpy (file, *argv);
	
	--argc;
	++argv;
    }

    if (!read && !write && !manage)
    {
	printf ("Usage: nvram [-rwm] filename\n");
	printf ("    -r to read, -w to write, -m to manage\n");
    }
    
    if (write)
    {
	Stream	*fp;

	DeleteNVRAMFile (file);
	printf ("Opening '%s' for write...\n", file);
	if (fp = (Stream *)OpenNVRAMFile (file, "w", 0, 0))
	{
	    strcpy (buf, "This is a test of NVRAM");
	    
	    if (WriteNVRAMFile (buf, 1, sizeof (buf), fp) != sizeof (buf))
	    {
		printf ("Couldn't write to file '%s'\n", file);
		manage = 1;
	    }
	    
	    if (CloseNVRAMFile (fp) == 0)
	    {
		printf ("Couldn't close file '%s'\n", file);
		manage = 1;
	    }
	}
	else
	{
	    printf ("Couldn't open '%s' for write\n", file);
	    manage = 1;
	}
    }
    
    if (read)
    {
	Stream	*fp;
	
	printf ("Opening '%s' for read...\n", file);	
	if (fp = (Stream *)OpenNVRAMFile (file, "r", 0, 0))
	{
	    ReadNVRAMFile (buf, 1, sizeof (buf), fp); 
	    CloseNVRAMFile (fp);
	}
	else
	    printf ("Couldn't open '%s' for read\n", file);
    }
    
    if (manage)
    {
	printf ("Managing NVRAM...\n");
	ManageNVRAM ();
    }
}

void *
ThreedoAlloc (int32 size)
{
    return (AllocMem (size, MEMTYPE_ANY | MEMTYPE_FILL | 0));
}

void
ThreedoFree (void *p, int32 size)
{
    FreeMem (p, size);
}

Item
GetScreenGroup ()
{
    return ((Item)0);
}

