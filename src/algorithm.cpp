#include "algorithm.h"
#include "neighborhoods.h"
#include <iostream>
#include <chrono>

double calculateTotalPenalty(const std::vector<int> &schedule, const std::vector<Order> &orders,
                             const std::vector<std::vector<int>> &setupTimes,
                             const std::vector<int> &initialSetupTimes)
{
    double totalPenaltyCost = 0.0;
    int currentTime = 0;
    int currentTask = -1;

    for (size_t i = 0; i < schedule.size(); ++i)
    {
        int taskId = schedule[i];
        const Order &order = orders[taskId];

        int setupTime = 0;
        if (currentTask >= 0)
        {
            setupTime = setupTimes[currentTask][taskId];
        }
        else
        {
            setupTime = initialSetupTimes[taskId];
        }
        currentTime += setupTime;

        currentTime += order.processingTime;

        if (currentTime > order.dueTime)
        {
            double penalty = order.penaltyRate * (currentTime - order.dueTime);
            totalPenaltyCost += penalty;
        }

        currentTask = taskId;
    }

    return totalPenaltyCost;
}

// Greedy Algorithm using Earliest Due Date (EDD) heuristic
std::vector<int> greedyAlgorithm(const std::vector<Order> &orders,
                                 const std::vector<std::vector<int>> &setupTimes,
                                 const std::vector<int> &initialSetupTimes,
                                 double &totalPenaltyCost)
{
    int n = orders.size();
    std::vector<int> schedule;
    int currentTime = 0;
    int currentTask = -1;

    totalPenaltyCost = 0.0;

    // Sort orders based on Earliest Due Date (EDD)
    std::vector<Order> sortedOrders = orders;
    std::sort(sortedOrders.begin(), sortedOrders.end(), [](const Order &a, const Order &b)
              { return a.dueTime < b.dueTime; });

    for (const auto &order : sortedOrders)
    {
        int taskId = order.id;

        int setupTime = (currentTask >= 0) ? setupTimes[currentTask][taskId] : initialSetupTimes[taskId];

        currentTime += setupTime + order.processingTime;

        if (currentTime > order.dueTime)
        {
            double penalty = order.penaltyRate * (currentTime - order.dueTime);
            totalPenaltyCost += penalty;
        }

        schedule.push_back(taskId);
        currentTask = taskId;
    }

    return schedule;
}

std::vector<int> adaptiveRVND(std::vector<int> &schedule, const std::vector<Order> &orders,
                              const std::vector<std::vector<int>> &setupTimes,
                              const std::vector<int> &initialSetupTimes)
{
    std::vector<std::function<bool(std::vector<int> &, const std::vector<Order> &,
                                   const std::vector<std::vector<int>> &,
                                   const std::vector<int> &)>>
        neighborhoods = {
            reinsertionNeighborhood,
            orOptNeighborhood,
            swapNeighborhood};

    std::vector<double> neighborhoodWeights(neighborhoods.size(), 1.0);
    std::mt19937 rng(std::random_device{}());

    bool improvement = true;
    int noImprovementCount = 0;

    while (improvement && noImprovementCount < MAX_NO_IMPROVEMENT_ITERATIONS)
    {
        adaptiveShuffle(neighborhoods, neighborhoodWeights, rng);

        improvement = false;
        for (size_t i = 0; i < neighborhoods.size(); ++i)
        {
            double previousPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);
            bool neighborhoodImprovement = neighborhoods[i](schedule, orders, setupTimes, initialSetupTimes);

            double currentPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);

            if (neighborhoodImprovement && currentPenalty < previousPenalty - IMPROVEMENT_THRESHOLD)
            {
                improvement = true;
                neighborhoodWeights[i] += 1.0;
                noImprovementCount = 0;
                break; // Restart with reshuffled neighborhoods
            }
            else
            {
                neighborhoodWeights[i] = std::max(0.1, neighborhoodWeights[i] * 0.9);
            }
        }

        if (!improvement)
        {
            noImprovementCount++;
        }
    }

    return schedule;
}

