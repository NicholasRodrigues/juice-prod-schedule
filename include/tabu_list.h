#ifndef TABU_LIST_H
#define TABU_LIST_H

#include <queue>
#include <unordered_set>

struct TabuList {
    std::unordered_set<size_t> tabuSet; // For O(1) lookup
    std::vector<size_t> tabuBuffer;      // Circular buffer
    int maxSize;
    int currentIndex;

    TabuList(int maxSize) : maxSize(maxSize), currentIndex(0) {
        tabuBuffer.reserve(maxSize);
    }

    void addSolution(size_t hash) {
        if (tabuBuffer.size() < maxSize) {
            tabuBuffer.push_back(hash);
            tabuSet.insert(hash);
        }
        else {
            size_t oldHash = tabuBuffer[currentIndex];
            tabuSet.erase(oldHash);
            tabuBuffer[currentIndex] = hash;
            tabuSet.insert(hash);
            currentIndex = (currentIndex + 1) % maxSize;
        }
    }

    bool isTabu(size_t hash) const {
        return tabuSet.find(hash) != tabuSet.end();
    }

    void clear() {
        tabuSet.clear();
        tabuBuffer.clear();
        currentIndex = 0;
    }
};

#endif // TABU_LIST_H
