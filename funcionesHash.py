import math
import matplotlib.pyplot as plt

# ==========================================
# 1. DEFINICIÓN DE LAS FUNCIONES HASH
# ==========================================

def hash_malo(clave, tamano_memoria):
    hash_valor = 5381  # Un número primo mágico de partida
    cadena= str(clave)
    for caracter in cadena:
        # La ecuación matemática es: (hash_valor * 33) + valor_ascii
        # Pero escrita con bits para que sea ultra rápida:
        hash_valor = ((hash_valor << 5) + hash_valor) + ord(caracter)
    return hash_valor % tamano_memoria

def hash_bueno(clave, tamano_memoria):
    # Hash nativo de Python para romper patrones
    return abs(hash(str(clave))) % tamano_memoria

# ==========================================
# 2. CONTADOR DE COLISIONES
# ==========================================

def contar_colisiones(funcion_hash, claves, tamano_memoria):
    casillas_ocupadas = set()
    colisiones = 0
    for clave in claves:
        indice = funcion_hash(clave, tamano_memoria)
        if indice in casillas_ocupadas:
            colisiones += 1
        else:
            casillas_ocupadas.add(indice)
    return colisiones

# ==========================================
# 3. CONFIGURACIÓN DE LA SIMULACIÓN
# ==========================================

M = 1000  # Tamaño de memoria fijo (Casillas)
# Generamos el eje X: Carga (N) variando desde 50 hasta 2000 elementos
cargas = list(range(50, 2050, 50))  

colisiones_teoricas = []
colisiones_reales_bueno = []
colisiones_reales_malo = []

# ==========================================
# 4. EJECUCIÓN DEL EXPERIMENTO
# ==========================================

for N in cargas:
    # Creamos un patrón de datos del mundo real: IDs secuenciales de 10 en 10
    claves_con_patron = [i * 10 for i in range(1, N + 1)]
    
    # A. Calcular valor Teórico (Modelo de Poisson)
    alpha = N / M
    ec = N - M * (1 - math.exp(-alpha))
    colisiones_teoricas.append(ec)
    
    # B. Simular colisiones con el Hash Bueno
    cb = contar_colisiones(hash_bueno, claves_con_patron, M)
    colisiones_reales_bueno.append(cb)
    
    # C. Simular colisiones con el Hash Malo
    cm = contar_colisiones(hash_malo, claves_con_patron, M)
    colisiones_reales_malo.append(cm)

# ==========================================
# 5. GENERACIÓN DEL GRÁFICO
# ==========================================

plt.figure(figsize=(12, 7))

# Línea del Modelo Matemático
plt.plot(cargas, colisiones_teoricas, label='Teórica (Modelo de Poisson)', 
         color='blue', linestyle='--', linewidth=2.5)

# Línea del Hash Bueno
plt.plot(cargas, colisiones_reales_bueno, label='Hash Bueno (Azar Uniforme)', 
         color='green', marker='o', markersize=5, alpha=0.8)

# Línea del Hash Malo
plt.plot(cargas, colisiones_reales_malo, label='Hash Malo (Lineal con Patrón)', 
         color='red', marker='x', markersize=6, alpha=0.8)

# Elementos visuales de guía
plt.axvline(x=M, color='purple', linestyle=':', linewidth=1.5, 
            label=f'Límite de memoria (M = {M})')

# Configuración de etiquetas y estilo
plt.title('Evaluación de Funciones Hash: Número de Colisiones vs Carga', fontsize=14, fontweight='bold')
plt.xlabel('Carga de la Tabla (Cantidad de elementos N)', fontsize=12)
plt.ylabel('Número Total de Colisiones', fontsize=12)
plt.grid(True, linestyle=':', alpha=0.6)
plt.legend(fontsize=11, loc='upper left')

# Mostrar el gráfico en pantalla
plt.show()