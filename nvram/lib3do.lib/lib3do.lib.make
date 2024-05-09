#
#	File:		Lib3DO.lib.make
#
#	Contains:	Make file for Lib3DOLib
#
#	Written by:	Joe Buczek
#
#	Copyright:	© 1992 by The 3DO Company. All rights reserved.
#				This material constitutes confidential and proprietary
#				information of the 3DO Company and shall not be used by
#				any Person or for any purpose except as expressly
#				authorized in writing by the 3DO Company.
#
#	Change History (most recent first):
#
#		 <5>	 8/13/93	JAY		clean up for 1p01 release
#		 <3>	  4/7/93	JAY		make the make consistant
#		 <2>	 3/18/93	JAY		turning debug on
#		 <1>	 3/17/93	JAY		This is the 2B1 release being checked in for the Cardinal build
#		<10>	 1/28/93	dsm		removed the d{threeDORelease}=1 define in the make file.
#		 <9>	  1/2/93	pgb		Modify Make to compile without symbols. Also switched include
#									order to look in local headers first.
#		 <8>	12/27/92	dsm		Modify Make to compile without symbols.
#		 <6>	12/14/92	JAY		remove -W compiler switch (suppress warnings)
#		 <5>	12/14/92	JAY		adding compiler switch threeDORelease to aid in
#									upgrading/downgrading between releases
#		 <4>	12/10/92	JML		Update for dependency on Parse3DO.h
#		 <3>	12/10/92	JML		Update for dependency on Portfolio.h
#		 <2>	12/10/92	JML		Updated to find interfaces and copy library to new location
#				 12/9/92	JML		For projector
#
#	To Do:
#

#####################################
#		Symbol definitions
#####################################
DebugFlag		=	0
ObjectDir		=	:Objects:
CC				=	armcc
ASM				=	armasm
LIBRARIAN		=	armlib

#####################################
#	Default compiler options
#####################################
CDebugOptions	= # -g		# turn on symbolic information
COptions		= {CDebugOptions} -zps0 -za1 -ff -i "{3DOIncludes}" -dDEBUG={DebugFlag}

SOptions		= -bi -i "{3DOIncludes}" # -g 

LOptions		= -c -o


#####################################
#		Object files
#####################################
OBJECTS			=	"{ObjectDir}OpenGraphics.c.o" ç
					"{ObjectDir}OpenMacLink.c.o" ç
					"{ObjectDir}OpenSPORT.c.o" ç
					"{ObjectDir}OpenAudio.c.o" ç
					"{ObjectDir}ShutDown.c.o" ç
					"{ObjectDir}GetFileSize.c.o" ç
					"{ObjectDir}ReadFile.c.o" ç
					"{ObjectDir}FindChunk.c.o" ç
					"{ObjectDir}GetChunk.c.o" ç
					"{ObjectDir}setCCBPLUTPtr.c.o" ç
					"{ObjectDir}setCCBDataPtr.c.o" ç
					"{ObjectDir}LoadCel.c.o" ç
					"{ObjectDir}OldLoadCel.c.o" ç
					"{ObjectDir}FreeAnimFrames.c.o" ç
					"{ObjectDir}LoadAnim.c.o" ç
					"{ObjectDir}OldLoadAnim.c.o" ç
					"{ObjectDir}LoadImage.c.o" ç
					"{ObjectDir}LoadSoundFX.c.o" ç
					"{ObjectDir}LoadSoundEffect.c.o" ç
					"{ObjectDir}BlockFile.c.o" ç
					"{ObjectDir}CenterCelOnScreen.c.o" ç
					"{ObjectDir}CenterRectf16.c.o" ç
					"{ObjectDir}DrawAnimCel.c.o" ç
					"{ObjectDir}DrawImage.c.o" ç
					"{ObjectDir}FadeFromBlack.c.o" ç
					"{ObjectDir}FadeInCel.c.o" ç
					"{ObjectDir}FadeOutCel.c.o" ç
					"{ObjectDir}FadeToBlack.c.o" ç
					"{ObjectDir}FrameBufferToCel.c.o" ç
					"{ObjectDir}FreeBuffer.c.o" ç
					"{ObjectDir}GetAnimCel.c.o" ç
					"{ObjectDir}LinkCel.c.o" ç
					"{ObjectDir}LoadFile.c.o" ç
					"{ObjectDir}MakeNewCel.c.o" ç
					"{ObjectDir}MakeNewDupCCB.c.o" ç
					"{ObjectDir}MapP2Cel.c.o" ç
					"{ObjectDir}MoveCel.c.o" ç
					"{ObjectDir}OffsetCel.c.o" ç
					"{ObjectDir}ParseAnim.c.o" ç
					"{ObjectDir}ParseCel.c.o" ç
					"{ObjectDir}PreMoveCel.c.o" ç
					"{ObjectDir}ReadControlPad.c.o" ç
					"{ObjectDir}SetCelScale.c.o" ç
					"{ObjectDir}SetChannel.c.o" ç
					"{ObjectDir}SetFadeInCel.c.o" ç
					"{ObjectDir}SetFadeOutCel.c.o" ç
					"{ObjectDir}SetMixer.c.o" ç
					"{ObjectDir}SetMixerChannel.c.o" ç
					"{ObjectDir}SetQuad.c.o" ç
					"{ObjectDir}SetRectf16.c.o" ç
					"{ObjectDir}UMemory.c.o" ç
					"{ObjectDir}UMemoryDebug.c.o" ç
					"{ObjectDir}WriteMacFile.c.o" ç
					ç
					"{ObjectDir}taTextLib.c.o" ç
					"{ObjectDir}TextLib.c.o" ç
					"{ObjectDir}FontLib.c.o" ç
					"{ObjectDir}FontBlit3To8_.s.o" ç
					"{ObjectDir}FontBlit5To8_.s.o" ç
					"{ObjectDir}DeleteCelMagic.c.o" ç
					"{ObjectDir}ChainCels.c.o" ç


#####################################
#	Default build rules
#####################################
All				É	Lib3DO.lib

{ObjectDir}		É	:

.c.o			É	.c
	{CC} {COptions} -o {TargDir}{Default}.c.o {DepDir}{Default}.c

.s.o			É	.s
	{ASM} {SOptions} -o {TargDir}{Default}.s.o {DepDir}{Default}.s

#####################################
#	Target build rules
#####################################
Lib3DO.lib		ÉÉ	Lib3DO.lib.make {OBJECTS}
	{LIBRARIAN}	{LOptions}		ç
				Lib3DO.lib		ç
				{OBJECTS}
	Duplicate -y Lib3DO.lib "{3DOLibs}"Lib3DO.lib

#####################################
#	Include file dependencies
#####################################

