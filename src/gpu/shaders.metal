#include <metal_stdlib>
using namespace metal;

// ── MATRIX MULTIPLY ──
// Each thread computes one element of C = A * B
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

// ── MEMORY BANDWIDTH ──
// Copies src buffer to dst — measures pure memory throughput
kernel void membench(
  device const float* src [[ buffer(0) ]],
  device       float* dst [[ buffer(1) ]],
  uint gid [[ thread_position_in_grid ]]
) {
  dst[gid] = src[gid];
}
