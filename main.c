#include<raylib.h>
#include<raymath.h>
#include<rlgl.h>
#include<stdio.h>

const int WIDTH = 800;
const int HEIGHT = 600;

#define MAX_TILES 4096
#define MAX_ENTS 64

int ActiveColor = 0;

int player = 1;

bool Layer = false;

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

int main(void) {

	InitWindow(WIDTH, HEIGHT, "Editor");

	SetExitKey(KEY_Q);

	Camera2D camera = {0};
	camera.target.x = 0;
	camera.target.y = 0;
	camera.zoom = 1.0f;

	while(!WindowShouldClose()) {
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			Vector2 delta = GetMouseDelta();
			delta = Vector2Scale(delta, -1.0f/camera.zoom);
			camera.target = Vector2Add(camera.target, delta);
			camera.target = Vector2Clamp(camera.target, (Vector2){0,0}, (Vector2){4096,4096});
		}

		BeginDrawing();
			ClearBackground(BLACK);

			BeginMode2D(camera);

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
			DrawFPS(0, 0);
		EndDrawing();

		if(IsKeyPressed(KEY_ZERO)) {
			ActiveColor = 0;
		}
		if(IsKeyPressed(KEY_ONE)) {
			ActiveColor = 1;
		}
		if(IsKeyPressed(KEY_TWO)) {
			ActiveColor = 2;
		}

		if(IsKeyPressed(KEY_P)) {
			player = 1;
		}
		if(IsKeyPressed(KEY_B)) {
			player = 2;
		}

		if(IsKeyPressed(KEY_S)) {
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

		if(IsKeyPressed(KEY_E)) {
			Layer = false;
		}

		if(IsKeyPressed(KEY_T)) {
			Layer = true;
		}


		//Tile placing code
		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

			if(Layer) {

				int camx = (int)camera.target.x;
				int camy = (int)camera.target.y;
				int _x = (int)GetMousePosition().x + camx;
				int _y = (int)GetMousePosition().y + camy;
				int _width = (int)(50 * camera.zoom);
				int _height = (int)(50 * camera.zoom);

				Vector2 AdjustedPos = {_x - (_x %_width), _y - (_y % _height)};

				Color _color;
				int _flag;

				switch(ActiveColor) {
					case 0:
						_color = BLACK;
						_flag = 0;
						break;
					case 1:
						_color = GREEN;
						_flag = 1;
						break;
					case 2:
						_color = RED;
						_flag = 2;
						break;
				}

				Tile tile = CreateTile(AdjustedPos, _flag, _color);

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
				int camx = (int)camera.target.x;
				int camy = (int)camera.target.y;
				int _x = (int)GetMousePosition().x + camx;
				int _y = (int)GetMousePosition().y + camy;
				int _width = (int)(50 * camera.zoom);
				int _height = (int)(50 * camera.zoom);

				Vector2 AdjustedPos = {_x - (_x %_width), _y - (_y % _height)};

				Color color;

				int _player;

				switch(player){
					case 1:
						_player = 1;
						color = BLUE;
						break;
					case 2:
					_player = 2;
					color = BROWN;
					break;
				}


				Ent ent = CreateEnt(AdjustedPos, player, color);

				for(int i = 0; i < MAX_ENTS; i++) {
					if(_ents[i].active) {
						continue;
					}
					_ents[i] = ent;
					break;
				}
			}
		}
	}

	CloseWindow();
	return 0;
}
