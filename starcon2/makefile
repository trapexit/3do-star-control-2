# Makefile for Star Control 2
# All source can be reached by this makefile.

# Where to stick the object files.
OBJDIR = :Objects:
SOBJDIR = :ShipObjects:
POBJDIR = :PlanetObjects:
CMOBJDIR = :CommObjects:

# Where to get the Planet source.
PLANETDIR = :Planets:

# Where to get the "Paul" source.
PAULDIR = :Planets:Paul:

# Where to get the alien ship source.
SHIPDIRS = �
	:ANDROSYN: �
	:ARILOU: �
	:BLACKURQ: �
	:CHENJESU: �
	:CHMMR: �
	:DRUUGE: �
	:HUMAN: �
	:ILWRATH: �
	:LASTBAT: �
	:MELNORME: �
	:MMRNMHRM: �
	:MYCON: �
	:ORZ: �
	:PKUNK: �
	:PROBE: �
	:SHOFIXTI: �
	:SIS_SHIP: �
	:SLYLANDR: �
	:SPATHI: �
	:SUPOX: �
	:SYREEN: �
	:THRADD: �
	:UMGAH: �
	:URQUAN: �
	:UTWIG: �
	:VUX: �
	:YEHAT: �
	:ZOQFOT:

# Where to get the alien conversation source.
COMMDIRS = �
	:COMM:ARILOU: �
	:COMM:BLACKUR: �
	:COMM:CHMMR: �
	:COMM:COMANDR: �
	:COMM:DRUUGE: �
	:COMM:ILWRATH: �
	:COMM:MELNORM: �
	:COMM:MYCON: �
	:COMM:ORZ: �
	:COMM:PKUNK: �
	:COMM:REBEL: �
	:COMM:SHOFIXT: �
	:COMM:SLYHOME: �
	:COMM:SLYLAND: �
	:COMM:SPATHI: �
	:COMM:SPAHOME: �
	:COMM:STARBAS: �
	:COMM:SUPOX: �
	:COMM:SYREEN: �
	:COMM:TALKPET: �
	:COMM:THRADD: �
	:COMM:UMGAH: �
	:COMM:URQUAN: �
	:COMM:UTWIG: �
	:COMM:VUX: �
	:COMM:YEHAT: �
	:COMM:ZOQFOT:

# Object modules shared by both melee and starcon2.	
OBJECTS = �
	{OBJDIR}GAMEINP.O �
	{OBJDIR}SHIPSTAT.O �
	{OBJDIR}BATTLE.O �
	{OBJDIR}MISC.O �
	{OBJDIR}BUILD.O �
	{OBJDIR}CLEANUP.O �
	{OBJDIR}GRAVITY.O �
	{OBJDIR}GRAVWELL.O �
	{OBJDIR}PROCESS.O �
	{OBJDIR}COLLIDE.O �
	{OBJDIR}STATUS.O �
	{OBJDIR}CONFIRM.O �
	{OBJDIR}INIT.O �
	{OBJDIR}INTEL.O �
	{OBJDIR}TRANS.O �
	{OBJDIR}CYBORG.O �
	{OBJDIR}UTILS.O �
	{OBJDIR}DEMO.O �
	{OBJDIR}SETTINGS.O �
	{OBJDIR}VELOCITY.O �
	{OBJDIR}DISPLIST.O �
	{OBJDIR}SETUP.O �
	{OBJDIR}SHIP.O �
	{OBJDIR}WEAPON.O �
	{OBJDIR}GALAXY.O �
	{OBJDIR}LOADSHIP.O �
	{OBJDIR}PLANDATA.O �
	{OBJDIR}MELEE.O �
	{OBJDIR}PICKMELE.O �
	{OBJDIR}MASTER.O �
	{OBJDIR}FMV.O �
	{OBJDIR}DUMMY.O �
	{OBJDIR}STARCON.O �
	{OBJDIR}BORDER.O �
	{OBJDIR}CLOCK.O �
	{OBJDIR}COMM.O �
	{OBJDIR}OSCILL.O �
	{OBJDIR}CREDITS.O �
	{OBJDIR}ENCOUNT.O �
	{OBJDIR}GAMEOPT.O �
	{OBJDIR}GENDEF.O �
	{OBJDIR}GLOBDATA.O �
	{OBJDIR}GRPINFO.O �
	{OBJDIR}HYPER.O �
	{OBJDIR}IPDISP.O �
	{OBJDIR}LOAD.O �
	{OBJDIR}OUTFIT.O �
	{OBJDIR}PICKSHIP.O �
	{OBJDIR}RESTART.O �
	{OBJDIR}SAVE.O �
	{OBJDIR}SHIPYARD.O �
	{OBJDIR}SIS.O �
	{OBJDIR}STARBASE.O �
	{OBJDIR}STARMAP.O �
	{OBJDIR}STATE.O �
	{OBJDIR}TACTRANS.O �
	{OBJDIR}RAMFILE.O �
	{OBJDIR}COMMGLUE.O �
	{OBJDIR}GETCHAR.O

