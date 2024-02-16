#include "raylib.h"
#include "raymath.h"
#include "main.h"
#include <stddef.h>
#include <ctype.h>
#include <stdlib.h>

EntityComponentList components;
const float playerSpeed = 100;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
	// Initialization
	//--------------------------------------------------------------------------------------
	const int screenWidth = 640;
	const int screenHeight = 360;

	InitWindow(screenWidth, screenHeight, "Trials of Yarbil");

	SetTargetFPS(60); // Set our game to run at 60 frames-per-second
	//--------------------------------------------------------------------------------------

	// Setup tilemap. Tilemap data is stored in a 1D array
	// const int emptyTile = 0;
	// const int wallTile = 1;
	// int tilemapData[TILEMAP_MAX_WIDTH * TILEMAP_MAX_HEIGHT];
	// for (size_t i = 0; i < sizeof(tilemapData) / sizeof(tilemapData[0]); i++)
	// {
	// 	tilemapData[i] = emptyTile;
	// }

	// tilemapData[1] = wallTile;
	// tilemapData[4] = wallTile;

	InitializeEntityComponentList();

	int playerId = NewPlayer();
	// SaveTilemap("map/map01.txt", tilemapData, sizeof(tilemapData) / sizeof(tilemapData[0]));

	components.spriteComponents[playerId].tex = LoadTexture("assets/testsprite.png");

	components.tileset.tileSize.x = TILE_SIZE;
	components.tileset.tileSize.y = TILE_SIZE;
	components.tileset.emptyTileId = 0;
	components.tileset.tiles[1] = LoadTexture("assets/testtile.png");

	SetTileRect((Rectangle){2, 2, 4, 1}, 1);
	float x = 0;
	// Main game loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		x += GetFrameTime();
		DirectionalInputSystem();

		PlayerInputSystem();

		ApplyVelocitySystem(GetFrameTime());
		// Update
		SetTileRect((Rectangle){(int)x, 20, 3, 1}, 2);
		SetTileRect((Rectangle){(int)x - 5, 20, 3, 1}, components.tileset.emptyTileId);
		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();
		{
			ClearBackground(RAYWHITE);

			DrawSpritesSystem();
			DrawTilesSystem();
		}
		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow(); // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}

// Returns the entity id of the set tile
int SetTile(Vector2 tilePos, int tileId)
{
	int entityId;
	if ((entityId = GetTileEntity(tilePos)) != NULL_ENTITY_ID)
	{
		components.tileComponents[entityId].tileId = tileId;
		return entityId;
	}

	entityId = NewEntity();
	components.tileComponents[entityId].entityId = entityId;
	components.tileComponents[entityId].tilePos = tilePos;
	components.tileComponents[entityId].tileId = tileId;
	return entityId;
}

void SetTileRect(Rectangle rect, int tileId)
{
	for (float x = rect.x; x < rect.x + rect.width; x++)
	{
		for (float y = rect.y; y < rect.y + rect.height; y++)
		{
			SetTile((Vector2){x, y}, tileId);
		}
	}
}

// Frees the entity with a tile component that has the given tile pos
void FreeTile(Vector2 tilePos)
{
	struct Tile *tile = GetTile(tilePos);
	if (tile)
		FreeEntity(tile->entityId);
}

// Returns the entity id at tile pos. If there is no tile component returns NULL_ENTITY_ID
int GetTileEntity(Vector2 tilePos)
{
	struct Tile *tile = GetTile(tilePos);
	return tile ? tile->entityId : NULL_ENTITY_ID;
}

// Returns the tile id at tile pos. Returns tileset.emptyTileId if there is no tile component
int GetTileId(Vector2 tilePos)
{
	struct Tile *tile = GetTile(tilePos);
	return tile ? tile->tileId : components.tileset.emptyTileId;
}

// Returns a pointer to the Tile component at the given tilePos. returns NULL if there is no tile component
struct Tile *GetTile(Vector2 tilePos)
{
	for (int i = 0, j = 0; j < components.totalActiveEntities && i < MAX_ENTITIES; i++)
	{
		// If the entity is not active then skip to the next iteration
		if (!components.entityIsActive[i])
			continue;

		if (components.tileComponents[i].entityId == i && Vector2Equals(components.tileComponents[i].tilePos, tilePos))
			return &components.tileComponents[i];

		j++;
	}
	return NULL;
}

void LoadTilemap(const char *fileName)
{
	// Load data or text
	// Free all entities with tile components
	// Create all entities with tiles from data
	// Unload data or text
}
void SaveTilemap(const char *fileName)
{
	// Create a byte array or string with each tile entity separated
	char data[MAX_ENTITIES * (sizeof(struct EntityData))];
	// How do I store an entity in text?
	// Store components. If entity doesn't have component leave empty, to avoid storing unnecessary and extraneous data.
	for (int i = 0, j = 0; j < components.totalActiveEntities && i < MAX_ENTITIES; i++)
	{
		// If the entity is not active then skip to the next iteration
		if (!components.entityIsActive[i])
			continue;

		if (components.tileComponents[i].entityId == i)
		{
		}

		j++;
	}
	// Save that info
}

