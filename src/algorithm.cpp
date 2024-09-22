#include "algorithm.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <climits>

int calcularMultaTotal(const std::vector<Pedido> &pedidos, const std::vector<std::vector<int>> &setup)
{
    int tempoAtual = 0;
    int multaTotal = 0;
    int n = pedidos.size();

    for (int i = 0; i < n; i++)
    {
        int atraso = std::max(0, tempoAtual + pedidos[i].tempoProcessamento - pedidos[i].prazo);
        multaTotal += atraso * pedidos[i].multaPorMinuto;
        tempoAtual += pedidos[i].tempoProcessamento;

        if (i + 1 < n)
        {
            tempoAtual += setup[i][i + 1]; 
        }
    }
    return multaTotal;
}

std::vector<Pedido> algoritmoGuloso(const std::vector<Pedido> &pedidos, const std::vector<std::vector<int>> &setup)
{
    std::vector<Pedido> ordenados = pedidos;
    std::sort(ordenados.begin(), ordenados.end(), [&](const Pedido &a, const Pedido &b)
              {
        double prioridadeA = (double)a.multaPorMinuto / (a.tempoProcessamento + setup[0][0]);
        double prioridadeB = (double)b.multaPorMinuto / (b.tempoProcessamento + setup[0][0]);
        return prioridadeA > prioridadeB; });
    return ordenados;
}

std::vector<Pedido> buscaLocal(std::vector<Pedido> pedidos, const std::vector<std::vector<int>> &setup)
{
    bool melhoria = true;

    while (melhoria)
    {
        melhoria = false;

        std::vector<void (*)(std::vector<Pedido> &)> vizinhancas = {
            [](std::vector<Pedido> &pedidos)
            {
                if (pedidos.size() < 2)
                    return;
                int i = rand() % pedidos.size();
                int j = rand() % pedidos.size();
                std::swap(pedidos[i], pedidos[j]);
            },
            [](std::vector<Pedido> &pedidos)
            {
                if (pedidos.size() < 2)
                    return;
                int i = rand() % pedidos.size();
                Pedido pedidoRemovido = pedidos[i];
                pedidos.erase(pedidos.begin() + i);
                int j = rand() % pedidos.size();
                pedidos.insert(pedidos.begin() + j, pedidoRemovido);
            },
            [](std::vector<Pedido> &pedidos)
            {
                if (pedidos.size() < 2)
                    return;
                int inicio = rand() % pedidos.size();
                int fim = inicio + rand() % (pedidos.size() - inicio);
                std::reverse(pedidos.begin() + inicio, pedidos.begin() + fim + 1);
            },
            [](std::vector<Pedido> &pedidos)
            {
                if (pedidos.size() < 4)
                    return;
                int tam = pedidos.size();
                int blocoTam = rand() % (tam / 2) + 1;
                int i = rand() % (tam - blocoTam);
                int j = rand() % (tam - blocoTam);
                for (int k = 0; k < blocoTam; ++k)
                {
                    std::swap(pedidos[i + k], pedidos[j + k]);
                }
            }};

        for (int k = 0; k < vizinhancas.size(); k++)
        {
            std::vector<Pedido> novaSolucao = pedidos;
            vizinhancas[k](novaSolucao); 

            int multaAtual = calcularMultaTotal(pedidos, setup);
            int multaNova = calcularMultaTotal(novaSolucao, setup);

            if (multaNova < multaAtual)
            {
                pedidos = novaSolucao; 
                melhoria = true;
                break;
            }
        }
    }

    return pedidos;
}