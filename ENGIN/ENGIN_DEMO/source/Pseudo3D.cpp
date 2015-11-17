#include "Pseudo3D.h"

#include <iostream>
#include <Engin\Renderer\Window.h>
#include <Engin\Renderer\Color.h>
#include <glm\gtc\type_ptr.hpp>
#include "irrKlang\irrKlang.h"

irrklang::ISoundEngine *SoundEngine = irrklang::createIrrKlangDevice();

namespace Engin
{
	namespace Game
	{
		Pseudo3D::Pseudo3D(Engin* engine)
		{
#pragma region INIT
			this->engine = engine;

			std::cout << "Scene started, dont go over Y" << std::endl;

			camera.initCamera(0.0f, 0.0f, 800.0f, 800.0f, 400.0f-1600.0f, 400.0f, 400, 400);
			camera2.initCamera(800.0f, 0.0f, 800.0f, 800.0f, 0.0f, 0.0f, 400, 100);
			camera3.initCamera(0.0f, 700.0f, 800.0f, 100.0f, 0.0f, 0.0f, 400, 50);

			shader = Resources::ResourceManager::getInstance().load<Resources::ShaderProgram>("resources/shaders/shader");
			textureShader = Resources::ResourceManager::getInstance().load<Resources::ShaderProgram>("resources/shaders/texture_shader");
			alphaShader = Resources::ResourceManager::getInstance().load<Resources::ShaderProgram>("resources/shaders/alpha_shader");
			batch.init(shader, 10240);
			textureBatch.setShader(textureShader);
			alphaTextureBatch.setShader(alphaShader);
			alphaTextureBatch.setSortMode(Renderer::TextureSortMode::FrontToBack);
			alphaTextureBatch2.setShader(alphaShader);
			alphaTextureBatch2.setSortMode(Renderer::TextureSortMode::FrontToBack);

			
			furball = Resources::ResourceManager::getInstance().load<Resources::Texture>("resources/furball_40.png");			
			doge1 = Resources::ResourceManager::getInstance().load<Resources::Texture>("resources/wall_tile_40.png");
			doge2 = Resources::ResourceManager::getInstance().load<Resources::Texture>("resources/wall_warning_tile_40.png");
			doge3 = Resources::ResourceManager::getInstance().load<Resources::Texture>("resources/wall_pine_tile_40.png");
			doge4 = Resources::ResourceManager::getInstance().load<Resources::Texture>("resources/wall_redwood_tile_40.png");
			doge5 = Resources::ResourceManager::getInstance().load<Resources::Texture>("resources/wall_cubes_tile_40.png");
			

			font = Resources::ResourceManager::getInstance().load<Resources::Font>("resources/arial.ttf");
			font->setPtSize(40);
			textCreator.createTextTexture(font, "|", 255, 100, 0);
			text = textCreator.getTexture();
			textCreator3.createTextTexture(font, "|", 255, 100, 0);
			text3 = textCreator3.getTexture();

			mapX = 24;
			mapY = 24;
			tileSize = 40;
			alpha = 0.0f;

			objectTiles.resize((mapX + 1)*(mapY + 1));
			emptyVector(1);

			//DDA
			w = 800;
			h = 800;

			moveSpeed = 0.11f;
			rotSpeed = 0.02f;
			player = glm::vec2(22.0f, 12.0f);
			
			sprite = glm::vec2(12.0f, 12.0f);
			spriteContainer.push_back(sprite);

			DDAlines.resize(w);
			DDAlinesTexX.resize(w);

			DDASpriteDrawData.resize(1);
			emptyVector(3);

			dirX = -1, dirY = 0; //initial direction vector
			planeX = 0.0f, planeY = 0.5; //the 2d raycaster version of camera plane

#pragma endregion

#pragma region WallTiles

			//MapSides
			for (int i = 0; i <= mapY; i++)
			{
				addIntoVector(1, glm::vec2(0, i), 1);
			}

			for (int i = 0; i <= mapY; i++)
			{
				addIntoVector(1, glm::vec2(mapX, i), 1);
			}

			for (int i = 0; i <= mapX; i++)
			{
				addIntoVector(1, glm::vec2(i, mapY), 1);
			}

			for (int i = 0; i <= mapX; i++)
			{
				addIntoVector(1, glm::vec2(i, 0), 3);
			}
			//---------------------

			for (int i = 1; i < 10; i++)
			{
				addIntoVector(1, glm::vec2(i, 10), 2);
			}

			for (int i = 1; i < 10; i++)
			{
				addIntoVector(1, glm::vec2(10, i), 2);
			}

			for (int i = 17; i < 23; i++)
			{
				addIntoVector(1, glm::vec2(i, 17), 4);
			}

			for (int i = 17; i < 23; i++)
			{
				addIntoVector(1, glm::vec2(i, 19), 5);
			}


#pragma endregion
			
		}

