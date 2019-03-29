#ifndef GL_ENGINE_CAMERA
#define GL_ENGINE_CAMERA

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace GlScene
{
	enum class FrustrumPoints {NearLD = 0, NearLU, NearRU, NearRD, FarLD, FarLU, FarRU, FarRD};
	class glCamera
	{
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 full_matrix;
		glm::vec3 Frustrum[8];
		glm::vec2 m_map_direction;
		glm::vec2 Frustrum_2d[5];

	public:
		glm::vec3 m_position;

		const glm::mat4 &CameraMatrix() const;
		const glm::mat4 &CameraViewMatrix() const;
		const glm::mat4 &CameraProjectionMatrix() const;
		const glm::vec3 &GetFrustrumPoint(FrustrumPoints point) const;
		void RecalculateFrustrum();
		void SetCameraLocation(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);
		void SetCameraLens(float FOV, float aspect, float near, float far);
		void SetCameraLens_Orto(float x1, float x2,float y1, float y2, float near, float far);
	};
}

#endif