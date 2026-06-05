#include <SDL3/SDL.h>
#include "engine.h"

#include <vector>

int main(int argc, char* argv[]) {
    // 1. Initialize SDL3 Video system
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL could not initialize! Error: %s", SDL_GetError());
        return -1;
    }

    int width = 800;
    int height = 600;

    // Pointers for our window and renderer
    SDL_Window* window = nullptr;
    SDL_Renderer* sdlRenderer = nullptr;

    // 2. SDL3 creates BOTH the window and the canvas renderer in one shot
    if (!SDL_CreateWindowAndRenderer("SDL3 Software Renderer", width, height, 0, &window, &sdlRenderer)) {
        SDL_Log("Window/Renderer creation failed: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // 3. Create the Texture (The blank poster that holds our pixel array)
    // SDL_PIXELFORMAT_XRGB8888 means: 8 bits for Red, 8 bits for Green, 8 bits for Blue (32-bit total integer)
    SDL_Texture* texture = SDL_CreateTexture(
        sdlRenderer, 
        SDL_PIXELFORMAT_XRGB8888, 
        SDL_TEXTUREACCESS_STREAMING, 
        width, height
    );

    if (!texture) {
        SDL_Log("Texture creation failed: %s", SDL_GetError());
        SDL_DestroyRenderer(sdlRenderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // 4. custom array! 800 * 600 integers.
    std::vector<uint32_t> pixel_array(width * height, 0);

    bool is_running = true;
    SDL_Event event;

    // Create the engine instance and initialize it
    Engine my_engine;
    my_engine.init(pixel_array.data(), width, height);

    // --- THE MAIN LOOP ---
    while (is_running) {
        
        // Check for window events (like clicking the close X button)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                is_running = false;
            }
        }

        //will update the engine with modified pixels
        
        my_engine.update_and_render();

        // 6. UPDATE THE SDL3 TEXTURE
        // This uploads your raw array data directly onto the texture
        SDL_UpdateTexture(texture, NULL, pixel_array.data(), width * sizeof(uint32_t));

        // 7. PRESENT TO MONITOR
        SDL_RenderClear(sdlRenderer);                        // Clear old frame
        SDL_RenderTexture(sdlRenderer, texture, NULL, NULL); // Draw our texture to the window
        SDL_RenderPresent(sdlRenderer);                      // Push to screen
    }

    // --- CLEAN UP (SDL3 Style) ---
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}