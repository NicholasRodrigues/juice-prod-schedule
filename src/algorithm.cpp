#include "algorithm.h"
#include "neighborhoods.h"
#include <iostream>
#include <chrono>
// nao entendi a diferenca entre o setup e initialsetup
double calculateTotalPenalty(const std::vector<int> &schedule, const std::vector<Order> &orders,
                             const std::vector<std::vector<int>> &setupTimes,
                             const std::vector<int> &initialSetupTimes)
{
    double totalPenaltyCost = 0.0; // custo total da penalidade
    int currentTime = 0;           // tempo da ultima tarefa processada
    int currentTask = -1;          // id da ultima tarefa processada, comeca sem nenhuma

    for (size_t i = 0; i < schedule.size(); ++i) // passa por cada pedido do schedule
    {
        int taskId = schedule[i];
        const Order &order = orders[taskId]; // referencia constante para o pedido atual da lista de orders

        int setupTime = 0;    //
        if (currentTask >= 0) // se a tarefa atual ja tiver sido definida
        {
            setupTime = setupTimes[currentTask][taskId]; // pega o tempo de setup entre a tarefa atual e a proxima
        }
        else
        {
            setupTime = initialSetupTimes[taskId]; // se nao, pega o tempo de setup inicial
        }
        currentTime += setupTime; // adiciona o tempo de setup ao tempo atual

        currentTime += order.processingTime; // adiciona o tempo de processamento ao tempo atual

        if (currentTime > order.dueTime) // caso o pedido esteja atrasado
        {
            double penalty = order.penaltyRate * (currentTime - order.dueTime); // calcula a penalidade
            totalPenaltyCost += penalty;                                        // soma a penalidade com a total
        }

        currentTask = taskId; // define a currentTask como task_id
    }

    return totalPenaltyCost;
}

// Greedy Algorithm using Earliest Due Date (EDD) heuristic
std::vector<int> greedyAlgorithm(const std::vector<Order> &orders,
                                 const std::vector<std::vector<int>> &setupTimes,
                                 const std::vector<int> &initialSetupTimes,
                                 double &totalPenaltyCost)
{
    int n = orders.size();     //
    std::vector<int> schedule; // vetor de tarefas
    int currentTime = 0;       // tempo atual
    int currentTask = -1;      // tarefa atual

    totalPenaltyCost = 0.0; // custo total da penalidade

    // Sort orders based on Earliest Due Date (EDD)
    std::vector<Order> sortedOrders = orders; // copia a lista de pedidos
    std::sort(sortedOrders.begin(), sortedOrders.end(), [](const Order &a, const Order &b)
              { return a.dueTime < b.dueTime; }); //[](const Order &a, const Order &b) { return a.dueTime < b.dueTime; } funcao LAMBDA
                                                  // ordena os pedidos pela data de entrega de forma crescente,
    // priorizando os pedidos mais perto de serem entregues, afim de minimizar a penalidade

    for (const auto &order : sortedOrders) // o auto é um tipo de variavel que o compilador atribui automaticamente, o & faz com que ele nao precise
                                           //  copiar as informacoes do vetor, e cada elemento do vetor é atribuido a variavel order
    {
        int taskId = order.id; // pega o id do pedido

        int setupTime = (currentTask >= 0) ? setupTimes[currentTask][taskId] : initialSetupTimes[taskId]; // se ja houve um pedido anteriro, ele pega o tempo de setup entre eles
                                                                                                          // se nao, ele pega o tempo de setup inicial

        currentTime += setupTime + order.processingTime; // adiciona o tempo de setup e processamento ao tempo atual

        if (currentTime > order.dueTime) // se o pedido estiver atrasado
        {
            double penalty = order.penaltyRate * (currentTime - order.dueTime); // calcula a penalidade
            totalPenaltyCost += penalty;                                        // soma a penalidade com o total
        }

        schedule.push_back(taskId); // adiciona o pedido ao schedule na ultima posicao
        currentTask = taskId;       // define a tarefa atual como a ultima adicionada
    }

    return schedule; // nada mais é do que a ordem do guloso
}

