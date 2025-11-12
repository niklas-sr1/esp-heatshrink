/*
    Memory Access Helpers for Heatshrink
    
    Provides safe, standards-compliant memory access functions that avoid
    undefined behavior from type punning and unaligned access.
    
    Copyright 2024
    Licensed under ISC License (compatible with heatshrink)
*/
#pragma once
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace heatshrink {

/**
 * @brief Safe memory read that avoids undefined behavior from type punning.
 * 
 * This function uses memcpy which is:
 * 1. Well-defined for reading any object representation
 * 2. Optimized by compilers to single instruction for small constant sizes
 * 3. Safe for unaligned access (compiler handles it appropriately for target)
 * 
 * @tparam T The type to read (must be trivially copyable)
 * @param ptr Pointer to memory location (may be unaligned)
 * @return Value read from memory
 */
template<typename T>
static inline T read_as(const void* ptr) noexcept {
    static_assert(std::is_trivially_copyable<T>::value, 
                  "T must be trivially copyable");
    T result;
    std::memcpy(&result, ptr, sizeof(T));
    return result;
}

/**
 * @brief Read a uint8_t from memory.
 * Specialization for single bytes (always aligned, no memcpy needed).
 */
template<>
inline uint8_t read_as<uint8_t>(const void* ptr) noexcept {
    return *static_cast<const uint8_t*>(ptr);
}

/**
 * @brief Pointer arithmetic helper that maintains type safety.
 * 
 * @tparam T Pointer type to return
 * @param ptr Base pointer
 * @param byte_offset Offset in bytes
 * @return Pointer offset by byte_offset bytes
 */
template<typename T>
static inline const T* offset_ptr(const void* ptr, ptrdiff_t byte_offset) noexcept {
    return reinterpret_cast<const T*>(
        static_cast<const uint8_t*>(ptr) + byte_offset
    );
}

/**
 * @brief Increment pointer by byte offset (modifies pointer in place).
 * 
 * @tparam T Pointer type
 * @param ptr Pointer to modify
 * @param byte_offset Number of bytes to advance
 */
template<typename T>
static inline void advance_ptr(T*& ptr, ptrdiff_t byte_offset) noexcept {
    ptr = reinterpret_cast<T*>(
        reinterpret_cast<uint8_t*>(ptr) + byte_offset
    );
}

/**
 * @brief Const-correct version of advance_ptr.
 */
template<typename T>
static inline void advance_ptr(const T*& ptr, ptrdiff_t byte_offset) noexcept {
    ptr = reinterpret_cast<const T*>(
        reinterpret_cast<const uint8_t*>(ptr) + byte_offset
    );
}

/**
 * @brief Check if a pointer is aligned for type T.
 * 
 * @tparam T Type to check alignment for
 * @param ptr Pointer to check
 * @return true if ptr is properly aligned for T
 */
template<typename T>
static inline bool is_aligned(const void* ptr) noexcept {
    return (reinterpret_cast<uintptr_t>(ptr) % alignof(T)) == 0;
}

} // namespace heatshrink
