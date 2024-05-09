/*
	File:		Endless.c

	Contains:	An example endless demo application

	Written by:	Francis Stanbach

	Copyright:	© 1993 by The 3DO Company. All rights reserved.
				This material constitutes confidential and proprietary
				information of the 3DO Company and shall not be used by
				any Person or for any purpose except as expressly
				authorized in writing by the 3DO Company.

	Change History (most recent first):

				 2/28/94	fjs		Start the Sample Code

	To Do:
*/

#include "types.h"
#include "debug.h"
#include "mem.h"
#include "graphics.h"
#include "hardware.h"
#include "Form3DO.h"
#include "Init3DO.h"
#include "Parse3DO.h"
#include "Utils3DO.h"
#include "Portfolio.h"

#include "StorageManager.h"

// #include "OpenGraphicsNTSCPAL.h"

// constants for the size of NTSC screen

#define kNTSCScreenWidth	320
#define kNTSCScreenHeight	240

// constants for the size of PAL screen

#define kPALScreenWidth		384
#define kPALScreenHeight	288

// cels for this example

#define kBackgroundCel	"Background Cel"
#define k3DOCel			"3DO Cel"


// global variables

Item gVBLIOReq;
ScreenContext gScreen;

CCB* gBackgroundCel = NULL;
CCB* g3DOCel = NULL;


// prototypes for funtions

bool Initialize(void);
void SwitchScreen(void);
void DrawEverything(void);
void SpinCel(CCB* cel, int32 frames);

// spin the cel forever

int main()
	{
	Boolean finished = false;
	int32 result;
	
	if (Initialize())
		while(!finished)			
			{
			result = ReadControlPad(JOYSTART);
			if (result & JOYSTART)
				{
				finished = TRUE;
				}
			else if (result & JOYFIREA)
				{
#if 1
				SMParameterBlock smpb;

				void* storageBlock;
				
				kprintf("about to call BrowseStorage!\n");
								
				storageBlock = ALLOCMEM(200 * 1024, MEMTYPE_ANY);
				
				memset(storageBlock, 0, 200 * 1024);
				
				smpb.bufferBegin = storageBlock;
				smpb.bufferCurrent = storageBlock;
				smpb.bufferEnd = (void*) ((int32) storageBlock + (200 * 1024));
				smpb.bytesNeeded = 30000;
				smpb.screenPtr = &gScreen;

				StorageManagerBrowse(&smpb);

				FREEMEM(storageBlock, 200 * 1024);

#else
				StorageManagerBrowse();
#endif

				}

			SpinCel(g3DOCel, 128);
			}
	
	return(1);
	}


void SpinCel(CCB* cel, int32 frames)
	{
	Point quad[4];
	Point saveQuad[4];
	Point middle;
	int32 xDiff, yDiff;
	frac16 cosine, sine, xtra, ytra;
	frac16  height, width, i, iInc;
	
	// save the full size of the cel
	
	saveQuad[0].pt_X = ConvertF16_32(cel->ccb_XPos);
	saveQuad[0].pt_Y = ConvertF16_32(cel->ccb_YPos);
	
	saveQuad[1].pt_X = saveQuad[0].pt_X + cel->ccb_Width;
	saveQuad[1].pt_Y = saveQuad[0].pt_Y;
	
	saveQuad[2].pt_X = saveQuad[1].pt_X;
	saveQuad[2].pt_Y = saveQuad[1].pt_Y + cel->ccb_Height;
	
	saveQuad[3].pt_X = saveQuad[0].pt_X;
	saveQuad[3].pt_Y = saveQuad[2].pt_Y;
	
	
	quad[0].pt_X = ConvertF16_32(cel->ccb_XPos);
	quad[0].pt_Y = ConvertF16_32(cel->ccb_YPos);
	
	quad[1].pt_X = quad[0].pt_X + cel->ccb_Width;
	quad[1].pt_Y = quad[0].pt_Y;
	
	quad[2].pt_X = quad[1].pt_X;
	quad[2].pt_Y = quad[1].pt_Y + cel->ccb_Height;
	
	quad[3].pt_X = quad[0].pt_X;
	quad[3].pt_Y = quad[2].pt_Y;
	
	middle.pt_X = (quad[0].pt_X + quad[2].pt_X) / 2;
	middle.pt_Y = (quad[0].pt_Y + quad[2].pt_Y) / 2;
	
	
	width = DivSF16(Convert32_F16(cel->ccb_Width), Convert32_F16(2));
	height = DivSF16(Convert32_F16(cel->ccb_Height), Convert32_F16(2));

	iInc = DivSF16(FULLCIRCLE, Convert32_F16(frames));

	for (i = Convert32_F16(64); i <= Convert32_F16(64+256); i = AddF16(i, iInc))
		{
		cosine = CosF16(i);
		sine = SinF16(i);
		
		xtra = MulSF16(cosine, width);
		ytra = MulSF16(sine, width);
		ytra = DivSF16(ytra, Convert32_F16(5));

		xDiff = ConvertF16_32(xtra);
		yDiff = ConvertF16_32(ytra);

		quad[0].pt_X = middle.pt_X - xDiff;
		quad[3].pt_X = middle.pt_X - xDiff;

		quad[1].pt_X = middle.pt_X + xDiff;
		quad[2].pt_X = middle.pt_X + xDiff;

		quad[0].pt_Y = middle.pt_Y - ConvertF16_32(height) - yDiff;
		quad[1].pt_Y = middle.pt_Y - ConvertF16_32(height) + yDiff;
		
		quad[2].pt_Y = middle.pt_Y + ConvertF16_32(height) - yDiff;
		quad[3].pt_Y = middle.pt_Y + ConvertF16_32(height) + yDiff;

		SwitchScreen();
		MapCel(cel, quad);
		DrawEverything();
		}
	
	// restore the full size of the cel
	
	MapCel(cel, saveQuad);
	}

