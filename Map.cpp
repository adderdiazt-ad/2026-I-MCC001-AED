#include <iostream>
#include <map>
#include <string>
#include <list>
#include <algorithm>
#include <iterator>
#include "types.h"
#include "foreach.h"
using namespace std;
void incremento5(pair<const string, TD>& item) {
    item.second *= 1.05; // Incrementa el precio en 5%
}
template <typename T, typename S>
void imprimir(pair<const T, S>& item) {
    cout << item.first << " -> " << item.second << "\n";
}
int main() {
    // Definimos el mapa: Producto (Clave) -> Precio (Valor)
    std::map<std::string, TD> catalogo;

    catalogo.emplace("Tablet", 299.99);
    catalogo.emplace("Laptop", 999.50);
    catalogo.emplace("Monitor", 150.75);
    catalogo.emplace("Teclado", 45.00);
    catalogo.emplace("Raton", 25.50);
    catalogo.emplace("Auriculares", 80.00);

    catalogo["Teclado"] = 40.00; // El operador [] actualiza el valor si la clave ya existe

    std::cout << "1. BUSQUEDA EXACTA (find)\n";
    std::cout << "---------------------------------\n";
    auto it_buscar = catalogo.find("Laptop");
    if (it_buscar != catalogo.end()) 
        std::cout << "Encontrado: " << it_buscar->first << " a $" << it_buscar->second << "\n\n";
    std::cout << "2. BUSQUEDA POR RANGOS (De Laptop a Tablet)\n";
    std::cout << "---------------------------------\n";
    
    auto it_inicio = catalogo.lower_bound("Laptop"); // Apunta a "Laptop" (o al siguiente mayor si no existiera)
    auto it_fin = catalogo.upper_bound("Tablet");    // Apunta al elemento estrictamente DESPUES de "Tablet"
    for (auto it = it_inicio; it != it_fin; ++it) {
        std::cout << it->first << " : $" << it->second << "\n";
    }
    std::cout << "\n";


    catalogo.erase("Raton"); // Producto descontinuado

    // 5. ITERACIÓN COMPLETA
    std::cout << "3. CATALOGO FINAL (Orden Alfabetico Garantizado)\n";
    std::cout << "---------------------------------\n";
    ForEach(catalogo.begin(), catalogo.end(), incremento5);
    ForEach(catalogo.begin(), catalogo.end(), imprimir<TS, TD>);

    std::list<std::pair<TI, TS>> lista_origen = {
        {3, "Carlos"},
        {1, "Ana"},
        {2, "Beatriz"}
    };

    // Destino: Construimos el mapa directamente con el rango de la lista
    std::map<TI, TS> mapa_destino(lista_origen.begin(), lista_origen.end());

    
    for (const auto& [id, nombre] : mapa_destino) {
        std::cout << id << " -> " << nombre << "\n";   
    }

    std::vector<TS> frutas = {"Manzana", "Pera", "Mango"};
    std::map<TS, TI> inventario;

    // Queremos que la fruta sea la Clave y su stock inicial (ej. 10) sea el Valor
    transform(frutas.begin(), frutas.end(), 
                   inserter(inventario, inventario.end()), 
                   [](const TS& fruta) {
                       return std::make_pair(fruta, 10); // Retorna el par Clave-Valor
                   });

    cout << "\nInventario Inicial:\n";                   
    ForEach(inventario.begin(), inventario.end(), imprimir<TS, TI>);
    return 0;
}