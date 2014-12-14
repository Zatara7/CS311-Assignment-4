////////////////////////////////////////////////////////////////////////////////
//
//  File          : smsa_workload.c
//  Description   : This is the main program generate workloads for the SMSA
//                  simulator.
//
//   Author : Patrick McDaniel
//   Last Modified : Sep 10 13:04:32 PDT 2013
//

// Include Files
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// Project Includes
#include <smsa.h>
#include <cmpsc311_log.h>
#include <cmpsc311_util.h>

// Defines
#define SMSA_WORKLOAD_ARGUMENTS "hv"
#define USAGE \
	"USAGE: smsa_wlgen [-h]\n" \
	"\n" \
	"where:\n" \
	"    -h - help mode (display this message)\n" \
	"    -v - verbose output\n" \
	"\n" \

//
// Functional Prototypes
int generate_SMSA_workload( void );
FILE *openWorkloadFile( unsigned char *fname );
int generateWorkloadRequest( FILE *fh, char * op, uint32_t addr, uint32_t len, uint32_t ch );
int closeWorkloadFile( FILE *fh );

//
// Functions

////////////////////////////////////////////////////////////////////////////////
//
// Function     : main
// Description  : The main function for the SMSA simulator
//
// Inputs       : argc - the number of command line parameters
//                argv - the parameters
// Outputs      : 0 if successful test, -1 if failure

