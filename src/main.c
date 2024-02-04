#include "raylib.h"
#include "raymath.h"
#include "main.h"
#include <stddef.h>

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

	EntityComponentList components;
	InitializeEntityComponentList(&components);

	// Main game loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		// Update
		//----------------------------------------------------------------------------------
		// TODO: Update your variables here
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(RAYWHITE);

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

void InitializeEntityComponentList(EntityComponentList *components)
{
	// Initialize Single Components
	components->directionalInputComponent.direction = Vector2Zero();

	// Initialize Components
	for (int i = 0; i < MAX_ENTITIES; i++)
	{
		components->colliderComponents[i].entityId = NULL_ENTITY_ID;
		components->positionComponents[i].entityId = NULL_ENTITY_ID;
		components->velocityComponents[i].entityId = NULL_ENTITY_ID;
		components->flagsComponents[i].entityId = NULL_ENTITY_ID;
	}

	// Total active entities and buffer size should be set to 0
	components->totalActiveEntities = 0;
	components->idBufferSize = 0;
}

// Creates a new entity in the given EntityComponentList components and returns the id of that entity in the list
int NewEntity(EntityComponentList *components)
{
	// Get the new entity's id. If the id buffer is larger than zero, then use the id at the top of the buffer stack
	// and decrease the buffer size,
	// otherwise use the amount of total active entities
	int entityId = components->idBufferSize > 0 ? components->idBuffer[components->idBufferSize--] : components->totalActiveEntities;

	// Increase the total active entities whether or not we get the id from the buffer or not.
	components->totalActiveEntities++;
	// Set the entity with entityId to be active
	components->entityIsActive[entityId] = true;
	// Give the entity a flags component
	components->flagsComponents[entityId].entityId = entityId;

	return entityId;
}

// Frees the entity at the given
void FreeEntity(EntityComponentList *components, int entityId)
{
	// If the entity is not active then we don't need to free it again
	if (!components->entityIsActive[entityId])
		return;
	// Set the entityId of all the components of this entity to null entity id
	components->colliderComponents[entityId].entityId = NULL_ENTITY_ID;
	components->positionComponents[entityId].entityId = NULL_ENTITY_ID;
	components->velocityComponents[entityId].entityId = NULL_ENTITY_ID;
	components->flagsComponents[entityId].entityId = NULL_ENTITY_ID;

	// Set the entity to not be active
	components->entityIsActive[entityId] = false;
	// Decrease the amount of active entities
	components->totalActiveEntities--;
	// Add the entity id to the id buffer
	components->idBuffer[components->idBufferSize++] = entityId;
}