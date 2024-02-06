#include "raylib.h"
#include "raymath.h"
#include "main.h"
#include <stddef.h>

EntityComponentList components;
const float playerSpeed = 100;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
	// Initialization
	//--------------------------------------------------------------------------------------
	const int screenWidth = 800;
	const int screenHeight = 450;

	InitWindow(screenWidth, screenHeight, "Trials of Yarbil");

	SetTargetFPS(60); // Set our game to run at 60 frames-per-second
	//--------------------------------------------------------------------------------------

	InitializeEntityComponentList();

	int playerId = NewPlayer();

	// Main game loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{

		DirectionalInputSystem();

		PlayerInputSystem();

		ApplyVelocitySystem(GetFrameTime());
		// Update
		//----------------------------------------------------------------------------------
		// TODO: Update your variables here
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(RAYWHITE);

		Vector2 playerPos = components.positionComponents[playerId].pos;

		DrawCircle((int)playerPos.x, (int)playerPos.y, 10, RED);

		DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow(); // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}

void DirectionalInputSystem()
{
	Vector2 direction = Vector2Zero();
	if (IsKeyDown(KEY_RIGHT))
		direction.x++;
	if (IsKeyDown(KEY_LEFT))
		direction.x--;
	if (IsKeyDown(KEY_DOWN))
		direction.y++;
	if (IsKeyDown(KEY_UP))
		direction.y--;

	components.directionalInputComponent.direction = direction;
}

void PlayerInputSystem()
{
	Vector2 direction = components.directionalInputComponent.direction;
	direction = Vector2Normalize(direction);
	Vector2 newVel = Vector2Scale(direction, playerSpeed);

	// j counts how many active entities the loop has gone through while i is the current index in the entityIsActive array
	for (int i = 0, j = 0; j < components.totalActiveEntities && i < MAX_ENTITIES; i++)
	{
		// If the entity is not active then skip to the next iteration
		if (!components.entityIsActive[i])
			continue;

		// Check if the entity accepts player input and if the entity has a velocity component
		if (components.flagsComponents[i].playerInput && components.velocityComponents[i].entityId == i)
		{
			components.velocityComponents[i].vel = newVel;
		}

		j++;
	}
}

void ApplyVelocitySystem(float delta)
{
	// j counts how many active entities the loop has gone through while i is the current index in the entityIsActive array
	for (int i = 0, j = 0; j < components.totalActiveEntities && i < MAX_ENTITIES; i++)
	{
		// If the entity is not active then skip to the next iteration
		if (!components.entityIsActive[i])
			continue;

		// Check if the entity has a position and velocity component
		if (components.positionComponents[i].entityId == i && components.velocityComponents[i].entityId == i)
		{
			Vector2 addPos = Vector2Scale(components.velocityComponents[i].vel, delta);
			components.positionComponents[i].pos.x += addPos.x;
			components.positionComponents[i].pos.y += addPos.y;
		}

		j++;
	}
}

void InitializeEntityComponentList()
{
	// Initialize Single Components
	components.directionalInputComponent.direction = Vector2Zero();

	// Initialize Components
	for (int i = 0; i < MAX_ENTITIES; i++)
	{
		components.colliderComponents[i].entityId = NULL_ENTITY_ID;
		components.positionComponents[i].entityId = NULL_ENTITY_ID;
		components.velocityComponents[i].entityId = NULL_ENTITY_ID;
		components.flagsComponents[i].entityId = NULL_ENTITY_ID;
	}

	// Total active entities and buffer size should be set to 0
	components.totalActiveEntities = 0;
	components.idBufferSize = 0;
}

// Creates a new entity in the given EntityComponentList components and returns the id of that entity in the list
int NewEntity()
{
	// Get the new entity's id. If the id buffer is larger than zero, then use the id at the top of the buffer stack
	// and decrease the buffer size,
	// otherwise use the amount of total active entities
	int entityId = components.idBufferSize > 0 ? components.idBuffer[components.idBufferSize--] : components.totalActiveEntities;

	// Increase the total active entities whether or not we get the id from the buffer or not.
	components.totalActiveEntities++;
	// Set the entity with entityId to be active
	components.entityIsActive[entityId] = true;
	// Give the entity a flags component
	components.flagsComponents[entityId].entityId = entityId;
	components.flagsComponents[entityId].playerInput = false;

	return entityId;
}

int NewPlayer()
{
	int playerId = NewEntity(components);

	components.positionComponents[playerId].entityId = playerId;
	components.velocityComponents[playerId].entityId = playerId;
	components.flagsComponents[playerId].playerInput = true;

	return playerId;
}

// Frees the entity at the given
void FreeEntity(int entityId)
{
	// If the entity is not active then we don't need to free it again
	if (!components.entityIsActive[entityId])
		return;
	// Set the entityId of all the components of this entity to null entity id
	components.colliderComponents[entityId].entityId = NULL_ENTITY_ID;
	components.positionComponents[entityId].entityId = NULL_ENTITY_ID;
	components.velocityComponents[entityId].entityId = NULL_ENTITY_ID;
	components.flagsComponents[entityId].entityId = NULL_ENTITY_ID;

	// Set the entity to not be active
	components.entityIsActive[entityId] = false;
	// Decrease the amount of active entities
	components.totalActiveEntities--;
	// Add the entity id to the id buffer
	components.idBuffer[components.idBufferSize++] = entityId;
}