# The objects created by the ship source.	
SHIPOBJS = �
	{SOBJDIR}ANDROSYN.O �
	{SOBJDIR}ARILOU.O �
	{SOBJDIR}BLACKURQ.O �
	{SOBJDIR}CHENJESU.O �
	{SOBJDIR}CHMMR.O �
	{SOBJDIR}DRUUGE.O �
	{SOBJDIR}HUMAN.O �
	{SOBJDIR}ILWRATH.O �
	{SOBJDIR}LASTBAT.O �
	{SOBJDIR}MELNORME.O �
	{SOBJDIR}MMRNMHRM.O �
	{SOBJDIR}MYCON.O �
	{SOBJDIR}ORZ.O �
	{SOBJDIR}PKUNK.O �
	{SOBJDIR}PROBE.O �
	{SOBJDIR}SHOFIXTI.O �
	{SOBJDIR}SIS_SHIP.O �
	{SOBJDIR}SLYLANDR.O �
	{SOBJDIR}SPATHI.O �
	{SOBJDIR}SUPOX.O �
	{SOBJDIR}SYREEN.O �
	{SOBJDIR}THRADD.O �
	{SOBJDIR}UMGAH.O �
	{SOBJDIR}URQUAN.O �
	{SOBJDIR}UTWIG.O �
	{SOBJDIR}VUX.O �
	{SOBJDIR}YEHAT.O �
	{SOBJDIR}ZOQFOT.O

# Alien conversation object modules -- shared by converse and starcon.	
COMMOBJS = �
	{CMOBJDIR}ARILOU.O �
	{CMOBJDIR}BLACKUR.O �
	{CMOBJDIR}DRUUGE.O �
	{CMOBJDIR}CHMMR.O �
	{CMOBJDIR}COMANDR.O �
	{CMOBJDIR}ILWRATH.O �
	{CMOBJDIR}MYCON.O �
	{CMOBJDIR}MELNORM.O �
	{CMOBJDIR}ORZ.O �
	{CMOBJDIR}PKUNK.O �
	{CMOBJDIR}REBEL.O �
	{CMOBJDIR}SHOFIXT.O �
	{CMOBJDIR}SLYLAND.O �
	{CMOBJDIR}SLYHOME.O �
	{CMOBJDIR}SPAHOME.O �
	{CMOBJDIR}SPATHI.O �
	{CMOBJDIR}STARBAS.O �
	{CMOBJDIR}SUPOX.O �
	{CMOBJDIR}SYREEN.O �
	{CMOBJDIR}TALKPET.O �
	{CMOBJDIR}THRADD.O �
	{CMOBJDIR}UMGAH.O �
	{CMOBJDIR}URQUAN.O �
	{CMOBJDIR}UTWIG.O �
	{CMOBJDIR}VUX.O �
	{CMOBJDIR}YEHAT.O �
	{CMOBJDIR}ZOQFOT.O

# The objects created by the planet source.
PLANETOBJS = �
	{POBJDIR}solarsys.O �
	{POBJDIR}planets.O �
	{POBJDIR}planscan.O �
	{POBJDIR}plangen.O �
	{POBJDIR}oval.O �
	{POBJDIR}scan.O �
	{POBJDIR}calc.O �
	{POBJDIR}orbits.O �
	{POBJDIR}surface.O �
	{POBJDIR}starmap.O �
	{POBJDIR}lander.O �
	{POBJDIR}cargo.O �
	{POBJDIR}devices.O �
	{POBJDIR}roster.O �
	{POBJDIR}report.O �
	{POBJDIR}gensol.O �
	{POBJDIR}genshof.O �
	{POBJDIR}gencol.O �
	{POBJDIR}genspa.O �
	{POBJDIR}genzoq.O �
	{POBJDIR}genmel.O �
	{POBJDIR}genpet.O �
	{POBJDIR}genchmmr.O �
	{POBJDIR}gensyr.O �
	{POBJDIR}genburv.O �
	{POBJDIR}gensly.O �
	{POBJDIR}gendru.O �
	{POBJDIR}genutw.O �
	{POBJDIR}genthrad.O �
	{POBJDIR}genmyc.O �
	{POBJDIR}genorz.O �
	{POBJDIR}genvault.O �
	{POBJDIR}genwreck.O �
	{POBJDIR}genvux.O �
	{POBJDIR}gensam.O �
	{POBJDIR}genyeh.O �
	{POBJDIR}genpku.O �
	{POBJDIR}gensup.O �
	{POBJDIR}genrain.O �
	{POBJDIR}genilw.O �
	{POBJDIR}pl_stuff.O

