#include "Portfolio.h"
#include "Init3DO.h"
#include "Parse3DO.h"
#include "Utils3DO.h"
#include "filefunctions.h"
#include "BlockFile.h"

int32	d[6] =
{
    7, 18, 25, 25, 18, 7
};

main (int argc, char **argv)
{
    int32	x, i, dx, n, err, b;
    
    dx = atoi (*(argv + 1));
    x = 0;
    i = 0;
    
    n = 6;
	
    err = 100;
    do
    {
	int32	j;
	
	j = dx * d[i];
	x += j / 100;
	if ((err -= j % 100) <= 0)
	{
	    x++;
	    err += 100;
	}
	i++;
	printf ("%d: %d\n", i, x); 
    } while (--n);
}
