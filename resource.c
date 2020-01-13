///////////////////////////////////////////////////////////////
//
// Sample implementation of getrlimit() and setrlimit() for Win32.
//
// Includes wrappers around fwrite() and _write() where the wrappers
// are resource limit aware.
//
//
///////////////////////////////////////////////////////////////

#include   <windows.h>
#include   "resource.h"
#include   <io.h>
#include   <errno.h>

static BOOL     rInitialized = FALSE;       // Indicates if the
      rlimit structure has been initialized

static rlimit_t rlimits[RLIM_NLIMITS];      // Resource limits array
      - on element for each limit we
                                            // keep track of.



///////////////////////////////////////////////////////////////
//
// InitializeRlimits()
//
// Sets the initial values in the rlimits arrar for the process.
//
///////////////////////////////////////////////////////////////
void InitializeRlimits()
{
    int    i;                               // Index variable
    //
    // Initialize the rlimits structure with 0 for the current value,
    // and 2^32 - 1 for the max.  This function could be modified
    // to read the initial values from...
    //     ...the registry...
    //     ...an environment variable...
    //     ...a disk file...
    //     ...other...
    // which would then be used to populate the rlimits structure.
    //
    for( i=0; i<RLIM_NLIMITS; i++ )
    {
        rlimits[i].rlim_cur = 0;
        rlimits[i].rlim_max = 0xffffffff;
    }
    rInitialized = TRUE;
}

/////////////////////////////////////////////////////////////////
// getrlimit()
//
// NOTE: Posix spec states function returns 0 on success and -1
//       when an error occurs and sets errno to the error code.
//       Currently, if an error occurs, the errno value is returned
//       rather than -1.  errno is not set.
//
///////////////////////////////////////////////////////////////int
      getrlimit(int resource, struct rlimit *rlp)
{
    int   iRet = 0;                     // return value - assume
success

    //
    // If we have not initialized the limits yet, do so now
    //
    if( !rInitialized )
        InitializeRlimits();

    //
    // Check to make sure the resource value is within range
    //
    if( (resource < 0) || (resource >= RLIM_NLIMITS) )
    {
        iRet = EINVAL;
    }

    //
    // Return both rlim_cur and rlim_max
    //
    *rlp = rlimits[resource];

    return iRet;
}

/////////////////////////////////////////////////////////////////
// setrlimit()
//
// NOTE: Posix spec states function returns 0 on success and -1
//       when an error occurs and sets errno to the error code.
//       Currently, if an error occurs, the errno value is returned
//       rather than -1.  errno is not set.
//
///////////////////////////////////////////////////////////////int
      setrlimit(int resource, const struct rlimit *rlp)
{
    int    iRet = 0;                     // return value - assume
success

    if( !rInitialized )
        InitializeRlimits();
    //
    // Check to make sure the resource value is within range
    //
    if( (resource < 0) || (resource >= RLIM_NLIMITS) )
    {
        iRet = EINVAL;
    }
    //
    // Only change the current limit - do not change the max limit.
    // We could pick some NT privilege, which if the user held, we
    // would allow the changing of rlim_max.
    //
    if( rlp->rlim_cur < rlimits[resource].rlim_max )
        rlimits[resource].rlim_cur = rlp->rlim_cur;
    else
        iRet = EINVAL;
    //
    // We should not let the user set the max value.  However,
    // since currently there is no defined source for initial
    // values, we will let the user change the max value.
    //
    rlimits[resource].rlim_max = rlp->rlim_max;

    return iRet;
}

/////////////////////////////////////////////////////////////////
// Wrap the real fwrite() with this rfwrite() function, which is
// resource limit aware.
//
//
///////////////////////////////////////////////////////////////size_t
      rfwrite( const void *buffer, size_t size, size_t count, FILE 
      *stream )
{
    long            position;
    size_t          written;
    __int64         liByteCount,
                    liPosition;
    //
    // Convert the count to a large integer (64 bit integer)
    //
    liByteCount = (__int64)count;

    //
    // Get the current file position
    // 
    position = ftell( stream );
    liPosition = (__int64)position;

    //
    // Check to make sure the write will not exceed the RLIMIT_FSIZE
limit.
    //
    if ( (liPosition + liByteCount) > rlimits[RLIMIT_FSIZE].rlim_cur
)
    {
        //
        // report an error
        //
        written = 0;

    }
    else
    {
        //
        // Do the actual write the user requested
        //
        written = fwrite( buffer, size, count, stream );
    }
    return written;
}

/////////////////////////////////////////////////////////////////
// Wrap the real _write() function with the _rwrite() function
// which is resource aware.
//
//
///////////////////////////////////////////////////////////////int
      _rwrite( int handle, const void *buffer, unsigned int count )
{
    long            position;
    DWORD           dwWritten;
    _int64          liByteCount,
                    liPosition;
    //
    // Convert the count to a large integer
    //
    liByteCount = (__int64)count;

    //
    // Get the Current file position
    //
    position = _tell( handle );
    liPosition = (__int64)position;

    //
    // Check to make sure the write will not exceed the RLIMIT_FSIZE
limit.
    //
    if ( (liPosition + liByteCount) > rlimits[RLIMIT_FSIZE].rlim_cur
)
    {
        //
        // report an error
        //
        dwWritten = 0;
    }
    else
    {
        //
        // Do the actual write the user requested
        //
        dwWritten = _write( handle, buffer, count );
    }
    return dwWritten;
}
