#include "starcon.h"

SIZE	sinetab[] =
{
    -FLT_ADJUST (1.000000),
    -FLT_ADJUST (0.995185),
    -FLT_ADJUST (0.980785),
    -FLT_ADJUST (0.956940),
    -FLT_ADJUST (0.923880),
    -FLT_ADJUST (0.881921),
    -FLT_ADJUST (0.831470),
    -FLT_ADJUST (0.773010),
    -FLT_ADJUST (0.707107),
    -FLT_ADJUST (0.634393),
    -FLT_ADJUST (0.555570),
    -FLT_ADJUST (0.471397),
    -FLT_ADJUST (0.382683),
    -FLT_ADJUST (0.290285),
    -FLT_ADJUST (0.195090),
    -FLT_ADJUST (0.098017),
    FLT_ADJUST (0.000000),
    FLT_ADJUST (0.098017),
    FLT_ADJUST (0.195090),
    FLT_ADJUST (0.290285),
    FLT_ADJUST (0.382683),
    FLT_ADJUST (0.471397),
    FLT_ADJUST (0.555570),
    FLT_ADJUST (0.634393),
    FLT_ADJUST (0.707107),
    FLT_ADJUST (0.773010),
    FLT_ADJUST (0.831470),
    FLT_ADJUST (0.881921),
    FLT_ADJUST (0.923880),
    FLT_ADJUST (0.956940),
    FLT_ADJUST (0.980785),
    FLT_ADJUST (0.995185),
    FLT_ADJUST (1.000000),
    FLT_ADJUST (0.995185),
    FLT_ADJUST (0.980785),
    FLT_ADJUST (0.956940),
    FLT_ADJUST (0.923880),
    FLT_ADJUST (0.881921),
    FLT_ADJUST (0.831470),
    FLT_ADJUST (0.773010),
    FLT_ADJUST (0.707107),
    FLT_ADJUST (0.634393),
    FLT_ADJUST (0.555570),
    FLT_ADJUST (0.471397),
    FLT_ADJUST (0.382683),
    FLT_ADJUST (0.290285),
    FLT_ADJUST (0.195090),
    FLT_ADJUST (0.098017),
    FLT_ADJUST (0.000000),
    -FLT_ADJUST (0.098017),
    -FLT_ADJUST (0.195090),
    -FLT_ADJUST (0.290285),
    -FLT_ADJUST (0.382683),
    -FLT_ADJUST (0.471397),
    -FLT_ADJUST (0.555570),
    -FLT_ADJUST (0.634393),
    -FLT_ADJUST (0.707107),
    -FLT_ADJUST (0.773010),
    -FLT_ADJUST (0.831470),
    -FLT_ADJUST (0.881921),
    -FLT_ADJUST (0.923880),
    -FLT_ADJUST (0.956940),
    -FLT_ADJUST (0.980785),
    -FLT_ADJUST (0.995185),
};

PROC(
COUNT ARCTAN, (delta_x, delta_y),
    ARG		(SIZE	delta_x)
    ARG_END	(SIZE	delta_y)
)
{
    register SIZE	v1, v2;
    static COUNT	atantab[] =
    {
	0,
	0,
	1,
	1,
	1,
	2,
	2,
	2,
	2,
	3,
	3,
	3,
	4,
	4,
	4,
	4,
	5,
	5,
	5,
	5,
	6,
	6,
	6,
	6,
	7,
	7,
	7,
	7,
	7,
	7,
	8,
	8,
	8,
    };

    v1 = delta_x;
    v2 = delta_y;
    if (v1 == 0 && v2 == 0)
	return (FULL_CIRCLE);

    if (v1 < 0)
	v1 = -v1;
    if (v2 < 0)
	v2 = -v2;
    if (v1 > v2)
	v1 = QUADRANT
		- atantab[(((DWORD)v2 << (CIRCLE_SHIFT - 1)) + (v1 >> 1)) / v1];
    else
	v1 = atantab[(((DWORD)v1 << (CIRCLE_SHIFT - 1)) + (v2 >> 1)) / v2];

    if (delta_x < 0)
	v1 = FULL_CIRCLE - v1;
    if (delta_y > 0)
	v1 = HALF_CIRCLE - v1;

    return (NORMALIZE_ANGLE (v1));
}

