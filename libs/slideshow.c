#include "Portfolio.h"
#include "Init3DO.h"
#include "Parse3DO.h"
#include "Utils3DO.h"

#include "filefunctions.h"

ScreenContext TheScreen;
ubyte* theImage = NULL;

bool InitGame(void)
{	
	ChangeDirectory("/remote");
		
	TheScreen.sc_nScreens = 1;
	
	if (!OpenGraphics(&TheScreen,1) || !OpenSPORT() || !OpenAudio())
	{
		printf("Can't open a folio!\n");
		return FALSE;
	}
	
//	Create_Thread;	
}
	
int main(int argc, char** argv)
{	
	if (InitGame())
	{	
		long lEvent;

		TheScreen.sc_curScreen = 0;
		do
		{
			if ((theImage = LoadImage("horde.dat/horde.img", NULL, NULL, &TheScreen)) == NULL)
			{
				printf("Cannot load the image\n");
				break;
			}
			DrawImage(TheScreen.sc_Screens[TheScreen.sc_curScreen], theImage, &TheScreen);
			DisplayScreen(TheScreen.sc_Screens[TheScreen.sc_curScreen], 0);
			lEvent = 0;
			do
			{
				lEvent = ReadControlPad(0);
			} while (!(lEvent & (JOYFIREA | JOYSTART)));
			FadeToBlack(&TheScreen, 1 * 60);
			Free(theImage);
			TheScreen.sc_curScreen = 1 - TheScreen.sc_curScreen;
		} while (!(lEvent & JOYSTART));
	
	}

//	DeleteThread
	printf ("Finis\n");
	ShutDown();
	return(0);
}
