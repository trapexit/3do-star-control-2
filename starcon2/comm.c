#include <ctype.h>
#include "starcon.h"
#include "commglue.h"

#define MAX_RESPONSES	8
#define BACKGROUND_VOL	32
#define FOREGROUND_VOL	128

#define SLIDER_Y	107
#define SLIDER_HEIGHT	15

// #define DEBUG

LOCDATA		CommData;
char		shared_phrase_buf[256];

typedef struct encounter_state
{
    PROC_PARAMETER(
    BOOLEAN (*InputFunc), (InputState, pES),
	ARG	(INPUT_STATE	InputState)
	ARG_END	(struct encounter_state	*pES)
//	ARG_END	(PVOID		pES)
    );
    COUNT	MenuRepeatDelay;

    COUNT	Initialized;
    BYTE	num_responses, cur_response, top_response;
    struct
    {
	RESPONSE_REF	response_ref;
	TEXT		response_text;
	RESPONSE_FUNC	response_func;
    } response_list[MAX_RESPONSES];

    TASK	AnimTask;

    COUNT	phrase_buf_index;
    char	phrase_buf[512];
} ENCOUNTER_STATE;
typedef ENCOUNTER_STATE	*PENCOUNTER_STATE;

enum
{
    UP_DIR,
    DOWN_DIR,
    NO_DIR
};
enum
{
    PICTURE_ANIM,
    COLOR_ANIM
};
typedef struct
{
    COUNT	Alarm;
    BYTE	Direction, FramesLeft;
    BYTE	AnimType;
    union
    {
	FRAME		CurFrame;
	COLORMAP	CurCMap;
    } AnimObj;
} SEQUENCE, *PSEQUENCE;

static PENCOUNTER_STATE	pCurInputState;
#ifdef DEBUG
static BOOLEAN do_exit;
#endif /* DEBUG */

PROC(STATIC
COORD add_text, (status, pTextIn),
    ARG		(int	status)
    ARG_END	(PTEXT	pTextIn)
)
{
    char	ch;
    COUNT	maxchars, numchars;
    TEXT	locText;
    PTEXT	pText;
    SIZE	leading, maxdescent, maxascent;
    LPBYTE	lpStr;
    SIZE	text_width;

    BatchGraphics ();
    maxchars = (COUNT)~0;
    GetContextFontExtents (&leading, &maxdescent, &maxascent);
    if (status <= -4)
    {
	text_width = SIS_SCREEN_WIDTH - (TEXT_X_OFFS << 2);

	pText = pTextIn;
    }
    else
    {
	text_width = SIS_SCREEN_WIDTH - ((TEXT_X_OFFS + 8) << 1);

	switch (status)
	{
	    case -3:
		SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));
		break;
	    case -2:
		SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x14, 0x14), 0x03));
		break;
	    case -1:
		SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x1A, 0x1A, 0x1A), 0x12));
		break;
	}

	maxchars = pTextIn->CharCount;
	locText = *pTextIn;
	locText.baseline.x -= 8;
	locText.CharCount = 1;
	ch = '*';
	locText.lpStr = (LPBYTE)&ch;
	DrawText (&locText);

	locText = *pTextIn;
	pText = &locText;
	pText->baseline.y -= leading;
    }

    numchars = 0;
    lpStr = (LPBYTE)pText->lpStr;

    do
    {
	pText->lpStr = (LPBYTE)lpStr;
	pText->CharCount = 1;
	pText->baseline.y += leading;

	{
	    BOOLEAN	eot;
	    RECT	r, old_r;
	    COUNT	OldCount;

	    eot = FALSE;
	    do
	    {
		old_r = r;
		OldCount = pText->CharCount;
		while (!(eot = (BOOLEAN)(
			(ch = *++lpStr) == '\0'
			|| ch == '\n'
			|| ch == '\r'
			|| lpStr - pText->lpStr >= maxchars
			)) && ch != ' ')
		    ;
		pText->CharCount = lpStr - pText->lpStr;
		TextRect (pText, &r, NULL_PTR);
	    } while (!eot && r.extent.width < text_width);

	    if (r.extent.width >= text_width)
	    {
		pText->CharCount = OldCount;
		r = old_r;
	    }

	    if (maxchars -= pText->CharCount)
		--maxchars;
	    numchars += pText->CharCount;
	    
	    if (pText->baseline.y < SIS_SCREEN_HEIGHT)
		DrawText (pText);

	    lpStr = pText->lpStr + pText->CharCount;
	    if (status < -4 && pText->baseline.y >= -status - 10)
	    {
		++lpStr;
		break;
	    }
	}
    } while ((ch = *lpStr++) != '\0' && ch != '\n' && ch != '\r' && maxchars);
    pText->lpStr = (LPBYTE)lpStr;

    UnbatchGraphics ();
    return (pText->baseline.y);
}

PROC(STATIC
void DrawSISComWindow, (),
    ARG_VOID
)
{
    CONTEXT	OldContext;

    if (LOBYTE (GLOBAL (CurrentActivity)) != WON_LAST_BATTLE)
    {
	RECT	r;

	OldContext = SetContext (SpaceContext);

	r.corner.x = 0;
	r.corner.y = SLIDER_Y + SLIDER_HEIGHT;
	r.extent.width = SIS_SCREEN_WIDTH;
	r.extent.height = SIS_SCREEN_HEIGHT - r.corner.y;
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x14), 0x01));
	DrawFilledRectangle (&r);

	SetContext (OldContext);
    }
#ifdef NEVER
    else
    {
#define NUM_CREDITS		5
	BYTE		j;
	TEXT		t;
	STRING		OldStrings;
	extern STRING	CreditStrings;

	t.baseline.x = SCREEN_WIDTH >> 1;
	t.baseline.y = RADAR_Y - 5;
	t.align = ALIGN_CENTER;

	OldContext = SetContext (ScreenContext);

	OldStrings = SetRelStringTableIndex (CreditStrings, -NUM_CREDITS);
	SetContextFont (MicroFont);
	BatchGraphics ();
	for (j = 0; j < NUM_CREDITS; ++j)
	{
	    SetContextForeGroundColor (BLACK_COLOR);
	    t.lpStr = (LPBYTE)GetStringAddress (OldStrings);
	    t.CharCount = GetStringLength (OldStrings);
	    OldStrings = SetRelStringTableIndex (OldStrings, 1);
	    DrawText (&t);
	    SetContextForeGroundColor (WHITE_COLOR);
	    t.lpStr = (LPBYTE)GetStringAddress (CreditStrings);
	    t.CharCount = GetStringLength (CreditStrings);
	    CreditStrings = SetRelStringTableIndex (CreditStrings, 1);
	    DrawText (&t);

	    if (j)
		t.baseline.y += 12;
	    else
	    {
		t.baseline.y += 16;
		SetContextFont (StarConFont);
	    }
	}
	UnbatchGraphics ();

	SetContext (OldContext);
    }
#endif /* NEVER */
}

