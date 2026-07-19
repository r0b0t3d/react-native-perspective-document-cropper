#include <cstdint>

// Android NDK's bundled libomp lacks __kmpc_dispatch_deinit (present in upstream
// LLVM's libomp, where it's an empty no-op — see
// llvm-project/openmp/runtime/src/kmp_dispatch.cpp), so opencv-mobile's arm64-v8a
// prebuilt (opencv_core.a / kleidicv_thread.a) fails to link against any current
// NDK. Providing the exact upstream body here as a weak symbol is safe and
// becomes a no-op override if a future NDK ships the real one.
extern "C" __attribute__((weak)) void __kmpc_dispatch_deinit(void* loc, int32_t gtid) {}