std::vector<int> adaptiveRVND(std::vector<int> &schedule, const std::vector<Order> &orders, // funcao que aplica a RVND
                              const std::vector<std::vector<int>> &setupTimes,
                              const std::vector<int> &initialSetupTimes)
{
    std::vector<std::function<bool(std::vector<int> &, const std::vector<Order> &,
                                   const std::vector<std::vector<int>> &,
                                   const std::vector<int> &)>>
        neighborhoods = {
            reinsertionNeighborhood,
            orOptNeighborhood,
            swapNeighborhood}; // vetor que contem as vizinhanças que vao ser aplicadas nos movimentos de busca local

    std::vector<double> neighborhoodWeights(neighborhoods.size(), 1.0); // vetor que contem os pesos das vizinhanças, que sao feitos de orde aleatoria por ser RVND
    std::mt19937 rng(std::random_device{}());

    bool improvement = true;    // flag de melhora
    int noImprovementCount = 0; // contador de melhora

    while (improvement && noImprovementCount < MAX_NO_IMPROVEMENT_ITERATIONS) // o while continua enquanto tiver melhoras ou o
                                                                              // numero de movimentos seguidos sem melhora forem menores que o maximo
    {
        adaptiveShuffle(neighborhoods, neighborhoodWeights, rng); // embaralha as vizinhanças

        improvement = false;                              // comeca assumindo que nao ha melhorias
        for (size_t i = 0; i < neighborhoods.size(); ++i) // passa por todos os tipos de vizinhancas
        {
            double previousPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);  // antes de calcular a vizinhanca ele pega a penalidade atual
            bool neighborhoodImprovement = neighborhoods[i](schedule, orders, setupTimes, initialSetupTimes); // aplica a vizinhanca i na solucao, e caso tenha
                                                                                                              // melhora (alguma movimentacao), retorna true

            double currentPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes); // recalcula a penalidade da solucao depois de aplicar a vizinhanca

            if (neighborhoodImprovement && currentPenalty < previousPenalty - IMPROVEMENT_THRESHOLD) // verifica se a nova penalidade é menor e se essa mudanca foi significativa
            {
                improvement = true;
                neighborhoodWeights[i] += 1.0; // caso tenha melhorado, o peso da vizinhanca aumenta
                noImprovementCount = 0;        // reseta o contador de movimentos sem melhora
                break;                         // Restart with reshuffled neighborhoods
            }
            else
            {
                neighborhoodWeights[i] = std::max(0.1, neighborhoodWeights[i] * 0.9); // caso nao tenha melhorado, o peso da vizinhanca diminui
            }
        }

        if (!improvement)
        {
            noImprovementCount++; // caso nao tenha melhorado, o contador de movimentos sem melhora aumenta
        }
    }

    return schedule; // retorna a solucao
}

// Shuffle neighborhoods based on their weights
void adaptiveShuffle(std::vector<std::function<bool(std::vector<int> &, const std::vector<Order> &, // funcao que embaralha as vizinhanças
                                                    const std::vector<std::vector<int>> &,
                                                    const std::vector<int> &)>> &neighborhoods, // vizinhancas existentes
                     std::vector<double> &neighborhoodWeights, std::mt19937 &g)                 // pesos das vizinhanças e gerador de numeros aleatorios
{
    if (neighborhoods.empty() || neighborhoodWeights.empty()) // checa se existem vizinhancas e pesos iniciais pra elas
    {
        std::cerr << "Error: Neighborhoods or weights are empty!" << std::endl;
        return;
    }

    // Normalize the weights
    double totalWeight = std::accumulate(neighborhoodWeights.begin(), neighborhoodWeights.end(), 0.0); // soma os pesos das vizinhanças
    std::vector<double> probabilities;                                                                 // vetor de probabilidades
    for (double weight : neighborhoodWeights)                                                          // passa por todos os pesos
    {
        probabilities.push_back(weight / totalWeight); // calcula a probabilidade de cada vizinhança (peso pelo total)
    }

    // Create a distribution based on normalized weights
    std::discrete_distribution<> dist(probabilities.begin(), probabilities.end()); // cria uma distribuicao discreta baseada nas probabilidades (valores)

    // Shuffle the neighborhoods based on the distribution
    std::vector<std::function<bool(std::vector<int> &, const std::vector<Order> &,
                                   const std::vector<std::vector<int>> &,
                                   const std::vector<int> &)>>

        shuffledNeighborhoods;

    // Ensuring that all neighborhoods are considered
    std::vector<int> indices(neighborhoods.size());  // preenche os vetores de índices com numeros de 0 a n-1 (n = numero de vizinhancas)
    std::iota(indices.begin(), indices.end(), 0);    // e embaralha eles, garantindo que todas as vizinhanças sejam consideradas de forma
    std::shuffle(indices.begin(), indices.end(), g); // aleatória e que as vizinhanças com maior peso tenham mais chances de serem escolhidas

    for (int idx : indices) // passa por todos os indices
    {
        shuffledNeighborhoods.push_back(neighborhoods[idx]); // adiciona as vizinhanças embaralhadas
    }

    neighborhoods = shuffledNeighborhoods; // atualiza as vizinhanças
}

