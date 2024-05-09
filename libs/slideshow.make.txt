#####################################
#		Symbol definitions
#####################################
Application		=	slideshow
DebugFlag		=	0
CC			=	armcc
ASM			=	armasm
LINK			=	armlink


#####################################
#	Default compiler options
#####################################
COptions		= -Wan -fa -zps0 -za1 -i "{3DOIncludes}" -d DEBUG={DebugFlag}

SOptions		= -bi -g -i "{3DOIncludes}"

LOptions		= -aif -r -b 0x00 -workspace 0x1000 


#####################################
#		Object files
#####################################
LIBS	=	"{3DOLibs}Lib3DO.lib"		ç
		"{3DOLibs}operamath.lib"	ç
		"{3DOLibs}filesystem.lib"	ç
		"{3DOLibs}graphics.lib"		ç
		"{3DOLibs}audio.lib"		ç
		"{3DOLibs}music.lib"		ç
		"{3DOLibs}input.lib"		ç
		"{3DOLibs}clib.lib"		ç
		"{3DOLibs}swi.lib"

# NOTE: Add object files here...
OBJECTS			=	"{Application}.c.o"


#####################################
#	Default build rules
#####################################
All				É	{Application}

#{ObjectDir}		É	:

.c.o			É	.c
	{CC} {COptions} -o {TargDir}{Default}.c.o {DepDir}{Default}.c

.s.o			É	.s
	{ASM} {SOptions} -o {TargDir}{Default}.s.o {DepDir}{Default}.s


#####################################
#	Target build rules
#####################################
{Application}		ÉÉ	{Application}.make {OBJECTS}
	{LINK}	{LOptions}					ç
			-o {Application}				ç
			"{3DOLibs}cstartup.o"		ç
			{OBJECTS}					ç
			{LIBS}
	SetFile {Application} -c 'EaDJ' -t 'PROJ'
	modbin "{Application}" -stack 0x1000
	Duplicate -y {Application} "{3DOremote}"
	echo "done."


#####################################
#	Include file dependencies
#####################################
{Application}.c		É
