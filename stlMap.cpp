#include <iostream>
#include <map>
#include <string>
#include "types.h"


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
    if (it_buscar != catalogo.end()) {
        std::cout << "Encontrado: " << it_buscar->first << " a $" << it_buscar->second << "\n\n";
    }


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
    for (const auto& [producto, precio] : catalogo) {
        std::cout << producto << " \t : $" << precio << "\n";
    }

    return 0;
}