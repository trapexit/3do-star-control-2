
#undef	KERNEL

#include "Portfolio.h"
#include "debug.h"

#include "ourVDL.h"

/*
 * Build a vdl DMA clut control word.
 */
static int32 
SetDMACtlWord (int32 clutColorWords)
{
	// the standard clut control flags are:
	//		link to next color set is relative
	//		use "current frame buffer" address from this color set
	//		use "previous frame buffer" address from this color set

	DMAControlWord	clutCtlWord;

	// zero all fields to begin with
	LONGWORD(clutCtlWord) = 0;

	clutCtlWord.useCurrentLineAddr = true;
	clutCtlWord.usePreviousLineAddr = true;
	clutCtlWord.numColorWords = (int) clutColorWords;
	clutCtlWord.scanLines = (int) 240;

	clutCtlWord.relativeAddrNextCLUT = true;
	clutCtlWord.videoDMA = true;

	return LONGWORD(clutCtlWord);
}


/*
 *	Build a VDL display control word.
 */
static int32 
SetDisplayCtlWord (void)
{
	VDLDisplayCtlWord	displayControl;

	LONGWORD(displayControl) = 0;

	displayControl.controlWord = 1;
	displayControl.controlWordType = 1;
	displayControl.horizontalInterpolation = 1;
	displayControl.windowHorizontalInterp = 1;
	displayControl.verticalInterpolation = 1;
	displayControl.windowVerticalInterp = 1;
	
	displayControl.blueLSBSource = 2;		// use bit 0 of frame-buffer data
	displayControl.HSource = 0;				// not from frame, but zero always
	displayControl.VSource = 0;				// not from frame, but zero always

	displayControl.windowBlueLSBSource = 2;	// use bit 0 of frame-buffer data
	displayControl.windowHSource = 0;
	displayControl.windowVSource = 0;

	displayControl.bypassCLUT = 1;

	return LONGWORD(displayControl);
}

/*
 * Fill in a ‘standard’ VDL - ie. one with a ‘System Clut’ ( standard 3DO IMAG 
 *	file displays correctly with this clut), and one clut entry per scanline.
 */
static void
SimpleInitVDL(VDL_REC *vdlRecPtr, Bitmap *destBitmap)
{
	uint32	*vdlClutEntry;
	int32	color_words;
	int	i;

	color_words = ((sizeof(VDL_REC) / sizeof(UInt32)) - kControlWords);
	
	// first, the DMA control word...
	vdlRecPtr->controlword = SetDMACtlWord(color_words - kFillerWords);

	// second, the 'current' scanline frame buffer address...
	vdlRecPtr->curLineBuffer = vdlRecPtr->prevLineBuffer = ((uint32)destBitmap->bm_Buffer);
	
	// and finally colors to use on this line.  also set them to null
	vdlClutEntry = vdlRecPtr->CLUTEntry;
	for (i = 0; i < kCLUTWords; i++)
	{
	    ubyte	color;
	
	    color = (i << 3) | (i >> 2);
	    *vdlClutEntry++ = MakeCLUTColorEntry (i, color, color, color);
	}

	vdlRecPtr->backgroundEntry		= VDL_NULLVDL;


	vdlRecPtr->filler1				= VDL_NULLVDL;
	vdlRecPtr->filler2				= VDL_NULLVDL;

	// The last entry of a VDL must point to the VDLEntry whose address can be found 
	//  in GrafBase->gf_VDLPostDisplay.  (** THIS LIMITATION SHOULD GO AWAY IN FINAL
	//  SILICON **)
	vdlRecPtr->nextVDLEntry	= (int32)GrafBase->gf_VDLPostDisplay;
	
	// and finally set the display control word for the screen
	vdlRecPtr->displayControl = SetDisplayCtlWord();
}

int32
AllocateVDL (VDLEntry **newVDL, Bitmap *screenBitMap)
{
    *newVDL = (VDLEntry *)ALLOCMEM (sizeof (VDL_REC),
	    MEMTYPE_ANY | (MEMTYPE_FILL | 0x00));
    SimpleInitVDL ((VDL_REC *)*newVDL, screenBitMap);
    
    return (0);
}
