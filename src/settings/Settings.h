#pragma once

#include <map>
#include <string>

class Settings {
private:
    std::map<int, bool> boolStore;
    std::map<int, int> intStore;

public:
    Settings();

    template<typename T>
    void store(int index, T data);

    template<typename T>
    std::optional<T> get(int index);

private:
    template<typename T>
    void storeToMap(int index, std::map<int, T> &map, T value);

    template<typename T>
    std::optional<T> readFromMap(int index, std::map<int, T> &map);

};
