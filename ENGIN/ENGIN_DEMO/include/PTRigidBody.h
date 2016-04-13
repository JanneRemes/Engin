#pragma once
#include "glm\glm.hpp"
#include "GL\glew.h"
#include "PTPhysicsWorld.h"

class PTPhysicsWorld;

class PTRigidBody
{
public:
	PTRigidBody(PTPhysicsWorld* world);
	~PTRigidBody();

	void update(GLfloat step);

	void setPosition(glm::vec2 &position);
	glm::vec2 getPosition() const;

	void setRotation(GLfloat &rotation);
	GLfloat getRotation() const;

	void setVelocity(glm::vec2& velocity);
	void setHalfSize(glm::vec2& halfSize);
	glm::vec2 getHalfSize();

	void setCollisionOn();
	void setCollisionOff();
	bool isColliding();
private:
	glm::vec2 position;
	GLfloat rotation;
	glm::vec2 velocity;
	glm::vec2 positionOld;
	GLfloat rotationOld;
	GLfloat angularVelocity;

	PTPhysicsWorld* world;

	glm::vec2 halfSize;

	bool colliding;
};