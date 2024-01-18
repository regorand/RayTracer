#pragma once

#include <map>
#include <string>
#include <optional>
#include "glm/vec2.hpp"

class DataStore {
private:

    template<typename T>
    struct DataElement {
        T* value;

        DataElement() {
            value = new T;
        }

        explicit DataElement(T data) {
            value = new T;
            *value = data;
        }

        ~DataElement() {
            delete value;
        }
    };

    std::map<int, DataElement<bool>> boolStore;
    std::map<int, DataElement<int>> intStore;
    std::map<int, DataElement<glm::vec2>> vec2Store;

public:
    DataStore();

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
    T getOrInit(int index) {
        std::map<int, DataElement<T>> *map = getMapP<T>();

        return *(*map)[index].value;
    }

    template<typename T>
    T getOrElse(int index, T elseVal) {
        std::optional<T> res = get<T>(index);
        return res.value_or(elseVal);
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
            // throwError(typeid(T).name());
            throwError("unknown type");
        }
        return mapOpt.value();
    }

    static void throwError(const std::string& typeName);
};

template<>
inline std::optional<std::map<int, DataStore::DataElement<bool>> *> DataStore::getMap() {
    return { &boolStore };
}

template<>
inline std::optional<std::map<int, DataStore::DataElement<int>> *> DataStore::getMap() {
    return { &intStore };
}

template<>
inline std::optional<std::map<int, DataStore::DataElement<glm::vec2>> *> DataStore::getMap() {
    return { &vec2Store };
}