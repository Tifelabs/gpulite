#import "metal_bench.hpp"
#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#import <chrono>

// M4 Pro class — larger matrix = more accurate GFLOPS measurement
static const uint32_t MATRIX_N   = 1024;          // 1024x1024
static const uint32_t MEM_FLOATS = 64 * 1024 * 1024; // 64M floats = 256MB

static NSString* inlineShaderSource = @R"(
  #include <metal_stdlib>
  using namespace metal;

  kernel void matmul(
    device const float* A [[ buffer(0) ]],
    device const float* B [[ buffer(1) ]],
    device       float* C [[ buffer(2) ]],
    constant     uint&  N [[ buffer(3) ]],
    uint2 gid [[ thread_position_in_grid ]]
  ) {
    if (gid.x >= N || gid.y >= N) return;
    float sum = 0.0f;
    for (uint k = 0; k < N; k++)
      sum += A[gid.y * N + k] * B[k * N + gid.x];
    C[gid.y * N + gid.x] = sum;
  }

  kernel void membench(
    device const float* src [[ buffer(0) ]],
    device       float* dst [[ buffer(1) ]],
    uint gid [[ thread_position_in_grid ]]
  ) {
    dst[gid] = src[gid];
  }
)";

static void runMatmul(id<MTLComputePipelineState> pso,
                      id<MTLCommandQueue> queue,
                      id<MTLBuffer> A, id<MTLBuffer> B,
                      id<MTLBuffer> C, id<MTLBuffer> N,
                      uint32_t dim) {
  id<MTLCommandBuffer>         cb  = [queue commandBuffer];
  id<MTLComputeCommandEncoder> enc = [cb computeCommandEncoder];
  [enc setComputePipelineState:pso];
  [enc setBuffer:A offset:0 atIndex:0];
  [enc setBuffer:B offset:0 atIndex:1];
  [enc setBuffer:C offset:0 atIndex:2];
  [enc setBuffer:N offset:0 atIndex:3];
  NSUInteger tg = MIN((NSUInteger)256, pso.maxTotalThreadsPerThreadgroup);
  [enc dispatchThreads:MTLSizeMake(dim, dim, 1)
   threadsPerThreadgroup:MTLSizeMake(tg, 1, 1)];
  [enc endEncoding];
  [cb commit];
  [cb waitUntilCompleted];
}

static void runMembench(id<MTLComputePipelineState> pso,
                        id<MTLCommandQueue> queue,
                        id<MTLBuffer> src, id<MTLBuffer> dst,
                        uint32_t count) {
  id<MTLCommandBuffer>         cb  = [queue commandBuffer];
  id<MTLComputeCommandEncoder> enc = [cb computeCommandEncoder];
  [enc setComputePipelineState:pso];
  [enc setBuffer:src offset:0 atIndex:0];
  [enc setBuffer:dst offset:0 atIndex:1];
  NSUInteger tg = MIN((NSUInteger)1024, pso.maxTotalThreadsPerThreadgroup);
  [enc dispatchThreads:MTLSizeMake(count, 1, 1)
   threadsPerThreadgroup:MTLSizeMake(tg, 1, 1)];
  [enc endEncoding];
  [cb commit];
  [cb waitUntilCompleted];
}

MetalBenchResult runMetalBenchmark() {
  MetalBenchResult result = {0, 0, false};

  id<MTLDevice> device = MTLCreateSystemDefaultDevice();
  if (!device) return result;
  result.supported = true;

  NSError* error = nil;
  MTLCompileOptions* opts = [[MTLCompileOptions alloc] init];
  id<MTLLibrary> lib = [device newLibraryWithSource:inlineShaderSource
                                            options:opts
                                              error:&error];
  if (!lib) return result;

  id<MTLComputePipelineState> matmulPSO =
    [device newComputePipelineStateWithFunction:[lib newFunctionWithName:@"matmul"]
                                          error:&error];
  id<MTLComputePipelineState> membenchPSO =
    [device newComputePipelineStateWithFunction:[lib newFunctionWithName:@"membench"]
                                          error:&error];
  if (!matmulPSO || !membenchPSO) return result;

  id<MTLCommandQueue> queue = [device newCommandQueue];

  // ── MATMUL BUFFERS ──
  uint32_t N    = MATRIX_N;
  size_t   matBytes = (size_t)N * N * sizeof(float);
  id<MTLBuffer> bufA = [device newBufferWithLength:matBytes options:MTLResourceStorageModeShared];
  id<MTLBuffer> bufB = [device newBufferWithLength:matBytes options:MTLResourceStorageModeShared];
  id<MTLBuffer> bufC = [device newBufferWithLength:matBytes options:MTLResourceStorageModeShared];
  id<MTLBuffer> bufN = [device newBufferWithBytes:&N length:sizeof(uint32_t)
                                          options:MTLResourceStorageModeShared];

  float* pA = (float*)bufA.contents;
  float* pB = (float*)bufB.contents;
  for (uint32_t i = 0; i < N * N; i++) {
    pA[i] = (float)(i % 7) * 0.1f;
    pB[i] = (float)(i % 5) * 0.1f;
  }

  // Warm up matmul
  runMatmul(matmulPSO, queue, bufA, bufB, bufC, bufN, N);

  // Timed matmul — run 3 times, take best
  double bestMatmul = 1e9;
  for (int r = 0; r < 3; r++) {
    auto t0 = std::chrono::high_resolution_clock::now();
    runMatmul(matmulPSO, queue, bufA, bufB, bufC, bufN, N);
    auto t1 = std::chrono::high_resolution_clock::now();
    double secs = std::chrono::duration<double>(t1 - t0).count();
    if (secs < bestMatmul) bestMatmul = secs;
  }

  // 2 * N^3 FLOPs for matrix multiply
  double flops   = 2.0 * N * N * N;
  result.gflops  = (float)(flops / bestMatmul / 1e9);

  // ── MEMBENCH BUFFERS ──
  size_t memBytes = (size_t)MEM_FLOATS * sizeof(float);
  id<MTLBuffer> bufSrc = [device newBufferWithLength:memBytes options:MTLResourceStorageModeShared];
  id<MTLBuffer> bufDst = [device newBufferWithLength:memBytes options:MTLResourceStorageModeShared];

  float* pSrc = (float*)bufSrc.contents;
  for (uint32_t i = 0; i < MEM_FLOATS; i++) pSrc[i] = (float)i;

  // Warm up membench
  runMembench(membenchPSO, queue, bufSrc, bufDst, MEM_FLOATS);

  // Timed membench — run 3 times, take best
  double bestMem = 1e9;
  for (int r = 0; r < 3; r++) {
    auto t2 = std::chrono::high_resolution_clock::now();
    runMembench(membenchPSO, queue, bufSrc, bufDst, MEM_FLOATS);
    auto t3 = std::chrono::high_resolution_clock::now();
    double secs = std::chrono::duration<double>(t3 - t2).count();
    if (secs < bestMem) bestMem = secs;
  }

  // read + write = 2x buffer size
  result.bandwidth_gb = (float)((2.0 * memBytes) / bestMem / 1e9);

  return result;
}