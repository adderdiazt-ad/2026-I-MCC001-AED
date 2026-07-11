#include <iostream>
#include <map>
#include <string>
#include "types.h"
int main() {
    
    std::map<std::string, TI> torneo;

    std::cout << "1. INICIALIZANDO TORNEO...\n";
    
    torneo.emplace("Zelda", 1500);
    torneo.emplace("Mario", 2100);
    torneo.emplace("Aloy", 1850);
    torneo.emplace("Kratos", 3000);

    torneo["Mario"] += 100;  
    torneo["Samus"] = 2500;   // si no existe la clave entonces lo crea

  
    try {
        torneo.at("Aloy") += 50;/// .at lanza una excepción si no existe la clave
        
    } catch (const std::out_of_range& e) {
        std::cout << "Error: Intentaste modificar un jugador que no existe.\n";
    }

  
    torneo.erase("Mario"); // Mario es descalificado y borrado del árbol


    std::cout << "\n3. TABLA DE POSICIONES FINAL:\n";
    std::cout << "---------------------------------\n";
    
    for (const auto& [nombre, puntaje] : torneo) {
        std::cout << nombre << " \t : " << puntaje << " pts\n";
    }

    return 0;
}