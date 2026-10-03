#pragma once
#include <cstdint>
#include <cstring>
#include <cmath>
#define PI 3.14159265358979323846f
template<class T> T constrain(T x, T lo, T hi) { return x < lo ? lo : x > hi ? hi : x; }
