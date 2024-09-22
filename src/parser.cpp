#include "parser.h"
#include <fstream>
#include <iostream>

// Função para ler uma instância de arquivo
bool lerInstancia(const std::string &caminhoArquivo, std::vector<Pedido> &pedidos, std::vector<std::vector<int>> &setup)
{
    std::ifstream arquivo(caminhoArquivo);
    if (!arquivo.is_open())
    {
        std::cerr << "Erro ao abrir o arquivo: " << caminhoArquivo << std::endl;
        return false;
    }

    int n;
    arquivo >> n;
    pedidos.resize(n);
    setup.resize(n, std::vector<int>(n));

    for (int i = 0; i < n; i++)
    {
        arquivo >> pedidos[i].tempoProcessamento;
    }
    for (int i = 0; i < n; i++)
    {
        arquivo >> pedidos[i].prazo;
    }
    for (int i = 0; i < n; i++)
    {
        arquivo >> pedidos[i].multaPorMinuto;
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            arquivo >> setup[i][j];
        }
    }

    if (arquivo.fail())
    {
        std::cerr << "Erro ao ler dados do arquivo: " << caminhoArquivo << std::endl;
        return false;
    }

    return true;
}