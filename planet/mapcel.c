#include "Portfolio.h"
#include "Init3DO.h"
#include "Parse3DO.h"
#include "Utils3DO.h"
#include "filefunctions.h"
#include "BlockFile.h"

void
myMapCel (CCB *ccb, Point *pt)
{
    int32	w, h, dx, dy, adj;
    
    w = ccb->ccb_Width;
    h = ccb->ccb_Height;
    dx = pt[1].pt_X - pt[0].pt_X;
    dy = pt[1].pt_Y - pt[0].pt_Y;
    ccb->ccb_HDX = (dx << 4) / w;
    ccb->ccb_HDY = (dy << 4) / w;
    if (dx || dy)
    {
	adj = (1 << 19) / w;
    if (dx > 0)
	ccb->ccb_HDX += adj;
    else if (dx < 0)
	ccb->ccb_HDX -= adj;
    if (dy > 0)
	ccb->ccb_HDY += adj;
    else if (dy < 0)
	ccb->ccb_HDY -= adj;
    }
    dx = pt[3].pt_X - pt[0].pt_X;
    dy = pt[3].pt_Y - pt[0].pt_Y;
    ccb->ccb_VDX = dx / h;
    ccb->ccb_VDY = dy / h;
    if (dx || dy)
    {
	adj = (1 << 15) / h;
    if (dx > 0)
	ccb->ccb_VDX += adj;
    else if (dx < 0)
	ccb->ccb_VDX -= adj;
    if (dy > 0)
	ccb->ccb_VDY += adj;
    else if (dy < 0)
	ccb->ccb_VDY -= adj;
    }

    w *= h;    
    dx = (pt[2].pt_X - pt[3].pt_X) - (pt[1].pt_X - pt[0].pt_X);
    dy = (pt[2].pt_Y - pt[3].pt_Y) - (pt[1].pt_Y - pt[0].pt_Y);
    ccb->ccb_HDDX = (dx << 4) / w;
    ccb->ccb_HDDY = (dy << 4) / w;
    if (dx || dy)
    {
	adj = (1 << 19) / w;
    if (dx > 0)
	ccb->ccb_HDDX += adj;
    else if (dx < 0)
	ccb->ccb_HDDX -= adj;
    if (dy > 0)
	ccb->ccb_HDDY += adj;
    else if (dy < 0)
	ccb->ccb_HDDY -= adj;
    }
        
    ccb->ccb_XPos = pt[0].pt_X + (1 << 15);
    ccb->ccb_YPos = pt[0].pt_Y + (1 << 15);
    
#if 0 
{
CCB	t;

t = *ccb;
MapCel (ccb, pt);
if (ccb->ccb_XPos != t.ccb_XPos
	|| ccb->ccb_YPos != t.ccb_YPos
	|| ccb->ccb_HDX != t.ccb_HDX
	|| ccb->ccb_HDY != t.ccb_HDY
	|| ccb->ccb_VDX != t.ccb_VDX
	|| ccb->ccb_VDY != t.ccb_VDY
	|| ccb->ccb_HDDX != t.ccb_HDDX
	|| ccb->ccb_HDDY != t.ccb_HDDY
	|| ccb->ccb_Width != t.ccb_Width
	|| ccb->ccb_Height != t.ccb_Height)
{
    printf ("theirs: %d,%d  ", ccb->ccb_XPos, ccb->ccb_YPos);
    printf ("%d,%d  ", ccb->ccb_HDX, ccb->ccb_HDY);
    printf ("%d,%d  ", ccb->ccb_VDX, ccb->ccb_VDY);
    printf ("%d,%d  ", ccb->ccb_HDDX, ccb->ccb_HDDY);
    printf ("%d,%d\n", ccb->ccb_Width, ccb->ccb_Height);
    printf ("mine: %d,%d  ", t.ccb_XPos, t.ccb_YPos);
    printf ("%d,%d  ", t.ccb_HDX, t.ccb_HDY);
    printf ("%d,%d  ", t.ccb_VDX, t.ccb_VDY);
    printf ("%d,%d  ", t.ccb_HDDX, t.ccb_HDDY);
    printf ("%d,%d\n", t.ccb_Width, t.ccb_Height);
}
}
#endif
}
