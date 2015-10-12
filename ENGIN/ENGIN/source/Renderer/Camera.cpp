#include "Engin\Renderer\Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace Engin
{
	namespace Renderer
	{
		Camera::Camera()
		{
		}

		Camera::~Camera()
		{
		}

		void Camera::initCamera(GLfloat viewPortX, GLfloat viewPortY, GLfloat viewPortWidth, GLfloat viewPortHeight, GLfloat worldX, GLfloat worldY, GLfloat coordUnitSize)
		{
			this->viewPortWidth = viewPortWidth;
			this->viewPortHeight = viewPortHeight;
			this->worldX = worldX;
			this->worldY = worldY;
			this->coordUnitSize = coordUnitSize;
			this->rotation = 0.0f;

			viewPort = glm::vec4(viewPortX, viewPortY, viewPortWidth, viewPortHeight);
			activateViewPort();

			glm::vec3 xAxis = glm::vec3(1.0, 0.0, 0.0);
			glm::vec3 yAxis = glm::vec3(0.0, 1.0, 0.0);
			glm::vec3 zAxis = glm::vec3(0.0, 0.0, 1.0);

			this->worldX = worldX / (viewPortWidth / (this->coordUnitSize * 2));

			camPos = glm::vec3(this->worldX, this->worldY, 0);
			camUp = yAxis;
			camRight = xAxis;
			camFront = -zAxis;

			V = glm::lookAt(camPos, camPos + camFront, camUp);

			zoomLevel = viewPortWidth;
			defaultZoomLevel = zoomLevel;
			DefaultZoomOn = true;

			P = glm::ortho(0.0f, zoomLevel, 0.0f, zoomLevel*viewPortHeight / viewPortWidth);
			VP = V*P*glm::rotate(this->rotation, glm::vec3(0.0, 0.0, 1.0));
		}

		glm::mat4 Camera::getVP()
		{
			return VP;
		}
		void Camera::activateViewPort()
		{
			glViewport(viewPort[0], viewPort[1], viewPort[2], viewPort[3]);
		}

		void Camera::setPosition(GLfloat worldX, GLfloat worldY)
		{
			if (DefaultZoomOn)
			{
				this->worldX = worldX / (viewPortWidth / (coordUnitSize * 2));
				this->worldY = worldY;
			}
			else
			{
				this->worldX = worldX / (viewPortWidth / (coordUnitSize * 2))*(defaultZoomLevel / zoomLevel);
				this->worldY = worldY*(defaultZoomLevel / zoomLevel);
			}

			camPos = glm::vec3(this->worldX, this->worldY, 0);
			V = glm::lookAt(camPos, camPos + camFront, camUp);
			VP = V*P*glm::rotate(this->rotation, glm::vec3(0.0, 0.0, 1.0));
		}

		glm::vec2 Camera::getPosition()
		{
			if (DefaultZoomOn)
			{
				return glm::vec2(worldX*(viewPortWidth / (coordUnitSize * 2)), worldY);
			}
			else
			{
				return glm::vec2(worldX*(viewPortWidth / (coordUnitSize * 2)) / (defaultZoomLevel / zoomLevel), worldY / (defaultZoomLevel / zoomLevel));
			}
		}

		void Camera::setPositionCenter(GLfloat worldX, GLfloat worldY)
		{
			if (DefaultZoomOn)
			{
				this->worldX = worldX / (viewPortWidth / (coordUnitSize * 2)) - 1.0f;
				this->worldY = worldY - 1.0f;
			}
			else
			{
				this->worldX = worldX / (viewPortWidth / (coordUnitSize * 2))*(defaultZoomLevel / zoomLevel) - 1.0f;
				this->worldY = worldY*(defaultZoomLevel / zoomLevel) - 1.0f;
			}

			camPos = glm::vec3(this->worldX, this->worldY, 0);
			V = glm::lookAt(camPos, camPos + camFront, camUp);
			VP = V*P*glm::rotate(this->rotation, glm::vec3(0.0, 0.0, 1.0));
		}

		glm::vec2 Camera::getPositionCenter()
		{
			if (DefaultZoomOn)
			{
				return glm::vec2(worldX*(viewPortWidth / (coordUnitSize * 2)) + 1.0f, worldY + 1.0f);
			}
			else
			{
				return glm::vec2(worldX*(viewPortWidth / (coordUnitSize * 2)) / (defaultZoomLevel / zoomLevel) + 1.0f, worldY / (defaultZoomLevel / zoomLevel) + 1.0f);
			}
		}

		void Camera::setZoomLevel(GLfloat size)
		{
			if (size*viewPortWidth != zoomLevel)
			{
				this->zoomLevel = size*viewPortWidth;
				P = glm::ortho(0.0f, zoomLevel, 0.0f, zoomLevel*viewPortHeight / viewPortWidth);
				VP = V*P*glm::rotate(this->rotation, glm::vec3(0.0, 0.0, 1.0));
				DefaultZoomOn = false;
			}
		}

		void Camera::setDefaultZoomLevel()
		{
			zoomLevel = viewPortWidth;
			P = glm::ortho(0.0f, zoomLevel, 0.0f, zoomLevel*viewPortHeight / viewPortWidth);
			VP = V*P*glm::rotate(this->rotation, glm::vec3(0.0, 0.0, 1.0));
			DefaultZoomOn = true;
		}

		void Camera::rotate(GLfloat rotation)
		{
			this->rotation = glm::radians(rotation);
			VP = VP*glm::rotate(this->rotation, glm::vec3(0.0, 0.0, 1.0));
		}

		GLfloat Camera::getRotation()
		{
			return rotation;
		}

		GLfloat Camera::getDefaultZoomLevel()
		{
			return defaultZoomLevel;
		}

		GLfloat Camera::getZoomLevel()
		{
			return zoomLevel;
		}
	}
}