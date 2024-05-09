#ifndef _DEMO_H
#define _DEMO_H

#ifndef DEMO_MODE
#define DEMO_MODE	0
#endif /* DEMO_MODE */
#ifndef CREATE_JOURNAL
#define CREATE_JOURNAL	0
#endif /* CREATE_JOURNAL */

PROC_GLOBAL(
INPUT_STATE demo_input, (InputRef, InputState),
    ARG		(INPUT_REF	InputRef)
    ARG_END	(INPUT_STATE	InputState)
);

extern INPUT_REF	DemoInput;

#if !(DEMO_MODE || CREATE_JOURNAL)

#define OpenJournal	SeedRandomNumbers
#define CloseJournal()	TRUE
#define JournalInput(is)

#else

PROC_GLOBAL(
void OpenJournal, (),
    ARG_VOID
);
PROC_GLOBAL(
BOOLEAN CloseJournal, (),
    ARG_VOID
);
#if !CREATE_JOURNAL
#define JournalInput(is)
#else /* CREATE_JOURNAL */
PROC_GLOBAL(
void JournalInput, (InputState),
    ARG_END	(INPUT_STATE	InputState)
);
#endif /* CREATE_JOURNAL */

#endif

#endif /* _DEMO_H */

