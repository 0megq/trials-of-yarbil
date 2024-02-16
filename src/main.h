#pragma once
#include "raylib.h"
#include <stddef.h>

#define NULL_ENTITY_ID -1
#define MAX_ENTITIES 100
#define TILEMAP_MAX_WIDTH 100
#define TILEMAP_MAX_HEIGHT 100
#define TILE_SIZE 16
#define MAX_TILE_TYPES 50

enum Element
{
	ELEMENT_FIRE,
	ELEMENT_WATER,
	ELEMENT_EARTH,
	ELEMENT_AIR,
	ELEMENT_NONE,
};

enum ShapeType
{
	TYPE_CIRCLE,
	TYPE_RECTANGLE,
};

union Shape
{
	Vector2 size;
	float radius;
};

// Stores information for how to render the tile components in an entity component list
struct Tileset
{
	int emptyTileId;
	Vector2 tileSize;
	Texture2D tiles[MAX_TILE_TYPES];
};

// Tile component
struct Tile
{
	int entityId;
	Vector2 tilePos;
	int tileId;
};

// Position Component
struct Position
{
	int entityId;
	Vector2 pos;
};

// Collider Component
struct Collider
{
	int entityId;
	enum ShapeType type;
	union Shape *shape;
};

// Velocity Component
struct Velocity
{
	int entityId;
	Vector2 vel;
};

// Texture Component
struct Sprite
{
	int entityId;
	Texture2D tex;
};

// Player Component
struct Player
{
	enum Element playerElement;
};

// There only needs to be one directional input component at a single time
struct DirectionalInput
{
	Vector2 direction;
};

// Every entity will have a flags component
struct Flags
{
	int entityId;
	unsigned receiveDirectionalInput : 1;
};

// Used for saving and loading data about a certain entity. This should include all components in the game
struct EntityData
{
	struct Tile tileComponent;
	struct Collider colliderComponent;
	struct Position positionComponent;
	struct Velocity velocityComponent;
	struct Sprite spriteComponent;
	struct Flags flagsComponent;
};

typedef struct EntityComponentList
{
	// Single Components i.e. components that are universal across entities like input
	struct DirectionalInput directionalInputComponent;

	// Components
	struct Tile tileComponents[MAX_ENTITIES];
	struct Collider colliderComponents[MAX_ENTITIES];
	struct Position positionComponents[MAX_ENTITIES];
	struct Velocity velocityComponents[MAX_ENTITIES];
	struct Sprite spriteComponents[MAX_ENTITIES];
	struct Flags flagsComponents[MAX_ENTITIES];

	// Other data
	bool entityIsActive[MAX_ENTITIES];
	int totalActiveEntities;
	int idBuffer[MAX_ENTITIES];
	int idBufferSize;
	struct Tileset tileset;
} EntityComponentList;

void DirectionalInputSystem(void);
void AttackInputSystem(void);
void DrawSpritesSystem(void);
void PlayerInputSystem(void);
void ApplyVelocitySystem(float delta);
void InitializeEntityComponentList(void);
int NewEntity(void);
int NewEntityFromData(struct EntityData *data);
int NewPlayer(void);
void FreeEntity(int entityId);
// void LoadTilemap(const char *fileName, int tilemapData[], size_t length);
// void SaveTilemap(const char *fileName, int tilemapData[], size_t length);
// int GetNextInteger(const char *str, int *pos);
int SetTile(Vector2 tilePos, int tileId);
void RemoveTile(Vector2 tilePos);
int GetTileEntity(Vector2 tilePos);
int GetTileId(Vector2 tilePos);
struct Tile *GetTile(Vector2 tilePos);
void SetTileRect(Rectangle rect, int tileId);
void DrawTilesSystem(void);