#pragma once

struct MetalBenchResult{
    float gflops;   /* Compute througput */
    float bandwidth_gb; /* Memory bandwidth in GB/s */
    bool supported; /* False if Metal unavailable */
};

MetalBenchResult runMetalBenchmark();

