#ifndef _STATE_H
#define _STATE_H

PROC_GLOBAL(
void InitPlanetInfo, (),
    ARG_VOID
);
PROC_GLOBAL(
void UninitPlanetInfo, (),
    ARG_VOID
);
PROC_GLOBAL(
void GetPlanetInfo, (),
    ARG_VOID
);
PROC_GLOBAL(
void PutPlanetInfo, (),
    ARG_VOID
);

PROC_GLOBAL(
void InitGroupInfo, (FirstTime),
    ARG_END	(BOOLEAN	FirstTime)
);
PROC_GLOBAL(
void UninitGroupInfo, (),
    ARG_VOID
);
PROC_GLOBAL(
BOOLEAN GetGroupInfo, (offset, which_group),
    ARG		(DWORD	offset)
    ARG_END	(BYTE	which_group)
);
PROC_GLOBAL(
DWORD PutGroupInfo, (offset, which_group),
    ARG		(DWORD	offset)
    ARG_END	(BYTE	which_group)
);
PROC_GLOBAL(
void BuildGroups, (),
    ARG_VOID
);

PROC_GLOBAL(
BOOLEAN GameOptions, (),
    ARG_VOID
);

#define MAX_DESC_CHARS	60
typedef BYTE		GAME_DESC[MAX_DESC_CHARS + 1];

#define STARINFO_FILE		"starinfo.dat"
#define STAR_BUFSIZE		(NUM_SOLAR_SYSTEMS * sizeof (DWORD) \
					+ 3800 * (3 * sizeof (DWORD)))
#define RANDGRPINFO_FILE	"randgrp.dat"
#define RAND_BUFSIZE		(4 * 1024)
#define DEFGRPINFO_FILE		"defgrp.dat"
#define DEF_BUFSIZE		(10 * 1024)

typedef enum
{
    STARINFO,
    RANDGRPINFO,
    DEFGRPINFO
} INFO_TYPE;


#endif /* _STATE_H */

