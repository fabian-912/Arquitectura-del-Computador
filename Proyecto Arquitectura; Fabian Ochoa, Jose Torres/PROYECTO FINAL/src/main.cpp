#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "cache.hpp"
#include "filesystem_profile.hpp"
#include "trace_generator.hpp"

struct RunResult {
    std::string fsName;
    std::size_t blockSize;
    std::size_t associativity;
    CacheStats stats;
};

std::vector<uint64_t> loadTraceFromFile(const std::string& path) {
    std::ifstream in(path);
    std::vector<uint64_t> trace;
    uint64_t addr;
    while (in >> addr) trace.push_back(addr);
    return trace;
}

int main(int argc, char** argv) {
    constexpr std::size_t numLines = 256;
    constexpr uint64_t addressSpaceBytes = 8ULL * 1024 * 1024; // 8 MB de "disco"
    constexpr std::size_t numAccesses = 200000;

    std::vector<uint64_t> trace;
    if (argc > 1) {
        trace = loadTraceFromFile(argv[1]);
        std::cout << "Traza cargada desde " << argv[1] << " (" << trace.size() << " accesos)\n\n";
    } else {
        trace = generateTrace(numAccesses, addressSpaceBytes, 0.85, 64);
        std::cout << "Traza sintetica generada (" << trace.size() << " accesos, localidad 85%)\n\n";
    }

    std::vector<FileSystemProfile> filesystems = {
        {"FAT",  512},
        {"Ext2", 4096},
    };

    std::vector<std::size_t> associativities = {1, 2, 4, 8};

    std::vector<RunResult> results;

    for (const auto& fs : filesystems) {
        for (auto assoc : associativities) {
            Cache cache(numLines, assoc, fs.blockSizeBytes);
            for (auto addr : trace) cache.access(addr);
            results.push_back({fs.name, fs.blockSizeBytes, assoc, cache.stats()});
        }
    }

    std::cout << std::left
              << std::setw(8)  << "FS"
              << std::setw(14) << "BloqueBytes"
              << std::setw(14) << "Asociatividad"
              << std::setw(12) << "Hits"
              << std::setw(12) << "Misses"
              << std::setw(10) << "HitRatio" << "\n";
    std::cout << std::string(70, '-') << "\n";

    for (const auto& r : results) {
        std::string assocLabel = (r.associativity == 1) ? "Directo" : (std::to_string(r.associativity) + "-vias");
        std::cout << std::left
                  << std::setw(8)  << r.fsName
                  << std::setw(14) << r.blockSize
                  << std::setw(14) << assocLabel
                  << std::setw(12) << r.stats.hits
                  << std::setw(12) << r.stats.misses
                  << std::fixed << std::setprecision(4) << r.stats.hitRatio() << "\n";
    }

    std::cout << "\nComparacion por sistema de archivos (mejor esquema de cache):\n";
    for (const auto& fs : filesystems) {
        const RunResult* best = nullptr;
        for (const auto& r : results) {
            if (r.fsName == fs.name && (best == nullptr || r.stats.hitRatio() > best->stats.hitRatio())) {
                best = &r;
            }
        }
        if (best) {
            std::cout << "  " << fs.name << " (bloque " << fs.blockSizeBytes << " bytes) -> mejor asociatividad: "
                      << best->associativity << ", hit ratio = "
                      << std::fixed << std::setprecision(4) << best->stats.hitRatio() << "\n";
        }
    }

    return 0;
}
