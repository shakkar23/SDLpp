#pragma once

#include <SDL2/SDL.h>

#include <iostream>
#include <stack>
#include <string>
#include <utility>

#ifndef UPS
constexpr int UPDATES_PER_SECOND = 60;
#elif
constexpr int UPDATES_PER_SECOND = UPS;
#endif

class Window {
   public:
    Window(const char* p_title, const int p_w, const int p_h);
    Window(const char* p_title, const double r_w, const double r_h);
    ~Window();

    // window related
    int getRefreshrate();
    std::pair<int, int> getWindowSize();
    void setWindowSize(int x, int y);
	void setTitle(const char* title);

    // drawing related
    bool render(SDL_Rect src, SDL_Rect dst, SDL_Texture* tex);

    void renderCopy(SDL_Texture* texture,
                    const SDL_Rect* srcrect,
                    const SDL_Rect* dstrect);

    void display();

   public:
    void clear();
    void drawCircle(int x, int y, int r);
    void drawRect(SDL_Rect rec);
    void drawRectFilled(SDL_Rect rec);
    // utility
    SDL_Texture* CreateTextureFromSurface(SDL_Surface* surface);
    SDL_Texture* CreateTextureFromWindow();
	SDL_Surface* getWindowSurface();


    void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void getDrawColor(Uint8& r, Uint8& g, Uint8& b, Uint8& a);

    static SDL_Rect getInnerRect(SDL_Rect parent, float aspect_ratio);
    static SDL_Rect getOuterRect(SDL_Rect parent, float aspect_ratio);
	static SDL_Rect getLogicalRect(SDL_Rect parent, SDL_Rect child, float sim_width, float sim_height);

    SDL_Renderer* getRenderer();
   private:
    SDL_Window* window;
    SDL_Renderer* renderer;


    std::stack<SDL_Color> colors;
};

//
// IMPLEMENTATION
//

inline Window::Window(const char* p_title, const int p_w, const int p_h)
    : window(NULL), renderer(NULL) {
    this->window =
        SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, p_w, p_h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if (this->window == NULL) {
        std::cout << "Window failed to init. Error: " << SDL_GetError() << std::endl;
    }

    renderer = SDL_CreateRenderer(
        this->window, -1, SDL_RENDERER_ACCELERATED | SDL_RendererFlags::SDL_RENDERER_PRESENTVSYNC);
    // SDL_RenderSetLogicalSize(this->renderer, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);

    SDL_Surface* surface;  // Declare an SDL_Surface to be filled in with pixel data from an image file
#define O 0x0000
#define _ 0xffff
    Uint16 pixels[16 * 16] = {// ...or with raw pixel data:
                              _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
                              _, _, _, _, _, _, O, O, O, O, O, _, _, _, _, _,
                              _, _, _, _, _, O, _, _, _, _, _, O, _, _, _, _,
                              _, _, _, _, O, _, _, _, _, _, _, O, _, _, _, _,
                              _, _, _, _, O, _, _, _, _, _, O, _, O, _, _, _,
                              _, _, O, O, _, _, O, _, _, _, _, _, O, O, _, _,
                              _, _, _, O, _, _, _, _, O, O, _, _, O, _, _, _,
                              _, _, _, O, _, _, _, _, O, O, _, O, O, _, _, _,
                              _, _, _, O, _, _, _, _, _, _, _, O, _, _, _, _,
                              _, _, _, O, _, _, _, _, _, _, _, O, _, _, _, _,
                              _, _, _, _, O, _, _, _, _, O, O, _, _, _, _, _,
                              _, _, _, _, _, O, O, O, O, _, _, O, _, _, _, _,
                              _, _, _, _, _, O, _, _, _, _, _, O, O, _, _, _,
                              _, _, _, _, _, O, _, _, _, _, _, _, _, _, _, _,
                              _, _, _, _, O, O, _, _, _, _, _, _, _, _, _, _,
                              _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _};
#undef _
#undef O
    surface = SDL_CreateRGBSurfaceFrom(pixels, 16, 16, 16, 32, 0x0f00, 0x00f0, 0x000f, 0xf000);

    // The icon is attached to the window pointer
    SDL_SetWindowIcon(this->window, surface);

    // ...and the surface containing the icon pixel data is no longer required.
    SDL_FreeSurface(surface);
}

inline Window::Window(const char* p_title, const double r_w, const double r_h) {
    SDL_DisplayMode mode{};
    SDL_GetCurrentDisplayMode(0, &mode);

    // call the other constructor
    auto tmp = Window(p_title, int(mode.w * r_w), int(mode.h * r_h));

    *this = tmp;

    tmp.renderer = NULL;
    tmp.window = NULL;
}

