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
SHIPDIRS = ç
	:ANDROSYN: ç
	:ARILOU: ç
	:BLACKURQ: ç
	:CHENJESU: ç
	:CHMMR: ç
	:DRUUGE: ç
	:HUMAN: ç
	:ILWRATH: ç
	:LASTBAT: ç
	:MELNORME: ç
	:MMRNMHRM: ç
	:MYCON: ç
	:ORZ: ç
	:PKUNK: ç
	:PROBE: ç
	:SHOFIXTI: ç
	:SIS_SHIP: ç
	:SLYLANDR: ç
	:SPATHI: ç
	:SUPOX: ç
	:SYREEN: ç
	:THRADD: ç
	:UMGAH: ç
	:URQUAN: ç
	:UTWIG: ç
	:VUX: ç
	:YEHAT: ç
	:ZOQFOT:

# Where to get the alien conversation source.
COMMDIRS = ç
	:COMM:ARILOU: ç
	:COMM:BLACKUR: ç
	:COMM:CHMMR: ç
	:COMM:COMANDR: ç
	:COMM:DRUUGE: ç
	:COMM:ILWRATH: ç
	:COMM:MELNORM: ç
	:COMM:MYCON: ç
	:COMM:ORZ: ç
	:COMM:PKUNK: ç
	:COMM:REBEL: ç
	:COMM:SHOFIXT: ç
	:COMM:SLYHOME: ç
	:COMM:SLYLAND: ç
	:COMM:SPATHI: ç
	:COMM:SPAHOME: ç
	:COMM:STARBAS: ç
	:COMM:SUPOX: ç
	:COMM:SYREEN: ç
	:COMM:TALKPET: ç
	:COMM:THRADD: ç
	:COMM:UMGAH: ç
	:COMM:URQUAN: ç
	:COMM:UTWIG: ç
	:COMM:VUX: ç
	:COMM:YEHAT: ç
	:COMM:ZOQFOT:

# Object modules shared by both melee and starcon2.	
OBJECTS = ç
	{OBJDIR}GAMEINP.O ç
	{OBJDIR}SHIPSTAT.O ç
	{OBJDIR}BATTLE.O ç
	{OBJDIR}MISC.O ç
	{OBJDIR}BUILD.O ç
	{OBJDIR}CLEANUP.O ç
	{OBJDIR}GRAVITY.O ç
	{OBJDIR}GRAVWELL.O ç
	{OBJDIR}PROCESS.O ç
	{OBJDIR}COLLIDE.O ç
	{OBJDIR}STATUS.O ç
	{OBJDIR}CONFIRM.O ç
	{OBJDIR}INIT.O ç
	{OBJDIR}INTEL.O ç
	{OBJDIR}TRANS.O ç
	{OBJDIR}CYBORG.O ç
	{OBJDIR}UTILS.O ç
	{OBJDIR}DEMO.O ç
	{OBJDIR}SETTINGS.O ç
	{OBJDIR}VELOCITY.O ç
	{OBJDIR}DISPLIST.O ç
	{OBJDIR}SETUP.O ç
	{OBJDIR}SHIP.O ç
	{OBJDIR}WEAPON.O ç
	{OBJDIR}GALAXY.O ç
	{OBJDIR}LOADSHIP.O ç
	{OBJDIR}PLANDATA.O ç
	{OBJDIR}MELEE.O ç
	{OBJDIR}PICKMELE.O ç
	{OBJDIR}MASTER.O ç
	{OBJDIR}FMV.O ç
	{OBJDIR}DUMMY.O ç
	{OBJDIR}STARCON.O ç
	{OBJDIR}BORDER.O ç
	{OBJDIR}CLOCK.O ç
	{OBJDIR}COMM.O ç
	{OBJDIR}OSCILL.O ç
	{OBJDIR}CREDITS.O ç
	{OBJDIR}ENCOUNT.O ç
	{OBJDIR}GAMEOPT.O ç
	{OBJDIR}GENDEF.O ç
	{OBJDIR}GLOBDATA.O ç
	{OBJDIR}GRPINFO.O ç
	{OBJDIR}HYPER.O ç
	{OBJDIR}IPDISP.O ç
	{OBJDIR}LOAD.O ç
	{OBJDIR}OUTFIT.O ç
	{OBJDIR}PICKSHIP.O ç
	{OBJDIR}RESTART.O ç
	{OBJDIR}SAVE.O ç
	{OBJDIR}SHIPYARD.O ç
	{OBJDIR}SIS.O ç
	{OBJDIR}STARBASE.O ç
	{OBJDIR}STARMAP.O ç
	{OBJDIR}STATE.O ç
	{OBJDIR}TACTRANS.O ç
	{OBJDIR}RAMFILE.O ç
	{OBJDIR}COMMGLUE.O ç
	{OBJDIR}GETCHAR.O

