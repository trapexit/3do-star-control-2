#ifndef _COMMGLUE_H
#define _COMMGLUE_H

extern LOCDATA	CommData;
extern char	shared_phrase_buf[256];
extern char	aiff_folder[];

#define PLAYER_SAID(r,i)	((r)==(i))
#define PHRASE_ENABLED(p)	\
	(*GetStringAddress ( \
		SetAbsStringTableIndex (CommData.PlayerPhrases, (p)-1) \
		) != '\0')
#define DISABLE_PHRASE(p)	\
	(*GetStringAddress ( \
		SetAbsStringTableIndex (CommData.PlayerPhrases, (p)-1) \
		) = '\0')
#define RESPONSE_TO_REF(R)	(R)

#define Response(i,a)		\
	DoResponsePhrase(i,(RESPONSE_FUNC)a,0)

typedef COUNT	RESPONSE_REF;

typedef PROC_PARAMETER(
	void (*RESPONSE_FUNC), (R),
	    ARG_END	(RESPONSE_REF	R)
	);

PROC_GLOBAL(
void DoResponsePhrase, (R, response_func, ConstructStr),
    ARG		(RESPONSE_REF	R)
    ARG		(RESPONSE_FUNC	response_func)
    ARG_END	(char		*ContstructStr)
);
PROC_GLOBAL(
void DoNPCPhrase, (lpStr),
    ARG_END	(LPBYTE	lpStr)
);

PROC_GLOBAL(
void NPCPhrase, (index),
    ARG_END	(COUNT index)
);

PROC_GLOBAL(
void GetAllianceName, (buf, name_1),
    ARG		(char		*buf)
    ARG_END	(RESPONSE_REF	name_1)
);

PROC_GLOBAL(
void construct_response, (buf, R, c_args),
    ARG		(char		*buf)
    ARG		(RESPONSE_REF	R)
    ARG		(vararg_dcl	c_args)
    ARG_VAR
);

PROC_GLOBAL(
LOCDATAPTR init_race, (comm_id),
    ARG_END	(RESOURCE	comm_id)
);

PROC_GLOBAL(
LOCDATAPTR init_arilou_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_blackurq_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_chmmr_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_commander_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_druuge_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_ilwrath_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_melnorme_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_mycon_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_orz_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_pkunk_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_rebel_yehat_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_shofixti_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_slyland_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_slylandro_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_spahome_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_spathi_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_starbase_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_supox_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_syreen_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_talkpet_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_thradd_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_umgah_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_urquan_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_utwig_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_vux_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_yehat_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_zoqfot_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
LOCDATAPTR init_umgah_comm, (),
    ARG_VOID
);

PROC_GLOBAL(
DWORD XFormPLUT, (ColorMapPtr, TimeInterval),
    ARG		(COLORMAPPTR	ColorMapPtr)
    ARG_END	(SIZE		TimeInterval)
);

#endif /* _COMMGLUE_H */

