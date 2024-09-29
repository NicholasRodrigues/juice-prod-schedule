#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include "parser.h"
#include "order.h"

void parseInputFile(const std::string &filename, std::vector<Order> &orders,
                    std::vector<std::vector<int>> &setupTimes,
                    std::vector<int> &initialSetupTimes)
{
    std::ifstream file(filename); // Recebe o nome do arquivo e tenta abri-lo

    if (!file.is_open())
    { // Caso o arquivo não tenha sido aberto, ele retorna o Error opening file
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    int numOrders;     // variavel responsavel por armazenar o numero de pedidos
    file >> numOrders; // lê o primeiro valor do arquivo e armazena na variavels numOrders

    // Skip the empty line
    std::string line;
    std::getline(file, line); // To move to the next line after numOrders
    std::getline(file, line); // Empty line

    orders.resize(numOrders); // ajusta o tamanho do vetor orders para o numero de arquivos lidos (VOLTAR DEPOIS)

    // Read processing times (array t)
    for (int i = 0; i < numOrders; ++i) // Percorre todos os pedidos
    {
        file >> orders[i].processingTime; //Armazena na posicao i da struct order na variavel processingTime
        orders[i].id = i; //define tambem o ID da order a partir desse mesmo i
    }

    // Read due times (array p)
    for (int i = 0; i < numOrders; ++i) // Percorre todos os pedidos
    {
        file >> orders[i].dueTime; // Armazena na posicao i da struct order na variavel due time
    }

    // Read penalty rates (array w)
    for (int i = 0; i < numOrders; ++i) // Percorre todos os pedidos
    {
        file >> orders[i].penaltyRate; // Armazena na posicao i da struct orders na variavel penaltyRate
        if (orders[i].penaltyRate < 0) //A taxa nunca pode ser inferiror a 0 
        {
            std::cerr << "Error: Negative penalty rate for order " << i << std::endl;
            orders[i].penaltyRate = 0.0; //Ajusta ela para 0
        }
    }

    // Skip the empty line before the setup times matrix
    std::getline(file, line);
    std::getline(file, line); // Empty line

    // Read initial setup times (s0j)
    initialSetupTimes.resize(numOrders); //ajusta o tamanho do vetor initialSetupTimes para o numero de pedidos
    for (int j = 0; j < numOrders; ++j) // Percorre todos os pedidos
    {
        if (!(file >> initialSetupTimes[j])) //Lê o tempo de preparacao e retorna um erro caso nao consiga fazer isso
        {
            std::cerr << "Error reading initial setup time for order " << j << std::endl;
            initialSetupTimes[j] = 0;
        }
    }

    // Read setup times matrix (sij)
    setupTimes.resize(numOrders, std::vector<int>(numOrders)); // ajusta o tamanho da matriz setupTimes para o numero de pedidos

    for (int i = 0; i < numOrders; ++i) // Percorre todos os pedidos em um loop aninhado para criar uma matriz
    {
        for (int j = 0; j < numOrders; ++j)
        {
            if (!(file >> setupTimes[i][j]))
            {
                std::cerr << "Error reading setup time between orders " << i << " and " << j << std::endl;
                setupTimes[i][j] = 0;
            }
            if (setupTimes[i][j] < 0)
            {
                std::cerr << "Error: Negative setup time between orders " << i << " and " << j << std::endl;
                setupTimes[i][j] = 0;
            }
        }
    }

    file.close();
}
