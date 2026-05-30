//#include "containers/linkedlist.h"
#include <fstream>
#include "containers/binarytree.h"
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
void AddY(Node &node, TI value1, TI value2){
    node.getDataRef() += value1 + value2;
}

template <typename Node, typename T>
bool IsLessThan(Node &node, T x){
    return node.getDataRef() < x;
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

   BinaryTree<AscendingBinaryTreeTrait<TI>> tree;
    tree.insert(5, 15);
    tree.insert(3, 25);
    tree.insert(7, 35);
    tree.insert(2, 45);
    tree.insert(4, 55);
    tree.insert(6, 65);
    tree.insert(8, 75);
    tree.insert(1, 85);

    cout << "Recorrido inorder forward: " << endl;
    for (auto it = tree.begin_forward_inorder(); it != tree.end_forward_inorder(); ++it) {
        cout << *it << ",";
    }
    cout <<"\n";
    
    cout << "\nRecorrido inorder backward: " << endl;
    for (auto it = tree.begin_backward_inorder(); it != tree.end_backward_inorder(); ++it) {
        cout << *it << ",";
    }
    using JI = BinaryTree<AscendingBinaryTreeTrait<TI>>::Node;
    cout << "\n-------------------\n";
    
    tree.ForEach(AddY<JI>, 10, 5);
    for (auto it = tree.begin_forward_preorder(); it != tree.end_forward_preorder(); ++it) {
        cout << *it << ",";
    }
    
    cout << "\nPrueba First That: " << endl;
    cout << "\n\n";
    tree.ForEach(Print<JI>, cout);
    cout << "\n\n";
    auto it = tree.FirstThat(IsLessThan<JI, TI>, 25);
    if (it != tree.end_forward_inorder())
        cout << "Primer menor a 5   : " << *it << endl;
    cout << "Fin recorrido con iteradores" << endl;
    cout<< "\n-------------------\n";
    cout<<tree<<endl;


    cout<< "\n--------PRUEBA DE ESCRITURA Y LECTURA-----------\n";
    std::cout << "Tamano actual: " << tree.size() << " nodos." << std::endl;

    // 2. GUARDAR EN DISCO (Usando tu operador <<)
    std::string filename = "arbol.bin";
    {
        std::ofstream outFile(filename, std::ios::binary);
        if (outFile.is_open()) {
            std::cout << "Guardando arbol en " << filename << "..." << std::endl;
            outFile << tree; 
            outFile.close();
            std::cout << "Guardado con exito." << std::endl;
        }
    }

    // 3. RECUPERAR DESDE EL DISCO (Usando tu operador >>)
    BinaryTree<AscendingBinaryTreeTrait<TI>> newTree;
    {
        std::ifstream inFile(filename, std::ios::binary);
        if (inFile.is_open()) {
            std::cout << "Recuperando arbol desde " << filename << "..." << std::endl;
            inFile >> newTree;
            inFile.close();
            std::cout << "Recuperacion exitosa." << std::endl;
        }
    }


    std::cout << "Tamano del nuevo arbol: " << newTree.size() << std::endl;






}


void ListsDemo(){
    LinkedListDemo();
}