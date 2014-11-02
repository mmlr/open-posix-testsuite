#include <signal.h>
#include <stdio.h>
#include "posixtest.h"

/*
 
 * Copyright (c) 2002, Intel Corporation. All rights reserved.
 * Created by:  rolla.n.selbak REMOVE-THIS AT intel DOT com
 * This file is licensed under the GPL license.  For the full content
 * of this license, see the COPYING file at the top level of this 
 * source tree.
 
 *  Test that the sigwait() function. If no signal in 'set' is pending at the 
 *  time of the call, the thread shall be suspended until one or more becomes 
 *  pending.
 *  1)  Block a signal from delivery.
 *  2)  Call sigwait()
 *  3)  Raise the signal.
 *  4)  Verify this process will return when the signal is sent.
 */


int main()
{
	sigset_t newmask, pendingset;
	int sig;

	/* Empty set of blocked signals */

	if ( ( sigemptyset( &newmask ) == -1 ) ||
	        ( sigemptyset( &pendingset ) == -1 ) )
	{
		printf( "Error in sigemptyset()\n" );
		return PTS_UNRESOLVED;
	}

	/* Add SIGALRM to the set of blocked signals */
	if ( sigaddset( &newmask, SIGALRM ) == -1 )
	{
		perror( "Error in sigaddset()\n" );
		return PTS_UNRESOLVED;
	}

	/* Block SIGALRM */
	if ( sigprocmask( SIG_SETMASK, &newmask, NULL ) == -1 )
	{
		printf( "Error in sigprocmask()\n" );
		return PTS_UNRESOLVED;
	}

	/* Send SIGALRM signal to this process.  Since it is blocked,
	 * it should be pending */
	if ( raise( SIGALRM ) != 0 )
	{
		printf( "Could not raise SIGALRM\n" );
		return PTS_UNRESOLVED;
	}

	/* Test that SIGALRM is pending */
	if ( sigpending( &pendingset ) == -1 )
	{
		printf( "Could not get pending signal set\n" );
		return PTS_UNRESOLVED;
	}

	if ( sigismember( &pendingset, SIGALRM ) != 1 )
	{
		printf( "Signal SIGALRM is not pending!\n" );
		return PTS_FAIL;
	}


	/* Call sigwait and test if it passed/failed*/
	if ( sigwait( &newmask, &sig ) != 0 )
	{
		printf( "Error in sigwait()\n" );
		return PTS_FAIL;
	}

	if ( sig != SIGALRM )
	{
		printf( "sigwait selected another signal\n" );
		return PTS_FAIL;
	}

	/* Test that SIGALRM is not pending anymore */
	if ( sigpending( &pendingset ) == -1 )
	{
		printf( "Could not get pending signal set\n" );
		return PTS_UNRESOLVED;
	}

	if ( sigismember( &pendingset, SIGALRM ) != 0 )
	{
		printf( "Signal SIGALRM is not pending!\n" );
		return PTS_FAIL;
	}


	/* If we get here, then the process was suspended until
	 * SIGALRM was raised.  */
	printf( "Test PASSED\n" );

	return PTS_PASS;

}
