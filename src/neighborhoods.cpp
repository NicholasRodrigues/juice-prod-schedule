#include "neighborhoods.h"
#include "algorithm.h"
#include <algorithm>
#include <set>
#include <iostream>
#include "schedule_data.h"
bool blockExchangeNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                                         const std::vector<std::vector<int>> &setupTimes,
                                         const std::vector<int> &initialSetupTimes) {
    const int n = scheduleData.schedule.size();
    int best_i = -1, best_j = -1, best_l = -1;
    bool improvementFound = false;
    double bestPenalty = scheduleData.totalPenalty;

    // Considerar tamanhos de bloco entre 2 e 4
    for (int l = 2; l <= 4; ++l) {
        for (int i = 0; i <= n - 2 * l; ++i) {
            for (int j = i + l; j <= n - l; ++j) {
                double deltaPenalty = calculateDeltaPenalty(scheduleData, orders, setupTimes, initialSetupTimes, i, j, l);

                if (deltaPenalty < 0) { // Melhoria encontrada
                    double newPenalty = scheduleData.totalPenalty + deltaPenalty;
                    if (newPenalty < bestPenalty) {
                        bestPenalty = newPenalty;
                        best_i = i;
                        best_j = j;
                        best_l = l;
                        improvementFound = true;
                    }
                }
            }
        }
    }

    if (improvementFound) {
        // Realizar a melhor troca encontrada
        std::swap_ranges(scheduleData.schedule.begin() + best_i,
                        scheduleData.schedule.begin() + best_i + best_l,
                        scheduleData.schedule.begin() + best_j);

        // Atualizar a penalidade total com a alteração calculada
        scheduleData.totalPenalty = bestPenalty;

        // Incrementar o contador de melhorias (assumindo que essa variável existe)
        block_exchange_improvement_count++;
        return true;
    }

    return false;
}
// Função para calcular a alteração na penalidade ao trocar dois blocos
double calculateDeltaPenalty(const ScheduleData &scheduleData,
                             const std::vector<Order> &orders,
                             const std::vector<std::vector<int>> &setupTimes,
                             const std::vector<int> &initialSetupTimes,
                             int i, int j, int l) {
    double delta = 0.0;
    const std::vector<int> &schedule = scheduleData.schedule;
    const int n = schedule.size();

    // Validação dos índices
    if (i < 0 || j < 0 || l <= 0 || i + l > n || j + l > n || j < i + l) {
        // Índices inválidos ou blocos se sobrepõem
        return 0.0;
    }

    // Função lambda para obter o tempo de setup entre dois trabalhos
    auto getSetupTime = [&](int prevJob, int nextJob) -> double {
        if (prevJob == -1) {
            // Caso não haja trabalho anterior, usar o setup inicial
            return initialSetupTimes[nextJob];
        }
        return setupTimes[prevJob][nextJob];
    };

    // Identificar trabalhos afetados antes da troca
    int job_before_i = (i > 0) ? schedule[i - 1] : -1;         // -1 indica nenhum trabalho anterior
    int job_i = schedule[i];
    int job_after_i_block = schedule[i + l - 1];
    int job_j = schedule[j];
    int job_after_j_block = (j + l < n) ? schedule[j + l] : -1;

    // Identificar trabalhos afetados depois da troca
    int job_before_j = (j > 0) ? schedule[j - 1] : -1;
    // Após a troca, o bloco na posição j será o que estava em i

    // Antes da troca, calcular setup times das transições afetadas
    double oldPenalty = 0.0;
    oldPenalty += getSetupTime(job_before_i, job_i);                 // A: Antes de i
    oldPenalty += getSetupTime(job_after_i_block, job_j);            // B: Entre os blocos
    oldPenalty += getSetupTime(job_before_j, job_j);                 // C: Antes de j
    if (j + l < n) {
        oldPenalty += getSetupTime(job_after_j_block, schedule[j + l]); // D: Após j + l
    }

    // Depois da troca, calcular setup times das novas transições
    double newPenalty = 0.0;
    newPenalty += getSetupTime(job_before_i, job_j);                 // A': Antes de i para j
    if (i + l < n) {
        newPenalty += getSetupTime(job_after_j_block, schedule[i + l]); // B': Após j + l para após i + l
    }
    newPenalty += getSetupTime(job_before_j, job_i);                 // C': Antes de j para i
    if (j + l < n) {
        newPenalty += getSetupTime(job_after_i_block, schedule[j + l]); // D': Após i + l para após j + l
    }

    // Cálculo da diferença na penalidade
    delta = newPenalty - oldPenalty;

    return delta;
}
bool blockShiftNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                            const std::vector<std::vector<int>> &setupTimes,
                            const std::vector<int> &initialSetupTimes)
{
    const int n = scheduleData.schedule.size();
    int best_i = -1, best_j = -1, best_l = -1;
    bool improvementFound = false;
    double bestPenalty = scheduleData.totalPenalty;

    // Consider block sizes from 1 to 5
    for (int l = 1; l <= 10; ++l)
    {
        for (int i = 0; i <= n - l; ++i)
        {
            for (int j = 0; j <= n - l; ++j)
            {
                if (j >= i && j <= i + l - 1) continue;  // Skip overlapping positions

                ScheduleData tempScheduleData = scheduleData;

                // Perform block shift in the temporary schedule
                std::vector<int> block(tempScheduleData.schedule.begin() + i, tempScheduleData.schedule.begin() + i + l);
                tempScheduleData.schedule.erase(tempScheduleData.schedule.begin() + i, tempScheduleData.schedule.begin() + i + l);

                // Adjust the insertion position if necessary
                int adjusted_j = j;
                if (j > i)
                {
                    adjusted_j -= l;
                }

                tempScheduleData.schedule.insert(tempScheduleData.schedule.begin() + adjusted_j, block.begin(), block.end());

                // Recalculate total penalty for the temporary schedule
                calculateTotalPenalty(tempScheduleData, orders, setupTimes, initialSetupTimes);

                if (const double newPenalty = tempScheduleData.totalPenalty; newPenalty < bestPenalty)
                {
                    block_shift_improvement_count++;
                    bestPenalty = newPenalty;
                    best_i = i;
                    best_j = j;
                    best_l = l;
                    improvementFound = true;
                }
            }
        }
    }

    if (improvementFound)
    {
        // Apply the best block shift to the actual schedule
        std::vector<int> block(scheduleData.schedule.begin() + best_i, scheduleData.schedule.begin() + best_i + best_l);
        scheduleData.schedule.erase(scheduleData.schedule.begin() + best_i, scheduleData.schedule.begin() + best_i + best_l);

        // Adjust the insertion position if necessary
        int adjusted_j = best_j;
        if (best_j > best_i)
        {
            adjusted_j -= best_l;
        }

        scheduleData.schedule.insert(scheduleData.schedule.begin() + adjusted_j, block.begin(), block.end());

        // Recalculate total penalty for the actual schedule
        calculateTotalPenalty(scheduleData, orders, setupTimes, initialSetupTimes);

        return true;
    }

    return false;
}

