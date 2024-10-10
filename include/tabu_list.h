#ifndef TABU_LIST_H
#define TABU_LIST_H

#include <queue>
#include <unordered_set>

struct TabuList {
    std::queue<size_t> tabuQueue;       // FIFO queue for Tabu List
    std::unordered_set<size_t> tabuSet; // O(1) lookup of Tabu solutions
    int maxSize;                        // Maximum length of the tabu list

    TabuList(int maxSize) : maxSize(maxSize) {}

    // Add a new solution to the Tabu List
    void addSolution(size_t hash) {
        if (tabuSet.size() >= maxSize) {
            size_t oldHash = tabuQueue.front();
            tabuQueue.pop();
            tabuSet.erase(oldHash);
        }
        tabuQueue.push(hash);
        tabuSet.insert(hash);
    }

    // Check if a solution is in the Tabu List
    bool isTabu(size_t hash) const {
        return tabuSet.find(hash) != tabuSet.end();
    }

    // Clear the Tabu List
    void clear() {
        std::queue<size_t> emptyQueue;
        std::swap(tabuQueue, emptyQueue);
        tabuSet.clear();
    }
};

#endif // TABU_LIST_H
