#include <iostream>
#include "Settings.h"

Settings::Settings() : boolStore() {}

template<>
void Settings::store<bool>(int index, bool data) {
    storeToMap(index, boolStore, data);
}

template<>
std::optional<int> Settings::get<int>(int index) {
    return readFromMap(index, intStore);
}

template<>
void Settings::store<int>(int index, int data) {
    storeToMap(index, intStore, data);
}

template<>
std::optional<bool> Settings::get<bool>(int index) {
    return readFromMap(index, boolStore);
}

template<typename T>
void Settings::storeToMap(int index, std::map<int, T> &map, T value) {
    map[index] = value;
}

template<typename T>
std::optional<T> Settings::readFromMap(int index, std::map<int, T> &map) {
    auto res = map.find(index);
    if (res == map.end()) {
        return std::nullopt;
    }
    return {res->second};
}

template<typename T>
void Settings::store(int index, T data) {
    std::cerr << "Storing values of type " << typeid(T).name() << " is not yet implemented" << std::endl;
}


template<typename T>
std::optional<T> Settings::get(int index) {
    std::cerr << "Storing values of type " << typeid(T).name() << " is not yet implemented" << std::endl;
    return std::nullopt;
}