PROC(STATIC
void DrawAlienFrame, (aframe, pSeq),
    ARG		(FRAME		aframe)
    ARG_END	(PSEQUENCE	pSeq)
)
{
    COUNT		i;
    STAMP		s;
    ANIMATION_DESCPTR	ADPtr;

    s.origin.x = -SAFE_X;
    s.origin.y = 0;
    if ((s.frame = CommData.AlienFrame) == 0)
	s.frame = aframe;
    
    BatchGraphics ();
    DrawStamp (&s);
    ADPtr = &CommData.AlienAmbientArray[i = CommData.NumAnimations];
    while (i--)
    {
	--ADPtr;

	if (!(ADPtr->AnimFlags & ANIM_MASK))
	{
	    s.frame = SetAbsFrameIndex (
		    s.frame,
		    ADPtr->StartIndex
		    );
	    DrawStamp (&s);
	    ADPtr->AnimFlags |= ANIM_DISABLED;
	}
	else if (pSeq)
	{
	    if (pSeq->AnimType == PICTURE_ANIM)
	    {
		s.frame = pSeq->AnimObj.CurFrame;
		DrawStamp (&s);
	    }
	    --pSeq;
	}
    }
    if (aframe && CommData.AlienFrame && aframe != CommData.AlienFrame)
    {
	s.frame = aframe;
	DrawStamp (&s);
    }
    UnbatchGraphics ();
}

static struct
{
    COLORMAPPTR	CMapPtr;
    SIZE	Ticks;
    TASK	XFormTask;
} TaskControl;

STATIC BOOLEAN	ColorChange;
STATIC BOOLEAN	VOLATILE XFormFlush;
STATIC COUNT	VOLATILE NumXFormTasks;

PROC(STATIC
void far xform_PLUT_task, (),
    ARG_VOID
)
{
    COLORMAPPTR	CurMapPtr;
    TASK	T;

    while ((T = TaskControl.XFormTask) == 0)
	TaskSwitch ();
    TaskControl.XFormTask = 0;
    CurMapPtr = TaskControl.CMapPtr;

    {
	SIZE	TDelta, TTotal;
	DWORD	CurTime;

	TTotal = TaskControl.Ticks;
	TaskControl.CMapPtr = 0;
	CurTime = GetTimeCounter ();
	do
	{
#define NUMBER_OF_PLUTVALS	32
#define NUMBER_OF_PLUT_UINT32s	(NUMBER_OF_PLUTVALS >> 1)
#define PLUT_BYTE_SIZE		(sizeof (DWORD) * NUMBER_OF_PLUT_UINT32s)
	    BYTE		i;
	    DWORD		StartTime;
	    DWORD		*pCurCMap;
	    COLORMAPPTR		ColorMapPtr;
	    extern DWORD	*_varPLUTs;

	    StartTime = CurTime;
	    CurTime = SleepTask (GetTimeCounter () + 2);
	    if (XFormFlush || (TDelta = (SIZE)(CurTime - StartTime)) > TTotal)
		TDelta = TTotal;

	    ColorMapPtr = CurMapPtr;
	    pCurCMap = (DWORD *)((BYTE *)_varPLUTs + (*ColorMapPtr * PLUT_BYTE_SIZE));
	    ColorMapPtr += 2;
	    i = NUMBER_OF_PLUT_UINT32s;
	    do
	    {
		SIZE	c0, c1;
		DWORD	v0, v1, val;

		v0 = *pCurCMap;
		v1 = MAKE_DWORD (
			MAKE_WORD (ColorMapPtr[3], ColorMapPtr[2]),
			MAKE_WORD (ColorMapPtr[1], ColorMapPtr[0])
			);
		ColorMapPtr += sizeof (DWORD);

		c0 = (v0 >> (10 + 16)) & 0x1F;
		c1 = (v1 >> (10 + 16)) & 0x1F;
		c0 += ((c1 - c0) * TDelta) / TTotal;
		val = (c0 & 0x1F) | (1 << 5);
		
		c0 = (v0 >> (5 + 16)) & 0x1F;
		c1 = (v1 >> (5 + 16)) & 0x1F;
		c0 += ((c1 - c0) * TDelta) / TTotal;
		val = (val << 5) | (c0 & 0x1F);

		c0 = (v0 >> (0 + 16)) & 0x1F;
		c1 = (v1 >> (0 + 16)) & 0x1F;
		c0 += ((c1 - c0) * TDelta) / TTotal;
		val = (val << 5) | (c0 & 0x1F);

		c0 = (v0 >> 10) & 0x1F;
		c1 = (v1 >> 10) & 0x1F;
		c0 += ((c1 - c0) * TDelta) / TTotal;
		val = (val << (5 + 1)) | (c0 & 0x1F) | (1 << 5);
		
		c0 = (v0 >> 5) & 0x1F;
		c1 = (v1 >> 5) & 0x1F;
		c0 += ((c1 - c0) * TDelta) / TTotal;
		val = (val << 5) | (c0 & 0x1F);

		c0 = (v0 >> 0) & 0x1F;
		c1 = (v1 >> 0) & 0x1F;
		c0 += ((c1 - c0) * TDelta) / TTotal;
		val = (val << 5) | (c0 & 0x1F);

		*pCurCMap++ = val;
	    } while (--i);

	    ColorChange = TRUE;

	} while (TTotal -= TDelta);
    }

    --NumXFormTasks;
    DeleteTask (T);
}

PROC(
void FlushPLUTXForms, (),
    ARG_VOID
)
{
    if (NumXFormTasks)
    {
	XFormFlush = TRUE;
	do
	    TaskSwitch ();
	while (NumXFormTasks);
	XFormFlush = FALSE;
    }
}

PROC(
DWORD XFormPLUT, (ColorMapPtr, TimeInterval),
    ARG		(COLORMAPPTR	ColorMapPtr)
    ARG_END	(SIZE		TimeInterval)
)
{
    if (ColorMapPtr)
    {
	DWORD	TimeOut;

	FlushPLUTXForms ();

	TaskControl.CMapPtr = ColorMapPtr;
	if ((TaskControl.Ticks = TimeInterval) <= 0)
	    TaskControl.Ticks = 1;	/* prevent divide by zero and negative fade */
	if (TimeInterval == 0 || (TaskControl.XFormTask = AddTask (
		xform_PLUT_task, 1024
		)) == 0)
	{
	    SetColorMap (ColorMapPtr);
	    ColorChange = TRUE;
	    TimeOut = GetTimeCounter ();
	}
	else
	{
	    do
		TaskSwitch ();
	    while (TaskControl.CMapPtr);

	    ++NumXFormTasks;
	    TimeOut = GetTimeCounter () + TaskControl.Ticks + 1;
	}
	TaskControl.CMapPtr = 0;

	return (TimeOut);
    }

    return (0);
}

PROC(STATIC
void RefreshResponses, (pES),
    ARG_END	(PENCOUNTER_STATE	pES)
)
{
    COORD	y;
    BYTE	response;
    SIZE	leading, maxdescent, maxascent;
    STAMP	s;

    SetContext (SpaceContext);
    GetContextFontExtents (&leading, &maxdescent, &maxascent);
    BatchGraphics ();

    DrawSISComWindow ();
    y = SLIDER_Y + SLIDER_HEIGHT + 1;
    for (response = pES->top_response; response < pES->num_responses; ++response)
    {
	pES->response_list[response].response_text.baseline.x = TEXT_X_OFFS + 8;
	pES->response_list[response].response_text.baseline.y = y + leading;
	pES->response_list[response].response_text.align = ALIGN_LEFT;
	if (response == pES->cur_response)
	    y = add_text (-1, &pES->response_list[response].response_text);
	else
	    y = add_text (-2, &pES->response_list[response].response_text);
    }

    if (pES->top_response)
    {
	s.origin.y = SLIDER_Y + SLIDER_HEIGHT + 1;
	s.frame = SetAbsFrameIndex (ActivityFrame, 6);
    }
    else if (y > SIS_SCREEN_HEIGHT)
    {
	s.origin.y = SIS_SCREEN_HEIGHT - 2;
	s.frame = SetAbsFrameIndex (ActivityFrame, 7);
    }
    else
	s.frame = 0;
    if (s.frame)
    {
	RECT	r;

	GetFrameRect (s.frame, &r);
	s.origin.x = SIS_SCREEN_WIDTH - r.extent.width - 1;
	DrawStamp (&s);
    }

    UnbatchGraphics ();
}

