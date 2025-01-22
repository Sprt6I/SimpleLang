#pragma once
#include <unordered_map>

template <typename T, typename K>
bool dic_find(std::unordered_map<T, K> dic, T val) {
    for (auto i : dic) {
        if ( i.first == val ) { return true; }
    }
    
    return false;
}