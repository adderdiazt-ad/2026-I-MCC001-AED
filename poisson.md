## El Modelo Matemático: Distribución de Poisson

Cuando mapeamos $N$ elementos en $M$ casillas de forma aleatoria, la probabilidad de que una casilla específica reciba exactamente $k$ elementos se modela de forma precisa mediante la **Distribución de Poisson** (una aproximación de la distribución binomial cuando $M$ y $N$ son grandes):

$$P(X = k) = \frac{\alpha^k \cdot e^{-\alpha}}{k!}$$

Para determinar el número esperado de colisiones totales ($E[C]$), podemos plantearlo desde la perspectiva opuesta: una colisión ocurre cada vez que un elemento entra en una casilla que *ya no estaba vacía*.

---

### Análisis de Casillas Ocupadas

Por lo tanto, el número esperado de casillas que **no quedarán vacías** (es decir, que tendrán al menos un elemento) es:

$$\text{Casillas Ocupadas} = M \cdot (1 - P(X = 0)) = M \cdot (1 - e^{-\alpha})$$

Como cada casilla ocupada representa el "primer elemento" que colonizó ese nido, todos los demás elementos restantes que queden flotando son, por definición, colisiones. Así, el modelo para el número esperado de colisiones es:

$$E[C] = N - M \cdot (1 - e^{-\alpha})$$

Sustituyendo el factor de carga $\alpha = \frac{N}{M}$:

$$E[C] = N - M \cdot \left(1 - e^{-\frac{N}{M}}\right)$$

---

### Evaluando el Comportamiento del Modelo

Hagamos una prueba mental con este modelo analizando tus 3 factores:

* **Si aumentas el tamaño de la memoria ($M \to \infty$):** El término $e^{-\frac{N}{M}}$ se aproxima a $e^0 = 1$. Entonces el paréntesis $\left(1 - 1\right)$ se vuelve $0$, reduciendo las colisiones esperadas $E[C] \to 0$. Esto valida tu teoría: **a más memoria, menos choques.**
* **Si aumentas la carga ($N \gg M$):** El término $e^{-\frac{N}{M}}$ se vuelve un número infinitesimalmente cercano a $0$. El modelo se reduce a $E[C] \approx N - M$. Esto significa que cuando la tabla está masivamente saturada, **prácticamente cada nuevo elemento insertado se convierte en una colisión garantizada.**