// int GetNextInteger(const char *str, int *intLength)
// {
// 	int number = -1;
// 	*intLength = 0;
// 	while (isdigit(*str))
// 	{
// 		(*intLength)++;
// 		if (number < 0)
// 			number = 0;
// 		number += number * 10 + *str - '0';
// 		str++;
// 	}
// 	return number;
// }

void DrawTilesSystem()
{
	for (int i = 0, j = 0; j < components.totalActiveEntities && i < MAX_ENTITIES; i++)
	{
		// If the entity is not active then skip to the next iteration
		if (!components.entityIsActive[i])
			continue;

		if (components.tileComponents[i].entityId == i && components.tileComponents[i].tileId != components.tileset.emptyTileId)
		{
			Vector2 tilePos = components.tileComponents[i].tilePos;
			Vector2 tileSize = components.tileset.tileSize;
			Texture2D tex = components.tileset.tiles[components.tileComponents[i].tileId];
			if (IsTextureReady(tex))
				DrawTextureV(tex, Vector2Multiply(tilePos, tileSize), WHITE);
			else // If the texture of the tile is not ready then display a placeholder
				DrawRectangleLines((int)(tilePos.x * tileSize.x), (int)(tilePos.y * tileSize.y), (int)tileSize.x, (int)tileSize.y, BLACK);
		}

		j++;
	}
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

// This must be called after BeginDrawing() and before EndDrawing()
void DrawSpritesSystem()
{
	for (int i = 0, j = 0; j < components.totalActiveEntities && i < MAX_ENTITIES; i++)
	{
		// If the entity is not active then skip to the next iteration
		if (!components.entityIsActive[i])
			continue;

		if (components.positionComponents[i].entityId == i && components.spriteComponents[i].entityId == i)
		{
			DrawTextureV(components.spriteComponents[i].tex, components.positionComponents[i].pos, WHITE);
		}

		j++;
	}
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
		if (components.flagsComponents[i].receiveDirectionalInput && components.velocityComponents[i].entityId == i)
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
		components.tileComponents[i].entityId = NULL_ENTITY_ID;
		components.positionComponents[i].entityId = NULL_ENTITY_ID;
		components.velocityComponents[i].entityId = NULL_ENTITY_ID;
		components.spriteComponents[i].entityId = NULL_ENTITY_ID;
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
	// Give the entity a flags component with default values
	components.flagsComponents[entityId].entityId = entityId;
	components.flagsComponents[entityId].receiveDirectionalInput = false;

	return entityId;
}

int NewEntityFromData(struct EntityData *data)
{
	int entityId = NewEntity();

	if (data->tileComponent.entityId != NULL_ENTITY_ID)			   // Check to see if the entity has the component
	{															   // by seeing if the entity id of the component is null or not
		components.tileComponents[entityId] = data->tileComponent; // Copy the info from the component
		components.tileComponents[entityId].entityId = entityId;   // Set the entity id to the proper id
	}
	if (data->colliderComponent.entityId != NULL_ENTITY_ID)
	{
		components.colliderComponents[entityId] = data->colliderComponent; // Copy the info from the component
		components.colliderComponents[entityId].entityId = entityId;	   // Set the entity id to the proper id
	}
	if (data->positionComponent.entityId != NULL_ENTITY_ID)
	{
		components.positionComponents[entityId] = data->positionComponent; // Copy the info from the component
		components.positionComponents[entityId].entityId = entityId;	   // Set the entity id to the proper id
	}
	if (data->spriteComponent.entityId != NULL_ENTITY_ID)
	{
		components.spriteComponents[entityId] = data->spriteComponent; // Copy the info from the component
		components.spriteComponents[entityId].entityId = entityId;	   // Set the entity id to the proper id
	}
	if (data->flagsComponent.entityId != NULL_ENTITY_ID)
	{
		components.flagsComponents[entityId] = data->flagsComponent; // Copy the info from the component
		components.flagsComponents[entityId].entityId = entityId;	 // Set the entity id to the proper id
	}
	return entityId;
}

int NewPlayer()
{
	int playerId = NewEntity();

	components.positionComponents[playerId].entityId = playerId;
	components.velocityComponents[playerId].entityId = playerId;
	components.spriteComponents[playerId].entityId = playerId;
	components.flagsComponents[playerId].receiveDirectionalInput = true;

	return playerId;
}

// Frees the entity at the given
void FreeEntity(int entityId)
{
	// If the entityId is the NULL_ENTITY_ID or if the entity is not active then we don't need to free it
	if (entityId == NULL_ENTITY_ID || !components.entityIsActive[entityId])
		return;
	// Set the entityId of all the components of this entity to null entity id
	components.colliderComponents[entityId].entityId = NULL_ENTITY_ID;
	components.tileComponents[entityId].entityId = NULL_ENTITY_ID;
	components.positionComponents[entityId].entityId = NULL_ENTITY_ID;
	components.velocityComponents[entityId].entityId = NULL_ENTITY_ID;
	components.spriteComponents[entityId].entityId = NULL_ENTITY_ID;
	components.flagsComponents[entityId].entityId = NULL_ENTITY_ID;

	// Set the entity to not be active
	components.entityIsActive[entityId] = false;
	// Decrease the amount of active entities
	components.totalActiveEntities--;
	// Add the entity id to the id buffer
	components.idBuffer[components.idBufferSize++] = entityId;
}