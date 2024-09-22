#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <vector>
#include <string>

struct Pedido
{
    int tempoProcessamento;
    int prazo;
    int multaPorMinuto;
};

int calcularMultaTotal(const std::vector<Pedido> &pedidos, const std::vector<std::vector<int>> &setup);

std::vector<Pedido> algoritmoGuloso(const std::vector<Pedido> &pedidos, const std::vector<std::vector<int>> &setup);

std::vector<Pedido> buscaLocal(std::vector<Pedido> pedidos, const std::vector<std::vector<int>> &setup);

#endif