#pragma once

#include "imgui.h"
#include "dataStructures//DataStore.h"

const float INDENT_DEPTH = 8.0;

void buildImguiMenu(DataStore &settings);

void buildImguiAppStats(DataStore &settings);

glm::vec2 ImVec2ToGlmVec2(ImVec2 vec);

void startIndent();

void endIndent();

void spacingAndSeperator();

void endAndNewIndent();