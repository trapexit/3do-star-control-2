#include "types.h"
#include "nodes.h"
#include "task.h"
#include "kernel.h"
#include "audio.h"
#include "operror.h"
#include "Portfolio.h"
#include "Init3DO.h"
#include "Parse3DO.h"
#include "Utils3DO.h"
#include "filefunctions.h"
#include "event.h"
#define DEBUG
#include "debug.h"

main ()
{
while (!ReadControlPad (0));
printf ("0 -- 0x%08x, 0x%08x\n", _MathBase, &_MathBase);
    OpenMathFolio ();
printf ("1 -- 0x%08x, 0x%08x\n", _MathBase, &_MathBase);
}