// 2-Opt Neighborhood (Reverses two segments of the schedule)
bool twoOptNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                        const std::vector<std::vector<int>> &setupTimes,
                        const std::vector<int> &initialSetupTimes) {
    const int n = scheduleData.schedule.size();
    int best_i = -1, best_j = -1;
    bool improvementFound = false;
    double bestPenalty = scheduleData.totalPenalty;

    for (int i = 0; i < n - 1; ++i) {
        // Limit j to ensure the block size does not exceed 10
        const int max_j = std::min(n - 1, i + 4); // i + 9 ensures block size <= 10
        for (int j = i + 1; j <= max_j; ++j) {
            ScheduleData tempScheduleData = scheduleData;

            // Apply the 2-opt move in the temporary schedule
            std::reverse(tempScheduleData.schedule.begin() + i, tempScheduleData.schedule.begin() + j + 1);

            // Recalculate total penalty for the temporary schedule
            calculateTotalPenalty(tempScheduleData, orders, setupTimes, initialSetupTimes);

            if (const double newPenalty = tempScheduleData.totalPenalty; newPenalty < bestPenalty) {
                bestPenalty = newPenalty;
                best_i = i;
                best_j = j;
                improvementFound = true;
            }
        }
    }

    if (improvementFound) {
        // Apply the best 2-opt move to the actual schedule
        std::reverse(scheduleData.schedule.begin() + best_i, scheduleData.schedule.begin() + best_j + 1);

        // Recalculate total penalty for the actual schedule
        calculateTotalPenalty(scheduleData, orders, setupTimes, initialSetupTimes);

        // Increment the improvement counter
        two_opt_improvement_count++;
        return true;
    }

    return false;
}
