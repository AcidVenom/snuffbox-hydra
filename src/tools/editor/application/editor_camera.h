#pragma once

#include <engine/ecs/entity.h>

namespace snuffbox
{
  namespace engine
  {
    class CameraComponent;
  }

  namespace editor
  {
    /**
    * @brief The editor camera to be created per scene when using the editor
    *        instance of Snuffbox
    *
    * @author Daniel Konings
    */
    class EditorCamera : public engine::Entity
    {

    public:

      /**
      * @see Entity::Entity
      */
      EditorCamera(engine::Scene* scene);

    protected:

      /**
      * @see Entity::OnUpdate
      */
      void OnUpdate(float dt) override;

    private:

      engine::CameraComponent* camera_; //!< The camera component
      glm::vec2 angle_; //!< The current angle of the camera
    };
  }
}