PROC(STATIC
LPBYTE FeedbackPlayerPhrase, (lpStr),
    ARG_END	(LPBYTE	lpStr)
)
{
    TEXT	ct;
    char	InResponse[] = "(In response to your statement)";

    ct.baseline.x = SIS_SCREEN_WIDTH >> 1;
    ct.baseline.y = SLIDER_Y + SLIDER_HEIGHT + 13;
    ct.align = ALIGN_CENTER;
    ct.CharCount = (COUNT)~0;
    ct.lpStr = (LPBYTE)InResponse;

    SetContext (SpaceContext);
    
    BatchGraphics ();
    DrawSISComWindow ();
    SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0xA, 0xC, 0x1F), 0x48));
    DrawText (&ct);
    ct.baseline.y += 16;

    if (lpStr[0] == '\0')
	++lpStr;
    else
    {
	SetContextForeGroundColor (BUILD_COLOR (MAKE_RGB15 (0x12, 0x14, 0x4F), 0x44));
	ct.lpStr = lpStr;
	add_text (-4, &ct);
	lpStr = ct.lpStr;
    }
    UnbatchGraphics ();
    
    return (lpStr);
}

PROC(STATIC
void near SetUpSequence, (pSeq),
    ARG_END	(PSEQUENCE	pSeq)
)
{
    COUNT		i;
    ANIMATION_DESCPTR	ADPtr;

    pSeq = &pSeq[i = CommData.NumAnimations];
    ADPtr = &CommData.AlienAmbientArray[i];
    while (i--)
    {
	--ADPtr;
	--pSeq;

	if (ADPtr->AnimFlags & COLORXFORM_ANIM)
	    pSeq->AnimType = COLOR_ANIM;
	else
	    pSeq->AnimType = PICTURE_ANIM;
	pSeq->Direction = UP_DIR;
	pSeq->FramesLeft = ADPtr->NumFrames;
	if (pSeq->AnimType == COLOR_ANIM)
	    pSeq->AnimObj.CurCMap = SetAbsColorMapIndex (
		    CommData.AlienColorMap,
		    ADPtr->StartIndex
		    );
	else
	    pSeq->AnimObj.CurFrame = SetAbsFrameIndex (
		    CommData.AlienFrame,
		    ADPtr->StartIndex
		    );

	if (ADPtr->AnimFlags & RANDOM_ANIM)
	{
	    if (pSeq->AnimType == COLOR_ANIM)
		pSeq->AnimObj.CurCMap =
			SetRelColorMapIndex (pSeq->AnimObj.CurCMap,
			(COUNT)random () % pSeq->FramesLeft);
	    else
		pSeq->AnimObj.CurFrame =
			SetRelFrameIndex (pSeq->AnimObj.CurFrame,
			(COUNT)random () % pSeq->FramesLeft);
	}
	else if (ADPtr->AnimFlags & YOYO_ANIM)
	{
	    --pSeq->FramesLeft;
	    if (pSeq->AnimType == COLOR_ANIM)
		pSeq->AnimObj.CurCMap =
			SetRelColorMapIndex (pSeq->AnimObj.CurCMap, 1);
	    else
		pSeq->AnimObj.CurFrame =
			IncFrameIndex (pSeq->AnimObj.CurFrame);
	}

	pSeq->Alarm =
		ADPtr->BaseRestartRate
		+ ((COUNT)random ()
		% (ADPtr->RandomRestartRate + 1))
		+ 1;
    }
}

PROC(STATIC
void UpdateSpeechGraphics, (Initialize),
    ARG_END	(BOOLEAN	Initialize)
)
{
    CONTEXT	OldContext;

    if (Initialize)
    {
	RECT	r, sr;
	FRAME	f;

	InitOscilloscope (0, 0, RADAR_WIDTH, RADAR_HEIGHT,
		SetAbsFrameIndex (ActivityFrame, 9));
	f = SetAbsFrameIndex (ActivityFrame, 2);
	GetFrameRect (f, &r);
	SetSliderImage (f);
	f = SetAbsFrameIndex (ActivityFrame, 5);
	GetFrameRect (f, &sr);
	InitSlider (0, SLIDER_Y, SIS_SCREEN_WIDTH, sr.extent.height,
		r.extent.width, r.extent.height, f);
    }

    OldContext = SetContext (RadarContext);
    Oscilloscope (!Initialize);
    SetContext (SpaceContext);
    Slider ();
    SetContext (OldContext);
}

