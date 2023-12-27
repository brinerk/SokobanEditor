#include<raylib.h>
#include<raymath.h>
#include<rlgl.h>
#include<stdio.h>

const int WIDTH = 800;
const int HEIGHT = 600;

#define MAX_TILES 4096

int ActiveColor = 0;

typedef struct Tile {

	bool active;
	Vector2 position;
	int flag;
	Color color;

} Tile;

Tile _tiles[MAX_TILES] = {0};

int _output[64][64] = {0};


Tile CreateTile(Vector2 position, int flag, Color color) {
	return (Tile) {
		.active = true,
		.position = position,
		.flag = flag,
		.color = color,
	};
}

int clearAll(void) {
	for(int i = 0; i<63; i++){
		for(int j = 0; j<63; j++){
			CreateTile((Vector2){i*50,j*50}, 0, BLACK);
		}
	}
	return 0;
}

int main(void) {

	InitWindow(WIDTH, HEIGHT, "Editor");

//	clearAll();
	
	SetExitKey(KEY_Q);

	Camera2D camera = {0};
	camera.target.x = 0;
	camera.target.y = 0;
	camera.zoom = 1.0f;
	
	printf("%f\n", camera.target.x);

	while(!WindowShouldClose()) {
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			Vector2 delta = GetMouseDelta();
			delta = Vector2Scale(delta, -1.0f/camera.zoom);
			camera.target = Vector2Add(camera.target, delta);
			camera.target = Vector2Clamp(camera.target, (Vector2){0,0}, (Vector2){4096,4096});
		}

/*		float wheel = GetMouseWheelMove();
		if(wheel !=0 ) {
			Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

			camera.offset = GetMousePosition();

			camera.target = mouseWorldPos;

			const float zoomIncrement = 0.125f;

			camera.zoom += (wheel*zoomIncrement);
			if (camera.zoom < zoomIncrement) camera.zoom = zoomIncrement;
		}
*/

		BeginDrawing();
			ClearBackground(BLACK);

			BeginMode2D(camera);

				for (int i = 0; i < MAX_TILES; i++) {
					if(!_tiles[i].active) {
						continue;
					}
					DrawRectangle(_tiles[i].position.x, _tiles[i].position.y, 50, 50, _tiles[i].color);
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

		if(IsKeyPressed(KEY_S)) {
			FILE *fptr;

			fptr = fopen("map", "w");

			for(int t = 0; t<MAX_TILES;t++){
				Vector2 p = _tiles[t].position;
				int f = _tiles[t].flag;
				if(p.x != 0){
				}
				_output[(int)(p.x/50)][(int)(p.y/50)] = f;
			}

			for(int o = 0; o < 63; o++){
				for(int O = 0; O < 63; O++){
					int w = _output[o][O];
					fprintf(fptr,"%d", w);
				}
			}
		//	fclose(fptr);
		}


		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

			int camx = (int)camera.target.x;
			int camy = (int)camera.target.y;
			int _x = (int)GetMousePosition().x + camx;
			int _y = (int)GetMousePosition().y + camy;
			int _width = (int)(50 * camera.zoom);
			int _height = (int)(50 * camera.zoom);

			Vector2 AdjustedPos = {_x - (_x %_width), _y - (_y % _height)};

			Color _color;
			bool _flag;

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

			for(int j = 0; j < MAX_TILES; j++) {
				if(_tiles[j].active) {
					continue;
				}
				_tiles[j] = tile;
				break;
			}
		}
	} 

	CloseWindow();
	return 0;
}
