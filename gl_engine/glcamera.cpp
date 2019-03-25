#include "glcamera.h"

namespace GlScene
{
    const glm::mat4 &glCamera::CameraMatrix() const
		{
			//full_matrix =  projection * view;
			//return projection * view;
			return full_matrix;
			
		}

		const glm::mat4 &glCamera::CameraViewMatrix() const
		{
			return view;
		}

		const glm::mat4 &glCamera::CameraProjectionMatrix() const
		{
			return projection;
		}

		const glm::vec3 &glCamera::GetFrustrumPoint(FrustrumPoints point) const
        {
            return Frustrum[static_cast<int>(point)];
        }
        
		void glCamera::RecalculateFrustrum()
        {
            glm::mat4 inverse = glm::inverse(full_matrix);
            Frustrum[static_cast<int>(FrustrumPoints::NearLD)] = inverse * glm::vec4(-1.0f,-1.0f,1.0f,1.0f);
            Frustrum[static_cast<int>(FrustrumPoints::NearLU)] = inverse * glm::vec4(-1.0f,-1.0f,1.0f,1.0f);
            Frustrum[static_cast<int>(FrustrumPoints::NearRU)] = inverse * glm::vec4(-1.0f,-1.0f,1.0f,1.0f);
            Frustrum[static_cast<int>(FrustrumPoints::NearRD)] = inverse * glm::vec4(-1.0f,-1.0f,1.0f,1.0f);
            Frustrum[static_cast<int>(FrustrumPoints::FarLD)] = inverse * glm::vec4(-1.0f,-1.0f,-1.0f,1.0f);
            Frustrum[static_cast<int>(FrustrumPoints::FarLU)] = inverse * glm::vec4(-1.0f,-1.0f,-1.0f,1.0f);
            Frustrum[static_cast<int>(FrustrumPoints::FarRU)] = inverse * glm::vec4(-1.0f,-1.0f,-1.0f,1.0f);
            Frustrum[static_cast<int>(FrustrumPoints::FarRD)] = inverse * glm::vec4(-1.0f,-1.0f,-1.0f,1.0f);
        }
        

		void glCamera::SetCameraLocation(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
		{
			m_position = position;
			view = glm::lookAt(position,target,up);
			full_matrix =  projection * view;
		}

		void glCamera::SetCameraLens(float FOV, float aspect, float near, float far)
		{
			projection = glm::perspective(glm::radians(FOV), aspect, near, far);
			full_matrix =  projection * view;
		}

		void glCamera::SetCameraLens_Orto(float x1, float x2,float y1, float y2, float near, float far)
		{
			projection = glm::ortho(x1, x2, y1, y2, near, far);
			full_matrix =  projection * view;
		}

}