PROC(STATIC
void ambient_anim_task, (),
    ARG_VOID
)
{
    SIZE		TalkAlarm;
    FRAME		TalkFrame;
    COUNT		i;
    DWORD		LastTime;
    FRAME		CommFrame;
    register PPRIMITIVE	pBatch;
    SEQUENCE		Sequencer[MAX_ANIMATIONS];
    PSEQUENCE		pSeq;
    ANIMATION_DESCPTR	ADPtr;
    DWORD		ActiveMask;
DWORD	LastOscillTime;

    while ((CommFrame = CommData.AlienFrame) == 0)
	TaskSwitch ();

    SetSemaphore (&GraphicsSem);
    MEMSET ((PSTR)&DisplayArray[0], 0, sizeof (DisplayArray));
    SetUpSequence (Sequencer);
    ClearSemaphore (&GraphicsSem);

    ActiveMask = 0;
    TalkAlarm = 0;
    TalkFrame = 0;
    LastTime = GetTimeCounter ();
LastOscillTime = LastTime;
    for (;;)
    {
	BOOLEAN	Change, CanTalk;
	DWORD	CurTime, ElapsedTicks;

	SleepTask (LastTime + 1);

	CurTime = SetSemaphore (&GraphicsSem);
	ElapsedTicks = CurTime - LastTime;
	LastTime = CurTime;

	Change = FALSE;
	i = CommData.NumAnimations;
	if (CommData.AlienFrame)
	    CanTalk = TRUE;
	else
	{
	    i = 0;
	    CanTalk = FALSE;
	}

	pBatch = &DisplayArray[MAX_ANIMATIONS + 1];
	pSeq = &Sequencer[i];
	ADPtr = &CommData.AlienAmbientArray[i];
	while (i--)
	{
	    --ADPtr;
	    --pSeq;
	    if (ADPtr->AnimFlags & ANIM_DISABLED)
		continue;
	    else if (pSeq->Direction == NO_DIR)
	    {
		if (!(ADPtr->AnimFlags
			& CommData.AlienTalkDesc.AnimFlags & WAIT_TALKING))
		    pSeq->Direction = UP_DIR;
	    }
	    else if ((DWORD)pSeq->Alarm > ElapsedTicks)
		pSeq->Alarm -= (COUNT)ElapsedTicks;
	    else
	    {
		if (!(ActiveMask & ADPtr->BlockMask)
			&& (--pSeq->FramesLeft
			|| ((ADPtr->AnimFlags & YOYO_ANIM)
			&& pSeq->Direction == UP_DIR)))
		{
		    ActiveMask |= 1L << i;
		    pSeq->Alarm = 
			    ADPtr->BaseFrameRate
			    + ((COUNT)random ()
			    % (ADPtr->RandomFrameRate + 1))
			    + 1;
		}
		else
		{
		    ActiveMask &= ~(1L << i);
		    pSeq->Alarm = 
			    ADPtr->BaseRestartRate
			    + ((COUNT)random ()
			    % (ADPtr->RandomRestartRate + 1))
			    + 1;
		    if (ActiveMask & ADPtr->BlockMask)
			continue;
		}

		if (pSeq->AnimType == COLOR_ANIM)
		{
		    XFormPLUT (
			    GetColorMapAddress (pSeq->AnimObj.CurCMap),
			    pSeq->Alarm - 1
			    );
		}
		else
		{
		    Change = TRUE;
		    --pBatch;
		    SetPrimNextLink (pBatch, (pBatch - &DisplayArray[0]) + 1);
		    SetPrimType (pBatch, STAMP_PRIM);
pBatch->Object.Stamp.origin.x = -SAFE_X;
		    pBatch->Object.Stamp.frame = pSeq->AnimObj.CurFrame;
		}

		if (pSeq->FramesLeft == 0)
		{
		    pSeq->FramesLeft = (BYTE)(ADPtr->NumFrames - 1);

		    if (pSeq->Direction == DOWN_DIR)
			pSeq->Direction = UP_DIR;
		    else if (ADPtr->AnimFlags & YOYO_ANIM)
			pSeq->Direction = DOWN_DIR;
		    else
		    {
			++pSeq->FramesLeft;
			if (pSeq->AnimType == COLOR_ANIM)
			    pSeq->AnimObj.CurCMap = SetRelColorMapIndex (
				    pSeq->AnimObj.CurCMap,
				    -pSeq->FramesLeft
				    );
			else
			    pSeq->AnimObj.CurFrame = SetRelFrameIndex (
				    pSeq->AnimObj.CurFrame,
				    -pSeq->FramesLeft
				    );
		    }
		}

		if (ADPtr->AnimFlags & RANDOM_ANIM)
		{
		    if (pSeq->AnimType == COLOR_ANIM)
			pSeq->AnimObj.CurCMap =
				SetAbsColorMapIndex (pSeq->AnimObj.CurCMap,
				ADPtr->StartIndex
				+ ((COUNT)random ()
				% ADPtr->NumFrames));
		    else
			pSeq->AnimObj.CurFrame =
				SetAbsFrameIndex (pSeq->AnimObj.CurFrame,
				ADPtr->StartIndex
				+ ((COUNT)random ()
				% ADPtr->NumFrames));
		}
		else if (pSeq->AnimType == COLOR_ANIM)
		{
		    if (pSeq->Direction == UP_DIR)
			pSeq->AnimObj.CurCMap = SetRelColorMapIndex (
				pSeq->AnimObj.CurCMap, 1
				);
		    else
			pSeq->AnimObj.CurCMap = SetRelColorMapIndex (
				pSeq->AnimObj.CurCMap, -1
				);
		}
		else
		{
		    if (pSeq->Direction == UP_DIR)
			pSeq->AnimObj.CurFrame =
				IncFrameIndex (pSeq->AnimObj.CurFrame);
		    else
			pSeq->AnimObj.CurFrame =
				DecFrameIndex (pSeq->AnimObj.CurFrame);
		}
	    }

	    if (pSeq->AnimType == PICTURE_ANIM
		    && (ADPtr->AnimFlags
		    & CommData.AlienTalkDesc.AnimFlags & WAIT_TALKING)
		    && pSeq->Direction != NO_DIR)
	    {
		COUNT	index;

		CanTalk = FALSE;
		if (!(pSeq->Direction != UP_DIR
			|| (index = GetFrameIndex (pSeq->AnimObj.CurFrame)) >
			ADPtr->StartIndex + 1
			|| (index == ADPtr->StartIndex + 1
			&& (ADPtr->AnimFlags & CIRCULAR_ANIM))))
		    pSeq->Direction = NO_DIR;
	    }
	}

	ADPtr = &CommData.AlienTalkDesc;
	if (CanTalk
		&& ADPtr->NumFrames
		&& (ADPtr->AnimFlags & WAIT_TALKING)
		&& !(CommData.AlienTransitionDesc.AnimFlags & PAUSE_TALKING))
	{
	    if ((long)TalkAlarm > (long)ElapsedTicks)
		TalkAlarm -= (SIZE)ElapsedTicks;
	    else
	    {
		BYTE	AFlags;
		SIZE	FrameRate;

		if (TalkAlarm > 0)
		    TalkAlarm = 0;
		else
		    TalkAlarm = -1;

		AFlags = ADPtr->AnimFlags;
		if (!(AFlags & (TALK_INTRO | TALK_DONE)))
		{
		    FrameRate =
			    ADPtr->BaseFrameRate
			    + ((COUNT)random ()
			    % (ADPtr->RandomFrameRate + 1));
		    if (TalkAlarm < 0
			    || GetFrameIndex (TalkFrame) ==
			    ADPtr->StartIndex)
		    {
			TalkFrame = SetAbsFrameIndex (CommFrame,
				ADPtr->StartIndex + 1
				+ ((COUNT)random ()
				% (ADPtr->NumFrames - 1)));
			FrameRate +=
				ADPtr->BaseRestartRate
				+ ((COUNT)random ()
				% (ADPtr->RandomRestartRate + 1));
		    }
		    else
		    {
			TalkFrame = SetAbsFrameIndex (CommFrame,
				ADPtr->StartIndex);
			if (ADPtr->AnimFlags & PAUSE_TALKING)
			{
			    if (!(CommData.AlienTransitionDesc.AnimFlags
				    & TALK_DONE))
			    {
				CommData.AlienTransitionDesc.AnimFlags |=
					PAUSE_TALKING;
				ADPtr->AnimFlags &=
					~PAUSE_TALKING;
			    }
			    else if (CommData.AlienTransitionDesc.NumFrames)
				ADPtr->AnimFlags |=
					TALK_DONE;
			    else
				ADPtr->AnimFlags &=
					~(WAIT_TALKING | PAUSE_TALKING);

			    FrameRate = 0;
			}
		    }
		}
		else
		{
		    ADPtr = &CommData.AlienTransitionDesc;
		    if (AFlags & TALK_INTRO)
		    {
			FrameRate =
				ADPtr->BaseFrameRate
				+ ((COUNT)random ()
				% (ADPtr->RandomFrameRate + 1));
			if (TalkAlarm < 0)
			    TalkFrame = SetAbsFrameIndex (CommFrame,
				    ADPtr->StartIndex);
			else
			    TalkFrame = IncFrameIndex (TalkFrame);

			if ((BYTE)(GetFrameIndex (TalkFrame)
				- ADPtr->StartIndex + 1) ==
				ADPtr->NumFrames)
			    CommData.AlienTalkDesc.AnimFlags &= ~TALK_INTRO;
		    }
		    else /* if (AFlags & TALK_DONE) */
		    {
			FrameRate =
				ADPtr->BaseFrameRate
				+ ((COUNT)random ()
				% (ADPtr->RandomFrameRate + 1));
			if (TalkAlarm < 0)
			    TalkFrame = SetAbsFrameIndex (CommFrame,
				    ADPtr->StartIndex
				    + ADPtr->NumFrames
				    - 1);
			else
			    TalkFrame = DecFrameIndex (TalkFrame);

			if (GetFrameIndex (TalkFrame) ==
				ADPtr->StartIndex)
			{
			    CommData.AlienTalkDesc.AnimFlags &=
				    ~(PAUSE_TALKING | TALK_DONE);
			    if (ADPtr->AnimFlags & TALK_INTRO)
				CommData.AlienTalkDesc.AnimFlags &= ~WAIT_TALKING;
			    else
			    {
				ADPtr->AnimFlags |=
					PAUSE_TALKING;
				ADPtr->AnimFlags &= ~TALK_DONE;
			    }
			    FrameRate = 0;
			}
		    }
		}

		Change = TRUE;
		--pBatch;
		SetPrimNextLink (pBatch, (pBatch - &DisplayArray[0]) + 1);
		SetPrimType (pBatch, STAMP_PRIM);
pBatch->Object.Stamp.origin.x = -SAFE_X;
		pBatch->Object.Stamp.frame = TalkFrame;

		TalkAlarm = FrameRate;
	    }
	}

if (LastOscillTime + (ONE_SECOND / 32) < CurTime || ColorChange)
{
    DRAW_STATE	OldDrawState;

    OldDrawState = SetContextDrawState (DEST_PIXMAP | DRAW_SUBTRACTIVE); // to force BUILD_FRAME
    BatchGraphics ();
    SetContextDrawState (OldDrawState);
}
	if (Change |= ColorChange)
	{
	    CONTEXT	OldContext;

	    OldContext = SetContext (TaskContext);

	    if (ColorChange)
	    {
		FRAME	F;
		
		F = CommData.AlienFrame;
		CommData.AlienFrame = CommFrame;
		DrawAlienFrame (TalkFrame, &Sequencer[CommData.NumAnimations - 1]);
		CommData.AlienFrame = F;
		ColorChange = FALSE;
	    }
	    else
	    {
		COUNT	StartIndex;

		SetPrimNextLink (&DisplayArray[MAX_ANIMATIONS], END_OF_LIST);

		StartIndex = pBatch - &DisplayArray[0];
		DrawBatch (&DisplayArray[0], StartIndex, 0);
		Change = FALSE;
	    }

	    SetContext (OldContext);
	}
if (LastOscillTime + (ONE_SECOND / 32) < CurTime)
{
    LastOscillTime = CurTime;
    UpdateSpeechGraphics (FALSE);
    Change = TRUE;
}
if (Change)
    UnbatchGraphics ();
	ClearSemaphore (&GraphicsSem);
    }
}

