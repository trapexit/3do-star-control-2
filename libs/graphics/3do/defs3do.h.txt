#ifndef _DEFS3DO_H
#define _DEFS3DO_H

extern int	_screen_width, _screen_height;

#define SCREEN_WIDTH	_screen_width
#define SCREEN_HEIGHT	_screen_height
#define SCREEN_DEPTH	8

#define VIEW_WIDTH	SCREEN_WIDTH
#define VIEW_HEIGHT	SCREEN_HEIGHT

#define BUILD_FRAME	(1 << 0)
#define FIND_PAGE	(1 << 1)
#define FIRST_BATCH	(1 << 2)
#define GRAB_OTHER	(1 << 3)
#define COLOR_CYCLE	(1 << 4)
#define CYCLE_PENDING	(1 << 5)
#define ENABLE_CYCLE	(1 << 6)

extern unsigned char	_batch_flags;

enum
{
    FadeAllToWhite = 250,
    FadeSomeToWhite,
    FadeAllToBlack,
    FadeAllToColor,
    FadeSomeToBlack,
    FadeSomeToColor
};

#endif /* _DEFS3DO_H */
