#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <string>

#include "types.h"

// Recursos globales compartidos
L resultado_global = 0; 
std::mutex balance_mutex;
//Objetivo emplear 10 hilos para incrementar un balance global en 200,000 pasos cada uno. 
//Para alcanzar un balance global total de 2 000 000.
// ============================================================================
// VARIANTE 1: Sin Mutex (Procesamiento paralelo desprotegido -> Race Condition)
// ============================================================================
void unsafe_deposit(TI iterations) {
    for (TI i = 0; i < iterations; ++i) 
        resultado_global++; 
}

// ============================================================================
// VARIANTE 2: Mutex en Bucle (Sincronización destructiva por alta contención)
// ============================================================================
void mutex_loop_deposit(TI iterations) {
    for (TI i = 0; i < iterations; ++i) {
        // Bloquea y desbloquea el mutex en CADA iteración (millones de veces)
        std::lock_guard<std::mutex> lock(balance_mutex);
        resultado_global++;
    }
}

// ============================================================================
// VARIANTE 3: Acumulación Local (Paralelismo real + Reducción al final)
// ============================================================================
void local_accumulate_deposit(TI iterations) {
    L local_balance = 0; // Memoria privada en el stack del hilo
    for (TI i = 0; i < iterations; ++i) 
        local_balance++; // Velocidad máxima del núcleo, sin competir con nadie
    // Al salir del bucle, consolidamos el resultado bloqueando el mutex UNA sola vez
    std::lock_guard<std::mutex> lock(balance_mutex);
    resultado_global += local_balance;
}

template <typename CountStrategy>
void run_simulation(CountStrategy strategy, const std::string& label, TI num_threads, TI iterations) {
    resultado_global = 0; 
    std::vector<std::thread> threads;
    auto start_time = std::chrono::high_resolution_clock::now();
    for (TI i = 0; i < num_threads; ++i) 
        threads.emplace_back(strategy, iterations);
    for (auto& t : threads) 
        t.join();
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<TD, std::milli> duration = end_time - start_time;

    std::cout << label << " Balance: " << resultado_global 
              << " | Tiempo: " << duration.count() << " ms\n";
}

int main() {
    const TI THREADS = 10;
    const TI ITERATIONS = 200000; 
    const L EXPECTED = (L)THREADS * ITERATIONS;

    std::cout << "=== PRUEBA DE RENDIMIENTO: CON PLANTILLAS ===\n";
    std::cout << "Hilos activos: " << THREADS << " | Pasos por hilo: " << ITERATIONS << "\n";
    std::cout << "Resultado correcto esperado: " << EXPECTED << "\n\n";

    run_simulation(unsafe_deposit, "[V1: SIN MUTEX]        ", THREADS, ITERATIONS);
    run_simulation(mutex_loop_deposit, "[V2: MUTEX EN BUCLE]   ", THREADS, ITERATIONS);
    run_simulation(local_accumulate_deposit, "[V3: ACUMULACION LOCAL]", THREADS, ITERATIONS);

    return 0;
}