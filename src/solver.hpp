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
  
  void update(){
    m_time += m_frame_dt;
    const float step_dt = getStepDt();
    for(uint32_t i{m_sub_steps};i--;){
      applyGravity();
      applyConstraint();
      updateObjects(step_dt);
    }
  }

  void setSimulationUpdateRate(uint32_t rate){
    m_frame_dt = 1.0f / static_cast<float>(rate);
  }

  void setConstraint(sf::Vector2f position, float radius){
    m_constraint_center = position;
    m_constraint_radius = radius;
  }

  void setSubStepsCount(uint32_t sub_steps){
    m_sub_steps=sub_steps;
  }

  void setObjectVelocity(VerletObject& object, sf::Vector2f v){
    object.setVelocity(v, getStepDt());
  }

  [[nodiscard]]
  const std::vector<VerletObject>& getObjects() const{
    return m_objects;
  }

  [[nodiscard]]
  sf::Vector3f getConstraint() const{
    return {m_constraint_center.x, m_constraint_center.y, m_constraint_radius};
  }

  [[nodiscard]]
  uint64_t getObjectsCount() const{
    return m_objects.size();
  }

  [[nodiscard]]
  float getTime() const{
    return m_time;
  }

  [[nodiscard]]
  float getStepDt() const{
    return m_frame_dt / static_cast<float>(m_sub_steps);
  }


private:
  uint32_t m_sub_steps = 1;
  sf::Vector2f m_gravity = {0.0f, 40.0f};
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
