#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

#include "ShaderLoad.h"

#define STARTS_WITH(str, substr) (str.rfind(substr, 0) == 0)
#define CONTAINS(container, element) (std::find(container.begin(), container.end(), element) != container.end())

std::string loadShaderSource_impl(std::string path, bool resolveCustomSymbols, std::vector<std::string> &includedFiles) {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("ShaderOld source not found");
    }

    // if (std::find(includedFiles.begin(), includedFiles.end(), path) != includedFiles.end())
    if (CONTAINS(includedFiles, path)) {
        return "// NOT INCLUDED AGAIN"; // already contains this file
    }
    includedFiles.push_back(path);

    std::ostringstream outBuffer;
    std::ifstream inStream(path);

    size_t lineIndex = 0;
    std::string line;
    while (std::getline(inStream, line)) {
        lineIndex++;
        if (STARTS_WITH(line, INCLUDE_PATTERN)) {
            if (resolveCustomSymbols) {
                int len = line.size() - INCLUDE_PATTERN_LENGTH - 3; // subtract pattern length, one space and two " symbols, remain with path
                std::string includePath = line.substr(INCLUDE_PATTERN_LENGTH + 2, len); // skip space and first " symbol

                if (!std::filesystem::exists(includePath)) {
                    std::cerr << "Include " << includePath << " in File " << path << " on line " << lineIndex << " not found!" << std::endl;
                    continue;
                }

                outBuffer   << "\n// Included from " << includePath << "\n"
                            << loadShaderSource_impl(includePath, resolveCustomSymbols, includedFiles) << "\n"
                            << "// End Include" << includePath << "\n";
            }
        } else {
            outBuffer << line << "\n";
        }
    }

    inStream.close();

    return outBuffer.str();
}

std::string loadShaderSource(std::string path, bool resolveCustomSymbols) {
    std::vector<std::string> includedFiles;
    std::string combined = loadShaderSource_impl(path, resolveCustomSymbols, includedFiles);
    std::replace(path.begin(), path.end(), '/', '-');
    std::ofstream out(DEBUG_OUT_DIR + path);
    out << combined;
    out.close();
    return combined;

    /*
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("ShaderOld source not found");
    }

    std::ostringstream outBuffer;
    std::ifstream inStream(path);

    size_t lineIndex = 0;
    std::string line;
    while (std::getline(inStream, line)) {
        lineIndex++;
        if (STARTS_WITH(line, INCLUDE_PATTERN)) {
            if (resolveCustomSymbols) {
                int len = line.size() - INCLUDE_PATTERN_LENGTH - 3; // subtract pattern length, one space and two " symbols, remain with path
                std::string includePath = line.substr(INCLUDE_PATTERN_LENGTH + 2, len); // skip space and first " symbol

                if (!std::filesystem::exists(includePath)) {
                    std::cerr << "Include " << includePath << " in File " << path << " on line " << lineIndex << " not found!" << std::endl;
                    continue;
                }

                outBuffer   << "\n// Included from " << includePath << "\n"
                            << loadShaderSource(includePath, resolveCustomSymbols) << "\n"
                            << "// End Include" << includePath << "\n";
            }
        } else {
            outBuffer << line << "\n";
        }
    }

    return outBuffer.str();
     */
}
