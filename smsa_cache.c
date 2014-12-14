////////////////////////////////////////////////////////////////////////////////
//
//  File           : smsa_cache.c
//  Description    : This is the cache for the SMSA simulator.
//
//   Author        : Hayder Sharhan
//   Last Modified : Nov 18 2013
//

// Include Files
#include <stdint.h>
#include <stdlib.h>
#include <cmpsc311_util.h>

// Project Include Files
#include <smsa_cache.h>

//
// Global Variables
SMSA_CACHE_LINE *cache; // Used for array of SMSA_CACHE_LINES
uint32_t numLines;	// Used for the number of lines in the cache


// Functions

////////////////////////////////////////////////////////////////////////////////
//
// Function     : smsa_init_cache
// Description  : Setup the block cache
//
// Inputs       : lines - the number of cache entries to create
// Outputs      : 0 if successful test, -1 if failure

int smsa_init_cache( uint32_t lines ) {
	cache = calloc(lines, sizeof(SMSA_CACHE_LINE)); 	// Put the cache in the heap with the number of lines needed

	
	// Initialize all the data inside the cache structure to -1 so the cache isn't confused
	int i;
	for (i = 0; i < lines; i++) {
		cache[i].drum = -1;
		cache[i].block = -1;
		cache[i].line = NULL;
	}

	numLines = lines;					// Set the global variable equal to the current
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : smsa_close_cache
// Description  : Clear cache and free associated memory
//
// Inputs       : none
// Outputs      : 0 if successful test, -1 if failure

int smsa_close_cache( void ) {
	int i;
	for (i = 0; i < numLines; i++) {	// Loop Through the cache
		if (cache[i].line != NULL) {
			free(cache[i].line);	// Free each one that isn't equal to null to avoid double freeing
			cache[i].line = NULL;	// Good practice
		}
	}
	free(cache);				// Free the cache structure
	cache = NULL;				// Good practice
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : smsa_get_cache_line
// Description  : Check to see if the cache entry is available
//
// Inputs       : drm - the drum ID to look for
//                blk - the block ID to lookm for
// Outputs      : pointer to cache entry if found, NULL otherwise

unsigned char *smsa_get_cache_line( SMSA_DRUM_ID drm, SMSA_BLOCK_ID blk ) {
	int i;
	for (i = 0; i < numLines; i++) {				// Loop through the cache
		if (drm == cache[i].drum && blk == cache[i].block) {	// Look for matching data
			gettimeofday(&cache[i].used, NULL);		// Update the time used
			return cache[i].line;				// Return matched data
		}
	}
	return NULL;							// Nothing was found
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : smsa_put_cache_line
// Description  : Put a new line into the cache
//
// Inputs       : drm - the drum ID to place
//                blk - the block ID to lplace
//                buf - the buffer to put into the cache
// Outputs      : 0 if successful, -1 otherwise

int smsa_put_cache_line( SMSA_DRUM_ID drm, SMSA_BLOCK_ID blk, unsigned char *buf ) {
	int i;
	
	// check for empty spot on the cache
	for (i = 0; i < numLines; i++) {			// Loop through the cache
		if (cache[i].line == NULL) {			// Check for an empty spot
			cache[i].drum = drm;			// Set data equal to each other
			cache[i].block = blk;
			gettimeofday(&cache[i].used, NULL); 	// To update timestamp
			cache[i].line = buf; 			// Set pointers to point to the same place
			return 0; 
		}
	}

	// loop to evict the least recently used cache index
	int iLRU = 0; 								// Variable to hold the index of the LRU cache entry
	long result = 0; 							// Variable to hold result of comparison
	for(i = 1; i < numLines; i++) {						// Loop through the cache entries
		result = compareTimes(&cache[iLRU].used, &cache[i].used);	// Compare the times of the entries
		if (result > 0); 						// If one was found
		{
			iLRU = i;						// Keep the index for later
		}
	}

	if(cache[iLRU].line != NULL) {						// If it isn't empty
		free(cache[iLRU].line);						// Empty it
		cache[iLRU].line = NULL;
	}
	cache[iLRU].drum = drm;							// Set data equal to each other at the found index
	cache[iLRU].block = blk;
	gettimeofday(&cache[iLRU].used, NULL);					// Same for time
	cache[iLRU].line = buf;							// Same for pointer
	return 0;
}