PROC(STATIC
BOOLEAN SpewPhrases, (wait_track),
    ARG_END	(COUNT	wait_track)
)
{
#define RBA_VOLUME		(0x70 << 8)
    BOOLEAN		ContinuityBreak;
    DWORD		TimeIn;
    COUNT		which_track;
    INPUT_STATE		InputState;
    FRAME		F;

    LinearTrackPlayer ();

    TimeIn = GetTimeCounter ();

    ContinuityBreak = FALSE;
    F = CommData.AlienFrame;
    if (wait_track == 0)
    {
	wait_track = which_track = (COUNT)~0;
	InputState = 0;
	goto Rewind;
    }

    if (!(which_track = PlayingTrack ()))
    {
	// initial start of player   
	if (wait_track == 1 || wait_track == ~0)
	{
	    ClearSemaphore (&GraphicsSem);
	    AdvanceTrack (RBA_VOLUME, RBA_VOLUME);
	    ResumeTrack ();
	    do
	    {
		TaskSwitch ();
	    } while (!(which_track = PlayingTrack ()));
	    SetSemaphore (&GraphicsSem);
	}
    }
    else if (which_track <= wait_track)
	ResumeTrack ();

    do
    {
	ClearSemaphore (&GraphicsSem);
	TimeIn = SleepTask (TimeIn + (ONE_SECOND / 64));
#if DEMO_MODE || CREATE_JOURNAL
	InputState = 0;
#else /* !(DEMO_MODE || CREATE_JOURNAL) */
	InputState = GetInputState (NormalInput);
#endif
	if (InputState & DEVICE_EXIT)
	    InputState = ConfirmExit ();

	SetSemaphore (&GraphicsSem);
	if (InputState & (DEVICE_BUTTON2 | DEVICE_EXIT))
	{
	    SetSliderImage (SetAbsFrameIndex (ActivityFrame, 8));
	    JumpTrack ();
	    CommData.AlienFrame = F;
	    return (FALSE);
	}

	if (which_track)
	{
	    if (GetInputXComponent (InputState) > 0)
	    {
		SetSliderImage (SetAbsFrameIndex (ActivityFrame, 3));
		FastForward ();
		ContinuityBreak = TRUE;
		CommData.AlienFrame = 0;
	    }
	    else if (GetInputXComponent (InputState) < 0)
	    {
Rewind:
		SetSliderImage (SetAbsFrameIndex (ActivityFrame, 4));
		FastReverse ();
		ContinuityBreak = TRUE;
		CommData.AlienFrame = 0;
	    }
	    else if (ContinuityBreak)
	    {
		SetSliderImage (SetAbsFrameIndex (ActivityFrame, 2));
		if ((which_track = PlayingTrack ())
			&& which_track <= wait_track)
		    ResumeTrack ();
		ContinuityBreak = FALSE;
	    }
	    else if (which_track == wait_track
		    || wait_track == (COUNT)~0)
		CommData.AlienFrame = F;
	}
    } while (ContinuityBreak
	    || ((which_track = PlayingTrack ()) && which_track <= wait_track));

    CommData.AlienFrame = F;

    if (wait_track == (COUNT)~0)
	SetSliderImage (SetAbsFrameIndex (ActivityFrame, 8));

    return (TRUE);
}

PROC(
void AlienTalkSegue, (wait_track),
    ARG_END	(COUNT	wait_track)
)
{
    if ((GLOBAL (CurrentActivity) & CHECK_ABORT)
	    || (CommData.AlienTransitionDesc.AnimFlags & TALK_INTRO))
	return;

    SetSemaphore (&GraphicsSem);

    if (!pCurInputState->Initialized)
    {
	SetColorMap (GetColorMapAddress (CommData.AlienColorMap));
	DrawAlienFrame (CommData.AlienFrame, NULL_PTR);
UpdateSpeechGraphics (TRUE);

	if (LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE
		|| (!GET_GAME_STATE (PLAYER_HYPNOTIZED)
		&& !GET_GAME_STATE (CHMMR_EMERGING)
		&& GET_GAME_STATE (CHMMR_BOMB_STATE) != 2
		&& (pMenuState == 0 || !GET_GAME_STATE (MOONBASE_ON_SHIP)
		|| GET_GAME_STATE (PROBE_ILWRATH_ENCOUNTER))))
	{
RECT	r;
    
if (pMenuState == 0)
{
    r.corner.x = SIS_ORG_X;
    r.corner.y = SIS_ORG_Y;
    r.extent.width = SIS_SCREEN_WIDTH;
    r.extent.height = SIS_SCREEN_HEIGHT;
    ScreenTransition (3, &r);
}
else
{
    r.corner.x = 0;
    r.corner.y = 0;
    r.extent.width = SCREEN_WIDTH;
    r.extent.height = SCREEN_HEIGHT;
    ScreenTransition (3, &r);
}
UnbatchGraphics ();
	}
	else
	{
	    BYTE	clut_buf[] = {FadeAllToColor};

UnbatchGraphics ();
	    if (GET_GAME_STATE (MOONBASE_ON_SHIP)
		    || GET_GAME_STATE (CHMMR_BOMB_STATE) == 2)
		XFormColorMap ((COLORMAPPTR)clut_buf, ONE_SECOND * 2);
	    else if (GET_GAME_STATE (CHMMR_EMERGING))
		XFormColorMap ((COLORMAPPTR)clut_buf, ONE_SECOND * 2);
	    else
		XFormColorMap ((COLORMAPPTR)clut_buf, ONE_SECOND * 5);
	}
	pCurInputState->Initialized = TRUE;

	SetMasterVolume (BACKGROUND_VOL);
	PlayMusic ((MUSIC_REF)CommData.AlienSong, TRUE, 1);

	{
	    DWORD	TimeOut;

	    TimeOut = GetTimeCounter () + (ONE_SECOND >> 1);
/*	    if (CommData.NumAnimations) */
		pCurInputState->AnimTask = AddTask (ambient_anim_task, 3072);

	    ClearSemaphore (&GraphicsSem);
	    SleepTask (TimeOut);
	    SetSemaphore (&GraphicsSem);
	}

	LastActivity &= ~CHECK_LOAD;
    }

if (wait_track == (COUNT)~0 || CommData.AlienTalkDesc.NumFrames)
{
    if (!(CommData.AlienTransitionDesc.AnimFlags & TALK_INTRO))
    {
	CommData.AlienTransitionDesc.AnimFlags |= TALK_INTRO;
	if (CommData.AlienTransitionDesc.NumFrames)
	    CommData.AlienTalkDesc.AnimFlags |= TALK_INTRO;
    }
		    
    CommData.AlienTransitionDesc.AnimFlags &= ~PAUSE_TALKING;
    if (CommData.AlienTalkDesc.NumFrames)
	CommData.AlienTalkDesc.AnimFlags |= WAIT_TALKING;

    while (CommData.AlienTalkDesc.AnimFlags & TALK_INTRO)
    {
	ClearSemaphore (&GraphicsSem);
	TaskSwitch ();
	SetSemaphore (&GraphicsSem);
    }
}

    if (!SpewPhrases (wait_track) || wait_track == (COUNT)~0)
	FadeSound (FOREGROUND_VOL, ONE_SECOND);
    else
	CommData.AlienTransitionDesc.AnimFlags &= ~TALK_INTRO;

if (wait_track == (COUNT)~0 || CommData.AlienTalkDesc.NumFrames)
{
    CommData.AlienTransitionDesc.AnimFlags |= TALK_DONE;
    if (CommData.AlienTalkDesc.AnimFlags & WAIT_TALKING)
	CommData.AlienTalkDesc.AnimFlags |= PAUSE_TALKING;
}

    ClearSemaphore (&GraphicsSem);

    do
	TaskSwitch ();
    while (CommData.AlienTalkDesc.AnimFlags & PAUSE_TALKING);
}