		Pseudo3D::~Pseudo3D()
		{
			std::cout << "Scene shutdown" << std::endl;
		}

		void Pseudo3D::update(GLfloat step)
		{

#pragma region DDAMovement
			//move forward if no wall in front of you			
			if (engine->keyboardInput->keyIsPressed(HID::KEYBOARD_W))
			{
				/*if (objectTiles[int((player.y * this->mapX) + (player.x + dirX*moveSpeed))] == 0)
				{*/
				player.x += dirX * moveSpeed;
				/*}
				if (objectTiles[int(((player.y + dirY*moveSpeed) * this->mapX) + (player.x))] == 0)
				{*/
				player.y += dirY * moveSpeed;
				/*}*/

				//if (worldMap[int(player.x + dirX * moveSpeed)][int(player.y)] == false) player.x += dirX * moveSpeed;
				//if (worldMap[int(player.x)][int(player.y + dirY * moveSpeed)] == false) player.y += dirY * moveSpeed;
			}
			//move backwards if no wall behind you
			if (engine->keyboardInput->keyIsPressed(HID::KEYBOARD_S))
			{
				/*if (objectTiles[int((player.y * this->mapX) + (player.x - dirX*moveSpeed))] == 0)
				{*/
				player.x -= dirX * moveSpeed;
				/*}
				if (objectTiles[int(((player.y - dirY*moveSpeed) * this->mapX) + (player.x))] == 0)
				{*/
				player.y -= dirY * moveSpeed;
				/*}*/

				//if (worldMap[int(player.x - dirX * moveSpeed)][int(player.y)] == false) player.x -= dirX * moveSpeed;
				//if (worldMap[int(player.x)][int(player.y - dirY * moveSpeed)] == false) player.y -= dirY * moveSpeed;
			}
			//rotate to the right   
			if (engine->keyboardInput->keyIsPressed(HID::KEYBOARD_D))
			{
				//both camera direction and camera plane must be rotated
				double oldDirX = dirX;
				dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
				dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
				double oldPlaneX = planeX;
				planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
				planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
			}
			//rotate to the left
			if (engine->keyboardInput->keyIsPressed(HID::KEYBOARD_A))
			{
				//both camera direction and camera plane must be rotated
				double oldDirX = dirX;
				dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
				dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
				double oldPlaneX = planeX;
				planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
				planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
			}
#pragma endregion
			alpha += 0.01;

			myTimer.start();

			//DDA test calculation starts.
			DDA();
			DDADrawSprites();

			myTimer.pause();

			//Information
			textCreator.createTextTexture(font, "Ray vision calculation time: " + std::to_string(myTimer.getLocalTime()) + " ms", 255, 100, 0);
			text = textCreator.getTexture();
			textCreator3.createTextTexture(font, "WASD " + std::to_string(player.x) + " " + std::to_string(player.y), 255, 100, 0);
			text3 = textCreator3.getTexture();

			//2d camera
			camera2.setPositionRotationOrigin((player.x*tileSize)+800,(player.y*tileSize));
			camera2.setRotation(glm::degrees(glm::atan(-dirX,dirY)));
		}

		void Pseudo3D::interpolate(GLfloat alpha)
		{
		}

