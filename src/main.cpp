/*

Feigenbaumdiagramm

xₙ₊₁ = axₙ(1 - xₙ)

*/

#include "raylib.h"
#include "iostream"
#include "string"

#define MIN_A 2.8 // sichtbares Intervall
#define MAX_A 4

#define MAX_X 1
#define COUNT 400 // Einschwingzeit
#define PLOT_VALUES 1000 // Iterationen
#define START_VALUE 0.2 // random

#define IMAGE_WIDTH 900
#define IMAGE_HEIGHT 600

void Init();

float GetNewValue(float a, float xn);
void PlotPoints(RenderTexture2D &canvas, int currentPixel);

int main()
{
	Init();

	RenderTexture2D canvas = LoadRenderTexture(IMAGE_WIDTH, IMAGE_HEIGHT);
	Vector2 drawOffset = {50, 20};

	BeginTextureMode(canvas);
	ClearBackground(WHITE);
	EndTextureMode();

	int p = 0;

	while (!WindowShouldClose())
	{
		if (p <= canvas.texture.width)
		{
			p++;

			PlotPoints(canvas, p);
		}

		BeginDrawing();
		ClearBackground({20, 40, 50, 255});

		DrawTextureRec(canvas.texture, {0, 0, (float)canvas.texture.width, (float)canvas.texture.height}, drawOffset, WHITE);

		int dist = 15;
		Vector2 endPoint1 = {drawOffset.x - dist, drawOffset.y};
		Vector2 endPoint2 = {drawOffset.x + canvas.texture.width, drawOffset.y + canvas.texture.height + dist};
		DrawLineEx(endPoint1, {drawOffset.x - dist, drawOffset.y + canvas.texture.height + drawOffset.x}, 2, WHITE);
		DrawLineEx({0, drawOffset.y + canvas.texture.height + dist}, endPoint2, 2, WHITE);
		DrawTriangle(endPoint1, {endPoint1.x - 4, endPoint1.y + 7}, {endPoint1.x + 4, endPoint1.y + 7}, WHITE);
		DrawTriangle(endPoint2, {endPoint2.x - 7, endPoint2.y - 4}, {endPoint2.x - 7, endPoint2.y + 4}, WHITE);

		for (int i = 1; i <= MAX_X * 10 - 1; i++)
		{
			float y = canvas.texture.height + drawOffset.y - (float)i / 10.0f * canvas.texture.height;
			DrawLineV({drawOffset.x - dist - 5, y}, {drawOffset.x - dist + 5, y}, WHITE);
			
			int fontSize = 10;
			std::string text = "0." + std::to_string(i);
			float yText = y - MeasureTextEx(GetFontDefault(), text.c_str(), fontSize, 0).y / 2.0f;
			DrawText(text.c_str(), 10, yText, fontSize, LIGHTGRAY);
		}

		DrawText("x", 20, 20, 10, WHITE);

		EndDrawing();
	}

	ExportImage(LoadImageFromTexture(canvas.texture), "feigenbaum_diagramm.png");

	CloseWindow();
	return 0;
}

void Init()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTraceLogLevel(LOG_NONE);
	InitWindow(1000, 700, "Feigenbaumdiagramm");
	SetExitKey(0);
	ChangeDirectory(GetApplicationDirectory());
	SetTargetFPS(120);
}

float GetNewValue(float a, float xn)
{
	return a * xn * (1 - xn);
}

void PlotPoints(RenderTexture2D &canvas, int currentPixel)
{
	// get a for every pixel from 0 to screen width
	float a = MIN_A + currentPixel / (float)canvas.texture.width * (MAX_A - MIN_A);
	float x = START_VALUE;


	for (int i = 0; i < COUNT + PLOT_VALUES; i++)
	{
		x = GetNewValue(a, x);

		if (i > COUNT)
		{
			BeginTextureMode(canvas);

			// convert y value to drawing coordinate system
			float y = canvas.texture.height * x;
			DrawPixelV({(float)currentPixel, y}, {230, 41, 55, 60});

			EndTextureMode();
		}
	}
}
