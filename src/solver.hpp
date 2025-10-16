#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <vector>

struct VerletObject {
  sf::Vector2f position;
  sf::Vector2f position_last;
  sf::Vector2f acceleration;
  float radius = 10.f;
  sf::Color color = sf::Color::White;

  VerletObject() = default;
  VerletObject(sf::Vector2f position_, float radius_)
      : position{position_}, position_last{position_}, acceleration{0.0f, 0.0f},
        radius{radius_} {}

  void update(float dt) {
    // compute how much we moved
    const sf::Vector2f displacement = position - position_last;
    // update position
    position_last = position;
    position = position + displacement + acceleration * (dt * dt);
    // reset acceleration
    acceleration = {};
  }

  void accelerate(sf::Vector2f a) { 
    acceleration += a; 
  }

  void setVelocity(sf::Vector2f v, float dt) {
    position_last = position - (v * dt);
  }

  void addVelocity(sf::Vector2f v, float dt) { 
    position_last -= v * dt; 
  }

  [[nodiscard]]
  sf::Vector2f getVelocity(float dt) const{
    return (position-position_last) / dt;
  }
};




class Solver
{
public:
  Solver()=default;

  VerletObject& addObject(sf::Vector2f position, float radius){
    return m_objects.emplace_back(position,radius);
  }

private:
  uint32_t m_sub_steps = 1;
  sf::Vector2f m_gravity = {0.0f, 1000.0f};
  sf::Vector2f m_constraint_center;
  float m_constraint_radius = 100.0f;
  std::vector<VerletObject> m_objects;
  float m_time = 0.0f;
  float m_frame_dt = 0.0f;

  void applyGravity(){
    for (auto& obj : m_objects){
      obj.accelerate(m_gravity);
    }
  }
  
  void applyConstraint(){
    for(auto& obj : m_objects){
      const sf::Vector2f v = m_constraint_center - obj.position;
      const float dist = std::sqrt(v.x*v.x+v.y*v.y);
      if(dist > (m_constraint_radius - obj.radius)){
        const sf::Vector2f n=v/dist;
        obj.position = m_constraint_center - n *(m_constraint_radius - obj.radius);
      }
    }
  }

  void updateObjects(float dt){
    for(auto& obj : m_objects){
      obj.update(dt);
    }
  }
};
