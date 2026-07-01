#ifndef __FOREACH_H__
#define __FOREACH_H__
#include <iostream>
#include <utility> // forward
#include <type_traits>
#include <functional>
using namespace std;

template <typename Iterator, typename Func, typename... Args>
void ForEach(Iterator begin, Iterator end, Func func, Args &&... args){
    for (auto it = begin; it != end; ++it)
        func(*it, forward<Args>(args)...);
    // cout<<endl;
}

// Variadic templates: template <typename ...Args>
// Variadic templates allow a function or class to accept an arbitrary
// number of arguments.
// Example: template <typename ...Args> func() { // ... }
template <typename Iterator, typename Func, typename... Args>
Iterator FirstThat(Iterator begin, Iterator end, Func func, Args &&... args){
    for (auto it = begin; it != end; ++it){
        if (func(*it, forward<Args>(args)...))
            return it;
    }
    return end;
}

template <typename Container, typename Func, typename... Args>
void ForEach(Container& v1, Func func, Args &&... args){
    ForEach(v1.begin(), v1.end(), func, forward<Args>(args)...);
}


template<typename Iter, typename Func, typename... Args>
decltype(auto) walk(Iter begin, Iter end, Func func, Args&&... args){
    using resultType= invoke_result_t<Func,decltype(*begin),Args...>;
    constexpr bool is_void = is_void_v<resultType>;
    for(auto it = begin; it !=end ; ++it){
        if constexpr (is_void){
            invoke(func,*it,forward<Args>(args)...);
        }else{
            if(invoke(func,*it,forward<Args>(args)...)) 
            return it;
        }
    }
    if constexpr(!is_void)
        return end;
}
#endif // __FOREACH_H__