		void Pseudo3D::draw()
		{
			textureBatch.begin();
			alphaTextureBatch.begin();
			alphaTextureBatch2.begin();

			//DDA draw test
			for (int i = 0; i < DDAlines.size(); i++)
			{
				if (DDAlines[i][2] > 0)
				{
					DDAtexture = DDAlines[i][3];
					if (DDAlines[i][1] <= 0)
					{
						depth = 0;
					}
					else
					{
						depth = (1 / DDAlines[i][1]);
					}


					switch (DDAtexture)
					{
					case 1:
					{
						//batch.drawQuad(DDAlines[i][0], DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], Renderer::clrRed);
						textureBatch.draw(doge1, &glm::vec4(DDAlinesTexX[i], 0.0f, 1.0f, tileSize), DDAlines[i][0] - 1600, DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], 0.0f, 1.0f, Renderer::clrWhite, 1.0f, depth);
						break;
					}

					case 2:
					{
						//batch.drawQuad(DDAlines[i][0], DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], Renderer::clrGreen);
						textureBatch.draw(doge2, &glm::vec4(DDAlinesTexX[i], 0.0f, 1.0f, tileSize), DDAlines[i][0] - 1600, DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], 0.0f, 1.0f, Renderer::clrWhite, 1.0f, depth);
						break;
					}
					case 3:
					{
						//batch.drawQuad(DDAlines[i][0], DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], Renderer::clrBlue);
						textureBatch.draw(doge3, &glm::vec4(DDAlinesTexX[i], 0.0f, 1.0f, tileSize), DDAlines[i][0] - 1600, DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], 0.0f, 1.0f, Renderer::clrWhite, 1.0f, depth);

						break;
					}
					case 4:
					{
						//batch.drawQuad(DDAlines[i][0], DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], Renderer::clrWhite);
						textureBatch.draw(doge4, &glm::vec4(DDAlinesTexX[i], 0.0f, 1.0f, tileSize), DDAlines[i][0] - 1600, DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], 0.0f, 1.0f, Renderer::clrWhite, 1.0f, depth);

						break;
					}
					case 5:
					{
						//batch.drawQuad(DDAlines[i][0], DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], { 1.0f, 1.0f, 0 });
						textureBatch.draw(doge5, &glm::vec4(DDAlinesTexX[i], 0.0f, 1.0f, tileSize), DDAlines[i][0] - 1600, DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], 0.0f, 1.0f, Renderer::clrWhite, 1.0f, depth);

						break;
					}

