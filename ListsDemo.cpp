#include "containers/linkedlist.h"
#include <fstream>
#include "containers/heap.h"
#include "util.h"
#include "types.h"
template <typename Node>
void Print(Node &node, ostream& os){
    os << node << ",";
}

template <typename Node>
void AddX(Node &node, typename Node::value_type value){
    node.getDataRef() += value;
}

template <typename Node>
void AddY(Node &node, typename Node::value_type value1, typename Node::value_type value2){
    node.GetDataRef() += value1 + value2;
}

template <typename Node, typename T>
bool IsLessThan(Node &node, T x){
    return node.GetDataRef() < x;
}

void LinkedListDemo(){
    /*
    LinkedList<DescendingLinkedListTrait<TI>> list1;
    list1.insert(6, 15);
    list1.insert(2, 25);
    list1.insert(9, 35);
    list1.insert(1, 45);
    list1.insert(7, 55);
    cout << "Lista descendente: " << list1 << endl;

    LinkedList<AscendingLinkedListTrait<TI>> list2;
    using LI = LinkedList<AscendingLinkedListTrait<TI>>::Node;
    list2.insert(6, 15);
    list2.insert(2, 25);
    list2.insert(9, 35);
    list2.insert(1, 45);
    list2.insert(7, 55);
    cout << "Lista ascendente : " << list2 << endl;

    list2.ForEach(AddX<LI>, 3);
    cout << "Prueba ForEach + 3: " << endl;
    list2.ForEach(Print<LI>, cout);
    list2.ForEach(AddY<LI>, 10, 11);
    cout << "Prueba ForEach + 10 + 11: " << endl;
    list2.ForEach(Print<LI>, cout);
    
    cout << "Prueba First That: " << endl;
    auto it = list2.FirstThat(IsGreaterThan<LI, TI>, 6);
    if (it != list2.end())
        cout << "Primer mayor a 6   : " << *it << endl;
    cout << "Fin recorrido con iteradores" << endl;

    cout << "Prueba PushFront: " << endl;
    list1.push_front(5, 5);
    cout << "Lista ascendente 1: " << list1 << endl;

    cout << "Prueba PopFront: " << endl;
    list1.pop_front();
    cout << "Lista ascendente 1: " << list1 << endl;

    LinkedList<AscendingLinkedListTrait<TI>> list3;
    cout << "Prueba PushBack: " << endl;
    list3.push_back(1, 10);
    list3.push_back(2, 20);
    list3.push_back(3, 30);
    cout << "Lista ascendente 3: " << list3 << endl;

    cout << "Prueba PopBack: " << endl;
    list3.pop_back();
    cout << "Lista ascendente 3: " << list3 << endl;

    list3.push_back(4, 40);
    cout << "Prueba Copy Constructor: " << endl;
    LinkedList<AscendingLinkedListTrait<TI>> list4(list3);
    cout << "Lista ascendente 4: " << list4 << endl;

    list3.push_back(5, 50);
    cout << "Prueba Move Constructor: " << endl;
    LinkedList<AscendingLinkedListTrait<TI>> list5 = move(list3);
    cout << "Lista ascendente 5: " << list5 << endl;

    cout << "Prueba del Destructor: " << endl;
    list4.~LinkedList();
    cout << "Lista ascendente 4: " << list4 << endl;

    cout << "Prueba del operador >>: "<<endl;
    ofstream ofs;
    ofs.open("lista1.txt");
    ofs <<  list1 <<   endl;
    ofs.close();

    ifstream file("lista1.txt");
    LinkedList<AscendingLinkedListTrait<TI>> list6;
    file >> list6;
    cout << "Lista 6 creada con datos de la lista 1: " << list6 << endl;

    cout << "Prueba operador []: " << endl;
    cout << "Lista5 [2]: " << list5[2] << endl;
    */

   std::cout << "========================================\n";
    std::cout << "   DEMO: ForEach y FirstThat (Heap)     \n";
    std::cout << "========================================\n\n";

    // Creamos un Max-Heap (Descendente) simulando tareas
    // Valor = Prioridad de la tarea, Ref = ID de la tarea
    Heap<AscendingHeapTrait<int>> tareas;
    ifstream file("lista1.txt");
    file >> tareas; // Cargar tareas desde un archivo
    std::cout << "Estado interno del Heap (Arreglo): " << tareas << "\n\n";

    // ==========================================
    // PRUEBA 1: Uso de ForEach
     using LI = Heap<AscendingHeapTrait<TI>>::Node;
     std::cout << "--- 1. Ejecutando ForEach ---" << std::endl;
    // ==========================================
    std::cout << "--- 1. Ejecutando ForEach ---" << std::endl;
    tareas.ForEach(AddY<LI>, 10, 11);
    cout << "Prueba ForEach + 10 + 11: " << endl;
    tareas.ForEach(Print<LI>, cout);
    std::cout << "\n========================================\n";
    auto it = tareas.FirstThat(IsLessThan<LI, TI>, 70);
    if (it != tareas.end())
        cout << "Primer menor a 70   : " << *it << endl;
    cout << "Fin recorrido con iteradores" << endl;

    std::cout << "\n========================================\n";
}

void ListsDemo(){
    LinkedListDemo();
}
