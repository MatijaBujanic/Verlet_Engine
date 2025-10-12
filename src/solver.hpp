#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <vector>

struct VerletObject {
  sf::Vector2f position_current;
  sf::Vector2f postition_old;
  sf::Vector2f acceleration;
};