PROC(STATIC
BOOLEAN	DoCommunication, (InputState, pES),
    ARG		(INPUT_STATE		InputState)
    ARG_END	(PENCOUNTER_STATE	pES)
)
{
    if (!(CommData.AlienTransitionDesc.AnimFlags & (TALK_INTRO | TALK_DONE)))
	AlienTalkSegue ((COUNT)~0);

    if (GLOBAL (CurrentActivity) & CHECK_ABORT)
	;
    else if (pES->num_responses == 0)
    {
	DWORD	TimeIn, TimeOut;

	TimeOut = FadeSound (0, ONE_SECOND * 3) + 2;
	TimeIn = GetTimeCounter ();
	do
	{
	    TimeIn = SleepTask (TimeIn + 1);
	    if (GetInputXComponent (GetInputState (NormalInput)) < 0)
	    {
		FadeSound (BACKGROUND_VOL, ONE_SECOND);
		SetSemaphore (&GraphicsSem);
		SpewPhrases (0);
		ClearSemaphore (&GraphicsSem);
		if (GLOBAL (CurrentActivity) & CHECK_ABORT)
		    break;
		TimeOut = FadeSound (0, ONE_SECOND * 2) + 2;
		TimeIn = GetTimeCounter ();
	    }
	} while (TimeIn <= TimeOut);
    }
    else
    {
	BYTE	response;

	if (pES->top_response == (BYTE)~0)
	{
	    pES->top_response = 0;
	    SetSemaphore (&GraphicsSem);
	    RefreshResponses (pES);
	    ClearSemaphore (&GraphicsSem);
	}

	if (InputState & DEVICE_BUTTON1)
	{
	    pES->phrase_buf_index =
		    pES->response_list[pES->cur_response].response_text.CharCount;
	    MEMCPY (
		    &pES->phrase_buf[0],
		    pES->response_list[pES->cur_response].response_text.lpStr,
		    pES->phrase_buf_index
		    );
	    pES->phrase_buf[pES->phrase_buf_index++] = '\0';

	    SetSemaphore (&GraphicsSem);
	    FeedbackPlayerPhrase ((LPBYTE)pES->phrase_buf);
	    StopTrack ();
	    SetSliderImage (SetAbsFrameIndex (ActivityFrame, 2));
	    ClearSemaphore (&GraphicsSem);

	    FadeSound (BACKGROUND_VOL, ONE_SECOND);

	    CommData.AlienTransitionDesc.AnimFlags &= ~(TALK_INTRO | TALK_DONE);
	    pES->num_responses = 0;
	    (*pES->response_list[pES->cur_response].response_func)
		    (pES->response_list[pES->cur_response].response_ref);
	}
	else
	{
	    response = pES->cur_response;
	    if (GetInputXComponent (InputState) < 0)
	    {
		FadeSound (BACKGROUND_VOL, ONE_SECOND);
		SetSemaphore (&GraphicsSem);
		FeedbackPlayerPhrase ((LPBYTE)pES->phrase_buf);
		SpewPhrases (0);
		if (!(GLOBAL (CurrentActivity) & CHECK_ABORT))
		{
		    RefreshResponses (pES);
		    FadeSound (FOREGROUND_VOL, ONE_SECOND);
		}
		ClearSemaphore (&GraphicsSem);
	    }
	    else if (GetInputYComponent (InputState) < 0)
		response = (BYTE)((response + (BYTE)(pES->num_responses - 1))
			% pES->num_responses);
	    else if (GetInputXComponent (InputState) > 0
		    || GetInputYComponent (InputState) > 0)
		response = (BYTE)((BYTE)(response + 1)
			% pES->num_responses);

	    if (response != pES->cur_response)
	    {
		COORD	y;

		SetSemaphore (&GraphicsSem);
		BatchGraphics ();
		add_text (-2, &pES->response_list[pES->cur_response].response_text);

		pES->cur_response = response;

		y = add_text (-1, &pES->response_list[pES->cur_response].response_text);
		if (response < pES->top_response)
		{
		    pES->top_response = 0;
		    RefreshResponses (pES);
		}
		else if (y > SIS_SCREEN_HEIGHT)
		{
		    pES->top_response = response;
		    RefreshResponses (pES);
		}
		UnbatchGraphics ();
		ClearSemaphore (&GraphicsSem);
	    }
	}
	
	return (TRUE);
    }

    SetSemaphore (&GraphicsSem);

    if (pES->AnimTask)
    {
	DeleteTask (pES->AnimTask);
	pES->AnimTask = 0;
    }
    CommData.AlienTransitionDesc.AnimFlags &= ~(TALK_INTRO | TALK_DONE);

    SetContext (SpaceContext);
    DestroyContext (ReleaseContext (TaskContext));
    TaskContext = 0;

    ClearSemaphore (&GraphicsSem);

    FlushPLUTXForms ();
    ColorChange = FALSE;

    FlushColorXForms ();
    StopMusic ();
    StopSound ();
    StopTrack ();
    SleepTask (FadeSound (FOREGROUND_VOL, 0) + 2);

    return (FALSE);
}

