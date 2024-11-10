#pragma once

#include <PerlinNoise/PerlinNoise.hpp>

constexpr int WIDTH = 800;
constexpr int HEIGHT = 800;
// This is the number of chunks in the map
constexpr int MAP_WIDTH = 40;
constexpr int MAP_HEIGHT = 40;

constexpr int CHUNK_RAD = 6;
constexpr int INVENTORY_COLUMNS = 3;
constexpr int INVENTORY_ROWS = 10;

const siv::PerlinNoise::seed_type seed = 123456u;