LIBPATH = "{HardDisk}:src:libs"

DEBUG =
CC = armcc {DEBUG} -zps1 -za1 -ff
LINK = armlink -aif -r -b 0x00 -workspace 0x4000

INCLUDES = -i {LIBPATH},':3DO',':'
FLAGS = -d __WATCOMC__=1 -d _VW=_screen_width -d _VH=_screen_height �
	-d SAFE_X=16 -d SAFE_Y=16 -d HAVE_EXTRA_SCREEN -d XTESTING

# add below
SYSLIBS		= �
	"{3DOLibs}StorageTuner.lib" �
	"{3DOLibs}Lib3DO.lib" �
	"{3DOLibs}music.lib" �
	"{3DOLibs}operamath.lib" �
	"{3DOLibs}filesystem.lib" �
	"{3DOLibs}graphics.lib" �
	"{3DOLibs}audio.lib" �
	"{3DOLibs}input.lib" �
	"{3DOLibs}clib.lib" �
	"{3DOLibs}swi.lib"
	
LIBS	= �
	{LIBPATH}:graphics:graphics.lib �
	{LIBPATH}:memory:memory.lib �
	{LIBPATH}:task:task.lib �
	{LIBPATH}:sound:sound.lib �
	{LIBPATH}:strings:strings.lib �
	{LIBPATH}:resource:resource.lib �
	{LIBPATH}:decomp:decomp.lib �
	{LIBPATH}:input:input.lib �
	{LIBPATH}:math:math.lib �
	{LIBPATH}:video:video.lib �
	{LIBPATH}::duck:duckloop:duckloop.lib

# Search current directory {:}, {SHIPDIRS}, {PLANETDIR}, and {PAULDIR} for source files.
# Target dirs for objects are {OBJDIR}, {SOBJDIR}, and {POBJDIR}.
# {OBJDIR} {SOBJDIR} {POBJDIR} � : {SHIPDIRS} {PLANETDIR} {PAULDIR}
# Search current source directory {:} for target objects in {OBJDIR}.
{OBJDIR} � :

# Search source directory {COMMDIRS} for target objects in {CMOBJDIR}.
{CMOBJDIR} � : {COMMDIRS}

# Search source directory {SHIPDIRS} for target objects in {SOBJDIR}.
{SOBJDIR} � {SHIPDIRS}

# Search source directories {PLANETDIR} and {PAULDIR} for target objects in {POBJDIR}.
{POBJDIR} � {PLANETDIR} {PAULDIR}

.o � .c
	{CC} -i :{DepDir} {INCLUDES} -i "{3doincludes}" {FLAGS} -o {TargDir}{Default}.o {DepDir}{Default}.c
	
.a � .s
	armasm -i "{3doincludes}" -o {TargDir}{Default}.a {DepDir}{Default}.s

# for debugging --
# add -d after {LINK}
# add -debug at end of modbin line
# comment out stripaif line

starcon �� {OBJECTS} {SHIPOBJS} {PLANETOBJS} {COMMOBJS} {LIBS}
	echo "Linking starcon..."
	{LINK} -o starcon "{3DOLibs}cstartup.o" �
		{OBJECTS} �
		{SHIPOBJS} �
		{PLANETOBJS} �
		{COMMOBJS} �
		{LIBS} {SYSLIBS}
	modbin starcon -stack 10240
	stripaif starcon
	duplicate -y starcon "{HardDisk}:3do:3do_os:"{3doRelease}":remote:starcon"
	echo "Done."
	beep

cleanall �
	Delete -y {OBJECTS}
	Delete -y {SHIPOBJS}
	Delete -y {PLANETOBJS}
	Delete -y {COMMOBJS}
	
test �
	delete -y :objects:GLOBDATA.o �
		:objects:HYPER.o �
		:objects:pickship.o �
		:objects:RESTART.o �
		:objects:STARCON.o
