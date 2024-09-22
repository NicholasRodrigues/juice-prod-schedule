#include "algorithm.h"
#include "parser.h"
#include <iostream>
#include <vector>
#include <string>
#include <dirent.h> // Biblioteca POSIX para leitura de diretórios

// Função para processar uma instância e aplicar a solução
void processarInstancia(const std::string &caminhoArquivo)
{
    std::vector<Pedido> pedidos;
    std::vector<std::vector<int>> setup;

    if (!lerInstancia(caminhoArquivo, pedidos, setup))
    {
        std::cerr << "Erro durante a leitura da instância: " << caminhoArquivo << std::endl;
        return;
    }

    std::vector<Pedido> solucaoInicial = algoritmoGuloso(pedidos, setup);
    int multaInicial = calcularMultaTotal(solucaoInicial, setup);

    std::vector<Pedido> melhorSolucao = buscaLocal(solucaoInicial, setup);
    int multaFinal = calcularMultaTotal(melhorSolucao, setup);

    std::cout << "Arquivo: " << caminhoArquivo << " | Custo final: " << multaFinal << std::endl;
}

void processarInstanciasNaPasta(const std::string &caminhoPasta)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(caminhoPasta.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            std::string nomeArquivo = ent->d_name;

            if (nomeArquivo == "." || nomeArquivo == ".." || nomeArquivo == ".DS_Store")
            {
                continue;
            }

            std::string caminhoArquivo = caminhoPasta + "/" + nomeArquivo;
            processarInstancia(caminhoArquivo);
        }
        closedir(dir);
    }
    else
    {
        std::cerr << "Erro ao abrir o diretório: " << caminhoPasta << std::endl;
    }
}

int main()
{
    srand(time(0)); 
    std::string caminhoPasta = "data";

    processarInstanciasNaPasta(caminhoPasta);

    return 0;
}