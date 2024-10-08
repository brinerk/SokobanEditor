#include<raylib.h>
#include<raymath.h>
#include<rlgl.h>
#include<stdio.h>
#include<string.h>
#define RAYGUI_IMPLEMENTATION
#include "../raygui-4.0/src/raygui.h"

const int WIDTH = 800;
const int HEIGHT = 600;

#define MAX_TILES 4096
#define MAX_ENTS 64


char layerString[20]; 

Color colors[] = {BLACK,GREEN,RED,BLUE,BROWN};

typedef struct Tile {

	bool active;
	Vector2 position;
	int flag;
	Color color;

} Tile;

typedef struct Ent {

	bool active;
	int player;
	Vector2 position;
	Color color;

} Ent;

Tile _tiles[MAX_TILES] = {0};
Ent _ents[MAX_ENTS] = {0};

int _output[64][64] = {0};


Tile CreateTile(Vector2 position, int flag, Color color) {
	return (Tile) {
		.active = true,
		.position = position,
		.flag = flag,
		.color = color,
	};
}

Ent CreateEnt(Vector2 position, int player, Color color) {
	return (Ent) {
		.active = true,
		.position = position,
		.player = player,
		.color = color,
	};
}

void saveFile() {
	FILE *fptr;

	fptr = fopen("map", "w");

	for(int t = 0; t<MAX_TILES;t++){
		Vector2 p = _tiles[t].position;
		int f = _tiles[t].flag;
		if(f != 0){
			fprintf(fptr, "%d, %d, %d\n", (int)(p.x/50), (int)(p.y/50), f);
		}
	}

	fprintf(fptr, "ENTITIES\n");
	for(int t = 0; t<MAX_ENTS;t++){
		Vector2 p = _ents[t].position;
		int f = _ents[t].player;
		if(f != 0){
			if(f == 1){
				fprintf(fptr, "%d, %d, %d\n", (int)(p.x/50), (int)(p.y/50), f);
			}
			else { 
				fprintf(fptr, "%d, %d, %d\n", (int)(p.x/50), (int)(p.y/50), f+t-1);
			}
		}
	}
	fclose(fptr);
}

//TODO Make it so you can't click through the GUI

