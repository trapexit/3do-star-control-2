#ifndef _NAMEREF_H
#define _NAMEREF_H

#define LoadCodeRes(r)			LoadCodeResInstance (r)
#define LoadColorMap(r)			LoadColorMapInstance (r)
#define LoadStringTable(r)		LoadStringTableInstance(r)
#define LoadSound(r)			LoadSoundInstance(r)
#define LoadMusic(r)			LoadMusicInstance(r)

#define INIT_INSTANCES()	\
	InstallGraphicResType (GFXRES); \
	InstallStringTableResType (STRTAB); \
	InstallMusicResType (MUSICRES); \
	InstallCodeResType (CODE)
#define UNINIT_INSTANCES()

#endif /* _NAMEREF_H */

