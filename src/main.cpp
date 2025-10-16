#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

int32_t main(int32_t, char *[]) {

  constexpr int32_t window_height = 1000;
  constexpr int32_t window_width = 1000;
  sf::ContextSettings settings;
  settings.antiAliasingLevel = 1;

  sf::VideoMode videoMode;
  videoMode.size = {window_width, window_height};

  sf::RenderWindow window(videoMode, "Verlet");
  const uint32_t frame_rate = 60;
}
