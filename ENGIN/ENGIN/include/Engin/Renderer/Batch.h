#pragma once

#include <vector>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include "Camera.h"
#include "Color.h"
#include "Engin\Resources\ShaderProgram.h"

namespace Engin
{
	namespace Renderer
	{
		// Represents a single vertex of a triangle.

		struct Vertex
		{
			glm::vec3 position;
			glm::vec4 color;

			Vertex() : position(0.0f), color(0.0f) {}
			Vertex(const glm::vec3& position, const glm::vec4& color) : position(position), color(color) {}
			Vertex(Vertex&& other) 
			{ 
				position = std::move(other.position); color = std::move(other.color); 
			}
		};

		// Simple batch renderer for triangles. 

		class Batch
		{
		public:
			Batch();
			~Batch();

			void init(Resources::ShaderProgram* shader, size_t maxVertices = defaultBatchSize);

			void draw(const std::vector<Vertex>& vertices);
			void drawTriangle(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, const Color& color, GLfloat opacity = 1.0f, GLfloat depth = 0.0f);
			void drawQuad(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const Color& color, GLfloat opacity = 1.0f, GLfloat depth = 0.0f);
			void flush(const Camera& camera);

			void clear() 
			{ 
				currentVertex = 0; 
			}

		private:
			std::vector<Vertex> vertices;

			static const size_t defaultBatchSize = 2048;

			GLuint VBO;
			GLuint currentVertex;

			Resources::ShaderProgram* shader;
		};
	}
}
