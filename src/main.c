#include "raylib.h"
#include "raymath.h"
#include "main.h"
#include <stddef.h>
#include <ctype.h>
#include <stdlib.h>
#include <strings.h>

EntityComponentList components;
const float playerSpeed = 100;
Texture2D loadedTextures[MAX_TEXTURES];

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
	// Initialization
	//--------------------------------------------------------------------------------------

	// TraceLog(LOG_INFO, "EntityData %llu, Tile: %llu, Collider: %llu, Pos: %llu, Vel: %llu, Sprite: %llu, Flags: %llu", sizeof(struct EntityData), sizeof(struct Tile), sizeof(struct Collider), sizeof(struct Position), sizeof(struct Velocity), sizeof(struct Sprite), sizeof(struct Flags));

	const int screenWidth = 960;
	const int screenHeight = 540;

	const int virtualScreenWidth = 320;
	const int virtualScreenHeight = 180;

	const float virtualRatio = (float)screenWidth / (float)virtualScreenWidth;

	InitWindow(screenWidth, screenHeight, "Trials of Yarbil");

	RenderTexture2D virtualScreen = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);

	// The target's height is flipped (in the source Rectangle), due to OpenGL reasons
	Rectangle sourceRec = {0.0f, 0.0f, (float)virtualScreen.texture.width, -(float)virtualScreen.texture.height};
	Rectangle destRec = {-virtualRatio, -virtualRatio, (float)screenWidth + (virtualRatio * 2), (float)screenHeight + (virtualRatio * 2)};

	Vector2 origin = {0.0f, 0.0f};

	LoadTextures();

	SetTargetFPS(60); // Set our game to run at 60 frames-per-second
	//--------------------------------------------------------------------------------------

	InitializeEntityComponentList();

	// New player with player sprite
	int playerId = NewPlayer();
	components.spriteComponents[playerId].texId = TEX_PLAYER;

	// Setup tileset
	components.tileset.tileSize.x = TILE_SIZE;
	components.tileset.tileSize.y = TILE_SIZE;
	components.tileset.emptyTileId = 0;
	components.tileset.tiles[1] = LoadTexture("assets/testtile.png");

	// Tile saving test
	SetTileRect((Rectangle){2, 2, 4, 1}, 1);
	int myTileId = SetTile((Vector2){3, 4}, 2);

	components.positionComponents[myTileId].entityId = myTileId;
	components.positionComponents[myTileId].pos = (Vector2){160, 90};
	components.spriteComponents[myTileId].entityId = myTileId;
	components.spriteComponents[myTileId].texId = TEX_TESTSPRITE;

	// struct EntityData testData;

	// testData.colliderComponent.entityId = -1;
	// testData.positionComponent.entityId = 1;
	// testData.positionComponent.pos = (Vector2){100, 0};
	// testData.velocityComponent.entityId = 1;
	// testData.spriteComponent.entityId = 1;
	// testData.spriteComponent.texId = TEX_PLAYER;
	// testData.tileComponent.entityId = -1;
	// testData.flagsComponent.entityId = 1;
	// testData.flagsComponent.receiveDirectionalInput = 1;
	// NewEntityFromData(&testData);

	// printf("before: %i ", components.totalActiveEntities);
	// FreeTiles();
	// printf("after: %i\n", components.totalActiveEntities);

	// Main game loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		if (IsKeyPressed(KEY_L))
			LoadTilemap("map/map.toy");
		else if (IsKeyPressed(KEY_S))
			SaveTilemap("map/map.toy");

		DirectionalInputSystem();

		PlayerInputSystem();

		ApplyVelocitySystem(GetFrameTime());
		// Update
		// SetTileRect((Rectangle){floorf(x), 5, 3, 1}, 2);
		// SetTileRect((Rectangle){floorf(x - 5), 5, 3, 1}, components.tileset.emptyTileId);
		// Draw
		//----------------------------------------------------------------------------------
		BeginTextureMode(virtualScreen);
		{
			ClearBackground(RAYWHITE);

			DrawSpritesSystem();
			DrawTilesSystem();
		}
		EndTextureMode();

		BeginDrawing();
		{
			ClearBackground(RAYWHITE);
			DrawTexturePro(virtualScreen.texture, sourceRec, destRec, origin, 0.0f, WHITE);
		}
		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	UnloadTextures();
	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow(); // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}

void LoadTextures(void)
{
	loadedTextures[TEX_PLAYER] = LoadTexture("assets/player.png");
	loadedTextures[TEX_TESTSPRITE] = LoadTexture("assets/testsprite.png");
}

