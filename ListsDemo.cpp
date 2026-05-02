#include "linkedlist.h"

void LinkedListDemo(){
    // 
    LinkedList<DescendingLinkedListTrait<TI>> list1;
    list1.insert(6, 15);
    list1.insert(2, 25);
    list1.insert(9, 35);
    list1.insert(1, 45);
    list1.insert(7, 55);
    cout << "\nLista descendente: " << list1 << endl;

    list1.pop_back();
    cout << "Lista descendente, pop_back():  " << list1 << endl;
    list1.push_front(1, 45);
    cout << "Lista descendente, push_front(): " << list1 << endl;
    list1.pop_front();
    cout << "Lista descendente, pop_front():  " << list1 << endl;

    auto lista1Copy = list1; // Copy constructor
    cout << "Copia de lista descendente: " << lista1Copy << endl;

    LinkedList<AscendingLinkedListTrait<TI>> list2;
    list2.insert(6, 15);
    list2.insert(2, 25);
    list2.insert(9, 35);
    list2.insert(1, 45);
    list2.insert(7, 55);
    cout << "\nLista ascendente : " << list2 << endl;

    list2.push_back(6,65);
    cout << "Lista ascendente : " << list2 << endl;
    cout << "Lista ascendente : " << list2[5] << endl;
    
    cout << "Ingresa un nodo: ";
    while (cin >> list2) {
        // 3. Imprimimos el estado de la lista usando tu operator<<
        cout << " -> Lista actual: " << list2 << endl;
        cout << "Ingresa otro nodo: ";
    }
    cout << "\nLectura finalizada." << endl;
    cout << "Estado final de la lista: " << list2 << endl;


}

void ListsDemo(){
    LinkedListDemo();
    
}
