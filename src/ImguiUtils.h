#pragma once

#include "imgui.h"
#include "settings/DataStore.h"

void buildImguiMenu(DataStore &settings);

void buildImguiAppStats(DataStore &settings);

glm::vec2 ImVec2ToGlmVec2(ImVec2 vec);