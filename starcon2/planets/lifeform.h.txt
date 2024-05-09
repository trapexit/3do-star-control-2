#ifndef _LIFEFORM_H
#define _LIFEFORM_H

#define BEHAVIOR_HUNT		(0 << 0)
#define BEHAVIOR_FLEE		(1 << 0)
#define BEHAVIOR_UNPREDICTABLE	(2 << 0)

#define BEHAVIOR_MASK		0x03
#define BEHAVIOR_SHIFT		0

#define AWARENESS_LOW		(0 << 2)
#define AWARENESS_MEDIUM	(1 << 2)
#define AWARENESS_HIGH		(2 << 2)

#define AWARENESS_MASK		0x0C
#define AWARENESS_SHIFT		(BEHAVIOR_SHIFT + 2)

#define SPEED_MOTIONLESS	(0 << 4)
#define SPEED_SLOW		(1 << 4)
#define SPEED_MEDIUM		(2 << 4)
#define SPEED_FAST		(3 << 4)

#define SPEED_MASK		0x30
#define SPEED_SHIFT		(AWARENESS_SHIFT + 2)

#define DANGER_HARMLESS		(0 << 6)
#define DANGER_WEAK		(1 << 6)
#define DANGER_NORMAL		(2 << 6)
#define DANGER_MONSTROUS	(3 << 6)

#define DANGER_MASK		0xC0
#define DANGER_SHIFT		(SPEED_SHIFT + 2)

#define NUM_CREATURE_TYPES	23
#define MAX_LIFE_VARIATION	3

#define CREATURE_AWARE		(BYTE)(1 << 7)

typedef struct
{
    BYTE	Attributes, ValueAndHitPoints;
} LIFEFORM_DESC;

extern CONST LIFEFORM_DESC	CreatureData[];

#endif /* _LIFEFORM_H */

