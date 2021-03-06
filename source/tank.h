#pragma once

#include "template.h"
#include <SDL2/SDL_render.h>

namespace PP2
{
class Tank
{
  public:
    Tank(float pos_x, float pos_y, alliances allignment, SDL_Texture* tank_sprite, SDL_Texture* smoke_sprite, float tar_x,
         float tar_y, float collision_radius, int health, float max_speed);

    ~Tank();

    void Tick();

    vec2<> Get_Position() const { return position; };

    float Get_collision_radius() const { return collision_radius; };

    bool Rocket_Reloaded() const { return reloaded; };

    void Reload_Rocket();

    void Deactivate();

    bool hit(int hit_value);

    void Draw(SDL_Renderer* screen);

    int CompareHealth(const Tank& other) const;

    void Push(const vec2<float>& direction, float magnitude);

    vec2<float> position;
    vec2<int> gridCell;
    vec2<float> speed;
    vec2<float> target;

    int health;

    float collision_radius;
    vec2<float> force;

    float max_speed;
    float reload_time;

    bool reloaded;
    bool active;

    alliances alliance;

    int current_frame;
    SDL_Texture* tank_sprite;
    SDL_Texture* smoke_sprite;

    SDL_Rect SrcR;
    SDL_Rect DestR;
};
} // namespace PP2
