#pragma once

#include <cstdint>

#ifdef __XTENSA__
#include <xtensa/config/core-isa.h>
#endif

namespace heatshrink {

    /**
     * @brief Provides feature flags of the architecture we're building for.
     * (Only for Xtensa because the RISC-V's don't have any useful features for our use case.)
     * 
     */
    struct Arch {
        
        /**
         * @brief Does this architecture support unaligned memory access?
         * 
         * This is required for the 32-bit optimizations to work correctly.
         * Most modern architectures support unaligned access, though it may
         * be slower than aligned access.
         */
        static constexpr bool UNALIGNED_ACCESS_OK =
            #if defined(__ARM_FEATURE_UNALIGNED)
                // ARM architectures with unaligned access support
                true
            #elif defined(__XTENSA__)
                // Xtensa (ESP32) supports unaligned access
                true
            #elif defined(__x86_64__) || defined(__i386__)
                // x86/x64 supports unaligned access
                true
            #elif defined(__riscv)
                // RISC-V may or may not support unaligned access
                // ESP32-C3 and similar do support it
                #if defined(ESP_PLATFORM)
                    true
                #else
                    // Conservative: assume RISC-V doesn't support it
                    false
                #endif
            #else
                // Unknown architecture - be conservative
                false
            #endif
            ;
        /**
         * @brief Are we running on an Xtensa architecture?
         * 
         */
        static constexpr bool XTENSA =
            #ifdef __XTENSA__
                true;
            #else
                false;
            #endif

        /**
         * @brief Do we have the ESP32-S3's ISA extensions?
         * 
         */
        static constexpr bool ESP32S3 =
            #if CONFIG_IDF_TARGET_ESP32S3
                true;
            #else
                false;
            #endif


        /**
         * @brief Do we have Xtensa's zero-overhead loops?
         * 
         */
        static constexpr bool XT_LOOP =
            #if XCHAL_HAVE_LOOPS
                true;
            #else
                false;
            #endif

        /**
         * @brief Does __builtin_clz() map to a hardware instruction?
         * 
         */
        static constexpr bool HW_CLZ =
            #if XCHAL_HAVE_NSA
                true;
            #else
                false;
            #endif

        /**
         * @brief Do we have hardware min/max operations?
         * 
         */
        static constexpr bool HW_MINMAX =
            #if XCHAL_HAVE_MINMAX
                true;
            #else
                false;
            #endif
    };
}