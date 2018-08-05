#include "tools/editor/application/editor_camera.h"

#include <engine/application/application.h>
#include <engine/components/transform_component.h>
#include <engine/components/camera_component.h>
#include <engine/services/input_service.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    EditorCamera::EditorCamera(engine::Scene* scene) :
      Entity(scene, true),
      camera_(nullptr),
      angle_(glm::vec2(0.0f, 0.0f))
    {
      camera_ = AddComponent<engine::CameraComponent>();
    }

    //--------------------------------------------------------------------------
    void EditorCamera::OnUpdate(float dt)
    {
      engine::InputService* input = 
        engine::Application::Instance()->GetService<engine::InputService>();

      float speed = 1.0f;
      if (input->IsKeyDown(engine::Keys::kLeftShift) == true)
      {
        speed *= 0.1f;
      }

      glm::vec3 move = glm::vec3(0.0f, 0.0f, 0.0f);

      engine::TransformComponent* t = transform();

      glm::vec3 fwd = t->Forward();
      glm::vec3 right = t->Right();

      float mx = 0.0f;
      float mz = 0.0f;

      if (input->IsKeyDown(engine::Keys::kW) == true)
      {
        mz = 1.0f;
      }
      else if (input->IsKeyDown(engine::Keys::kS) == true)
      {
        mz = -1.0f;
      }

      if (input->IsKeyDown(engine::Keys::kA) == true)
      {
        mx = -1.0f;
      }
      else if (input->IsKeyDown(engine::Keys::kD) == true)
      {
        mx = 1.0f;
      }

      speed *= dt;

      move = (fwd * mz) + (right * mx);
      move = glm::length(move) == 0.0f ? move : (glm::normalize(move) * speed);

      glm::vec3 p = t->GetLocalPosition();
      p += move;

      t->SetLocalPosition(p);

      if (input->IsMouseButtonDown(engine::MouseButtons::kLeft) == true)
      {
        glm::vec2 d = input->GetMouseMovement();

        if (glm::length(d) == 0.0f)
        {
          return;
        }

        angle_.x += d.y * 0.33f;
        angle_.y += d.x * 0.33f;

        t->SetRotationEuler(glm::vec3(angle_.x, angle_.y, 0.0f));
      }
    }
  }
}