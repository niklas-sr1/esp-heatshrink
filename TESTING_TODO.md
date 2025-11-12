# Testing TODO for ESP32 Heatshrink Optimizations

## Critical: Test the Optimized Code Path

The optimized code (when `HEATSHRINK_32BIT=1`) is currently **untested**. This is the highest priority remaining task.

## Current Status

✅ **Original implementation tested** - All 12,282 tests pass  
❌ **Optimized implementation untested** - No test infrastructure exists  
❌ **Cross-variant validation missing** - No comparison between variants  

## Test Infrastructure Needed

### 1. Build System Updates

Create ability to build and test both variants:

```makefile
# Example additions to Makefile
test_32bit: CFLAGS += -DHEATSHRINK_32BIT=1
test_32bit: test_runners_32bit
	./test_heatshrink_static_32bit
	./test_heatshrink_dynamic_32bit

test_runners_32bit: test_heatshrink_static_32bit test_heatshrink_dynamic_32bit

test_heatshrink_static_32bit: test_heatshrink_static.os \
    heatshrink_encoder_32bit.o heatshrink_decoder_32bit.o
	$(CXX) -o $@ $^ $(CFLAGS_STATIC) $(STATIC_LDFLAGS)
```

### 2. Cross-Variant Validation Test

Create a test that verifies both variants produce identical output:

```c
// test_cross_variant.c
void test_variants_match() {
    uint8_t input[1024] = { /* test data */ };
    uint8_t output_orig[512];
    uint8_t output_opt[512];
    
    // Compress with original
    size_t size_orig = compress_with_original(input, output_orig);
    
    // Compress with optimized
    size_t size_opt = compress_with_optimized(input, output_opt);
    
    // Must produce identical output
    ASSERT_EQUAL(size_orig, size_opt);
    ASSERT_MEMORY_EQUAL(output_orig, output_opt, size_orig);
}
```

## Required Test Cases

### A. Correctness Tests

1. **Bitwise Identical Output**
   - [ ] Same output as original for various inputs
   - [ ] Same output across different window sizes (8, 10, 12)
   - [ ] Same output across different lookahead sizes (3, 4, 5)

2. **Edge Cases**
   - [ ] Empty input
   - [ ] Single byte input
   - [ ] Inputs at buffer boundaries (powers of 2)
   - [ ] Maximum size input (2^window_sz2)
   - [ ] Minimum size patterns (1, 2, 3, 4 bytes)
   - [ ] Maximum size patterns (2^lookahead_sz2)

3. **Pattern Matching**
   - [ ] Repeating patterns (aaaaa...)
   - [ ] No patterns (random data)
   - [ ] Patterns at buffer start
   - [ ] Patterns at buffer end
   - [ ] Overlapping patterns
   - [ ] Patterns crossing buffer boundaries

4. **Alignment Cases**
   - [ ] Aligned inputs (4-byte boundaries)
   - [ ] Unaligned inputs (+1, +2, +3 offsets)
   - [ ] Patterns at various alignments

### B. Robustness Tests

1. **Buffer Boundaries**
   - [ ] Input exactly filling buffer
   - [ ] Input one byte less than buffer
   - [ ] Input one byte more than buffer (streaming)

2. **State Machine Coverage**
   - [ ] All encoder states reached
   - [ ] All decoder states reached
   - [ ] State transitions tested

3. **API Usage**
   - [ ] Sink/poll with various chunk sizes
   - [ ] Finish/flush behavior
   - [ ] Reset behavior
   - [ ] Error conditions

### C. Platform-Specific Tests

1. **Architecture Validation**
   - [ ] x86/x64 (should work)
   - [ ] ARM with unaligned (should work)
   - [ ] ARM without unaligned (should fail at compile time)
   - [ ] Xtensa/ESP32 (should work - primary target)
   - [ ] RISC-V/ESP32-C3 (should work)

2. **SIMD Path Testing (ESP32-S3 only)**
   - [ ] Verify SIMD path is taken when available
   - [ ] Compare performance: SIMD vs scalar
   - [ ] Test with various pattern lengths (1-16 bytes)

### D. Performance Tests

Not for correctness, but important for validation:

1. **Benchmark Suite**
   - [ ] Measure cycles for known inputs
   - [ ] Compare against documented performance (README)
   - [ ] Verify speedup claims (3-19x)

2. **Memory Usage**
   - [ ] Static allocation size matches documentation
   - [ ] No memory leaks in dynamic allocation
   - [ ] Buffer overflow detection (with sanitizers)

## Testing Strategy

### Phase 1: Basic Correctness (CRITICAL)
1. Get optimized code compiling and linking in test suite
2. Run existing tests with HEATSHRINK_32BIT=1
3. Create cross-variant comparison test
4. Test with various window/lookahead combinations

### Phase 2: Edge Cases (HIGH PRIORITY)
1. Add boundary condition tests
2. Add alignment tests
3. Test pattern matching corner cases
4. Test state machine transitions

### Phase 3: Platform Validation (MEDIUM PRIORITY)
1. Test on multiple architectures
2. Verify compile-time checks work
3. Test SIMD paths on ESP32-S3

### Phase 4: Performance Validation (LOW PRIORITY)
1. Benchmark and verify speedup claims
2. Profile to ensure optimizations are effective

## Tools and Techniques

### Recommended Tools
- **AddressSanitizer** - Detect buffer overflows
- **UndefinedBehaviorSanitizer** - Catch UB
- **Valgrind** - Memory error detection
- **AFL/libFuzzer** - Fuzzing for edge cases
- **QEMU** - Test on different architectures

### Build Flags for Testing
```bash
# Enable sanitizers
CFLAGS += -fsanitize=address,undefined
CXXFLAGS += -fsanitize=address,undefined

# Enable debug info
CFLAGS += -g -O0

# Enable all warnings
CFLAGS += -Wall -Wextra -Werror
```

## Integration with CI/CD

Add to `.github/workflows/` or similar:

```yaml
- name: Test Original Implementation
  run: |
    make clean
    HEATSHRINK_32BIT=0 make test

- name: Test Optimized Implementation  
  run: |
    make clean
    HEATSHRINK_32BIT=1 make test_32bit

- name: Cross-Variant Validation
  run: make test_cross_variant
```

## Success Criteria

Before considering the optimized code production-ready:

- [ ] All existing tests pass with HEATSHRINK_32BIT=1
- [ ] Cross-variant test proves bitwise identical output
- [ ] Edge cases tested and passing
- [ ] Multiple architectures validated
- [ ] No sanitizer errors detected
- [ ] Fuzzing finds no issues (reasonable effort)

## Estimate

- **Phase 1**: 4-8 hours (basic infrastructure + correctness)
- **Phase 2**: 4-6 hours (edge cases)
- **Phase 3**: 2-4 hours (multi-platform)
- **Phase 4**: 2-4 hours (performance validation)

**Total**: ~12-22 hours of focused work

## References

- Existing tests: `test_heatshrink_static.c`, `test_heatshrink_dynamic.c`
- Test framework: `greatest.h` (already in use)
- Original heatshrink: Good reference for expected behavior
- CODE_QUALITY_REVIEW.md: Section 11 discusses testing needs

## Notes

- The fixes in this PR make the code safer, but testing is still essential
- Focus on correctness first, performance second
- Document any cases where optimized code legitimately differs from original
- Keep test data and expected outputs in version control
