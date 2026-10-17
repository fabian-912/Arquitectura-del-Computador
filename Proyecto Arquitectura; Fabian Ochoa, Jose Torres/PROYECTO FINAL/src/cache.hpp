#pragma once

#include <cstdint>
#include <vector>
#include <list>
#include <algorithm>

struct CacheStats {
    uint64_t hits = 0;
    uint64_t misses = 0;

    double hitRatio() const {
        uint64_t total = hits + misses;
        return total == 0 ? 0.0 : static_cast<double>(hits) / static_cast<double>(total);
    }
};

class Cache {
public:
    Cache(std::size_t numLines, std::size_t associativity, std::size_t blockSizeBytes)
        : blockSizeBytes_(blockSizeBytes), associativity_(associativity) {
        numSets_ = numLines / associativity_;
        sets_.resize(numSets_);
    }

    void access(uint64_t address) {
        uint64_t blockNum = address / blockSizeBytes_;
        std::size_t setIdx = static_cast<std::size_t>(blockNum % numSets_);
        uint64_t tag = blockNum / numSets_;

        auto& set = sets_[setIdx];
        auto it = std::find(set.begin(), set.end(), tag);

        if (it != set.end()) {
            stats_.hits++;
            set.erase(it);
            set.push_front(tag);
            return;
        }

        stats_.misses++;
        if (set.size() >= associativity_) {
            set.pop_back();
        }
        set.push_front(tag);
    }

    const CacheStats& stats() const { return stats_; }
    std::size_t numSets() const { return numSets_; }
    std::size_t associativity() const { return associativity_; }
    std::size_t blockSizeBytes() const { return blockSizeBytes_; }

private:
    std::size_t blockSizeBytes_;
    std::size_t associativity_;
    std::size_t numSets_;
    std::vector<std::list<uint64_t>> sets_; // cada lista: LRU order de tags (front = mas reciente)
    CacheStats stats_;
};
