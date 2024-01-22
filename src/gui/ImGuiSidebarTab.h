#pragma once

#include <vector>
#include "dataStructures//DataStore.h"
#include "dataStructures/Events.h"

struct ImGuiSidebarTab {
    virtual void buildImguiTab(DataStore &settings, std::vector<Event> &events) = 0;
};