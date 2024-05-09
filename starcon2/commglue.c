#include "starcon.h"
#include "commglue.h"

char aiff_folder[60];

PROC(
void NPCPhrase, (index),
    ARG_END	(COUNT	index)
)
{
    char	aifname[80];

    sprintf (aifname, "starcon2/%s%03u.abx", aiff_folder, index - 1);
#ifdef TESTING
printf ("'%s'\n", aifname);
#endif /* TESTING */
    SpliceTrack (aifname);
}

//#define BVT_PROBLEM

PROC(
void GetAllianceName, (buf, name_1),
    ARG		(char		*buf)
    ARG_END	(RESPONSE_REF	name_1)
)
{
    COUNT	i;
    STRING	S;

    i = GET_GAME_STATE (NEW_ALLIANCE_NAME);
    S = SetAbsStringTableIndex (CommData.PlayerPhrases, (name_1 - 1) + i);
    GetStringContents (S, (STRINGPTR)buf, FALSE);
    if (i == 3)
    {
	i = strlen (buf);
	buf += i;
#ifdef BVT_PROBLEM
if (i && buf[-1] == '\0') --buf;
#endif /* BVT_PROBLEM */
	strcpy (buf, GLOBAL_SIS (CommanderName));
	i = strlen (buf);
	buf += i;
	GetStringContents (SetRelStringTableIndex (S, 1), (STRINGPTR)buf, FALSE);
    }
}

PROC(
void construct_response, (buf, R, c_args),
    ARG		(char		*buf)
    ARG		(RESPONSE_REF	R)
    ARG		(vararg_dcl	c_args)
    ARG_VAR
)
{
    char	*name;
    vararg_list	vlist;

    vararg_start (vlist, c_args);
    do
    {
	COUNT	len;
	STRING	S;
	
	S = SetAbsStringTableIndex (CommData.PlayerPhrases, R - 1);
	len = GetStringLength (S);
	strncpy (buf, (char *)GetStringAddress (S), len);
	buf += len;
#ifdef BVT_PROBLEM
if (len && buf[-1] == '\0') --buf;
#endif /* BVT_PROBLEM */
	if (name = vararg_val (vlist, char *))
	{
	    len = strlen (name);
	    strncpy (buf, name, len);
	    buf += len;

	    if ((R = vararg_val (vlist, RESPONSE_REF)) == (RESPONSE_REF)-1)
		name = 0;
	}
    } while (name);
    vararg_end (vlist);
    
    *buf = '\0';
}

PROC(
LOCDATAPTR init_race, (comm_id),
    ARG_END	(RESOURCE	comm_id)
)
{
	switch (comm_id)
	{
		case ARILOU_CONVERSATION:
			return ((LOCDATAPTR)init_arilou_comm ());
		case BLACKURQ_CONVERSATION:
			return ((LOCDATAPTR)init_blackurq_comm ());
		case CHMMR_CONVERSATION:
			return ((LOCDATAPTR)init_chmmr_comm ());
		case COMMANDER_CONVERSATION:
			if (!GET_GAME_STATE (STARBASE_AVAILABLE))
				return ((LOCDATAPTR)init_commander_comm ());
			else
				return ((LOCDATAPTR)init_starbase_comm ());
		case DRUUGE_CONVERSATION:
			return ((LOCDATAPTR)init_druuge_comm ());
		case ILWRATH_CONVERSATION:
			return ((LOCDATAPTR)init_ilwrath_comm ());
		case MELNORME_CONVERSATION:
			return ((LOCDATAPTR)init_melnorme_comm ());
		case MYCON_CONVERSATION:
			return ((LOCDATAPTR)init_mycon_comm ());
		case ORZ_CONVERSATION:
			return ((LOCDATAPTR)init_orz_comm ());
		case PKUNK_CONVERSATION:
			return ((LOCDATAPTR)init_pkunk_comm ());
		case SHOFIXTI_CONVERSATION:
			return ((LOCDATAPTR)init_shofixti_comm ());
		case SLYLANDRO_CONVERSATION:
			return ((LOCDATAPTR)init_slyland_comm ());
		case SLYLANDRO_HOME_CONVERSATION:
			return ((LOCDATAPTR)init_slylandro_comm ());
		case SPATHI_CONVERSATION:
			if (!(GET_GAME_STATE (GLOBAL_FLAGS_AND_DATA) & (1 << 7)))
				return ((LOCDATAPTR)init_spathi_comm ());
			else
				return ((LOCDATAPTR)init_spahome_comm ());
		case SUPOX_CONVERSATION:
			return ((LOCDATAPTR)init_supox_comm ());
		case SYREEN_CONVERSATION:
			return ((LOCDATAPTR)init_syreen_comm ());
		case TALKING_PET_CONVERSATION:
			return ((LOCDATAPTR)init_talkpet_comm ());
		case THRADD_CONVERSATION:
			return ((LOCDATAPTR)init_thradd_comm ());
		case UMGAH_CONVERSATION:
			return ((LOCDATAPTR)init_umgah_comm ());
		case URQUAN_CONVERSATION:
			return ((LOCDATAPTR)init_urquan_comm ());
		case UTWIG_CONVERSATION:
			return ((LOCDATAPTR)init_utwig_comm ());
		case VUX_CONVERSATION:
			return ((LOCDATAPTR)init_vux_comm ());
		case YEHAT_REBEL_CONVERSATION:
			return ((LOCDATAPTR)init_rebel_yehat_comm ());
		case YEHAT_CONVERSATION:
			return ((LOCDATAPTR)init_yehat_comm ());
		case ZOQFOTPIK_CONVERSATION:
			return ((LOCDATAPTR)init_zoqfot_comm ());
		default:
			return ((LOCDATAPTR)init_chmmr_comm ());
	}
}