# The objects created by the ship source.	
SHIPOBJS = ç
	{SOBJDIR}ANDROSYN.O ç
	{SOBJDIR}ARILOU.O ç
	{SOBJDIR}BLACKURQ.O ç
	{SOBJDIR}CHENJESU.O ç
	{SOBJDIR}CHMMR.O ç
	{SOBJDIR}DRUUGE.O ç
	{SOBJDIR}HUMAN.O ç
	{SOBJDIR}ILWRATH.O ç
	{SOBJDIR}LASTBAT.O ç
	{SOBJDIR}MELNORME.O ç
	{SOBJDIR}MMRNMHRM.O ç
	{SOBJDIR}MYCON.O ç
	{SOBJDIR}ORZ.O ç
	{SOBJDIR}PKUNK.O ç
	{SOBJDIR}PROBE.O ç
	{SOBJDIR}SHOFIXTI.O ç
	{SOBJDIR}SIS_SHIP.O ç
	{SOBJDIR}SLYLANDR.O ç
	{SOBJDIR}SPATHI.O ç
	{SOBJDIR}SUPOX.O ç
	{SOBJDIR}SYREEN.O ç
	{SOBJDIR}THRADD.O ç
	{SOBJDIR}UMGAH.O ç
	{SOBJDIR}URQUAN.O ç
	{SOBJDIR}UTWIG.O ç
	{SOBJDIR}VUX.O ç
	{SOBJDIR}YEHAT.O ç
	{SOBJDIR}ZOQFOT.O

# Alien conversation object modules -- shared by converse and starcon.	
COMMOBJS = ç
	{CMOBJDIR}ARILOU.O ç
	{CMOBJDIR}BLACKUR.O ç
	{CMOBJDIR}DRUUGE.O ç
	{CMOBJDIR}CHMMR.O ç
	{CMOBJDIR}COMANDR.O ç
	{CMOBJDIR}ILWRATH.O ç
	{CMOBJDIR}MYCON.O ç
	{CMOBJDIR}MELNORM.O ç
	{CMOBJDIR}ORZ.O ç
	{CMOBJDIR}PKUNK.O ç
	{CMOBJDIR}REBEL.O ç
	{CMOBJDIR}SHOFIXT.O ç
	{CMOBJDIR}SLYLAND.O ç
	{CMOBJDIR}SLYHOME.O ç
	{CMOBJDIR}SPAHOME.O ç
	{CMOBJDIR}SPATHI.O ç
	{CMOBJDIR}STARBAS.O ç
	{CMOBJDIR}SUPOX.O ç
	{CMOBJDIR}SYREEN.O ç
	{CMOBJDIR}TALKPET.O ç
	{CMOBJDIR}THRADD.O ç
	{CMOBJDIR}UMGAH.O ç
	{CMOBJDIR}URQUAN.O ç
	{CMOBJDIR}UTWIG.O ç
	{CMOBJDIR}VUX.O ç
	{CMOBJDIR}YEHAT.O ç
	{CMOBJDIR}ZOQFOT.O

# The objects created by the planet source.
PLANETOBJS = ç
	{POBJDIR}solarsys.O ç
	{POBJDIR}planets.O ç
	{POBJDIR}planscan.O ç
	{POBJDIR}plangen.O ç
	{POBJDIR}oval.O ç
	{POBJDIR}scan.O ç
	{POBJDIR}calc.O ç
	{POBJDIR}orbits.O ç
	{POBJDIR}surface.O ç
	{POBJDIR}starmap.O ç
	{POBJDIR}lander.O ç
	{POBJDIR}cargo.O ç
	{POBJDIR}devices.O ç
	{POBJDIR}roster.O ç
	{POBJDIR}report.O ç
	{POBJDIR}gensol.O ç
	{POBJDIR}genshof.O ç
	{POBJDIR}gencol.O ç
	{POBJDIR}genspa.O ç
	{POBJDIR}genzoq.O ç
	{POBJDIR}genmel.O ç
	{POBJDIR}genpet.O ç
	{POBJDIR}genchmmr.O ç
	{POBJDIR}gensyr.O ç
	{POBJDIR}genburv.O ç
	{POBJDIR}gensly.O ç
	{POBJDIR}gendru.O ç
	{POBJDIR}genutw.O ç
	{POBJDIR}genthrad.O ç
	{POBJDIR}genmyc.O ç
	{POBJDIR}genorz.O ç
	{POBJDIR}genvault.O ç
	{POBJDIR}genwreck.O ç
	{POBJDIR}genvux.O ç
	{POBJDIR}gensam.O ç
	{POBJDIR}genyeh.O ç
	{POBJDIR}genpku.O ç
	{POBJDIR}gensup.O ç
	{POBJDIR}genrain.O ç
	{POBJDIR}genilw.O ç
	{POBJDIR}pl_stuff.O

