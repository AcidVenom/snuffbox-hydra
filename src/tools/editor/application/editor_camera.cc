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
      camera_(nullptr)
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
        speed *= 5.0f;
      }

      glm::vec3 move = glm::vec3(0.0f, 0.0f, 0.0f);

      engine::TransformComponent* t = transform();

      glm::vec3 fwd = t->Forward();
      float mz = 0.0f;

      if (input->IsKeyDown(engine::Keys::kW) == true)
      {
        mz = 1.0f;
      }
      else if (input->IsKeyDown(engine::Keys::kS) == true)
      {
        mz = -1.0f;
      }

      move = fwd * mz * speed * dt;

      glm::vec3 p = t->GetLocalPosition();
      p += move;

      t->SetLocalPosition(p);
    }
  }
}