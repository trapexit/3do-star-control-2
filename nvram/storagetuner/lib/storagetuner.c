/* $Id: $ */


/****************************************************************************/


/* Copyright (C) 1994, The 3DO Company.
 * All Rights Reserved
 * Confidential and Proprietary
 */


/****************************************************************************/


#include "types.h"
#include "filesystem.h"
#include "filefunctions.h"
#include "st.h"
#include "storagetuner.h"


/*****************************************************************************/


Err StorageTunerRequest(Item screenGroup, List *memoryLists,
                        TagArg *args)
{
Err        err;
CodeHandle code;
STParms    stp;

    if (args)
        return (STORAGETUNER_ERR_BADTAG);

    stp.stp_ScreenGroup = screenGroup;
    stp.stp_MemoryLists = memoryLists;

    err = LoadCode("$tuner/storagetuner", &code);
    if (err >= 0)
    {
        err = ExecuteAsSubroutine(code, 0, (char **)&stp);
        UnloadCode(code);
    }

    return (err);
}
