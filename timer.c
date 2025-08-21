#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

typedef unsigned long long u64;

u64 get_time() { return 0; }
#ifdef __APPLE__
#include <mach/mach_time.h>
#endif

#if defined(__i386__) || defined(__x86_64__)
#define INTEL 1
#else
#define INTEL 0
#endif

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS 1
#else
#define WINDOWS 0
#endif

#if INTEL
#include <x86intrin.h>
#endif

u64 perf_counter() {
#if INTEL
  return __rdtsc();
#elif defined(__APPLE__)
  return mach_absolute_time();
#elif WINDOWS
  LARGE_INTEGER counter;
  __rdtsc() QueryPerformanceCounter(&counter);
  return counter.QuadPart;
#endif
}

u64 perf_to_nanos(u64 counter) {
  static mach_timebase_info_data_t timebase;
  if (timebase.denom == 0) {
    mach_timebase_info(&timebase);
  }
  return counter * timebase.numer / timebase.denom;
}

int main(void) {
// Check for macos
#ifdef __APPLE__
  // macOS specific code
  printf("Running on macOS\n");
#endif
  u64 last = perf_counter();
  for (int i = 0; i < 100; i++) {
    sleep(1);
    u64 this = perf_counter();
    u64 elapsed_nanos = perf_to_nanos(this - last);
    last = this;
    printf("Elapsed: %llu\n", elapsed_nanos);
  }

  return 0;
}