int main( int argc, char *argv[] )
{
	// Local variables
	int ch;

	// Process the command line parameters
	while ((ch = getopt(argc, argv, SMSA_WORKLOAD_ARGUMENTS)) != -1) {

		switch (ch) {

		case 'h': // Help, print usage
			fprintf( stderr, USAGE );
			return( -1 );

		case 'v': // Verbose flag
			initializeLogWithFilehandle( CMPSC311_LOG_STDOUT );
			enableLogLevels( LOG_INFO_LEVEL );
			break;

		default:  // Default (unknown)
			fprintf( stderr, "Unknown command line option (%c), aborting.\n", ch );
			return( -1 );
		}
	}

	// Generate a new collection of workload files
	generate_SMSA_workload();

	// Return successfully
	return( 0 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : generate_SMSA_workloads
// Description  : This function generates the workload files
//
// Inputs       :
// Outputs      : 0 if successful test, -1 if failure

int generate_SMSA_workload( void ) {

#define GET_RANDOM_VLENGTH() (getRandomValue(1, SMSA_MAXIMUM_RDWR_SIZE))
#define GET_RANDOM_VADDRESS() (getRandomValue( 0, MAX_SMSA_VIRTUAL_ADDRESS-2 ))
#define GET_RANDOM_DISK_AND_BLOCK(d,b) { \
	d = getRandomValue( 0, SMSA_DISK_ARRAY_SIZE-1 );\
	b = getRandomValue( 0, SMSA_BLOCK_SIZE-1 ); \
}

	// Local variables
	int i, ch;
	uint32_t addr, len;
	FILE *fhandle = NULL;
	SMSA_DRUM_ID drm;
	SMSA_BLOCK_ID blk;

	// Simple (walk blocks 1 each)
	fhandle = openWorkloadFile( (unsigned char *)"simple.dat" );
	addr = 0;
	while ( addr < MAX_SMSA_VIRTUAL_ADDRESS ) {

		// Generate the block write
		generateWorkloadRequest( fhandle, SMSA_WORKLOAD_WRITE, addr, SMSA_BLOCK_SIZE, (addr>>8)&0xff );
		addr += SMSA_BLOCK_SIZE;

		// Generate a random block read
		if ( getRandomValue(0,1) ) {
			GET_RANDOM_DISK_AND_BLOCK(drm,blk);
			addr = (drm*SMSA_DISK_SIZE)+(blk*SMSA_BLOCK_SIZE);
			generateWorkloadRequest( fhandle, SMSA_WORKLOAD_READ, addr, SMSA_BLOCK_SIZE, 0x0 );
		}

	}
	closeWorkloadFile( fhandle );

	// Linear
	fhandle = openWorkloadFile( (unsigned char *)"linear.dat" );
	addr = 0;
	while ( addr < MAX_SMSA_VIRTUAL_ADDRESS ) {
		len = getRandomValue( 1, SMSA_MAXIMUM_RDWR_SIZE );
		ch = getRandomValue( 1, 255 );
		// Check to make sure we are not walking passed the end of the array
		if ( addr+len > MAX_SMSA_VIRTUAL_ADDRESS ) {
			len = MAX_SMSA_VIRTUAL_ADDRESS-addr;
		}
		generateWorkloadRequest( fhandle, SMSA_WORKLOAD_WRITE, addr, len, ch );
		addr += len;

		// Generate a random block read
		if ( getRandomValue(0,1) ) {
			len = GET_RANDOM_VLENGTH();
			addr = GET_RANDOM_VADDRESS();
			if ( addr+len < MAX_SMSA_VIRTUAL_ADDRESS ) {
				generateWorkloadRequest( fhandle, SMSA_WORKLOAD_READ, addr, len, 0x0 );
			}
		}

	}
	closeWorkloadFile( fhandle );

	// Random workload
	// First wipe the disk with constant data
	fhandle = openWorkloadFile( (unsigned char *)"random.dat" );
	addr = 0;
	while ( addr < MAX_SMSA_VIRTUAL_ADDRESS ) {
		generateWorkloadRequest( fhandle, SMSA_WORKLOAD_WRITE, addr, SMSA_BLOCK_SIZE, (addr>>8)&0xfe );
		addr += SMSA_BLOCK_SIZE;
	}

	// Now do a bunch of crazy writes of random data
	for (i=0; i<10000; i++) {
		addr = getRandomValue( 0, MAX_SMSA_VIRTUAL_ADDRESS-2 );
		len = getRandomValue( 1, SMSA_MAXIMUM_RDWR_SIZE );
		ch = getRandomValue( 1, 255 );
		if ( addr+len > MAX_SMSA_VIRTUAL_ADDRESS ) {
			len = MAX_SMSA_VIRTUAL_ADDRESS-addr;
		}
		generateWorkloadRequest( fhandle, SMSA_WORKLOAD_WRITE, addr, len, ch );

		// Generate a random block read
		if ( getRandomValue(0,1) ) {
			len = GET_RANDOM_VLENGTH();
			addr = GET_RANDOM_VADDRESS();
			if ( addr+len < MAX_SMSA_VIRTUAL_ADDRESS ) {
				generateWorkloadRequest( fhandle, SMSA_WORKLOAD_READ, addr, len, 0x0 );
			}
		}
	}
	closeWorkloadFile( fhandle );

	// Return successfully
	return( 0 );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function     : openWorkloadFile
// Description  : open the workload file and ready for generation
//
// Inputs       : fname - the name of the workload file
// Outputs      : the file handle or NULL if failure

FILE *openWorkloadFile( unsigned char *fname ) {

	// Do the open, reporting the error if needed
	FILE *fh;
	if ( (fh=fopen((char *)fname, "w+")) == NULL) {
		logMessage( LOG_ERROR_LEVEL, "Failed to open workload file [%d], error [%s]",
				fname, strerror(errno) );
		return( NULL );
	}

	// Add a mount as first statement, return file handle
	fprintf( fh, "%s\n", SMSA_WORKLOAD_MOUNT );
	return( fh );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : generateWorkloadRequest
// Description  : generate and write request to the workload file
//
// Inputs       : fh - the file handle
//                op - the operation to perform
//                addr - the address
//                len - the length of the write
//                ch - the character to repeat
// Outputs      : 0 if successful, -1 otherwise

int generateWorkloadRequest( FILE *fh, char * op, uint32_t addr, uint32_t len, uint32_t ch ) {

	// Print to log and log the creation
	fprintf( fh, "%s %u %u %u\n", op, addr, len, ch );
	logMessage( LOG_INFO_LEVEL, "Generating workload request : %7s %7u %3u %3u\n", op, addr, len, ch );
	return( 0 );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function     : closeWorkloadFile
// Description  : close the workload file
//
// Inputs       : fh - the file handle
// Outputs      : 0 if successful, -1 otherwise

int closeWorkloadFile( FILE *fh ) {

	// Add a unmount and sign as last statements, close, return
	fprintf( fh, "%s\n", SMSA_WORKLOAD_SIGNALL );
	fprintf( fh, "%s\n", SMSA_WORKLOAD_UNMOUNT );
	fclose( fh );
	return( 0 );
}
