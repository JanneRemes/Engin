#include <Engin\Engin.h>
#include <Engin\Renderer\Window.h>
#include <Engin\Renderer\Camera.h>
#include <Engin\Renderer\Batch.h>
#include <Engin\Renderer\Color.h>

namespace //camera test
{
	GLfloat alpha = 0.0f;
}

void renderTilemap(float startX, float startY, float tileWidth, float tileHeight, size_t worldWidth, size_t worldHeight, Engin::Renderer::Batch& batch);

int main(int argc, char** argv)
{
	Engin::Engin engine;
	Engin::Renderer::Camera camera;
	Engin::Renderer::Camera camera2;

	engine.init("resources/engine.ini");

	camera.initCamera(0.0f, 0.0f, 400.0f, 720.0f, 0.0f, 0.0f, 1.0f);
	camera2.initCamera(400.0f, 0.0f, 880.0f, 720.0f, 300.0f, 0.0f, 1.0f);

	camera.setZoomLevel(0.5f);
	//camera.setRotation(20.0f);

	Engin::Resources::Shader shader("resources/shaders/vertex.shader", "resources/shaders/fragment.shader");

	Engin::Renderer::Batch batch(&shader, 4096);

	while (engine.update())
	{
		batch.drawTriangle(500.0f, 45.0f, 750.0f, 250.0f, 65.0f, 480.0f, Engin::Renderer::clrRed, 0.0f);
		batch.drawTriangle(10.0f, 10.0f, 100.0f, 10.0f, 50.0f, 50.0f, Engin::Renderer::clrGreen, 0.0f);
		batch.drawQuad(200.0f, 200.0f, 50.0f, 50.0f, Engin::Renderer::clrWhite, 0.0f);
		renderTilemap(0.0f, 0.0f, 32.0f, 32.0f, 25, 25, batch);

		//camera test
		//camera2.setZoomLevel(4.0f - cos(alpha)*2); //Zoom position could be relative to camera center if even possible.
		camera2.setPosition(700.0f, 0.0f);
		camera2.setRotation(alpha*30);		

		camera.setPositionCenter(200.0f + 200.0f * sin(alpha), 0.0f);
		alpha += 0.01f;
		
		camera.activateViewPort();
		batch.flush(camera);

		camera2.activateViewPort();
		batch.flush(camera2);

		batch.clear();
	}
	
	engine.quit();

	return 0;
}

void renderTilemap(float startX, float startY, float tileWidth, float tileHeight, size_t worldWidth, size_t worldHeight, Engin::Renderer::Batch& batch)
{
	for (size_t y = 0; y < worldHeight; y++)
	{
		for (size_t x = 0; x < worldWidth; x++)
		{
			if ((x + y) % 2)
			{
				batch.drawQuad(startX + x * tileWidth, startY + y * tileHeight, tileWidth, tileHeight, Engin::Renderer::clrWhite, 0.0f);
			}
			else
			{
				batch.drawQuad(startX + x * tileWidth, startY + y * tileHeight, tileWidth, tileHeight, Engin::Renderer::clrBlack, 0.0f);
			}
		}
	}
}