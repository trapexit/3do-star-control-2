#ifndef _LANDER_H
#define _LANDER_H

#define NUM_TEXT_FRAMES		32

typedef struct
{
    BOOLEAN	InTransit;

    SOUND	OldMenuSounds;

    COUNT	ElementLevel, MaxElementLevel,
		BiologicalLevel;
    COUNT	ElementAmounts[NUM_ELEMENT_CATEGORIES];

    COUNT	NumFrames;
    char	AmountBuf[10];
    TEXT	MineralText[3];

    COLOR	ColorCycle[NUM_TEXT_FRAMES >> 1];

    BYTE	TectonicsChance, WeatherChance, FireChance;
} PLANETSIDE_DESC;
typedef PLANETSIDE_DESC	*PPLANETSIDE_DESC;

extern CONTEXT			ScanContext;

PROC_GLOBAL(
void DoDiscoveryReport, (ReadOutSounds),
    ARG_END	(SOUND	ReadOutSounds)
);

#endif /* _LANDER_H */