// Perturbation function for ILS
void perturbSolution(std::vector<int> &schedule) // funcao que perturba a solucao, usada para escapar de otimos locais
                                                    // permite exploracao de novas regioes do espaço de busca
{
    int n = schedule.size();
    std::mt19937 rng(std::random_device{}()); //gerador de numeros aleatorios
    std::uniform_int_distribution<int> dist(0, n - 1); //distibuição uniforme de 0 a n-1

    // Randomly select subsequence length
    int l = std::uniform_int_distribution<int>(PERTURBATION_STRENGTH_MIN, PERTURBATION_STRENGTH_MAX)(rng); //seleciona um tamanho de subsequencia aleatorio
                                                                                                            // que sera retirado da solucao e inserido em outra posicao
    // Perform a random reinsertion perturbation    
    //  Using reinsertion perturbation as it is less destructive than other perturbation methods
    int i = dist(rng) % (n - l + 1); //seleciona uma posicao aleatoria i para ser removida 
    int j = dist(rng); //seleciona a posicao j para ser inserida

    if (j >= i && j <= i + l - 1) //caso o j caia dentro do I, ele é ajustado
    {
        j = (j + l) % n;
    }

    std::vector<int> subsequence(schedule.begin() + i, schedule.begin() + i + l);
    schedule.erase(schedule.begin() + i, schedule.begin() + i + l);
    int insertPos = (j > i) ? j - l : j;
    schedule.insert(schedule.begin() + insertPos, subsequence.begin(), subsequence.end());
} // a perturbação é feita retirando uma subsequencia de tamanho l da solução e inserindo em outra posição

std::vector<int> ILS(const std::vector<int> &initialSchedule, const std::vector<Order> &orders, // funcao que implementa o ILS
                     const std::vector<std::vector<int>> &setupTimes,
                     const std::vector<int> &initialSetupTimes)
{
    std::vector<int> bestSolution = initialSchedule; // melhor solucao, que comeca como a solucao inicial
    double bestPenalty = calculateTotalPenalty(bestSolution, orders, setupTimes, initialSetupTimes); // penalidade da melhor solucao

    std::vector<int> currentSolution = bestSolution; //solucao atual, que vai ser modificada durante o processo, iniciando com a bestsolution, que tambem é a inicial
    int noImprovementIterations = 0; //interacoes sem melhora
    int perturbationStrength = PERTURBATION_STRENGTH_MIN; //força da perturbacao

    for (int iter = 0; iter < MAX_ILS_ITERATIONS; ++iter) //passa por todos os elementos ate o max de iteracoes definido
    {
        currentSolution = adaptiveRVND(currentSolution, orders, setupTimes, initialSetupTimes); //usa o RVND para encontrar a melhor solucao
        double currentPenalty = calculateTotalPenalty(currentSolution, orders, setupTimes, initialSetupTimes); //calcula a penalidade da solucao atual

        if (currentPenalty < bestPenalty - IMPROVEMENT_THRESHOLD) // verifica se a nova penalidade é menor e se essa mudanca foi significativa
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