					//shade colors
					case 6:
					{
						//batch.drawQuad(DDAlines[i][0], DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], { 0.6f, 0.0f, 0.0f });
						textureBatch.draw(doge1, &glm::vec4(DDAlinesTexX[i], 0.0f, 1.0f, tileSize), DDAlines[i][0] - 1600, DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], 0.0f, 1.0f, { 0.5f, 0.5f, 0.5f }, 1.0f, depth);
						break;
					}

					case 7:
					{
						//batch.drawQuad(DDAlines[i][0], DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], { 0.0f, 0.6f, 0.0f });
						textureBatch.draw(doge2, &glm::vec4(DDAlinesTexX[i], 0.0f, 1.0f, tileSize), DDAlines[i][0] - 1600, DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], 0.0f, 1.0f, { 0.5f, 0.5f, 0.5f }, 1.0f, depth);
						break;
					}
					case 8:
					{
						//batch.drawQuad(DDAlines[i][0], DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], { 0.0f, 0.0f, 0.6f });
						textureBatch.draw(doge3, &glm::vec4(DDAlinesTexX[i], 0.0f, 1.0f, tileSize), DDAlines[i][0] - 1600, DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], 0.0f, 1.0f, { 0.5f, 0.5f, 0.5f }, 1.0f, depth);

						break;
					}
					case 9:
					{
						//batch.drawQuad(DDAlines[i][0], DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], { 0.6f, 0.6f, 0.6f });
						textureBatch.draw(doge4, &glm::vec4(DDAlinesTexX[i], 0.0f, 1.0f, tileSize), DDAlines[i][0] - 1600, DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], 0.0f, 1.0f, { 0.5f, 0.5f, 0.5f }, 1.0f, depth);
						break;
					}
					case 10:
					{
						//batch.drawQuad(DDAlines[i][0], DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], { 0.6f, 0.6f, 0 });
						textureBatch.draw(doge5, &glm::vec4(DDAlinesTexX[i], 0.0f, 1.0f, tileSize), DDAlines[i][0] - 1600, DDAlines[i][1], 1.0f, DDAlines[i][2] - DDAlines[i][1], 0.0f, 1.0f, { 0.5f, 0.5f, 0.5f }, 1.0f, depth);
						break;
					}

					default:
						break;
					}
				}
			}

			for (int i = 0; i < spriteContainer.size(); i++)
			{
				if (DDASpriteDrawData[i][0] > -800 && DDASpriteDrawData[i][0] < (800 + tileSize) && DDASpriteDrawData[i][3]>0)
				{
					if (DDASpriteDrawData[i][1] <= 0)
					{
						depth = 1.0;
					}
					else
					{
						depth = (1 / DDASpriteDrawData[i][1]);
					}
					alphaTextureBatch.draw(furball, &glm::vec4(0.0f, 0.0f, furball->getWidth(), furball->getHeight()), DDASpriteDrawData[i][0] - 1600, DDASpriteDrawData[i][1], furball->getWidth(), furball->getHeight(), 0.0f, DDASpriteDrawData[i][2], Renderer::clrRed, 1.0f, depth);
				}
				
			}
			//---------------

			//2D camera draw
			for (int i = 0; i <= mapY; i++)
			{
				for (int j = 0; j <= mapX; j++)
				{
					if (objectTiles[i*mapX + j] == 1)
					{
						textureBatch.draw(doge1, (j * tileSize) + 800, i * tileSize, 1.0f, 0.4f);
					}
					else if (objectTiles[i*mapX + j] == 2)
					{
						textureBatch.draw(doge2, (j * tileSize) + 800, i * tileSize, 1.0f, 0.4f);
					}
					else if (objectTiles[i*mapX + j] == 3)
					{
						textureBatch.draw(doge3, (j * tileSize) + 800, i * tileSize, 1.0f, 0.4f);
					}
					else if (objectTiles[i*mapX + j] == 4)
					{
						textureBatch.draw(doge4, (j * tileSize) + 800, i * tileSize, 1.0f, 0.4f);
					}
					else if (objectTiles[i*mapX + j] == 5)
					{
						textureBatch.draw(doge5, (j * tileSize) + 800, i * tileSize, 1.0f, 0.4f);
					}
				}				
			}
			for (int i = 0; i < spriteContainer.size(); i++)
			{
				alphaTextureBatch.draw(furball, &glm::vec4(0.0f, 0.0f, furball->getWidth(), furball->getHeight()), spriteContainer[i].x*tileSize + 800, spriteContainer[i].y*tileSize, furball->getWidth(), furball->getHeight(), 0.0f, 1.0f, Renderer::clrRed, 1.0f, 0.8f);
			}

			//player
			alphaTextureBatch.draw(furball, &glm::vec4(0.0f,0.0f,furball->getWidth(),furball->getHeight()),player.x*tileSize + 800, player.y*tileSize, furball->getWidth() ,furball->getHeight(),camera2.getRotation(),1.0f,Renderer::clrWhite,1.0f, 0.9f);

			//---------------

			//Hud
			alphaTextureBatch2.draw(text, &glm::vec4(0.0f, 0.0f, text->getWidth(), text->getHeight()), camera3.getPositionRotationOrigin().x + glm::cos(alpha)*400.0f-(text->getWidth()/2), camera3.getPositionRotationOrigin().y, text->getWidth(), text->getHeight(), 0.0f, 1.0f, Renderer::clrWhite, 1.0f, 1.0f);
			alphaTextureBatch2.draw(text3, camera3.getPositionRotationOrigin().x - (text3->getWidth()/2), camera3.getPositionRotationOrigin().y - 50, 1.0f, 1.0f);

			textureBatch.end();
			alphaTextureBatch.end();
			alphaTextureBatch2.end();

			camera.activateViewPort();
			batch.flush(camera);
			textureBatch.flush(camera);
			alphaTextureBatch.flush(camera);

			camera2.activateViewPort();
			textureBatch.flush(camera2);
			alphaTextureBatch.flush(camera2);

			camera3.activateViewPort();
			alphaTextureBatch2.flush(camera3);

			textureBatch.clear();
			alphaTextureBatch.clear();
			alphaTextureBatch2.clear();
			batch.clear();
		}

		void Pseudo3D::emptyVector(int vectorAsNumber)
		{
			switch (vectorAsNumber)
			{
			case 1:
			{
				for (int i = 0; i < objectTiles.size(); i++)
				{
					objectTiles[i] = 0;
				}
				break;
			}
			case 2:
			{
				for (int i = 0; i < DDAlinesTexX.size(); i++)
				{
					DDAlinesTexX[i] = 0;
				}
				break;
			}
			case 3:
			{
				for (int i = 0; i < DDAlines.size(); i++)
				{
					DDAlines[i] = glm::vec4(0, 0, 0, 0);
				}
				break;
			}
			default:
				break;
			}
		}

		void Pseudo3D::addIntoVector(int vectorAsNumber, glm::vec2 xy, int tiletype)
		{
			switch (vectorAsNumber)
			{
			case 1:
			{
				if (xy.x >= 0 && xy.y >= 0)
				{
					objectTiles[xy.y * mapX + xy.x] = tiletype;
				}
				break;
			}
			default:
				break;
			}
		}

		void Pseudo3D::DDA()
		{
			for (int x = 0; x < w; x++)
			{
				//calculate ray position and direction 
				double cameraX = 2 * x / double(w) - 1; //x-coordinate in camera space
				double rayPosX = player.x;
				double rayPosY = player.y;
				double rayDirX = dirX + planeX * cameraX;
				double rayDirY = dirY + planeY * cameraX;

				//which box of the map we're in  
				DDAX = int(rayPosX);
				DDAY = int(rayPosY);

				//length of ray from current position to next x or y-side
				double sideDistX;
				double sideDistY;

				//length of ray from one x or y-side to next x or y-side
				double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
				double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
				double perpWallDist;

				//what direction to step in x or y-direction (either +1 or -1)
				int stepX;
				int stepY;

				int hit = 0; //was there a wall hit?
				int side; //was a NS or a EW wall hit?

				//calculate step and initial sideDist
				if (rayDirX < 0)
				{
					stepX = -1;
					sideDistX = (rayPosX - DDAX) * deltaDistX;
				}
				else
				{
					stepX = 1;
					sideDistX = (DDAX + 1.0 - rayPosX) * deltaDistX;
				}
				if (rayDirY < 0)
				{
					stepY = -1;
					sideDistY = (rayPosY - DDAY) * deltaDistY;
				}
				else
				{
					stepY = 1;
					sideDistY = (DDAY + 1.0 - rayPosY) * deltaDistY;
				}

				//perform DDA
				while (hit == 0)
				{
					//jump to next map square, OR in x-direction, OR in y-direction
					if (sideDistX < sideDistY)
					{
						sideDistX += deltaDistX;
						DDAX += stepX;
						side = 0;
					}
					else
					{
						sideDistY += deltaDistY;
						DDAY += stepY;
						side = 1;
					}
					//Check if ray has hit a wall
					if (objectTiles[DDAY * this->mapX + DDAX] > 0)
					{
						hit = 1;
					}
					//if (worldMap[mapX][mapY] > 0) hit = 1;
				}

				//Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
				if (side == 0)
					perpWallDist = fabs((DDAX - rayPosX + (1 - stepX) / 2) / rayDirX);
				else
					perpWallDist = fabs((DDAY - rayPosY + (1 - stepY) / 2) / rayDirY);

				//Calculate height of line to draw on screen
				int lineHeight = glm::abs(int(h / perpWallDist));

				//calculate lowest and highest pixel to fill in current stripe
				int drawStart = -lineHeight / 2 + h / 2;
				if (drawStart < 0)drawStart = 0;
				int drawEnd = lineHeight / 2 + h / 2;
				if (drawEnd >= h)drawEnd = h - 1;

				//texturing calculations
				int texNum = objectTiles[DDAY * this->mapX + DDAX] - 1; //1 subtracted from it so that texture 0 can be used!

				//calculate value of wallX
				double wallX; //where exactly the wall was hit
				if (side == 1) wallX = rayPosX + ((DDAY - rayPosY + (1 - stepY) / 2) / rayDirY) * rayDirX;
				else       wallX = rayPosY + ((DDAX - rayPosX + (1 - stepX) / 2) / rayDirX) * rayDirY;
				wallX -= floor((wallX));

				//x coordinate on the texture
				int texX = int(wallX * double(tileSize));
				if (side == 0 && rayDirX > 0) texX = tileSize - texX - 1;
				if (side == 1 && rayDirY < 0) texX = tileSize - texX - 1;

				//choose wall color			
				switch (objectTiles[DDAY * this->mapX + DDAX])
				{
				case 1:  drawColor = 1;  break; //red
				case 2:  drawColor = 2;  break; //green
				case 3:  drawColor = 3;   break; //blue
				case 4:  drawColor = 4;  break; //white
				default: drawColor = 5; break; //yellow
				}
				if (side == 1) { drawColor += 5; }
				//else { DDAtexture = 2; }
				
				//Saving calculated data for camera slice 
				DDAlines[x] = glm::vec4(x, drawStart, drawEnd, drawColor);
				DDAlinesTexX[x] = texX;
				//---------------------------------------
			}			
		}

		void Pseudo3D::DDADrawSprites()
		{
			for (int i = 0; i < spriteContainer.size(); i++)
			{
				double spriteX;
				double spriteY;
				double spriteXout;
				double spriteYout;
				double spriteScale;

				spriteX = spriteContainer[i].x - player.x;
				spriteY = spriteContainer[i].y - player.y;

				glm::vec2 transform;

				//Creating sprite transform vector
				transform = glm::vec2(spriteX, spriteY) * glm::inverse(glm::mat2x2(planeX,dirX,planeY,dirY));

				//Calculating camera x for sprite				
				spriteXout = (w / 2)*(1 + transform.x / transform.y);				

				//Calculating camera y and scale for sprite
				double spriteHeight = glm::abs(h / transform.y);
				spriteYout = -spriteHeight / 2 + h / 2;
				spriteScale = 1 / (tileSize / spriteHeight);

				DDASpriteDrawData[i] = glm::vec4(spriteXout,spriteYout,spriteScale,transform.y);
			}
		}
	}
}