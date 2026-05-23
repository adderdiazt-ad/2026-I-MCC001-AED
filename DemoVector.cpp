#include <iostream>
#include <thread>
#include <mutex>
#include "containers/vector.h"

using namespace std;

template <typename T>
void Print(VectorNode<T> &value, ostream& os){
    os << value << ",";
}

template <typename T>
void AddOne(VectorNode<T> &node){    
    static mutex mtx;
    scoped_lock<mutex> lock(mtx);
    ++node;
}

template <typename T>
void AddX(VectorNode<T> &node, T x){
    node += x;
}

template <typename T>
bool IsMultipleOf(VectorNode<T> &node, T x){
    return node.GetDataRef() % x == 0;
}

template <typename T>
bool IsGreaterThan(VectorNode<T> &node, T x){
    return node.GetDataRef() > x;
}

void DemoVector(){
    Vector< VectorTraits<TI> > v1;

    v1.push_back(7, 15);
    v1.push_back(5, 10);
    v1.push_back(25, 5);
    v1.push_back(10, 20);
    v1.push_back(13, 25);
    
    cout << "Recorrido con iteradores" << endl;
    v1.ForEach(Print<TI>, cout);
    cout << endl;
    v1.ForEach(AddOne<TI>);
    v1.ForEach(Print<TI>, cout);
    cout << endl;
    
    v1.ForEach(AddX<TI>, 10);
    v1.ForEach(Print<TI>, cout);
    cout << endl;
    int a = 3;
    v1.ForEach([a](auto& node){   node.GetDataRef() *= a; });
    v1.ForEach(Print<TI>, cout);
    cout << endl << a << endl;

    // Vector<TI>::forward_iterator it = v1.FirstThat(IsMultipleOf<TI>, 21);
    auto it = v1.FirstThat(IsMultipleOf<TI>, 21);
    if (it != v1.end())
        cout << "Primer multiplo de 21: " << *it << endl;
    // Vector<TI>::backward_iterator it2 = v1.ReverseFirstThat(IsMultipleOf<TI>, 21);
    auto it2 = v1.ReverseFirstThat(IsGreaterThan<TI>, 100);
    if (it2 != v1.rend())
        cout << "Primer mayor a 100   : " << *it2 << endl;
    cout << "Fin recorrido con iteradores" << endl;

    cout << v1.ToString() << endl;
    cout << "Vector:" << v1 << " despues" << endl;
//  cout.operator<<("Vector:");
//  ==========================
//               cout << v1 
    cout << "Size: " << v1.size() << endl;

    Vector< VectorTraits<TD> > v2;
    v2.push_back(7.5, 1);
    v2.push_back(5.2, 2);
    v2.push_back(25.1, 3);
    v2.push_back(10.3, 4);
    v2.push_back(13.4, 5);

    cout << v2.ToString() << endl;
    cout << "Size: " << v2.size() << endl;

    Vector< VectorTraits<TS> > v3;
    v3.push_back("Hello", 5);
    v3.push_back("World", 10);
    v3.push_back("AED", 15);

    cout << v3.ToString() << endl;
    v3.ForEach(AddX<TS>, "-X");
    cout << v3.ToString() << endl;
    v3.ReverseForEach(Print<TS>, cout);
    cout << endl;
    cout << "Size: " << v3.size() << endl;
}

// DemoConcurrentVector
void DemoConcurrentVector(){
    Vector< VectorTraits<TI> > v(4);
    v.push_back(0, 0);
    v.push_back(0, 0);
    v.push_back(0, 0);
    v.push_back(0, 0);

    // Cada thread itera el vector 100,000 veces e incrementa cada elemento
    // Sin sincronizacion → race condition en los contadores
    auto worker = [&v](int thread_id){
        for(int i = 0; i < 100000; i++)
            v.ForEach(AddOne<TI>);
        cout << "Thread " << thread_id << " terminado\n";
    };

    thread t1(worker, 1);
    thread t2(worker, 2);
    thread t3(worker, 3);
    thread t4(worker, 4);
    thread t5(worker, 5);

    t1.join(); t2.join(); t3.join(); t4.join(); t5.join();

    // Resultado esperado sin race condition: 4 elementos * 100000 * 5 threads = 500000
    cout << "Resultado (esperado 500000): " << v << endl;
}