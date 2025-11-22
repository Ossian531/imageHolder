#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;
using namespace std::chrono;

SDL_HitTestResult MyHitTestCallback(SDL_Window* window, const SDL_Point* area, void* data) {
    return SDL_HITTEST_DRAGGABLE;
}

int main(int argc, char *argv[]){
    fs::path choosenPath;
    if (argc > 1) {
        choosenPath = fs::current_path() / argv[1];
    }

    else {
    
        const std::string home = getenv("HOME");
        const std::string dirPath = home + "/Pictures/Screenshots/";


        fs::file_time_type mostRecentTime;
        bool first = true;

        for (const auto& entry : fs::directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                fs::file_time_type lastWriteTime = fs::last_write_time(entry);
                if (lastWriteTime > mostRecentTime | first) {
                    mostRecentTime = lastWriteTime;
                    choosenPath = entry.path();
                    first = false;
                }
            }
        }
    }


    SDL_SetHint(SDL_HINT_VIDEODRIVER, "x11");
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface* imageSurface = IMG_Load(choosenPath.string().c_str());
    if (!imageSurface) {
        SDL_Quit();
        return 1;
    }

    int startposX, startposY, width = imageSurface->w, height = imageSurface->h;
    SDL_GetGlobalMouseState(&startposX, &startposY);
    SDL_Window* window = SDL_CreateWindow("Holder", startposX, startposY, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS | SDL_WINDOW_SKIP_TASKBAR);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);
    SDL_RenderCopy(renderer,imageTexture, NULL, NULL);
    bool quit = false;
    SDL_Event event;
    SDL_SetWindowHitTest(window, MyHitTestCallback, nullptr);

    while (!quit) {
        SDL_RenderClear(renderer); // Clear the screen first
        SDL_RenderCopy(renderer, imageTexture, NULL, NULL); // Draw the texture to the screen
        SDL_RenderPresent(renderer);
        if (SDL_WaitEvent(&event) != 0) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_q) {
                        quit = true;
                    }
                    else if(event.key.keysym.sym == SDLK_r){
                        imageSurface = IMG_Load(choosenPath.string().c_str());
                        if (!imageSurface) {
                            SDL_Quit();
                            return 1;
                        }
                        imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
                        SDL_FreeSurface(imageSurface);
                        SDL_RenderCopy(renderer,imageTexture, NULL, NULL);
                        SDL_RenderPresent(renderer);
                    }
                    else if(event.key.keysym.sym == SDLK_MINUS && (event.key.keysym.mod & KMOD_CTRL)) {
                        width *= 0.8;
                        height *= 0.8;
                        if (width == 0 || height == 0) {
                            quit = true;
                            break;
                        }
                        SDL_SetWindowSize(window, width, height);
                        SDL_RenderCopy(renderer,imageTexture, NULL, NULL);
                    }
                    else if(event.key.keysym.sym == SDLK_PLUS && (event.key.keysym.mod & KMOD_CTRL)) {
                        width *= 1.2;
                        height *= 1.2;
                        SDL_SetWindowSize(window, width, height);
                        SDL_RenderCopy(renderer,imageTexture, NULL, NULL);
                    }
                    break;
            }
        }
    }

    // Clean up
    SDL_DestroyTexture(imageTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
