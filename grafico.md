```mermaid
classDiagram
    direction TB

    %% --- Clases Externas/Dependencias ---
    class std_vector~Node~ {
        <<STL Container>>
        +data Node*
        +size() size_t
        +push_back()
        +pop_back()
    }
    class std_mutex {
        <<OS Resource>>
        +lock()
        +unlock()
    }
    class forward_iterator {
        <<Vector Iterator Pattern>>
    }

    %% --- Clase HeapNode ---
    class HeapNode~T~ {
        -T m_data
        -Ref m_ref
        +HeapNode(data, ref)
        +GetData() T
        +GetRef() Ref
    }

    %% --- Estructuras de Configuración (Traits) ---
    class BaseContainerTrait {
        <<Interface>>
        +value_type
        +Node
    }
    class AscendingTrait { <<Interface>> +Comp }
    class DescendingTrait { <<Interface>> +Comp }

    class AscendingHeapTrait {
        <<Struct>>
    }
    class DescendingHeapTrait {
        <<Struct>>
    }

    %% --- Clase Principal Heap ---
    class Heap~Traits~ {
        <<Main Class>>
        %% Atributos Privados (Composición)
        -std_vector~Node~ m_heap
        -Comp m_comp
        -mutable std_mutex m_mtx
        
        %% Métodos Públicos Clave
        +Heap()
        +Heap(vector_pairs)
        +insert(value, ref)
        +extract() pair
        +build_heap(vector_pairs)
        +peek_root() value_type
        
        %% Métodos Públicos de Iteración/Funcionales
        +begin() forward_iterator
        +end() forward_iterator
        +ForEach(func)
        +FirstThat(func)
        
        %% Métodos Privados (Lógica interna)
        -heapify_up(index)
        -heapify_down(index)
    }

    %% --- Relaciones ---
    
    %% Configuración de Tipos
    AscendingHeapTrait --|> BaseContainerTrait
    AscendingHeapTrait --|> AscendingTrait
    DescendingHeapTrait --|> BaseContainerTrait
    DescendingHeapTrait --|> DescendingTrait
    
    %% Uso de Traits en Heap
    Heap ..> Traits : "Configurado por (Template)"
    Traits ..> HeapNode : "Define Node como"

    %% Composición (Lo que el Heap CONTIENE internamente)
    Heap *-- std_vector~Node~ : "m_heap (Almacenamiento)"
    std_vector~Node~ "1" *-- "*" HeapNode : "Contiene"
    Heap *-- std_mutex : "m_mtx (Hilo Seguro)"
    Heap *-- Traits_Comp : "m_comp (Lógica de Orden)"

    %% Herramientas que genera
    Heap ..> forward_iterator : "Crea (begin/end)"

    %% Notas Explicativas
    note for Heap "Implementación de Montículo\nUsando un Vector como Árbol Binario"
    note for HeapNode "Almacena la dupla\n(Dato de Prioridad + Referencia Externa)"
    ```