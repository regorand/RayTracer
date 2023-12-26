#pragma once

#include <map>
#include <string>

class Settings {
private:

    template<typename T>
    struct DataElement {
        T* value;

        DataElement() {
            value = new T;
        }

        DataElement(T data) {
            value = new T;
            *value = data;
        }

        ~DataElement() {
            delete value;
        }
    };

    std::map<int, DataElement<bool>> boolStore;
    std::map<int, DataElement<int>> intStore;

public:
    Settings();

    template<typename T>
    void store(int index, T data) {
        std::map<int, DataElement<T>> *map = getMapP<T>();
        *(*map)[index].value = data;
    }

    template<typename T>
    std::optional<T> get(int index) {
        std::map<int, DataElement<T>> *map = getMapP<T>();

        auto res = map->find(index);
        if (res == map->end()) {
            return std::nullopt;
        }
        return {*(res->second.value)};
    }

    template<typename T>
    bool exists(int index) {
        auto map = getMapP<T>();

        return map->find(index) != map->end();
    }

    template<typename T>
    T *getPointer(int index) {
        std::map<int, DataElement<T>> *map = getMapP<T>();

        return (*map)[index].value;
    }
private:
    template<typename T>
    std::optional<std::map<int, DataElement<T>> *> getMap() {
        return std::nullopt;
    }

    template<typename T>
    std::map<int, DataElement<T>> *getMapP() {
        std::optional<std::map<int, DataElement<T>> *> mapOpt = getMap<T>();
        if (!mapOpt.has_value()) {
            throwError(typeid(T).name());
        }
        return mapOpt.value();
    }

    static void throwError(const std::string& typeName);
};