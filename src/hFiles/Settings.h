#pragma once

#include <PerlinNoise/PerlinNoise.hpp>

constexpr int WIDTH = 800;
constexpr int HEIGHT = 800;
// This is the number of chunks in the map
constexpr int MAP_WIDTH = 20;
constexpr int MAP_HEIGHT = 20;

constexpr int CHUNK_RAD = 4;

const siv::PerlinNoise::seed_type seed = 123456u;

