#ifndef _SCAN_H
#define _SCAN_H

typedef struct
{
    POINT	start;
    COUNT	start_dot,
		num_dots,
		dots_per_semi;
} SCAN_DESC;
typedef SCAN_DESC	*PSCAN_DESC;
typedef SCAN_DESC	near *NPSCAN_DESC;
typedef SCAN_DESC	near *LPSCAN_DESC;

typedef struct
{
    PPOINT	line_base;
    COUNT	num_scans, num_same_scans;
    PSCAN_DESC	scan_base;
} SCAN_BLOCK;
typedef SCAN_BLOCK	*PSCAN_BLOCK;
typedef SCAN_BLOCK	near *NPSCAN_BLOCK;
typedef SCAN_BLOCK	near *LPSCAN_BLOCK;

#define SAME_SCAN		(1 << 16)
#define SCALED_ROOT_TWO	92682L	/* root 2 * (1 << 16) */
#define SCALE_FACTOR		16

#endif /* _SCAN_H */