int main(void) {

	int ActiveColor = 0;
	int player = 1;

	bool Layer = false;

	int ItemListScrollIndex = 0;
	int ItemListActive = 0;
	bool LayerButtonPressed = false;
	bool SaveButtonPressed = false;
	bool QuitButtonPressed = false;
	bool shouldClose = false;

	InitWindow(WIDTH, HEIGHT, "Editor");

	SetExitKey(KEY_Q);

	Camera2D camera = {0};
	camera.target.x = 0;
	camera.target.y = 0;
	camera.zoom = 1.0f;

	while(!WindowShouldClose()) {

		int camx = (int)camera.target.x;
		int camy = (int)camera.target.y;
		int _x = (int)GetMousePosition().x + camx;
		int _y = (int)GetMousePosition().y + camy;
		int _width = (int)(50 * camera.zoom);
		int _height = (int)(50 * camera.zoom);

		Vector2 AdjustedPos = {_x - (_x %_width), _y - (_y % _height)};

		if(Layer) {
			strcpy(layerString, "Tiles");
		} else {
			strcpy(layerString, "Entities");
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			Vector2 delta = GetMouseDelta();
			delta = Vector2Scale(delta, -1.0f/camera.zoom);
			camera.target = Vector2Add(camera.target, delta);
			camera.target = Vector2Clamp(camera.target, (Vector2){0,0}, (Vector2){4096,4096});
		}

		BeginDrawing();
			ClearBackground(BLACK);

			BeginMode2D(camera);
				if(Layer) {
					DrawRectangle(AdjustedPos.x, AdjustedPos.y, 50, 50, colors[ActiveColor]);
				} else {
					DrawCircle(AdjustedPos.x + 25, AdjustedPos.y + 25, 25, colors[player]);
				}

				for (int i = 0; i < MAX_TILES; i++) {
					if(!_tiles[i].active) {
						continue;
					}
					DrawRectangle(_tiles[i].position.x, _tiles[i].position.y, 50, 50, _tiles[i].color);
				}

				for (int j = 0; j < MAX_ENTS; j++) {
					if(!_ents[j].active) {
						continue;
					}
					DrawCircle(_ents[j].position.x + 25, _ents[j].position.y + 25, 25, _ents[j].color);
				}

				rlPushMatrix();
					rlTranslatef(0, 25*50,0);
					rlRotatef(90,1,0,0);
					DrawGrid(100, 50);
				rlPopMatrix();

			EndMode2D();

			if(Layer) {
				//Tiles
				GuiListView((Rectangle) { 660, 28, 104, 504}, "Erase;Floor;Goal", &ItemListScrollIndex, &ItemListActive);
				ActiveColor = ItemListActive;
			} else {
				//Entities
				GuiListView((Rectangle) { 660, 28, 104, 504}, "Erase;Player;Box", &ItemListScrollIndex, &ItemListActive);
				player = ItemListActive;
			}
			if(GuiButton((Rectangle) { 652, 536, 120, 24 }, "Toggle Layer")) Layer = !Layer;
			if(GuiButton((Rectangle) { 10, 58, 120, 24 }, "Quit")) shouldClose = true;
			SaveButtonPressed = GuiButton((Rectangle) { 10, 28, 120, 24 }, "Save");

			DrawText(TextFormat("Layer: %s", layerString), 0, 0, 24, WHITE);
		EndDrawing();

		if(Layer) {
			if(IsKeyPressed(KEY_ZERO)) {
				ActiveColor = 0;
			}
			if(IsKeyPressed(KEY_ONE)) {
				ActiveColor = 1;
			}
			if(IsKeyPressed(KEY_TWO)) {
				ActiveColor = 2;
			}
		} else {
			if(IsKeyPressed(KEY_D)) {
				player = 0;
			}
			if(IsKeyPressed(KEY_P)) {
				player = 1;
			}
			if(IsKeyPressed(KEY_B)) {
				player = 2;
			}
		}

		if(IsKeyPressed(KEY_S)) {
			saveFile();
		}

		if(IsKeyPressed(KEY_E)) {
			Layer = false;
		}

		if(IsKeyPressed(KEY_T)) {
			Layer = true;
		}


		//Tile placing code
		//TODO make sure no overlaps
		//TODO Can't delete the last object placed
		//TODO Make sure clicking UI doesn't place an item
		//TODO If you haven't selected anything in the UI you get weird junk data
		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

			if(Layer) {

				int _flag;
				
				if(ActiveColor == 0) {
					for(int i = 0; i < MAX_TILES; i++) {
						if(_tiles[i].position.x == AdjustedPos.x && _tiles[i].position.y == AdjustedPos.y) {
							_tiles[i].active = false;
						}
					}
				}

				Tile tile;

				if(ActiveColor!=0) {
					tile = CreateTile(AdjustedPos, ActiveColor, colors[ActiveColor]);
				}

				for(int i = 0; i < MAX_TILES; i++) {
					if(_tiles[i].active) {
						continue;
					}
					_tiles[i] = tile;
					printf("%d",_tiles[i].flag);
					break;
				}
			}

			else {

				int _player;


				//delte entities
				if(player == 0) {
					for(int i = 0; i < MAX_ENTS; i++) {
						if(_ents[i].position.x == AdjustedPos.x && _ents[i].position.y == AdjustedPos.y) {
							_ents[i].active = false;
						}
					}
				}

				Ent ent;
				if(player!=0) {
					ent = CreateEnt(AdjustedPos, player, colors[player+2]);
				}

				for(int i = 0; i < MAX_ENTS; i++) {
					if(_ents[i].active) {
						continue;
					}
					_ents[i] = ent;
					break;
				}
			}
		}
		if(shouldClose) break;
	}

	CloseWindow();
	return 0;
}
