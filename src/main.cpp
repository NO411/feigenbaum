/*

Feigenbaumdiagramm

xₙ₊₁ = axₙ(1 - xₙ)
*/

#include "raylib.h"
#include <iostream>
#include <string>
#include <vector>

#define START_VALUE 0.2
#define IMAGE_WIDTH 900
#define IMAGE_HEIGHT 600
#define BG_COLOR {20, 40, 50, 255}
#define BG_COLOR_2 {10, 30, 40, 255}
#define FONT_SIZE 10
#define FONT_SIZE_2 20

long double MIN_A = 2.9; // sichtbares Intervall
long double MAX_A = 4;
long double SETTLING_TIME = 1000; // Einschwingzeit
long double PLOT_VALUES = 1000;	  // Iterationen

class Slider
{
private:
	static const int length;
	static const int textDist;
	static const int dragRadius;

	Vector2 pos;
	float sliderPercentage;

	std::string name;
	long double min;
	long double max;

	long double *setConstant;
	long double value;

	bool moving = false;

public:
	Slider(Vector2 pos, std::string name, long double min, long double max, long double *setConstant, long double initVar) : pos(pos), name(name), min(min), max(max), setConstant(setConstant), value(initVar)
	{
		sliderPercentage = (initVar - min) / (max - min);
		UpdateValue();
	}

	std::string DoubleToString(long double n)
	{
		std::string ret = std::to_string(n);
		ret.resize(5);
		if (ret.back() == '.')
		{
			ret.resize(4);
		}
		return ret;
	}

	void Render()
	{
		Color dragColor = WHITE;
		if (moving)
		{
			dragColor = GRAY;
		}
		DrawLineEx(pos, {pos.x + length, pos.y}, 4, BG_COLOR);
		DrawCircle(pos.x + length * sliderPercentage, pos.y, dragRadius, dragColor);
		DrawText(name.c_str(), pos.x + length / 2 - MeasureText(name.c_str(), FONT_SIZE_2) / 2, pos.y - MeasureTextEx(GetFontDefault(), name.c_str(), FONT_SIZE_2, 0).y - (dragRadius + textDist), FONT_SIZE_2, BG_COLOR);

		std::string minString = DoubleToString(min);
		Vector2 minMeasure = MeasureTextEx(GetFontDefault(), minString.c_str(), FONT_SIZE, 0);
		DrawText(minString.c_str(), pos.x - (dragRadius + textDist) - minMeasure.x, pos.y - minMeasure.y / 2, FONT_SIZE, WHITE);

		std::string maxString = DoubleToString(max);
		Vector2 maxMeasure = MeasureTextEx(GetFontDefault(), maxString.c_str(), FONT_SIZE, 0);
		DrawText(maxString.c_str(), pos.x + length + dragRadius + textDist, pos.y - minMeasure.y / 2, FONT_SIZE, WHITE);

		std::string valueString = DoubleToString(value);
		DrawText(valueString.c_str(), pos.x + sliderPercentage * length - MeasureText(valueString.c_str(), FONT_SIZE) / 2, pos.y + (dragRadius + textDist), FONT_SIZE, WHITE);
	}

	bool Selected()
	{
		return CheckCollisionPointCircle(GetMousePosition(), {pos.x + length * sliderPercentage, pos.y}, dragRadius);
	}

	void SetConstant()
	{
		*setConstant = value;
	}

	void UpdateValue()
	{
		value = sliderPercentage * (max - min) + min;
	}

	void Update()
	{
		if (IsMouseButtonPressed(0) && Selected())
		{
			moving = true;
		}

		if (IsMouseButtonReleased(0))
		{
			moving = false;
		}

		if (moving)
		{
			sliderPercentage = (GetMouseX() - pos.x) / length;
			if (sliderPercentage < 0)
			{
				sliderPercentage = 0;
			}
			else if (sliderPercentage > 1)
			{
				sliderPercentage = 1;
			}
		}

		UpdateValue();
	}

	long double GetValue()
	{
		return value;
	}

