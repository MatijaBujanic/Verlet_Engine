#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <vector>
#include <iostream>

struct PairHash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

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
      updateObjects(step_dt);
      checkCollisionOptimized(step_dt);
      //checkCollisions(step_dt);
      applyConstraint();
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
  float getStepDt() const {
    float step_dt = m_frame_dt / static_cast<float>(m_sub_steps);
    return step_dt;
  }


private:
  uint32_t m_sub_steps = 10;
  sf::Vector2f m_gravity = {0.f, 1000.f};
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

  void checkCollisions(float dt)
  {
    const float    response_coef = 0.75f;
    const uint64_t objects_count = m_objects.size();
    // Iterate on all objects
    for (uint64_t i{0}; i < objects_count; ++i) {
      VerletObject& object_1 = m_objects[i];
      // Iterate on object involved in new collision pairs
      for (uint64_t k{i + 1}; k < objects_count; ++k) {
        VerletObject&      object_2 = m_objects[k];
        const sf::Vector2f v        = object_1.position - object_2.position;
        const float        dist2    = v.x * v.x + v.y * v.y;
        const float        min_dist = object_1.radius + object_2.radius;
        // Check overlapping
        if (dist2 < min_dist * min_dist) {
          const float        dist  = sqrt(dist2);
          const sf::Vector2f n     = v / dist;
          const float mass_ratio_1 = object_1.radius / (object_1.radius + object_2.radius);
          const float mass_ratio_2 = object_2.radius / (object_1.radius + object_2.radius);
          const float delta        = 0.5f * response_coef * (dist - min_dist);
          // Update positions
          object_1.position -= n * (mass_ratio_2 * delta);
          object_2.position += n * (mass_ratio_1 * delta);
        }
      }
    }
  }

  void checkCollisionOptimized(float dt)
  {
    const float response_coef = 0.75f;
    const uint64_t object_count = m_objects.size();

    if(object_count<2) return;

    const float cell_size = 50.0f;
    std::unordered_map<std::pair<int,int>, std::vector<uint64_t>, PairHash> grid;
    
    // Build grid
    for(uint64_t i{0}; i < object_count; i++)
    {
      int cell_x = static_cast<int>(m_objects[i].position.x/cell_size);
      int cell_y = static_cast<int>(m_objects[i].position.y/cell_size);
      grid[{cell_x,cell_y}].push_back(i);
    }

    // Check collisions
    for(const auto& [cell,indices] : grid){
      const auto& [cell_x, cell_y] = cell;

      for(uint64_t i=0; i < indices.size(); i++)
      {
        VerletObject& obj1 = m_objects[indices[i]];  
        //const float r1 = obj1.radius;

        // Checkobjects in same cell
        for (uint64_t j = i + 1; j < indices.size(); ++j) {
            checkCollisionPair(obj1, m_objects[indices[j]], response_coef);
        }

         for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                // Skip the center cell (we already checked same cell)
                if (dx == 0 && dy == 0) continue;
                
                int neighbor_x = cell_x + dx;
                int neighbor_y = cell_y + dy;
                auto neighbor_key = std::make_pair(neighbor_x, neighbor_y);
                
                // Check if this neighbor cell exists and has objects
                auto neighbor_it = grid.find(neighbor_key);
                if (neighbor_it != grid.end()) {
                    const auto& neighbor_indices = neighbor_it->second;
                    
                    // Check all objects in the neighbor cell against current object
                    for (uint64_t k = 0; k < neighbor_indices.size(); ++k) {
                        // Only check if neighbor object has higher index to avoid duplicate checks
                        if (neighbor_indices[k] > indices[i]) {
                            checkCollisionPair(obj1, m_objects[neighbor_indices[k]], response_coef);
                        }
                    }
                }
            }
        }

      }
    }

  }



  void checkCollisionPair(VerletObject& obj1, VerletObject& obj2, float response_coef)
  {
    const sf::Vector2f v = obj1.position - obj2.position;
    const float dist2 = v.x * v.x + v.y * v.y;
    const float min_dist = obj1.radius + obj2.radius;

    if(dist2 < min_dist * min_dist)
    {
      const float dist = sqrt(dist2);
      if(dist < 0.0001f)return;

      const sf::Vector2f n = v / dist;
      const float mass_ratio_1 = obj1.radius / (obj1.radius + obj2.radius);
      const float mass_ratio_2 = 1.0f - mass_ratio_1;
      const float delta = 0.5f * response_coef * (dist - min_dist);

      obj1.position -= n * (mass_ratio_2 * delta);
      obj2.position += n * (mass_ratio_1 * delta);
    }
    else return;
  }

  void applyConstraint()
  {
    for (auto& obj : m_objects) {
      const sf::Vector2f to_obj = obj.position - m_constraint_center;
      const float dist = sqrt(to_obj.x * to_obj.x + to_obj.y * to_obj.y);
      const float constraint_boundary = m_constraint_radius - obj.radius;

      if (dist > constraint_boundary) {
        const sf::Vector2f n = to_obj / dist;
        obj.position = m_constraint_center + n * constraint_boundary;
      }
    }
  }
  void updateObjects(float dt){
    for(auto& obj : m_objects){
      obj.update(dt);
    }
  }
};