void UnloadTextures(void)
{
	for (size_t i = 0; i < MAX_TEXTURES; i++)
		if (IsTextureReady(loadedTextures[i]))
			UnloadTexture(loadedTextures[i]);
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
	for (float y = rect.y; y < rect.y + rect.height; y++)
	{
		for (float x = rect.x; x < rect.x + rect.width; x++)
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
	int dataSize;
	unsigned char *rawData = LoadFileData(fileName, &dataSize);
	size_t tileEntityCount = (size_t)dataSize / sizeof(struct EntityData);
	struct EntityData data[tileEntityCount];
	size_t bytesToCopy = tileEntityCount * sizeof(struct EntityData);
	memcpy(data, rawData, bytesToCopy);
	// Free all entities with tile components
	FreeTiles();
	// Create all entities with tiles from data
	for (size_t i = 0; i < tileEntityCount; i++)
	{
		NewEntityFromData(&data[i]);
	}
	// Unload data or text
	UnloadFileData(rawData);
}

void SaveTilemap(const char *fileName)
{
	// Create an array with all the entity data
	struct EntityData data[MAX_ENTITIES];
	size_t tileCount = 0;

	// How do I store an entity in text?
	// Store components. TODO: If entity doesn't have component leave empty, to avoid storing unnecessary and extraneous data.
	for (int i = 0, j = 0; j < components.totalActiveEntities && i < MAX_ENTITIES; i++)
	{
		// If the entity is not active then skip to the next iteration
		if (!components.entityIsActive[i])
			continue;

		if (components.tileComponents[i].entityId == i)
			data[tileCount++] = GetEntityData(i);

		j++;
	}

	// Save that info
	SaveFileData(fileName, data, (int)(tileCount * sizeof(struct EntityData)));
}

void FreeTiles(void)
{
	for (int i = 0, j = 0; j < components.totalActiveEntities && i < MAX_ENTITIES; i++)
	{
		// If the entity is not active then skip to the next iteration
		if (!components.entityIsActive[i])
			continue;

		if (components.tileComponents[i].entityId == i)
		{
			FreeEntity(i);
			j--; // go back down one because now there are less entities
		}
		j++;
	}
}

struct EntityData GetEntityData(int entityId)
{
	struct EntityData data;
	data.tileComponent = components.tileComponents[entityId];
	data.positionComponent = components.positionComponents[entityId];
	data.colliderComponent = components.colliderComponents[entityId];
	data.spriteComponent = components.spriteComponents[entityId];
	data.velocityComponent = components.velocityComponents[entityId];
	data.flagsComponent = components.flagsComponents[entityId];
	return data;
}

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
			Texture2D tex;
			if (components.tileComponents[i].tileId < MAX_TILE_TYPES && IsTextureReady(tex = components.tileset.tiles[components.tileComponents[i].tileId]))
				DrawTextureV(tex, Vector2Multiply(tilePos, tileSize), WHITE);
			else // If the texture of the tile is not ready then display a placeholder
				DrawRectangleLines((int)(tilePos.x * tileSize.x), (int)(tilePos.y * tileSize.y), (int)tileSize.x, (int)tileSize.y, BLACK);
		}

		j++;
	}
}

void DirectionalInputSystem(void)
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
void DrawSpritesSystem(void)
{
	for (int i = 0, j = 0; j < components.totalActiveEntities && i < MAX_ENTITIES; i++)
	{
		// If the entity is not active then skip to the next iteration
		if (!components.entityIsActive[i])
			continue;

		if (components.positionComponents[i].entityId == i && components.spriteComponents[i].entityId == i)
		{
			if (components.spriteComponents[i].texId < MAX_TEXTURES && IsTextureReady(loadedTextures[components.spriteComponents[i].texId]))
				DrawTextureV(loadedTextures[components.spriteComponents[i].texId], components.positionComponents[i].pos, WHITE);
			else
				DrawCircleV(components.positionComponents[i].pos, 10, RED); // Draw a red circle in place of the invalid texture
		}

		j++;
	}
}

void PlayerInputSystem(void)
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

void InitializeEntityComponentList(void)
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
int NewEntity(void)
{
	// Get the new entity's id. If the id buffer is larger than zero, then use the id at the top of the buffer stack
	// and decrease the buffer size,
	// otherwise use the amount of total active entities
	int entityId = components.idBufferSize > 0 ? components.idBuffer[--components.idBufferSize] : components.totalActiveEntities;
	if (entityId >= MAX_ENTITIES) // Too many entities
	{
		TraceLog(LOG_FATAL, "Ran out of entity memory. Tried to create entity #%i out of %i total entities", entityId + 1, MAX_ENTITIES);
		abort();
	}

	components.tileComponents[entityId].entityId = -1;
	components.positionComponents[entityId].entityId = -1;
	components.colliderComponents[entityId].entityId = -1;
	components.spriteComponents[entityId].entityId = -1;
	components.velocityComponents[entityId].entityId = -1;

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
	if (data->velocityComponent.entityId != NULL_ENTITY_ID)
	{
		components.velocityComponents[entityId] = data->velocityComponent; // Copy the info from the component
		components.velocityComponents[entityId].entityId = entityId;	   // Set the entity id to the proper id
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

int NewPlayer(void)
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