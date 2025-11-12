#ifndef HEATSHRINK_CONFIG_H
#define HEATSHRINK_CONFIG_H

/* When building as an ESP-IDF component, configuration comes from sdkconfig.h via Kconfig.
 * For non-ESP-IDF builds, configuration uses the defaults or can be overridden by defining
 * these macros before including this header. */

/* Check if we're building with ESP-IDF (sdkconfig.h available) */
#if __has_include("sdkconfig.h")
    #include "sdkconfig.h"
    #define HEATSHRINK_ESP_IDF_BUILD 1
#else
    #define HEATSHRINK_ESP_IDF_BUILD 0
#endif

/* Should functionality assuming dynamic allocation be used? */
#ifndef HEATSHRINK_DYNAMIC_ALLOC
    #if HEATSHRINK_ESP_IDF_BUILD
        /* Use Kconfig setting from ESP-IDF */
        #ifdef CONFIG_HEATSHRINK_DYNAMIC_ALLOC_ENABLED
            #define HEATSHRINK_DYNAMIC_ALLOC 1
        #else
            #define HEATSHRINK_DYNAMIC_ALLOC 0
        #endif
    #else
        /* Default for non-ESP-IDF builds */
        #define HEATSHRINK_DYNAMIC_ALLOC 1
    #endif
#endif

/* Should the (32-bit) optimized variant be built? (Faster on 32 bit architectures.) */
#ifndef HEATSHRINK_32BIT
    #if HEATSHRINK_ESP_IDF_BUILD && defined(CONFIG_HEATSHRINK_32BIT)
        #define HEATSHRINK_32BIT CONFIG_HEATSHRINK_32BIT
    #else
        #define HEATSHRINK_32BIT 1
    #endif
#endif

#if HEATSHRINK_DYNAMIC_ALLOC
    /* Optional replacement of malloc/free */
    #ifndef HEATSHRINK_MALLOC
        #define HEATSHRINK_MALLOC(SZ) malloc(SZ)
    #endif
    #ifndef HEATSHRINK_FREE
        #define HEATSHRINK_FREE(P, SZ) free(P)
    #endif
#else
    /* Required parameters for static configuration */
    #ifndef HEATSHRINK_STATIC_INPUT_BUFFER_SIZE
        #if HEATSHRINK_ESP_IDF_BUILD && defined(CONFIG_HEATSHRINK_STATIC_INPUT_BUFFER_SIZE)
            #define HEATSHRINK_STATIC_INPUT_BUFFER_SIZE CONFIG_HEATSHRINK_STATIC_INPUT_BUFFER_SIZE
        #else
            #define HEATSHRINK_STATIC_INPUT_BUFFER_SIZE 32
        #endif
    #endif
    
    #ifndef HEATSHRINK_STATIC_WINDOW_BITS
        #if HEATSHRINK_ESP_IDF_BUILD && defined(CONFIG_HEATSHRINK_STATIC_WINDOW_BITS)
            #define HEATSHRINK_STATIC_WINDOW_BITS CONFIG_HEATSHRINK_STATIC_WINDOW_BITS
        #else
            #define HEATSHRINK_STATIC_WINDOW_BITS 8
        #endif
    #endif
    
    #ifndef HEATSHRINK_STATIC_LOOKAHEAD_BITS
        #if HEATSHRINK_ESP_IDF_BUILD && defined(CONFIG_HEATSHRINK_STATIC_LOOKAHEAD_BITS)
            #define HEATSHRINK_STATIC_LOOKAHEAD_BITS CONFIG_HEATSHRINK_STATIC_LOOKAHEAD_BITS
        #else
            #define HEATSHRINK_STATIC_LOOKAHEAD_BITS 4
        #endif
    #endif
#endif

/* Turn on logging for debugging. */
#ifndef HEATSHRINK_DEBUGGING_LOGS
    #if HEATSHRINK_ESP_IDF_BUILD && defined(CONFIG_HEATSHRINK_DEBUGGING_LOGS)
        #define HEATSHRINK_DEBUGGING_LOGS CONFIG_HEATSHRINK_DEBUGGING_LOGS
    #else
        #define HEATSHRINK_DEBUGGING_LOGS 0
    #endif
#endif

/* Use indexing for faster compression. (Increases RAM requirement by ~200%, disables use
   of the 32-bit search functions.) */
#ifndef HEATSHRINK_USE_INDEX
    #if HEATSHRINK_ESP_IDF_BUILD && defined(CONFIG_HEATSHRINK_USE_INDEX)
        #define HEATSHRINK_USE_INDEX CONFIG_HEATSHRINK_USE_INDEX
    #else
        #define HEATSHRINK_USE_INDEX 0
    #endif
#endif

#endif
