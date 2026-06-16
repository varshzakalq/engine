#pragma once

#include "main_scene.h"
#include "timer.h"
#include <vector>

class Game{
private:
    Scene scene;
    std::vector<Instance*> cubes;
    bool initialized = false;
    Timer timer;
public:
    void update();
    const std::vector<Instance>& get_render_buffer_from_main_scene() const{
        return scene.get_render_buffer();
    }

};
    

