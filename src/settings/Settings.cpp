#include "Settings.h"

Settings::Settings() : boolStore() {}

template<>
std::optional<std::map<int, Settings::DataElement<bool>> *> Settings::getMap() {
    return { &boolStore };
}

template<>
std::optional<std::map<int, Settings::DataElement<int>> *> Settings::getMap() {
    return { &intStore };
}

void Settings::throwError(const std::string& typeName) {
    throw std::runtime_error("type: " + typeName + " is not yet implemented int settings store");
}
