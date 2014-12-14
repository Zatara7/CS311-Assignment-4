#ifndef SMSA_CACHE_INCLUDED
#define SMSA_CACHE_INCLUDED

////////////////////////////////////////////////////////////////////////////////
//
//  File           : smsa_cache.h
//  Description    : This is the cache for the SMSA simulator.
//
//   Author        : Patrick McDaniel
//   Last Modified : Fri Oct 11 03:23:03 EDT 2013
//

// Include Files
#include <stdint.h>
#include <sys/time.h>

// Project Include Files
#include <smsa.h>

//
// Type Definitions

// This is the structure for the cache line
typedef struct {
    SMSA_DRUM_ID     drum;  // This is the drum for the cache line
    SMSA_BLOCK_ID    block; // This is the block ID for the cache line
    struct timeval   used;  // A timestamp of the last use of this entry
    unsigned char   *line;  // This is cache entru itslef
} SMSA_CACHE_LINE;


//
// Funtional Prototypes

// Setup the block cache
int smsa_init_cache( uint32_t lines );

// Clear cache and free associated memory
int smsa_close_cache( void );

// Check to see if the cache entry is available
unsigned char *smsa_get_cache_line( SMSA_DRUM_ID drm, SMSA_BLOCK_ID blk );

// Put a new line into the cache
int smsa_put_cache_line( SMSA_DRUM_ID drm, SMSA_BLOCK_ID blk, unsigned char *buf );

#endif