	void SetMin(long double m)
	{
		min = m;
	}
};

const int Slider::length = 150;
const int Slider::textDist = 5;
const int Slider::dragRadius = 8;

class Button
{
private:
public:
	Button();
};

void Init();

long double GetNewValue(long double a, long double xn);
void PlotPoints(RenderTexture2D &canvas, int currentPixel);
void RenderCoordinateSystem(Texture2D &texture, Vector2 &offset, float lineLength, int dist);

int main()
{
	Init();

	RenderTexture2D canvas = LoadRenderTexture(IMAGE_WIDTH, IMAGE_HEIGHT);
	Vector2 drawOffset = {50, 30};
	float partingLineX = IMAGE_WIDTH + drawOffset.x * 2;

	std::vector<Slider> sliders = {
		{{partingLineX + drawOffset.x, 50}, "min a", 0, 4, &MIN_A, MIN_A},
		{{partingLineX + drawOffset.x, 130}, "max a", 2, 4, &MAX_A, MAX_A},
		{{partingLineX + drawOffset.x, 210}, "settling time", 0, 2000, &SETTLING_TIME, SETTLING_TIME},
		{{partingLineX + drawOffset.x, 290}, "plot values", 0, 3000, &PLOT_VALUES, PLOT_VALUES},
	};

	auto setConstants = [&sliders]()
	{
		for (auto &slider : sliders)
		{
			slider.SetConstant();
		}
	};

	setConstants();

	enum Sliders
	{
		MIN_A_SLIDER,
		MAX_A_SLIDER,
		SETTLING_TIME_SLIDER,
		PLOT_VALUES_SLIDER,
	};

	// coordinate system
	int dist = 15;
	float lineLength = 10;

	int p = 0;
	auto clear = [&canvas, &p]()
	{
		BeginTextureMode(canvas);
		ClearBackground(BG_COLOR);
		EndTextureMode();
		p = 0;
	};
	clear();

	while (!WindowShouldClose())
	{
		if (p <= canvas.texture.width)
		{
			p++;

			PlotPoints(canvas, p);
		}

		for (auto &slider : sliders)
		{
			slider.Update();
		}

		sliders[Sliders::MAX_A_SLIDER].SetMin(sliders[Sliders::MIN_A_SLIDER].GetValue());

		if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_R))
		{
			setConstants();
			clear();
		}

		BeginDrawing();
		ClearBackground(BG_COLOR);

		DrawTextureRec(canvas.texture, {0, 0, (float)canvas.texture.width, (float)canvas.texture.height}, drawOffset, WHITE);
		RenderCoordinateSystem(canvas.texture, drawOffset, lineLength, dist);

		DrawLine(partingLineX, 0, partingLineX, GetScreenHeight(), WHITE);
		DrawRectangle(partingLineX, 0, GetScreenWidth() - partingLineX, GetScreenHeight(), BG_COLOR_2);

		for (auto &slider : sliders)
		{
			slider.Render();
		}

		DrawText("Press STRG + R\nto re-plot the graph!", partingLineX + 20, 400, FONT_SIZE_2, WHITE);
		DrawText("Press STRG + S\nto save!", partingLineX + 20, 500, FONT_SIZE_2, WHITE);

		std::string fps = (std::to_string(GetFPS()) + " FPS");
		Vector2 measureFPS = MeasureTextEx(GetFontDefault(), fps.c_str(), FONT_SIZE, 0);
		DrawText(fps.c_str(), GetScreenWidth() - measureFPS.x - 10, GetScreenHeight() - measureFPS.y - 5, FONT_SIZE, WHITE);

		EndDrawing();
	}

	ExportImage(LoadImageFromTexture(canvas.texture), "feigenbaum_diagramm.png");
	UnloadRenderTexture(canvas);
	CloseWindow();
	return 0;
}

