
#pragma once
#include <stdexcept>

/*
 * Simple class overriding runtime_error so I have specific error to throw and to catch
 */
class import_scene_error: public std::runtime_error {
public:
    import_scene_error(std::string message)
        : std::runtime_error(message) {}
};
