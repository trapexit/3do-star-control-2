#
#	File:		Makefile
#
#	Contains:	Make file for building StorageTuner Utility
#
#	Written by:	Francis Stanbach
#
#	Copyright:	© 1993 by The 3DO Company. All rights reserved.
#				This material constitutes confidential and proprietary
#				information of the 3DO Company and shall not be used by
#				any Person or for any purpose except as expressly
#				authorized in writing by the 3DO Company.
#
#	Change History (most recent first):
#
#		 <1>	 1/7/94		FJS		first stolen
#
#	To Do:
#

#####################################
#		Symbol definitions
#####################################
# DebugFlag		=	0
ObjDir			=	:Objects:
DObjDir			=	:ObjectsDebug:
CC				=	armcc
ASM				=	armasm
LINK			=	armlink
LIBRARIAN		=	armlib

#####################################
#	Default compiler options
#####################################


COptions		= {CDebugOptions} -zps0 -za1 -i "{3DOIncludes}" -i :lib: -i :tuner: -d DEBUG={DebugFlag}

SOptions		= -bi -g -i "{3DOIncludes}"

LDebugOptions	= -d 
LOptions		= -aif -r -b 0x00 -workspace 4096

LinkOptions		= -o

#####################################
#		Object files
#####################################
LIBS			=	"{3DOLibs}Lib3DO.lib"			ç
					"{3DOLibs}operamath.lib"		ç
					"{3DOLibs}graphics.lib"			ç
					"{3DOLibs}filesystem.lib"		ç
					"{3DOLibs}input.lib"			ç
					"{3DOLibs}clib.lib"				ç
					"{3DOLibs}swi.lib"

OBJECTS			=	"{ObjDir}st.c.o"				ç
					"{ObjDir}stlists.c.o"			ç
					"{ObjDir}stmem.c.o"
					
#
# debug stuff
#

DebugLIBS		=	"{3DOLibs}Lib3DO.lib"			ç
					"{3DOLibs}operamath.lib"		ç
					"{3DOLibs}graphics.lib"			ç
					"{3DOLibs}filesystem.lib"		ç
					"{3DOLibs}input.lib"			ç
					"{3DOLibs}clib.lib"				ç
					"{3DOLibs}swi.lib"

DebugOBJECTS	=	"{DObjDir}st.c.o"				ç
					"{DObjDir}stlists.c.o"			ç
					"{DObjDir}stmem.c.o"
					


#####################################
#	Default build rules
#####################################
All				É	StorageTuner StorageTunerDebug

{ObjDir}		É	: 

{ObjDir}		É	:Tuner:
{ObjDir}		É	:Examples:
{ObjDir}		É	:lib:

{DObjDir}		É	: 

{DObjDir}		É	:Tuner:

.c.o			É	.c
	{CC} {COptions} -o {TargDir}{Default}.c.o {DepDir}{Default}.c

.s.o			É	.s
	{ASM} {SOptions} -o {TargDir}{Default}.s.o {DepDir}{Default}.s


#####################################
#	Target build rules
#####################################

NoDebug É
	Set DebugFlag 0

all É StorageTunerExample StorageTuner

StorageTunerExample  É {ObjDir}Storagetunerexample.c.o StorageTuner.lib
	{LINK}	{LOptions}						ç
			-o {targ}						ç
			"{3DOLibs}"subroutinestartup.o				ç
			{ObjDir}storagetunerexample.c.o	ç
			StorageTuner.lib				ç
			{LIBS}
	SetFile {targ} -c 'EaDJ' -t 'PROJ'
	modbin {targ} -stack 4096
	duplicate -y "{targ}" "{3DORemote}"StorageTuner:

StorageTuner.lib  É {ObjDir}storagetuner.c.o
	{LIBRARIAN}	-c {LinkOptions} 	ç
				{targ}				ç
				{ObjDir}storagetuner.c.o

StorageTuner	É	Makefile NoDebug {LIBS} {OBJECTS}
	{LINK}	{LOptions}						ç
			-o {targ}						ç
			"{3DOLibs}"subroutinestartup.o	ç
#			{3DOLibs}subroutinestartup.o				ç
			{OBJECTS}						ç
			{LIBS}
	SetFile {targ} -c 'EaDJ' -t 'PROJ'
	modbin {targ} -stack 4096
	stripaif "{targ}" -o Dev:Null -s "{targ}".sym
	duplicate -y "{targ}" "{3DORemote}"StorageTuner:

#####################################
#	Debug build rules
#####################################

StorageTunerDebug	É	Makefile DoDebugOptions {DebugLIBS} {DebugOBJECTS}
	{LINK}	{LDebugOptions} {LOptions}		ç
			-o {targ}						ç
			"{3DOLibs}"subroutinestartup.o	ç
#			{3DOLibs}subroutinestartup.o				ç
			{DebugOBJECTS}					ç
			{DebugLIBS}
	SetFile {targ} -c 'EaDJ' -t 'PROJ'
	modbin {targ} -debug -stack 4096
	stripaif "{targ}" -o "{targ}" -s "{targ}".sym
	duplicate -y "{targ}" "{3DORemote}"StorageTuner:
	duplicate -y "{targ}".sym "{3DORemote}"StorageTuner:


# the Weasel dependency is a trick that I have used here to avoid
# creating separate build commands for debug and non-debug objects
# its not a good way to do it

DoDebugOptions	É
	set CDebugOptions -g
	set DebugFlag 1
