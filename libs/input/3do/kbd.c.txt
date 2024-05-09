#include <dos.h>
#include <conio.h>
#include "compiler.h"

#define TAB_SIZE	0x80

#define EOI		0x20
#define KB_DATA		0x60
#define KB_CTRL		0x61

STATIC BYTE	key_hit, key_buf_head, key_buf_tail,
		key_buf_tab[TAB_SIZE],
		key_transition_tab[TAB_SIZE];
STATIC BYTE	key_xlat_tab[] =
{
    0, 0x1B, '1', '2', '3', '4', '5', '6',		/* 00 */
    '7', '8', '9', '0', '-', '=', 0x08, 0x09,	/* 08 */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',		/* 10 */
    'o', 'p', '[', ']', 0x0A, 139, 'a', 's',		/* 18 */
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',		/* 20 */
    0x27, '`', 137, '\\', 'z', 'x', 'c', 'v',		/* 28 */
    'b', 'n', 'm', ',', '.', '/', 138, 0,		/* 30 */
    140, ' ', 0, 141, 142, 143, 144, 145,		/* 38 */
    146, 147, 148, 149, 150, 0, 0, 132,		/* 40 */
    130, 133, 0, 128, 0, 129, 0, 134,		/* 48 */
    131, 135, 136, 0x7F, 0, 0, 0, 151,		/* 50 */
    152, 0, 0, 0, 0, 0, 0, 0,			/* 58 */
    0, 0, 0, 0, 0, 0, 0, 0,				/* 60 */
    0, 0, 0, 0, 0, 0, 0, 0,				/* 68 */
    0, 0, 0, 0, 0, 0, 0, 0,				/* 70 */
    0, 0, 0, 0, 0, 0, 0, 0,				/* 78 */
/* when shift/ctrl keys are down */
    0, 0x1B, '!', '@', '#', '$', '%', '^',		/* 00 */
    '&', '*', '(', ')', '_', '+', 0x08, 0x09,	/* 08 */
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',		/* 10 */
    'O', 'P', '{', '}', 0x0A, 139, 'A', 'S',		/* 18 */
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',		/* 20 */
    0x22, '~', 137, '|', 'Z', 'X', 'C', 'V',		/* 28 */
    'B', 'N', 'M', '<', '>', '?', 138, 0,		/* 30 */
    140, ' ', 0, 141, 142, 143, 144, 145,		/* 38 */
    146, 147, 148, 149, 150, 0, 0, 132,		/* 40 */
    130, 133, 0, 128, 0, 129, 0, 134,		/* 48 */
    131, 135, 136, 0x7F, 0, 0, 0, 151,		/* 50 */
    152, 0, 0, 0, 0, 0, 0, 0,			/* 58 */
    0, 0, 0, 0, 0, 0, 0, 0,				/* 60 */
    0, 0, 0, 0, 0, 0, 0, 0,				/* 68 */
    0, 0, 0, 0, 0, 0, 0, 0,				/* 70 */
    0, 0, 0, 0, 0, 0, 0, 0,				/* 78 */
};

PROC(STATIC
void INTERRUPT key_int, (),
    ARG_VOID
)
{
    BYTE	KeyData, KeyStat;

    _enable ();

    KeyData = inp (KB_DATA);
    KeyStat = (KeyData >> 7) ^ 1;
    KeyData &= ~(1 << 7);
    if (key_transition_tab[KeyData] ^= (key_transition_tab[KeyData] ^ KeyStat))
    {
#define CAPS_LOCK_SCAN	58
	if ((key_hit = KeyData) == CAPS_LOCK_SCAN)
	{
	}

	if ((KeyStat = (key_buf_tail + 1) & (TAB_SIZE - 1)) != key_buf_head)
	{
	    key_buf_tab[key_buf_tail] = KeyData;
	    key_buf_tail = KeyStat;
	}
    }

    KeyData = inp (KB_CTRL);
    outp (KB_CTRL, KeyData | 0x80);
    outp (KB_CTRL, KeyData);

    outp (EOI, 0x20);
}

STATIC void	__far *orig_pm09;

PROC(
void InstallKey, (),
    ARG_VOID
)
{
    void	__far *fh;
    union REGS	r;

    r.x.eax = 0x0204;
    r.h.bl = 0x09;
    int386 (0x31, &r, &r);
    orig_pm09 = MK_FP (r.w.cx, r.x.edx);

    fh = (void __far *)key_int;

    r.x.eax = 0x0205;
    r.h.bl = 0x09;
    r.x.edx = FP_OFF (fh);
    r.w.cx = FP_SEG (fh);
    int386 (0x31, &r, &r);
}

PROC(
void RestoreKey, (),
    ARG_VOID
)
{
    union REGS	r;

    r.x.eax = 0x0205;
    r.h.bl = 0x09;
    r.x.edx = FP_OFF (orig_pm09);
    r.w.cx = FP_SEG (orig_pm09);
    int386 (0x31, &r, &r);
}

PROC(
void FlushInput, (),
    ARG_VOID
)
{
    key_buf_head = key_buf_tail = key_hit = 0;
}

PROC(
BYTE KeyHit, (),
    ARG_VOID
)
{
    BYTE	KeyData;

    KeyData = key_hit;
    key_hit = 0;

    return (KeyData);
}

PROC(
BYTE KeyDown, (which_scan),
    ARG_END	(BYTE	which_scan)
)
{
    return (key_transition_tab[which_scan]);
}

PROC(
BYTE GetASCIIKey, (),
    ARG_VOID
)
{
    if (key_buf_head != key_buf_tail)
    {
	BYTE	ch;

	ch = key_buf_tab[key_buf_head];
	key_buf_head = (key_buf_head + 1) & (TAB_SIZE - 1);

	return (key_xlat_tab[ch]);
    }

    return (0);
}

PROC(
BYTE ASCIIToScan, (which_key),
    ARG_END	(BYTE	which_key)
)
{
    BYTE	scan;

    for (scan = 0; scan < 255 && key_xlat_tab[scan] != which_key; ++scan)
	;

    return (scan);
}

PROC_GLOBAL(
BYTE GetKeyState, (key),
    ARG_END	(BYTE	key)
)
{
    return (key_transition_tab[ASCIIToScan (key)]);
}

PROC(
BYTE ScanToASCII, (which_scan),
    ARG_END	(BYTE	which_scan)
)
{
    return (key_xlat_tab[which_scan]);
}
