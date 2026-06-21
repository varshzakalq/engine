#pragma once

#include "main_scene.h"
#include "timer.h"
#include <vector>
#include "lighting.h"
class Game{
private:
    Scene scene;
    std::vector<Instance*> cubes;
    std::vector<point_light> lights;
    bool initialized = false;
    Timer timer;
public:
    void update();
    const std::vector<Instance>& get_render_buffer_from_main_scene() const{
        return scene.get_render_buffer();
    }
    vector<point_light>& get_lights(){
        return lights;
    }

};
    