PROC(
void DoResponsePhrase, (R, response_func, ConstructStr),
    ARG		(RESPONSE_REF	R)
    ARG		(RESPONSE_FUNC	response_func)
    ARG_END	(char		*ConstructStr)
)
{
    PENCOUNTER_STATE	pES = pCurInputState;

    if (GLOBAL (CurrentActivity) & CHECK_ABORT)
	return;
	    
    if (pES->num_responses == 0)
    {
	pES->cur_response = 0;
	pES->top_response = (BYTE)~0;
    }

    pES->response_list[pES->num_responses].response_ref = R;
    if (pES->response_list[pES->num_responses].response_text.lpStr = (LPBYTE)ConstructStr)
	pES->response_list[pES->num_responses].response_text.CharCount = strlen (ConstructStr);
    else
    {
	STRING	locString;
	
	locString = SetAbsStringTableIndex (CommData.PlayerPhrases, R - 1);
	pES->response_list[pES->num_responses].response_text.lpStr =
		(LPBYTE)GetStringAddress (locString);
	pES->response_list[pES->num_responses].response_text.CharCount =
		GetStringLength (locString);
//#define BVT_PROBLEM
#ifdef BVT_PROBLEM
if (pES->response_list[pES->num_responses].response_text.lpStr[
	pES->response_list[pES->num_responses].response_text.CharCount - 1
	] == '\0')
    --pES->response_list[pES->num_responses].response_text.CharCount;
#endif /* BVT_PROBLEM */
    }
    pES->response_list[pES->num_responses].response_func = response_func;
    ++pES->num_responses;
}

PROC(STATIC
void HailAlien, (),
    ARG_VOID
)
{
    MEM_HANDLE		hOldIndex;
    ENCOUNTER_STATE	ES;
    FONT		PlayerFont, OldFont;

    pCurInputState = &ES;
    MEMSET (pCurInputState, 0, sizeof (*pCurInputState));

    ES.InputFunc = DoCommunication;
    hOldIndex = SetResourceIndex (hResIndex);
    PlayerFont = CaptureFont (LoadGraphic (PLAYER_FONT));
    SetResourceIndex (hOldIndex);

    CommData.AlienFrame = CaptureDrawable (
	    LoadGraphicInstance ((RESOURCE)CommData.AlienFrame)
	    );
    CommData.AlienColorMap = CaptureColorMap (
	    LoadColorMapInstance ((RESOURCE)CommData.AlienColorMap)
	    );
    CommData.AlienSong = LoadMusicInstance ((RESOURCE)CommData.AlienSong);
    CommData.PlayerPhrases = CaptureStringTable (
	    LoadStringTableInstance ((RESOURCE)CommData.PlayerPhrases)
	    );

    ES.phrase_buf_index = 1;
    ES.phrase_buf[0] = '\0';

    SetContext (SpaceContext);
    SetContextDrawState (DEST_PIXMAP | DRAW_REPLACE);
    OldFont = SetContextFont (PlayerFont);

    {
	RECT	r;
DRAW_STATE	OldDrawState;

	TaskContext = CaptureContext (CreateContext ());
	SetContext (TaskContext);
	SetContextFGFrame (Screen);
	GetFrameRect (CommData.AlienFrame, &r);
	r.corner.y = SIS_ORG_Y;
	r.extent.width = SIS_SCREEN_WIDTH;

OldDrawState = SetContextDrawState (DEST_PIXMAP | DRAW_SUBTRACTIVE);
BatchGraphics ();
SetContextDrawState (OldDrawState);
	if (LOBYTE (GLOBAL (CurrentActivity)) == WON_LAST_BATTLE)
	{
	    r.corner.x = (SCREEN_WIDTH - SIS_SCREEN_WIDTH) >> 1;
	    SetContextClipRect (&r);
	}
	else
	{
	    r.corner.x = SIS_ORG_X;
	    SetContextClipRect (&r);

	    if (pMenuState == 0)
	    {
		RepairSISBorder ();
		ClearSemaphore (&GraphicsSem);
		DrawMenuStateStrings ((BYTE)~0, (BYTE)~0);
		SetSemaphore (&GraphicsSem);
	    }
	    else	/* in starbase */
	    {
		ClearSemaphore (&GraphicsSem);
		DrawSISFrame ();
		SetSemaphore (&GraphicsSem);
		if (GET_GAME_STATE (STARBASE_AVAILABLE))
		{
		    DrawSISMessage ((LPSTR)starbase_str_array[TALK_COMMANDER]);
		    DrawSISTitle ((PBYTE)"Starbase");
		}
		else
		{
		    DrawSISMessage (NULL_PTR);
		    DrawSISTitle ((PBYTE)GLOBAL_SIS (PlanetName));
		}
	    }
	}

	DrawSISComWindow ();
    }

    ClearSemaphore (&GraphicsSem);

    LastActivity |= CHECK_LOAD;		/* prevent spurious input */
    (*CommData.init_encounter_func) ();
    DoInput ((PVOID)&ES);
    if (!(GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD)))
	(*CommData.uninit_encounter_func) ();

    SetSemaphore (&GraphicsSem);

    DestroyStringTable (ReleaseStringTable (CommData.PlayerPhrases));
    DestroyMusic ((MUSIC_REF)CommData.AlienSong);
    DestroyColorMap (ReleaseColorMap (CommData.AlienColorMap));
    DestroyDrawable (ReleaseDrawable (CommData.AlienFrame));

    SetContext (SpaceContext);
    SetContextFont (OldFont);
    DestroyFont (ReleaseFont (PlayerFont));

    CommData.PlayerPhrases = 0;
    pCurInputState = 0;
}

PROC(
COUNT InitCommunication, (which_comm),
    ARG_END	(RESOURCE	which_comm)
)
{
    COUNT	status;
    MEM_HANDLE	hOldIndex, hIndex;
    LOCDATAPTR	LocDataPtr;
    
    SetSemaphore (&GraphicsSem);

    if (LastActivity & CHECK_LOAD)
    {
	LastActivity &= ~CHECK_LOAD;
	if (which_comm != COMMANDER_CONVERSATION)
	{
	    if (LOBYTE (LastActivity) == 0)
	    {
		ClearSemaphore (&GraphicsSem);
		DrawSISFrame ();
		SetSemaphore (&GraphicsSem);
	    }
	    else
	    {
		ClearSISRect (DRAW_SIS_DISPLAY);
		RepairSISBorder ();
	    }
	    DrawSISMessage (NULL_PTR);
	    if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
		DrawHyperCoords (
			GLOBAL (ShipStamp.origin)
			);
	    else if (HIWORD (GLOBAL (ShipStamp.frame)) == 0)
		DrawHyperCoords (CurStarDescPtr->star_pt);
	    else
		DrawSISTitle ((PBYTE)GLOBAL_SIS (PlanetName));
	}
    }

    if (which_comm == 0)
	status = URQUAN_PROBE_SHIP, which_comm = URQUAN_CONVERSATION;
    else
    {
	if (which_comm == YEHAT_REBEL_CONVERSATION)
	    status = YEHAT_REBEL_SHIP, which_comm = YEHAT_CONVERSATION;
	else if ((status = GET_PACKAGE (which_comm)
		- GET_PACKAGE (ARILOU_CONVERSATION)
		+ ARILOU_SHIP) >= YEHAT_REBEL_SHIP)
	    status = HUMAN_SHIP;	/* conversation exception, set to self */
	ActivateStarShip (status, SPHERE_TRACKING);

	if (which_comm == ORZ_CONVERSATION
		|| (which_comm == TALKING_PET_CONVERSATION
		&& (!GET_GAME_STATE (TALKING_PET_ON_SHIP)
		|| LOBYTE (GLOBAL (CurrentActivity)) == IN_LAST_BATTLE))
		|| (which_comm != CHMMR_CONVERSATION
		&& which_comm != SYREEN_CONVERSATION
		))//&& (ActivateStarShip (status, CHECK_ALLIANCE) & BAD_GUY)))
	    BuildBattle (1);
    }

    hOldIndex = SetResourceIndex (hResIndex);
    if ((hIndex = OpenResourceIndexInstance (which_comm)) == 0)
    {
	SET_GAME_STATE (BATTLE_SEGUE, 1);
	LocDataPtr = 0;
    }
    else
    {
	SetResourceIndex (hIndex);

	if (LocDataPtr = (LOCDATAPTR)init_race (
		status != YEHAT_REBEL_SHIP ? which_comm : YEHAT_REBEL_CONVERSATION
		))
	    CommData = *LocDataPtr;
    }

    ClearSemaphore (&GraphicsSem);

    if (GET_GAME_STATE (BATTLE_SEGUE) == 0)
	status = HAIL;
    else if ((status = InitEncounter ()) == HAIL && LocDataPtr)
    {
	SET_GAME_STATE (BATTLE_SEGUE, 0);
    }
    else
    {
	status = ATTACK;
	SET_GAME_STATE (BATTLE_SEGUE, 1);
    }

    SetSemaphore (&GraphicsSem);

    if (status == HAIL)
	HailAlien ();
    else if (LocDataPtr)
	(*CommData.uninit_encounter_func) ();

    SetResourceIndex (hOldIndex);
    CloseResourceIndex (hIndex);

    ClearSemaphore (&GraphicsSem);

    status = 0;
    if (!(GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD)))
    {
	if (LOBYTE (GLOBAL (CurrentActivity)) == IN_LAST_BATTLE
		&& (GLOBAL (glob_flags) & CYBORG_ENABLED))
	    ReinitQueue (&GLOBAL (npc_built_ship_q));

	SET_GAME_STATE (GLOBAL_FLAGS_AND_DATA, 0);
	if (status = (GET_GAME_STATE (BATTLE_SEGUE)
		&& GetHeadLink (&GLOBAL (npc_built_ship_q))))
	{
	    BuildBattle (0);
	    EncounterBattle ();
	}
	else
	{
	    SET_GAME_STATE (BATTLE_SEGUE, 0);
	}
    }

    UninitEncounter ();

    return (status);
}