inline Window::~Window() {
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
}

inline int Window::getRefreshrate() {
    int displayIndex = SDL_GetWindowDisplayIndex(this->window);
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(displayIndex, 0, &mode);
    return mode.refresh_rate;
}

inline std::pair<int, int> Window::getWindowSize() {
    int x, y;
    SDL_GetWindowSize(this->window, &x, &y);
    return {x, y};
}

inline void Window::setWindowSize(int x, int y) {
    SDL_SetWindowSize(this->window, x, y);
}

inline void Window::setTitle(const char* title) {
	SDL_SetWindowTitle(window, title);
}

// passing in a src with all zeros will grab the entire texture
// returns weather it worked or not
inline bool Window::render(SDL_Rect src, SDL_Rect dst, SDL_Texture* tex) {
    auto checkIfSet = [](SDL_Rect box) {if ((box.x == 0) && (box.y == 0) && (box.w == 0) && (box.h == 0)) return true; else return false; };
    
    auto err = SDL_RenderCopy(this->renderer, tex, (checkIfSet(src)) ? NULL : &src, &dst);

    if (err != 0) {
        SDL_Log("SDL2 Error: %s", SDL_GetError());
        return false;
    }
    return true;
}

inline void Window::renderCopy(SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect) {
    SDL_RenderCopy(renderer, texture, srcrect, dstrect);
}

inline SDL_Renderer* Window::getRenderer() {
    return renderer;
}

inline void Window::display() { SDL_RenderPresent(this->renderer); }

inline void Window::clear() { SDL_RenderClear(this->renderer); }

inline void Window::drawCircle(int X, int Y, int r) {
    const int32_t diameter = (r * 2);

    int32_t x = (r - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y) {
        //  Each of the following renders an octant of the circle
        SDL_Point points[8] = {
            {X + x, Y - y},
            {X + x, Y + y},
            {X - x, Y - y},
            {X - x, Y + y},
            {X + y, Y - x},
            {X + y, Y + x},
            {X - y, Y - x},
            {X - y, Y + x}};
        SDL_RenderDrawPoints(renderer, points, 8);

        if (error <= 0) {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0) {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}

inline void Window::drawRect(SDL_Rect rec) {
    SDL_RenderDrawRect(renderer, &rec);
}

inline void Window::drawRectFilled(SDL_Rect rec) {
    SDL_RenderFillRect(renderer, &rec);
}

inline SDL_Texture* Window::CreateTextureFromSurface(SDL_Surface* surface) {
    return SDL_CreateTextureFromSurface(this->renderer, surface);
}

inline SDL_Texture* Window::CreateTextureFromWindow() {
    auto surface = SDL_GetWindowSurface(window);
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    return texture;
}

inline SDL_Surface* Window::getWindowSurface() {
	return SDL_GetWindowSurface(window);
}

inline void Window::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

inline void Window::getDrawColor(Uint8& r, Uint8& g, Uint8& b, Uint8& a) {
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
}

inline SDL_Rect Window::getInnerRect(SDL_Rect parent, float aspect_ratio) {
    int height, width;
    if ((float)parent.w / parent.h > aspect_ratio) {
        height = std::min(parent.h, int(parent.w / aspect_ratio));
        width = int(height * aspect_ratio);
    } else {
        width = std::min(parent.w, int(parent.h * aspect_ratio));
        height = int(width / aspect_ratio);
    }

    SDL_Rect board = {parent.x + (parent.w - width) / 2, parent.y + (parent.h - height) / 2, width, height};

    return board;
}


inline SDL_Rect Window::getOuterRect(SDL_Rect parent, float aspect_ratio) {
    int height, width;
    if ((double)parent.w / parent.h > aspect_ratio) {
        height = std::max(parent.h, int(parent.w / aspect_ratio));
        width = parent.w;
    } else {
        width = std::max(parent.w, int(parent.h * aspect_ratio));
        height = parent.h;
    }

    SDL_Rect board = { parent.x + (parent.w - width) / 2, parent.y + (parent.h - height) / 2, width, height };

    return board;
}


inline SDL_Rect Window::getLogicalRect(SDL_Rect parent, SDL_Rect child, float sim_width, float sim_height) {
    SDL_Rect logical = {
        parent.x + int(
            (parent.w * (child.x / sim_width))
            ),
        parent.y + int(
			(parent.h * (child.y / sim_height))
            ),
        int(
            std::ceil(parent.w * (child.w / sim_width))
            ),
        int(
            std::ceil(parent.h * (child.h / sim_height))
            )
    };

	return logical;
}
