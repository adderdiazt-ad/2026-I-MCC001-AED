#include <iostream>
#include <variant>
#include <vector>
#include <string>

#include "types.h"

template<class... Ts> 
struct overloaded : Ts... { 
    using Ts::operator()...; 
};

// Guía de deducción explícita (Obligatoria en C++17, opcional a partir de C++20)
template<class... Ts> 
overloaded(Ts...) -> overloaded<Ts...>;


// ============================================================================
// 2. LAS ENTIDADES (Estructuras de datos puras, sin herencia tradicional)
// ============================================================================
struct Warrior {
    TS name;
    TI sword_sharpness;
    void charge() const {
        std::cout << "[Guerrero] " << name << " arremete con espada (Filo: " << sword_sharpness << "%).\n";
    }
};

struct Mage {
    TS name;
    TI mana;
    void cast_spell() const {
        std::cout << "[Mago] " << name << " lanza una bola de fuego (Mana restante: " << mana << ").\n";
    }
};

struct Archer {
    TS name;
    TI arrows;
    void shoot() const {
        std::cout << "[Arquero] " << name << " dispara una flecha (Flechas: " << arrows << ").\n";
    }
};


// ============================================================================
// 3. EL CONTENEDOR POLIMÓRFICO MODERNO
// ============================================================================
using Enemy = std::variant<Warrior, Mage, Archer>;


int main() {
   
    std::vector<Enemy> unidades;

    unidades.push_back(Warrior{"Ragnar", 95});
    unidades.push_back(Mage{"Gandalf", 150});
    unidades.push_back(Archer{"Legolas", 30});
    unidades.push_back(Warrior{"Thorin", 80});

    std::cout << "=== SIMULACION DE ENCUENTRO (Polimorfismo con std::variant) ===\n\n";

    for (const auto& enemy : unidades) {
        // std::visit evalúa en tiempo de compilación qué tipo contiene la variante
        // y ejecuta la lambda correspondiente con tipado estático seguro.
        std::visit(overloaded {
            [](const Warrior& w) { w.charge(); },
            [](const Mage& m)    { m.cast_spell(); },
            [](const Archer& a)  { a.shoot(); }
        }, enemy);
    }

    std::cout << "\n=== INSPECCION Y MODIFICACION DE DATOS RELEVANTES ===\n\n";

    for (auto& enemy : unidades) {
        std::visit(overloaded {
            [](Warrior& w) { 
                w.sword_sharpness -= 10; 
                std::cout << w.name << " desgasto su espada.\n";
            },
            [](Mage& m) { 
                m.mana -= 20; 
                std::cout << m.name << " consumio mana.\n";
            },
            [](Archer& a) { 
                a.arrows--; 
                std::cout << a.arrows << " flechas le quedan a " << a.name << ".\n";
            }
        }, enemy);
    }

    return 0;
}