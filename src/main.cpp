#include <iostream>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include "algorithm.h"
#include "parser.h"
#include "schedule_data.h"
#include <random>

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    // Check if a file was passed as an argument
    if (argc != 2) //checa se na execução o nome do arquivo foi passado
    {
        std::cerr << "Usage: " << argv[0] << " <instance_file_path>" << std::endl; //se nao for, retorna um erro
        return 1;
    }

    std::string filepath = argv[1]; //O path do arquivo sempre é o argumento que vem depois da própria execução do arquivo main
    if (!fs::exists(filepath)) //Checa se o arquivo existe naquele path solicitado
    {
        std::cerr << "Error: File does not exist: " << filepath << std::endl; //Retorna um erro caso o arquivo nao exista
        return 1;
    }

    std::vector<Order> orders; //Inicializa um vetor de Order, que é uma struct que contém os dados de cada um dos pedidos de uma squedule
    std::vector<std::vector<int>> setupTimes; //Inicializa um vetor de setupTimes de cada um dos tipos de suco
    std::vector<int> initialSetupTimes; //Inicializa um vetor de setupTimes inicial, onde temos o tempo de setup que cada um dos sucos precisa pra começar

    // Map of optimal penalties for each instance
    std::unordered_map<std::string, double> optimalPenalties = { //Aqui armazenamos em uma hash table os valores ótimos de cada um dos schedules solicitados no trabalho
            {"n60A", 453},
            {"n60B", 1757},
            {"n60C", 0},
            {"n60D", 69102},
            {"n60E", 58935},
            {"n60F", 93045},
            {"n60G", 64632},
            {"n60H", 145007},
            {"n60I", 43286},
            {"n60J", 53555},
            {"n60K", 458752},
            {"n60L", 332941},
            {"n60M", 516926},
            {"n60N", 492572},
            {"n60O", 527459},
            {"n60P", 396183}};

    std::string filename = fs::path(filepath).filename().string(); //Aqui pegamos apenas o nome do arquivo, sem o path dele
    std::string instanceName = filename.substr(0, filename.find('.')); //Aqui pegamos o nome desse arquivo e retiramos a extensao dele, pra ficar igual a hash table

    std::cout << "Processing file: " << filepath << std::endl;

    parseInputFile(filepath, orders, setupTimes, initialSetupTimes); //Aqui, passamos o arquivo pelo parser, para extrair cada uma das informações do pedido

    double totalPenaltyCost = 0.0; //Inicializamos o custo de penalidade total em 0
    auto start = std::chrono::high_resolution_clock::now(); //Começamos a contar o tempo de execução

    std::random_device rd; //Aqui inicializamos um random device
    unsigned int seed = rd(); //Aqui criamos uma variavel seed, para usarmos no random e garantirmos que mesmo dentro de uma randomizacao os resultados tenham um padrao escolhido por nos
    std::mt19937 rng(seed);

//    std::vector<int> schedule = {29, 57, 40, 54, 9, 10, 12, 19, 60, 50, 55, 15, 37, 34, 18, 16, 2, 8, 46, 51, 23, 44, 17, 20, 39, 26, 48, 41, 1, 25, 56, 58, 30, 21, 27, 28, 6, 36, 43, 3, 7, 45, 22, 13, 31, 42, 5, 14, 11, 49, 33, 47, 24, 35, 52, 32, 38, 4, 53, 59};
//    calculateTotalPenaltyForSchedule(schedule, orders, setupTimes, initialSetupTimes);
//    return 0;


    // Perform ILS optimization with Adaptive RVND
    std::vector<int> optimizedSchedule = GRASP(orders, setupTimes, initialSetupTimes, totalPenaltyCost, rng); //Aqui chamamos a função GRASP, que acaba sendo a principal do algoritmo inteiro, por executar todas as operações dentro dela

    // Create a ScheduleData object and populate it with the optimized schedule
    ScheduleData scheduleData;
    scheduleData.schedule = optimizedSchedule;

    // Post-optimization cost calculation using the ScheduleData object
    calculateTotalPenalty(scheduleData, orders, setupTimes, initialSetupTimes);

    // Output optimized results
    std::cout << "OPTIMIZED_SCHEDULE: ";
    for (int i : optimizedSchedule)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    std::cout << "TOTAL_PENALTY_OPTIMIZED: " << scheduleData.totalPenalty << std::endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "EXECUTION_TIME: " << elapsed.count() << std::endl;

    double optimalPenalty = optimalPenalties[instanceName];
    double gap = ((scheduleData.totalPenalty - optimalPenalty) / optimalPenalty) * 100;
    std::cout << "GAP_OPTIMIZED: " << gap << "%" << std::endl;


    return 0;
}
