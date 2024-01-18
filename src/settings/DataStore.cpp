#include <stdexcept>
#include "DataStore.h"

DataStore::DataStore() : boolStore() {}

void DataStore::throwError(const std::string& typeName) {
    throw std::runtime_error("type: " + typeName + " is not yet implemented in data store");
}
