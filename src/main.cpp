#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "solver.hpp"
#include "renderer.hpp"


int32_t main(int32_t, char *[]) {

  constexpr int32_t window_height = 1000;
  constexpr int32_t window_width = 1000;
  sf::ContextSettings settings;
  settings.antiAliasingLevel = 1;

  sf::VideoMode videoMode;
  videoMode.size = {window_width, window_height};

  sf::RenderWindow window(videoMode, "Verlet");
  const uint32_t frame_rate = 144;

  // std::cout<<"proba"<<std::endl;

  Solver solver;
  Renderer renderer{window};

  // Solver configuration
  solver.setConstraint({static_cast<float>(window_width) * 0.5f, static_cast<float>(window_height) * 0.5f}, 450.0f);
  solver.setSubStepsCount(8);
  solver.setSimulationUpdateRate(frame_rate);

  // Set simulation attributes
  const float        object_spawn_delay    = 0.025f;
  const float        object_spawn_speed    = 1200.0f;
  const sf::Vector2f object_spawn_position = {500.0f, 200.0f};
  const float        object_min_radius     = 1.0f;
  const float        object_max_radius     = 20.0f;
  const uint32_t     max_objects_count     = 1000;
  const float        max_angle             = 1.0f;

  sf::Clock clock;
  int obj_count=0;
  // Main loop
  while (window.isOpen()) {

    while (const std::optional event =  window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }
    if(obj_count == 0){
      solver.addObject({200.f, 300.f}, 20.f); // Spawn near center
      obj_count++;
    }

    solver.update();
    window.clear(sf::Color::White);
    renderer.render(solver);
    window.display();
  }

  return 0;

}
