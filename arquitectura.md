```mermaid
classDiagram
    direction TB

    %% ==========================================
    %% DECLARACIÓN DE JERARQUÍA (Fuerza el layout Top-Down)
    %% ==========================================
    general_node <|-- LLNode
    general_node <|-- DLLNode
    
    general_iterator <|-- forwardIterator
    general_iterator <|-- backwardIterator
    
    LinkedList <|-- DoubleLinkedList
    LinkedList <|-- CircleLinkedList
    DoubleLinkedList <|-- CircleDoubleLinkedList

    %% ==========================================
    %% CAPA 1: BASES GENÉRICAS Y ABSTRACTAS (Cima)
    %% ==========================================
    class general_node~T~ {
        #T m_data
        #Ref m_ref
        +getData() T
        +getRef() Ref
        +setData(T) void
        +setRef(Ref) void
    }

    class general_iterator~Node~ {
        #Node* m_pCurrent
        +operator*() T
        +operator!=() bool
        +operator==() bool
    }

    %% ==========================================
    %% CAPA 2: NODOS, ITERADORES Y LISTA BASE
    %% ==========================================
    class LLNode~T~ {
        #LLNode* m_pNext
        +getNext() LLNode*
        +setNext(LLNode*) void
    }

    class DLLNode~T~ {
        #DLLNode* m_pNext
        #DLLNode* m_pPrev
        +getNext() DLLNode*
        +setNext(DLLNode*) void
        +getPrev() DLLNode*
        +setPrev(DLLNode*) void
    }

    class forwardIterator~Container~ {
        +operator++() forwardIterator
    }

    class backwardIterator~Container~ {
        +operator--() backwardIterator
    }

    class LinkedList~Traits~ {
        #Node* m_pRoot
        #Node* m_pTail
        #size_t m_size
        #mutex m_mtx
        +push_back(T, Ref)
        +push_front(T, Ref)
        +insert(T, Ref)
        +pop_front() pair
        +pop_back() pair
        +ForEach(Func)
        +FirstThat(Func) forwardIterator
        +begin() forwardIterator
        +end() forwardIterator
    }

    %% ==========================================
    %% CAPA 3: LISTAS ESPECIALIZADAS
    %% ==========================================
    class DoubleLinkedList~Traits~ {
        #internal_insert()
        +insert(T, Ref)
        +push_back(T, Ref)
        +push_front(T, Ref)
        +pop_front() pair
        +pop_back() pair
        +rbegin() backwardIterator
        +rend() backwardIterator
    }

    class CircleLinkedList~Traits~ {
        +insert(T, Ref)
        +push_back(T, Ref)
        +push_front(T, Ref)
        +pop_front() pair
        +pop_back() pair
        +ForEach(Func)
        +FirstThat(Func) forwardIterator
    }

    %% ==========================================
    %% CAPA 4: COMPOSICIÓN FINAL (Base del diagrama)
    %% ==========================================
    class CircleDoubleLinkedList~Traits~ {
        -breakCircularLinks()
        -fixCircularLinks()
        +insert(T, Ref)
        +push_back(T, Ref)
        +push_front(T, Ref)
        +pop_front() pair
        +pop_back() pair
        +ForEach(Func)
        +FirstThat(Func) forwardIterator
        +ReverseForEach(Func)
    }

    %% ==========================================
    %% RELACIONES DE DEPENDENCIA Y COMPOSICIÓN
    %% ==========================================
    
    LinkedList "1" *-- "many" LLNode : Usa
    DoubleLinkedList "1" *-- "many" DLLNode : Usa

    LinkedList ..> forwardIterator : Crea
    DoubleLinkedList ..> backwardIterator : Crea
    DoubleLinkedList ..> forwardIterator : Crea
    
    
    ```