#pragma once

#include <PerlinNoise/PerlinNoise.hpp>

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
constexpr int WIDTH = WINDOW_WIDTH/1.5;
constexpr int HEIGHT = WINDOW_HEIGHT/1.5;


// This is the number of chunks in the map
constexpr int MAP_WIDTH = 40;
constexpr int MAP_HEIGHT = 40;

constexpr int CHUNK_RAD = 6;
constexpr int INVENTORY_COLUMNS = 3;
constexpr int INVENTORY_ROWS = 10;

const siv::PerlinNoise::seed_type seed = 123456u;

