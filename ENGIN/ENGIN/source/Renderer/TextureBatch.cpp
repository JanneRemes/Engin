#include "Engin\Renderer\TextureBatch.h"

#include <algorithm>
#include <glm\gtc\type_ptr.hpp>

namespace Engin
{
	namespace Renderer
	{

		TextureBatch::TextureBatch() : shader(nullptr), IBO(0), VBO(0), textureQueueCount(0), textureQueueArraySize(0), vertexBufferPos(0)
		{
			createBuffers();
		}

		TextureBatch::~TextureBatch()
		{
			glDeleteBuffers(1, &IBO);
			glDeleteBuffers(1, &VBO);
		}

		void TextureBatch::createBuffers()
		{
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * maxBatchSize * vertiecsPerTexture, nullptr, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			vertices.reserve(initialQueueSize * vertiecsPerTexture);

			createIndexValues();

			glGenBuffers(1, &IBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), indices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		void TextureBatch::createIndexValues()
		{
			size_t size = maxBatchSize * indicesPerTexture;
			indices.reserve(size);

			for (unsigned short i = 0; i < size; i += vertiecsPerTexture)
			{
				indices.push_back(i);
				indices.push_back(i + 1);
				indices.push_back(i + 2);

				indices.push_back(i + 1);
				indices.push_back(i + 3);
				indices.push_back(i + 2);
			}
		}

		void TextureBatch::flush(const Camera& camera)
		{
			if (!textureQueueCount)
				return;

			prepareForRendering();
			sortTextures();

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, color)));
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, position)));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, uv)));

			shader->bind();

			glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(camera.getVP()));
			glUniform1i(glGetUniformLocation(shader->getProgram(), "texture"), 0);

			Resources::Texture* batchTexture = nullptr;
			size_t batchStart = 0;

			for (size_t i = 0; i < textureQueueCount; i++)
			{
				Resources::Texture* texture = sortedTextures[i]->texture;

				if (texture != batchTexture)
				{
					if (i > batchStart)
					{
						renderBatch(batchTexture, batchStart, i - batchStart, camera);
					}

					batchTexture = texture;
					batchStart = i;
				}
			}

			renderBatch(batchTexture, batchStart, textureQueueCount - batchStart, camera);

			shader->unbind();

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			textureQueueCount = 0;
			vertexBufferPos = 0;
			
			sortedTextures.clear();
		}

		void TextureBatch::prepareForRendering()
		{
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			const TextureInfo* t = nullptr;

			for (size_t i = 0; i < textureQueueCount; i++)
			{
				t = sortedTextures[i];
				vertices.push_back(Vertex(t->topLeft.x, t->topLeft.y, t->depth, 
					t->color.r, t->color.g, t->color.b, t->color.a, 
					t->texCoords.x, t->texCoords.y));
				vertices.push_back(Vertex(t->topRight.x, t->topRight.y, t->depth,
					t->color.r, t->color.g, t->color.b, t->color.a,
					t->texCoords.x, t->texCoords.y));
				vertices.push_back(Vertex(t->bottomLeft.x, t->bottomLeft.y, t->depth,
					t->color.r, t->color.g, t->color.b, t->color.a,
					t->texCoords.x, t->texCoords.y));
				vertices.push_back(Vertex(t->bottomRight.x, t->bottomRight.y, t->depth,
					t->color.r, t->color.g, t->color.b, t->color.a,
					t->texCoords.x, t->texCoords.y));
			}

			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), (void*)(vertices.data()));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			vertices.clear();
		}

		void TextureBatch::growTextureQueue()
		{
			size_t newSize = std::max(initialQueueSize, textureQueueArraySize * 2);

			std::unique_ptr<TextureInfo[]> newArray(new TextureInfo[newSize]);

			for (size_t i = 0; i < textureQueueCount; i++)
			{
				newArray[i] = textureQueue[i];
			}

			textureQueue = std::move(newArray);
			textureQueueArraySize = newSize;

			sortedTextures.clear();
		}

		void TextureBatch::growSortedTextures()
		{
			size_t previousSize = sortedTextures.size();

			sortedTextures.resize(textureQueueCount);

			for (size_t i = previousSize; i < textureQueueCount; i++)
			{
				sortedTextures[i] = &textureQueue[i];
			}
		}

		void TextureBatch::sortTextures()
		{
			if (sortedTextures.size() < textureQueueCount)
			{
				growSortedTextures();
			}

			std::sort(sortedTextures.begin(), sortedTextures.begin() + textureQueueCount, [](TextureInfo const* x, TextureInfo const* y) -> bool
			{
				return x->texture < y->texture;
			});
		}

		void TextureBatch::renderBatch(Resources::Texture* texture, size_t start, size_t count, const Camera& camera)
		{
			texture->bindTexture(GL_TEXTURE0 + 0);

			while (count > 0)
			{
				size_t batchSize = count;

				size_t remainingSpace = maxBatchSize - vertexBufferPos;

				if (batchSize > remainingSpace)
				{
					if (remainingSpace < minBatchSize)
					{
						vertexBufferPos = 0;
						batchSize = std::min(count, maxBatchSize);
					}
					else
					{
						batchSize = remainingSpace;
					}
				}

				glDrawElements(GL_TRIANGLES, indicesPerTexture * batchSize, GL_UNSIGNED_SHORT, (void*)((indicesPerTexture * vertexBufferPos * sizeof(unsigned short))));
			
				count -= batchSize;

				vertexBufferPos += batchSize;
			}

			// TODO (Pidgin): texture->unbindTexture();
		}

		void TextureBatch::draw(Resources::Texture* texture, glm::vec4* textureRegion, float x, float y, float width, float height, float rotation, float scale, const Color& color, float opacity, float depth)
		{
			if (textureQueueCount >= textureQueueArraySize)
			{
				growTextureQueue();
			}

			TextureInfo* textureInfo = &textureQueue[textureQueueCount];

			if (textureRegion)
			{
				textureInfo->texCoords = *textureRegion;
			}
			else
			{
				textureInfo->texCoords = glm::vec4(0.0f, 0.0f, (GLfloat)texture->getWidth(), (GLfloat)texture->getHeight());
			}

			textureInfo->color = glm::vec4(color.r, color.b, color.g, opacity);
			textureInfo->depth = depth;

			textureInfo->topLeft = glm::vec2(x, y + height);
			textureInfo->topRight = glm::vec2(x + width, y + height);
			textureInfo->bottomLeft = glm::vec2(x, y);
			textureInfo->bottomLeft = glm::vec2(x + width, y);

			textureQueueCount++;
		}
	}
}