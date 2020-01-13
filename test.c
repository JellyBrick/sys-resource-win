/////////////////////////////////////////////////////////////////
// Test program to test the rlimit functions
//
//
//
///////////////////////////////////////////////////////////////
#include "resource.h"

int main(void) {
    int       i;
    rlimit_t  limits,
              lim;
    FILE      *fh;

    //
    // First set a limit
    //
    limits.rlim_cur = 1000;
    limits.rlim_max = 5000;
    setrlimit( RLIMIT_FSIZE, &limits );

    //
    // Now read and display the limit
    //
    getrlimit( RLIMIT_FSIZE, &lim );
    printf( "\nLimits\n  cur = %I64d.\n  max = %I64d.\n",
      lim.rlim_cur, lim.rlim_max );

    //
    // Now create a file and start writing to it...
    //
    fh = fopen( "a.a", "w" );
    for( i=0; i<10000; i++ )
        if( !rfwrite("a", 1, 1, fh) )
            break;

    printf( "\nBroke out of write loop with i = %d.\n", i );

  return 0;
}
