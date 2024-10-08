#
#	File:		Makefile
#
#	Contains:	Make file for building StorageTuner Utility
#
#	Written by:	Francis Stanbach
#
#	Copyright:	� 1993 by The 3DO Company. All rights reserved.
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
LIBS			=	"{3DOLibs}Lib3DO.lib"			�
					"{3DOLibs}operamath.lib"		�
					"{3DOLibs}graphics.lib"			�
					"{3DOLibs}filesystem.lib"		�
					"{3DOLibs}input.lib"			�
					"{3DOLibs}clib.lib"				�
					"{3DOLibs}swi.lib"

OBJECTS			=	"{ObjDir}st.c.o"				�
					"{ObjDir}stlists.c.o"			�
					"{ObjDir}stmem.c.o"
					
#
# debug stuff
#

DebugLIBS		=	"{3DOLibs}Lib3DO.lib"			�
					"{3DOLibs}operamath.lib"		�
					"{3DOLibs}graphics.lib"			�
					"{3DOLibs}filesystem.lib"		�
					"{3DOLibs}input.lib"			�
					"{3DOLibs}clib.lib"				�
					"{3DOLibs}swi.lib"

DebugOBJECTS	=	"{DObjDir}st.c.o"				�
					"{DObjDir}stlists.c.o"			�
					"{DObjDir}stmem.c.o"
					


#####################################
#	Default build rules
#####################################
All				�	StorageTuner StorageTunerDebug

{ObjDir}		�	: 

{ObjDir}		�	:Tuner:
{ObjDir}		�	:Examples:
{ObjDir}		�	:lib:

{DObjDir}		�	: 

{DObjDir}		�	:Tuner:

.c.o			�	.c
	{CC} {COptions} -o {TargDir}{Default}.c.o {DepDir}{Default}.c

.s.o			�	.s
	{ASM} {SOptions} -o {TargDir}{Default}.s.o {DepDir}{Default}.s


#####################################
#	Target build rules
#####################################

NoDebug �
	Set DebugFlag 0

all � StorageTunerExample StorageTuner

StorageTunerExample  � {ObjDir}Storagetunerexample.c.o StorageTuner.lib
	{LINK}	{LOptions}						�
			-o {targ}						�
			"{3DOLibs}"subroutinestartup.o				�
			{ObjDir}storagetunerexample.c.o	�
			StorageTuner.lib				�
			{LIBS}
	SetFile {targ} -c 'EaDJ' -t 'PROJ'
	modbin {targ} -stack 4096
	duplicate -y "{targ}" "{3DORemote}"StorageTuner:

StorageTuner.lib  � {ObjDir}storagetuner.c.o
	{LIBRARIAN}	-c {LinkOptions} 	�
				{targ}				�
				{ObjDir}storagetuner.c.o

StorageTuner	�	Makefile NoDebug {LIBS} {OBJECTS}
	{LINK}	{LOptions}						�
			-o {targ}						�
			"{3DOLibs}"subroutinestartup.o	�
#			{3DOLibs}subroutinestartup.o				�
			{OBJECTS}						�
			{LIBS}
	SetFile {targ} -c 'EaDJ' -t 'PROJ'
	modbin {targ} -stack 4096
	stripaif "{targ}" -o Dev:Null -s "{targ}".sym
	duplicate -y "{targ}" "{3DORemote}"StorageTuner:

#####################################
#	Debug build rules
#####################################

StorageTunerDebug	�	Makefile DoDebugOptions {DebugLIBS} {DebugOBJECTS}
	{LINK}	{LDebugOptions} {LOptions}		�
			-o {targ}						�
			"{3DOLibs}"subroutinestartup.o	�
#			{3DOLibs}subroutinestartup.o				�
			{DebugOBJECTS}					�
			{DebugLIBS}
	SetFile {targ} -c 'EaDJ' -t 'PROJ'
	modbin {targ} -debug -stack 4096
	stripaif "{targ}" -o "{targ}" -s "{targ}".sym
	duplicate -y "{targ}" "{3DORemote}"StorageTuner:
	duplicate -y "{targ}".sym "{3DORemote}"StorageTuner:


# the Weasel dependency is a trick that I have used here to avoid
# creating separate build commands for debug and non-debug objects
# its not a good way to do it

DoDebugOptions	�
	set CDebugOptions -g
	set DebugFlag 1