// Shuffle neighborhoods based on their weights
void adaptiveShuffle(std::vector<std::function<bool(std::vector<int> &, const std::vector<Order> &,
                                                    const std::vector<std::vector<int>> &,
                                                    const std::vector<int> &)>> &neighborhoods,
                     std::vector<double> &neighborhoodWeights, std::mt19937 &g)
{
    if (neighborhoods.empty() || neighborhoodWeights.empty())
    {
        std::cerr << "Error: Neighborhoods or weights are empty!" << std::endl;
        return;
    }

    // Normalize the weights
    double totalWeight = std::accumulate(neighborhoodWeights.begin(), neighborhoodWeights.end(), 0.0);
    std::vector<double> probabilities;
    if (totalWeight == 0.0)
    { // handling cases where all weights are zero
        for (size_t i = 0; i < neighborhoodWeights.size(); ++i)
        {
            probabilities.push_back(1.0 / neighborhoodWeights.size());
        }
    }
    else
    {
        for (double weight : neighborhoodWeights)
        {
            probabilities.push_back(weight / totalWeight);
        }
    }

    // Create a distribution based on normalized weights
    std::discrete_distribution<> dist(probabilities.begin(), probabilities.end());

    // Shuffle the neighborhoods based on the distribution
    std::vector<std::function<bool(std::vector<int> &, const std::vector<Order> &,
                                   const std::vector<std::vector<int>> &,
                                   const std::vector<int> &)>>
        shuffledNeighborhoods;

    // Ensuring that all neighborhoods are considered
    std::vector<int> indices(neighborhoods.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), g);

    for (int idx : indices)
    {
        shuffledNeighborhoods.push_back(neighborhoods[idx]);
    }

    neighborhoods = shuffledNeighborhoods;
}

// Perturbation function for ILS
void perturbSolution(std::vector<int> &schedule)
{
    int n = schedule.size();
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, n - 1);

    // Randomly select subsequence length
    int l = std::uniform_int_distribution<int>(PERTURBATION_STRENGTH_MIN, PERTURBATION_STRENGTH_MAX)(rng);

    // Perform a random reinsertion perturbation
    //  Using reinsertion perturbation as it is less destructive than other perturbation methods
    int i = dist(rng) % (n - l + 1);
    int j = dist(rng);

    if (j >= i && j <= i + l - 1)
    {
        j = (j + l) % n;
    }

    std::vector<int> subsequence(schedule.begin() + i, schedule.begin() + i + l);
    schedule.erase(schedule.begin() + i, schedule.begin() + i + l);
    int insertPos = (j > i) ? j - l : j;
    schedule.insert(schedule.begin() + insertPos, subsequence.begin(), subsequence.end());
}

std::vector<int> ILS(const std::vector<int> &initialSchedule, const std::vector<Order> &orders,
                     const std::vector<std::vector<int>> &setupTimes,
                     const std::vector<int> &initialSetupTimes)
{
    std::vector<int> bestSolution = initialSchedule;
    double bestPenalty = calculateTotalPenalty(bestSolution, orders, setupTimes, initialSetupTimes);

    std::vector<int> currentSolution = bestSolution;
    int noImprovementIterations = 0;
    int perturbationStrength = PERTURBATION_STRENGTH_MIN;

    for (int iter = 0; iter < MAX_ILS_ITERATIONS; ++iter)
    {
        currentSolution = adaptiveRVND(currentSolution, orders, setupTimes, initialSetupTimes);
        double currentPenalty = calculateTotalPenalty(currentSolution, orders, setupTimes, initialSetupTimes);

        if (currentPenalty < bestPenalty - IMPROVEMENT_THRESHOLD)
        {
            bestSolution = currentSolution;
            bestPenalty = currentPenalty;
            noImprovementIterations = 0;
            perturbationStrength = PERTURBATION_STRENGTH_MIN; // Reset perturbation strength
        }
        else
        {
            noImprovementIterations++;
            if (noImprovementIterations >= MAX_NO_IMPROVEMENT_ITERATIONS)
            {
                // Increase perturbation strength to escape local optima
                perturbationStrength = std::min(perturbationStrength + 1, PERTURBATION_STRENGTH_MAX);
                noImprovementIterations = 0;
            }
        }

        // Perturb the current solution
        perturbSolution(currentSolution);
    }

    return bestSolution;
}
