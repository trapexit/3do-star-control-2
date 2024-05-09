#include "resintrn.h"
#include "declib.h"

PROC(
MEM_HANDLE GetResourceData, (fp, length, mem_flags),
    ARG		(FILE		*fp)
    ARG		(DWORD		length)
    ARG_END	(MEM_FLAGS	mem_flags)
)
{
    MEM_HANDLE	RData;
    DECODE_REF	fh;

    if (length == ~(DWORD)0)
	length = LengthResFile (fp), fh = 0;
    else if (fh = copen (fp, FILE_STREAM, STREAM_READ))
	cfilelength (fh, &length);
    else
	length -= sizeof (DWORD);

    if (RData = AllocResourceData (length, mem_flags))
    {
	RESOURCE_DATAPTR	RDPtr;

	LockResourceData (RData, &RDPtr);
	if (RDPtr == NULL_PTR)
	{
	    FreeResourceData (RData);
	    RData = 0;
	}
	else
	{
	    COUNT	num_read;

	    do
	    {
#define READ_LENGTH	0x00007FFFL
		num_read = length >= READ_LENGTH ?
			(COUNT)READ_LENGTH : (COUNT)length;
		if (fh)
		{
		    if (cread (RDPtr, 1, num_read, fh) != num_read)
			break;
		}
		else
		{
		    if (ReadResFile (RDPtr, 1, num_read, fp) != num_read)
			break;
		}
		FAR_PTR_ADD (&RDPtr, num_read);
	    } while (length -= num_read);

	    UnlockResourceData (RData);
	    if (length > 0)
	    {
		FreeResourceData (RData);
		RData = 0;
	    }
	}
    }

    cclose (fh);

    return (RData);
}


