#ifndef __BASE_TRAIT_H__
#define __BASE_TRAIT_H__
#include <functional> // less, greater

using namespace std;

template <typename _T, typename _Node>
struct BaseContainerTrait{
    using value_type =  _T;
    using Node       =  _Node;
};

template <typename _T>
struct AscendingTrait{
    using Comp = less<_T>;
};

template <typename _T>
struct DescendingTrait{
    using Comp = greater<_T>;
};

template <typename _T, typename _Node, typename _logic>
struct customTrait: public BaseContainerTrait<_T, _Node>,
                    public _logic
{

};

#endif // __BASE_TRAIT_H__