void Init()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTraceLogLevel(LOG_NONE);
	InitWindow(IMAGE_WIDTH + 350, IMAGE_HEIGHT + 100, "Feigenbaumdiagramm");
	SetWindowMinSize(IMAGE_WIDTH + 350, IMAGE_HEIGHT + 100);
	SetExitKey(0);
	ChangeDirectory(GetApplicationDirectory());
	SetTargetFPS(120);
}

long double GetNewValue(long double a, long double xn)
{
	return a * xn * (1 - xn);
}

void PlotPoints(RenderTexture2D &canvas, int currentPixel)
{
	// get a for every pixel from 0 to screen width
	long double a = MIN_A + currentPixel / (long double)canvas.texture.width * (MAX_A - MIN_A);
	long double x = START_VALUE;

	for (int i = 0; i < SETTLING_TIME + PLOT_VALUES; i++)
	{
		x = GetNewValue(a, x);

		if (i > SETTLING_TIME)
		{
			BeginTextureMode(canvas);
			BeginBlendMode(BLEND_ADDITIVE);

			// convert y value to drawing coordinate system
			long double y = canvas.texture.height * x;
			DrawPixelV({(float)currentPixel, (float)y}, {0, 255, 255, 20});

			EndBlendMode();
			EndTextureMode();
		}
	}
}

void RenderCoordinateSystem(Texture2D &texture, Vector2 &offset, float lineLength, int dist)
{
	Vector2 endPoint1 = {offset.x - dist, offset.y};
	Vector2 endPoint2 = {offset.x + texture.width, offset.y + texture.height + dist};

	DrawLineEx(endPoint1, {offset.x - dist, offset.y + texture.height + offset.x}, 2, WHITE);
	DrawLineEx({0, offset.y + texture.height + dist}, endPoint2, 2, WHITE);
	DrawTriangle(endPoint1, {endPoint1.x - 4, endPoint1.y + 7}, {endPoint1.x + 4, endPoint1.y + 7}, WHITE);
	DrawTriangle(endPoint2, {endPoint2.x - 7, endPoint2.y - 4}, {endPoint2.x - 7, endPoint2.y + 4}, WHITE);

	for (int i = 0; i <= 10; i++)
	{
		// y - axis
		float y = texture.height + offset.y - (float)i / 10.0f * texture.height;
		float x = offset.x - dist;
		DrawLineV({x - lineLength / 2, y}, {x + lineLength / 2, y}, WHITE);

		std::string text = std::to_string(i / 10.0f);
		text.resize(3);
		Vector2 measureText = MeasureTextEx(GetFontDefault(), text.c_str(), FONT_SIZE, 0);
		float yText = y - measureText.y / 2.0f;
		DrawText(text.c_str(), (offset.x - dist) / 2.0f - measureText.x / 2.0f, yText, FONT_SIZE, WHITE);

		// x - axis
		y = texture.height + offset.y + dist;
		x = offset.x + (float)i / 10.0f * texture.width;
		DrawLineV({x, y - lineLength / 2}, {x, y + lineLength / 2}, WHITE);

		text = std::to_string(MIN_A + (MAX_A - MIN_A) * (float)i / 10.0f);
		text.resize(5);
		float xText = x - MeasureTextEx(GetFontDefault(), text.c_str(), FONT_SIZE, 0).x / 2.0f;
		DrawText(text.c_str(), xText, y + dist, FONT_SIZE, WHITE);
	}

	std::string x = "x";
	Vector2 measureX = MeasureTextEx(GetFontDefault(), x.c_str(), FONT_SIZE_2, 0);
	DrawText(x.c_str(), offset.x - dist - measureX.x / 2.0f, offset.y / 2.0f - measureX.y / 2.0f, FONT_SIZE_2, WHITE);

	std::string a = "a";
	Vector2 measureA = MeasureTextEx(GetFontDefault(), a.c_str(), FONT_SIZE_2, 0);
	DrawText(a.c_str(), texture.width + offset.x + offset.y / 2.0f - measureA.y / 2.0f, texture.height + offset.y + dist - measureA.y / 2.0f, FONT_SIZE_2, WHITE);
}