PROC(
void RaceCommunication, (),
    ARG_VOID
)
{
    COUNT		i, status;
    HSTARSHIP		hStarShip;
    SHIP_FRAGMENTPTR	FragPtr;
    RESOURCE		RaceComm[] =
    {
	RACE_COMMUNICATION
    };
    extern ACTIVITY	NextActivity;

    if (LOBYTE (GLOBAL (CurrentActivity)) == IN_LAST_BATTLE)
    {
	ReinitQueue (&GLOBAL (npc_built_ship_q));
	CloneShipFragment (SAMATRA_SHIP, &GLOBAL (npc_built_ship_q), 0);
	InitCommunication (TALKING_PET_CONVERSATION);
	if (!(GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD))
		&& GLOBAL_SIS (CrewEnlisted) != (COUNT)~0)
	{
	    GLOBAL (CurrentActivity) = WON_LAST_BATTLE;
	}
	return;
    }
	/* going into talking pet conversation */
    else if (NextActivity & CHECK_LOAD)
    {
	BYTE	ec;

	ec = GET_GAME_STATE (ESCAPE_COUNTER);

	if (GET_GAME_STATE (FOUND_PLUTO_SPATHI) == 1)
	    InitCommunication (SPATHI_CONVERSATION);
	else if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) == 0)
	    InitCommunication (TALKING_PET_CONVERSATION);
	else if (GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & ((1 << 4) | (1 << 5)))
	    InitCommunication (ILWRATH_CONVERSATION);
	else
	    InitCommunication (CHMMR_CONVERSATION);
 	if (GLOBAL_SIS (CrewEnlisted) != (COUNT)~0)
	{
	    NextActivity = GLOBAL (CurrentActivity) & ~START_ENCOUNTER;
	    if (LOBYTE (NextActivity) == IN_INTERPLANETARY)
		NextActivity |= START_INTERPLANETARY;
	    GLOBAL (CurrentActivity) |= CHECK_LOAD;	/* fake a load game */
	}

	SET_GAME_STATE (ESCAPE_COUNTER, ec);
	return;
    }
    else if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
    {
	ReinitQueue (&GLOBAL (npc_built_ship_q));
	if (GET_GAME_STATE (ARILOU_SPACE_SIDE) >= 2)
	{
	    InitCommunication (ARILOU_CONVERSATION);
	    return;
	}
	else
	{
	    COUNT		NumShips;
	    HENCOUNTER		hEncounter;
	    ENCOUNTERPTR	EncounterPtr;

	    hEncounter = GetHeadEncounter ();
	    LockEncounter (hEncounter, &EncounterPtr);

	    NumShips = LONIBBLE (EncounterPtr->SD.Index);
	    for (i = 0; i < NumShips; ++i)
	    {
		CloneShipFragment (
			EncounterPtr->SD.Type,
			&GLOBAL (npc_built_ship_q), 0
			);
	    }

	    CurStarDescPtr = (STAR_DESCPTR)&EncounterPtr->SD;
	    UnlockEncounter (hEncounter);
	}
    }

    hStarShip = GetHeadLink (&GLOBAL (npc_built_ship_q));
    FragPtr = (SHIP_FRAGMENTPTR)LockStarShip (
	    &GLOBAL (npc_built_ship_q), hStarShip
	    );
    i = GET_RACE_ID (FragPtr);
    UnlockStarShip (
	    &GLOBAL (npc_built_ship_q), hStarShip
	    );

   status = InitCommunication (RaceComm[i]);

    if (GLOBAL (CurrentActivity) & (CHECK_ABORT | CHECK_LOAD))
	return;

    if (i == CHMMR_SHIP)
	ReinitQueue (&GLOBAL (npc_built_ship_q));

    if (LOBYTE (GLOBAL (CurrentActivity)) == IN_INTERPLANETARY)
    {
		/* if used destruct code in interplanetary */
	if (i == SLYLANDRO_SHIP && status == 0)
	    ReinitQueue (&GLOBAL (npc_built_ship_q));
    }
    else
    {
	PEXTENDED_STAR_DESC	pESD;

	if (pESD = (PEXTENDED_STAR_DESC)CurStarDescPtr)
	{
	    BYTE	i, NumShips;

	    NumShips = (BYTE)CountLinks (&GLOBAL (npc_built_ship_q));
	    pESD->Index = MAKE_BYTE (
		    NumShips, HINIBBLE (pESD->Index)
		    );
	    pESD->Index |= ENCOUNTER_REFORMING;
	    if (status == 0)
		pESD->Index |= ONE_SHOT_ENCOUNTER;

	    for (i = 0; i < NumShips; ++i)
	    {
		HSTARSHIP		hStarShip;
		SHIP_FRAGMENTPTR	TemplatePtr;

		hStarShip = GetStarShipFromIndex (
			&GLOBAL (npc_built_ship_q), i
			);
		TemplatePtr = (SHIP_FRAGMENTPTR)LockStarShip (
			&GLOBAL (npc_built_ship_q), hStarShip
			);
		pESD->ShipList[i] = TemplatePtr->ShipInfo;
		pESD->ShipList[i].var1 = GET_RACE_ID (TemplatePtr);
		UnlockStarShip (
			&GLOBAL (npc_built_ship_q), hStarShip
			);
	    }

	    ReinitQueue (&GLOBAL (npc_built_ship_q));
	    CurStarDescPtr = 0;
	}
    }
}