LIBPATH = "{HardDisk}:src:libs"

DEBUG =
CC = armcc {DEBUG} -zps1 -za1 -ff
LINK = armlink -aif -r -b 0x00 -workspace 0x4000

INCLUDES = -i {LIBPATH},':3DO',':'
FLAGS = -d __WATCOMC__=1 -d _VW=_screen_width -d _VH=_screen_height ç
	-d SAFE_X=16 -d SAFE_Y=16 -d HAVE_EXTRA_SCREEN -d XTESTING

# add below
SYSLIBS		= ç
	"{3DOLibs}StorageTuner.lib" ç
	"{3DOLibs}Lib3DO.lib" ç
	"{3DOLibs}music.lib" ç
	"{3DOLibs}operamath.lib" ç
	"{3DOLibs}filesystem.lib" ç
	"{3DOLibs}graphics.lib" ç
	"{3DOLibs}audio.lib" ç
	"{3DOLibs}input.lib" ç
	"{3DOLibs}clib.lib" ç
	"{3DOLibs}swi.lib"
	
LIBS	= ç
	{LIBPATH}:graphics:graphics.lib ç
	{LIBPATH}:memory:memory.lib ç
	{LIBPATH}:task:task.lib ç
	{LIBPATH}:sound:sound.lib ç
	{LIBPATH}:strings:strings.lib ç
	{LIBPATH}:resource:resource.lib ç
	{LIBPATH}:decomp:decomp.lib ç
	{LIBPATH}:input:input.lib ç
	{LIBPATH}:math:math.lib ç
	{LIBPATH}:video:video.lib ç
	{LIBPATH}::duck:duckloop:duckloop.lib

# Search current directory {:}, {SHIPDIRS}, {PLANETDIR}, and {PAULDIR} for source files.
# Target dirs for objects are {OBJDIR}, {SOBJDIR}, and {POBJDIR}.
# {OBJDIR} {SOBJDIR} {POBJDIR} É : {SHIPDIRS} {PLANETDIR} {PAULDIR}
# Search current source directory {:} for target objects in {OBJDIR}.
{OBJDIR} É :

# Search source directory {COMMDIRS} for target objects in {CMOBJDIR}.
{CMOBJDIR} É : {COMMDIRS}

# Search source directory {SHIPDIRS} for target objects in {SOBJDIR}.
{SOBJDIR} É {SHIPDIRS}

# Search source directories {PLANETDIR} and {PAULDIR} for target objects in {POBJDIR}.
{POBJDIR} É {PLANETDIR} {PAULDIR}

.o É .c
	{CC} -i :{DepDir} {INCLUDES} -i "{3doincludes}" {FLAGS} -o {TargDir}{Default}.o {DepDir}{Default}.c
	
.a É .s
	armasm -i "{3doincludes}" -o {TargDir}{Default}.a {DepDir}{Default}.s

# for debugging --
# add -d after {LINK}
# add -debug at end of modbin line
# comment out stripaif line

starcon ÉÉ {OBJECTS} {SHIPOBJS} {PLANETOBJS} {COMMOBJS} {LIBS}
	echo "Linking starcon..."
	{LINK} -o starcon "{3DOLibs}cstartup.o" ç
		{OBJECTS} ç
		{SHIPOBJS} ç
		{PLANETOBJS} ç
		{COMMOBJS} ç
		{LIBS} {SYSLIBS}
	modbin starcon -stack 10240
	stripaif starcon
	duplicate -y starcon "{HardDisk}:3do:3do_os:"{3doRelease}":remote:starcon"
	echo "Done."
	beep

cleanall É
	Delete -y {OBJECTS}
	Delete -y {SHIPOBJS}
	Delete -y {PLANETOBJS}
	Delete -y {COMMOBJS}
	
test É
	delete -y :objects:GLOBDATA.o ç
		:objects:HYPER.o ç
		:objects:pickship.o ç
		:objects:RESTART.o ç
		:objects:STARCON.o
