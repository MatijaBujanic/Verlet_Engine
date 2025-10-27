#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "solver.hpp"
#include "renderer.hpp"

const float PI = 3.1415926535;


static sf::Color getRainbow(float t)
{
    const float r = sin(t);
    const float g = sin(t + 0.33f * 2.0f * PI);
    const float b = sin(t + 0.66f * 2.0f * PI);
    return {static_cast<uint8_t>(255.0f * r * r),
            static_cast<uint8_t>(255.0f * g * g),
            static_cast<uint8_t>(255.0f * b * b)};
}



int32_t main(int32_t, char *[]) {
  
  srand(time(0));

  constexpr int32_t window_height = 1300;
  constexpr int32_t window_width = 1300;
  sf::ContextSettings settings;
  settings.antiAliasingLevel = 1;

  sf::VideoMode videoMode;
  videoMode.size = {window_width, window_height};

  sf::RenderWindow window(videoMode, "Verlet");
  const uint32_t frame_rate = 144;
  window.setFramerateLimit(frame_rate);

  // std::cout<<"proba"<<std::endl;

  Solver solver;
  Renderer renderer{window};

  // Solver configuration
  solver.setConstraint({static_cast<float>(window_width) * 0.5f, static_cast<float>(window_height) * 0.5f}, 600.0f);
  solver.setSubStepsCount(8);
  solver.setSimulationUpdateRate(frame_rate);

  // Set simulation attributes
  const float        object_spawn_delay    = 0.08f;
  const float        object_spawn_speed    = 1000.0f;
  const sf::Vector2f object_spawn_position = {700.0f, 200.0f};
  const float        object_min_radius     = 5.0f;
  const float        object_max_radius     = 20.0f;
  const uint32_t     max_objects_count     = 800;
  const float        max_angle             = 2.0f;

  sf::Clock clock;
  //int obj_count=0;
  // Main loop
  while (window.isOpen()) {

    while (const std::optional event =  window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
      else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if(keyPressed->scancode==sf::Keyboard::Scancode::Escape)
          window.close();
      }
    }


    if (solver.getObjectsCount() < max_objects_count && clock.getElapsedTime().asSeconds() >= object_spawn_delay) {
      clock.restart();
      auto&       object = solver.addObject(object_spawn_position, 
                                            static_cast <float> (rand()) / static_cast <float> (RAND_MAX)
                                            *object_max_radius + object_min_radius);
      const float t      = solver.getTime();
      const float angle  = max_angle * sin(t) + PI * 0.5f;
      solver.setObjectVelocity(object, object_spawn_speed * sf::Vector2f{cosf(angle), sinf(angle)});
      object.color = getRainbow(t);
    }

    solver.update();
    window.clear(sf::Color::White);
    renderer.render(solver);
    window.display();
  }

  return 0;

}
