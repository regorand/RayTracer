#include "DataStore.h"

DataStore::DataStore() : boolStore() {}

template<>
std::optional<std::map<int, DataStore::DataElement<bool>> *> DataStore::getMap() {
    return { &boolStore };
}

template<>
std::optional<std::map<int, DataStore::DataElement<int>> *> DataStore::getMap() {
    return { &intStore };
}

template<>
std::optional<std::map<int, DataStore::DataElement<glm::vec2>> *> DataStore::getMap() {
    return { &vec2Store };
}

void DataStore::throwError(const std::string& typeName) {
    throw std::runtime_error("type: " + typeName + " is not yet implemented in data store");
}