void SwitchScreen()
	{
	gScreen.sc_curScreen = 1 - gScreen.sc_curScreen;
	}

void DrawEverything()
	{	
	WaitVBL(gVBLIOReq, 1);
	DrawScreenCels(gScreen.sc_Screens[gScreen.sc_curScreen], gBackgroundCel);
	DrawScreenCels(gScreen.sc_Screens[gScreen.sc_curScreen], g3DOCel);

	DisplayScreen(gScreen.sc_Screens[gScreen.sc_curScreen], 0);
	}

// initialize the folios and global variables
	
bool Initialize()
	{
	Boolean NTSCMode;
	gVBLIOReq = GetVBLIOReq();
	
	gScreen.sc_nScreens = 2;
	
#if OS1p2
	if (!OpenGraphicsNTSCPAL(&gScreen, 2) || !OpenSPORT())
#else
	if (!OpenGraphics(&gScreen, 2) || !OpenSPORT())
#endif
		{
		DIAGNOSTIC("Can't open a folio!");
		return FALSE;
		}
		
	// NTSCMode = (GrafBase->gf_VBLFreq == NTSC_FREQUENCY);
	NTSCMode = true;
	
	if (OpenMathFolio() < 0)
		{
		DIAGNOSTIC("Cannot open MathFolio.\n");
		return FALSE;
		}
	
	if ((gBackgroundCel = LoadCel(kBackgroundCel, MEMTYPE_CEL)) == NULL)
		{
		DIAGNOSTIC("Cannot load Background cel");
		return FALSE;
		}
	else
		{
		Point quad[4];
		
		LAST_CEL(gBackgroundCel);
		
		// stretch the background cel to cover the entire screen
		
		quad[0].pt_X = 0;
		quad[0].pt_Y = 0;
		quad[1].pt_Y = 0;
		quad[3].pt_X = 0;
		
		if (NTSCMode)
			{
			quad[1].pt_X = kNTSCScreenWidth;
			quad[2].pt_X = kNTSCScreenWidth;
			quad[2].pt_Y = kNTSCScreenHeight;
			quad[3].pt_Y = kNTSCScreenHeight;
			}
		else
			{
			quad[1].pt_X = kPALScreenWidth;
			quad[2].pt_X = kPALScreenWidth;
			quad[2].pt_Y = kPALScreenHeight;
			quad[3].pt_Y = kPALScreenHeight;
			}
		
		MapCel(gBackgroundCel, quad);
		}


	if ((g3DOCel = LoadCel(k3DOCel, MEMTYPE_CEL)) == NULL)
		{
		DIAGNOSTIC("Cannot load g3DOCel");
		return FALSE;
		}
	else
		{
		if (NTSCMode)
			{
			g3DOCel->ccb_XPos = Convert32_F16((kNTSCScreenWidth - g3DOCel->ccb_Width) / 2);
			g3DOCel->ccb_YPos = Convert32_F16((kNTSCScreenHeight - g3DOCel->ccb_Height) / 2);
			}
		else
			{
			g3DOCel->ccb_XPos = Convert32_F16((kPALScreenWidth - g3DOCel->ccb_Width) / 2);
			g3DOCel->ccb_YPos = Convert32_F16((kPALScreenHeight - g3DOCel->ccb_Height) / 2);
			}
		}
		
	gScreen.sc_curScreen = 0;
	
	return true;
	}

