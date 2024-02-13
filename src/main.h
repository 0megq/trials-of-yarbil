#pragma once
#include "raylib.h"

#define NULL_ENTITY_ID -1
#define MAX_ENTITIES 100
#define TILEMAP_MAX_WIDTH 100
#define TILEMAP_MAX_HEIGHT 100
#define TILE_SIZE 16

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

typedef struct EntityComponentList
{
	// Single Components i.e. components that are universal across entities like input
	struct DirectionalInput directionalInputComponent;

	// Components
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
} EntityComponentList;

void DrawTilemap(int tilemapData[], size_t mapWidth, size_t mapHeight);
void DirectionalInputSystem();
void DrawSpritesSystem();
void PlayerInputSystem();
void ApplyVelocitySystem(float delta);
void InitializeEntityComponentList();
int NewEntity();
int NewPlayer();
void FreeEntity(int entityId);
void LoadTilemap(const char *fileName, int tilemapData[], size_t length);
void SaveTilemap(const char *fileName, int tilemapData[], size_t length);
int GetNextInteger(const char *str, int *pos);