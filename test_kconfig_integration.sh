#!/bin/bash
# Test script to validate Kconfig integration with heatshrink_config.h
# This script tests that configuration works correctly in different scenarios:
# - Non-ESP-IDF builds with default configuration
# - Non-ESP-IDF builds with macro overrides
# - ESP-IDF builds with simulated sdkconfig.h (dynamic allocation)
# - ESP-IDF builds with simulated sdkconfig.h (static allocation with custom parameters)
# - Header compilation with different configurations
#
# Run this script to verify the configuration system is working correctly.

set -e

echo "=========================================="
echo "Testing Kconfig Integration"
echo "=========================================="
echo ""

# Test 1: Non-ESP-IDF build (defaults)
echo "Test 1: Non-ESP-IDF build with defaults"
echo "----------------------------------------"
cat > /tmp/test_non_esp.c << 'EOF'
#include <stdio.h>
#include <assert.h>
#include "heatshrink_config.h"

int main() {
    // Should use defaults for non-ESP-IDF build
    assert(HEATSHRINK_DYNAMIC_ALLOC == 1);
    assert(HEATSHRINK_32BIT == 1);
    assert(HEATSHRINK_USE_INDEX == 0);
    assert(HEATSHRINK_DEBUGGING_LOGS == 0);
    printf("✓ Non-ESP-IDF defaults are correct\n");
    return 0;
}
EOF
gcc -I. /tmp/test_non_esp.c -o /tmp/test_non_esp && /tmp/test_non_esp
echo ""

# Test 2: Non-ESP-IDF build with override
echo "Test 2: Non-ESP-IDF build with macro override"
echo "----------------------------------------------"
cat > /tmp/test_override.c << 'EOF'
#include <stdio.h>
#include <assert.h>
#define HEATSHRINK_DYNAMIC_ALLOC 0
#define HEATSHRINK_32BIT 0
#include "heatshrink_config.h"

int main() {
    // Should respect external defines
    assert(HEATSHRINK_DYNAMIC_ALLOC == 0);
    assert(HEATSHRINK_32BIT == 0);
    assert(HEATSHRINK_STATIC_WINDOW_BITS == 8);
    assert(HEATSHRINK_STATIC_LOOKAHEAD_BITS == 4);
    assert(HEATSHRINK_STATIC_INPUT_BUFFER_SIZE == 32);
    printf("✓ External macro overrides work correctly\n");
    return 0;
}
EOF
gcc -I. /tmp/test_override.c -o /tmp/test_override && /tmp/test_override
echo ""

# Test 3: ESP-IDF build simulation (dynamic allocation)
echo "Test 3: ESP-IDF build with dynamic allocation"
echo "----------------------------------------------"
mkdir -p /tmp/esp_dynamic
cat > /tmp/esp_dynamic/sdkconfig.h << 'EOF'
/* Simulated sdkconfig.h for ESP-IDF build with dynamic allocation */
#define CONFIG_HEATSHRINK_DYNAMIC_ALLOC_ENABLED 1
#define CONFIG_HEATSHRINK_32BIT 1
#define CONFIG_HEATSHRINK_USE_INDEX 0
#define CONFIG_HEATSHRINK_DEBUGGING_LOGS 0
EOF

cat > /tmp/test_esp_dynamic.c << 'EOF'
#include <stdio.h>
#include <assert.h>
#include "heatshrink_config.h"

int main() {
    // Should use CONFIG_* defines from sdkconfig.h
    assert(HEATSHRINK_DYNAMIC_ALLOC == 1);
    assert(HEATSHRINK_32BIT == 1);
    assert(HEATSHRINK_USE_INDEX == 0);
    assert(HEATSHRINK_DEBUGGING_LOGS == 0);
    printf("✓ ESP-IDF dynamic allocation configuration works\n");
    return 0;
}
EOF
gcc -I. -I/tmp/esp_dynamic /tmp/test_esp_dynamic.c -o /tmp/test_esp_dynamic && /tmp/test_esp_dynamic
echo ""

# Test 4: ESP-IDF build simulation (static allocation)
echo "Test 4: ESP-IDF build with static allocation and custom params"
echo "----------------------------------------------------------------"
mkdir -p /tmp/esp_static
cat > /tmp/esp_static/sdkconfig.h << 'EOF'
/* Simulated sdkconfig.h for ESP-IDF build with static allocation */
#define CONFIG_HEATSHRINK_STATIC_ALLOC_ENABLED 1
#define CONFIG_HEATSHRINK_STATIC_WINDOW_BITS 10
#define CONFIG_HEATSHRINK_STATIC_LOOKAHEAD_BITS 5
#define CONFIG_HEATSHRINK_STATIC_INPUT_BUFFER_SIZE 64
#define CONFIG_HEATSHRINK_32BIT 0
#define CONFIG_HEATSHRINK_USE_INDEX 1
#define CONFIG_HEATSHRINK_DEBUGGING_LOGS 1
EOF

cat > /tmp/test_esp_static.c << 'EOF'
#include <stdio.h>
#include <assert.h>
#include "heatshrink_config.h"

int main() {
    // Should use CONFIG_* defines from sdkconfig.h
    assert(HEATSHRINK_DYNAMIC_ALLOC == 0);
    assert(HEATSHRINK_STATIC_WINDOW_BITS == 10);
    assert(HEATSHRINK_STATIC_LOOKAHEAD_BITS == 5);
    assert(HEATSHRINK_STATIC_INPUT_BUFFER_SIZE == 64);
    assert(HEATSHRINK_32BIT == 0);
    assert(HEATSHRINK_USE_INDEX == 1);
    assert(HEATSHRINK_DEBUGGING_LOGS == 1);
    printf("✓ ESP-IDF static allocation with custom parameters works\n");
    return 0;
}
EOF
gcc -I. -I/tmp/esp_static /tmp/test_esp_static.c -o /tmp/test_esp_static && /tmp/test_esp_static
echo ""

# Test 5: Verify headers compile with different configurations
echo "Test 5: Verify headers compile with static allocation"
echo "------------------------------------------------------"
cat > /tmp/test_compile_static.c << 'EOF'
#define HEATSHRINK_DYNAMIC_ALLOC 0
#include "heatshrink_encoder.h"
#include "heatshrink_decoder.h"
#include <stdio.h>

int main() {
    heatshrink_encoder encoder;
    heatshrink_decoder decoder;
    printf("✓ Headers compile successfully with static allocation\n");
    return 0;
}
EOF
gcc -I. -c /tmp/test_compile_static.c -o /tmp/test_compile_static.o
echo ""

echo "=========================================="
echo "All Kconfig integration tests passed! ✓"
echo "=========================================="
