#ifndef _REDBOOK_H
#define _REDBOOK_H

PROC_GLOBAL(
BOOLEAN _is_red_book, (TrackInfo),
    ARG_END	(LPBYTE	TrackInfo)
);
PROC_GLOBAL(
BOOLEAN _play_red_book, (TrackInfo, Loop),
    ARG		(LPBYTE		TrackInfo)
    ARG_END	(BOOLEAN	Loop)
);
PROC_GLOBAL(
void _stop_red_book, (),
    ARG_VOID
);
PROC_GLOBAL(
BOOLEAN _red_book_playing, (),
    ARG_VOID
);
PROC_GLOBAL(
void _pause_red_book, (),
    ARG_VOID
);
PROC_GLOBAL(
void _resume_red_book, (),
    ARG_VOID
);

#endif /* _REDBOOK_H */
