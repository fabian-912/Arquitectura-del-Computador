#pragma once

#include <vector>
#include <cstdint>
#include <random>

inline std::vector<uint64_t> generateTrace(std::size_t numAccesses,
                                            uint64_t addressSpaceBytes,
                                            double localityProb,
                                            std::size_t workingSetSize,
                                            unsigned seed = 42) {
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<uint64_t> fullRange(0, addressSpaceBytes - 1);
    std::uniform_real_distribution<double> coin(0.0, 1.0);

    std::vector<uint64_t> workingSet;
    workingSet.reserve(workingSetSize);
    for (std::size_t i = 0; i < workingSetSize; ++i) {
        workingSet.push_back(fullRange(rng));
    }
    std::uniform_int_distribution<std::size_t> pickFromSet(0, workingSetSize - 1);

    std::vector<uint64_t> trace;
    trace.reserve(numAccesses);

    for (std::size_t i = 0; i < numAccesses; ++i) {
        if (coin(rng) < localityProb) {
            trace.push_back(workingSet[pickFromSet(rng)]);
        } else {
            uint64_t addr = fullRange(rng);
            trace.push_back(addr);
            workingSet[pickFromSet(rng)] = addr; // el working set evoluciona
        }
    }
    return trace;
}
