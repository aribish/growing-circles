#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>

#define WIN_WIDTH 640
#define WIN_HEIGHT 640
#define ANGLE 10
#define PADDING 50

#define GROWTH_PER_SECOND 30
#define GROWTH_AMOUNT 1

// does this really need to be an enum?
typedef enum {
	BLACK,
	WHITE,
	RED,
	GREEN,
	BLUE
} Color;

const SDL_Color COLORS[] = {
	{0, 0, 0, 0xff},
	{0xff, 0xff, 0xff, 0xff},
	{0xff, 0, 0, 0xff},
	{0, 0xff, 0, 0xff},
	{0, 0, 0xff, 0xff}
};

typedef struct {
	double x, y;
} CirclePoint;

typedef struct {
	unsigned int x, y, r;
	Color color;
} Circle;

SDL_Window* win;
SDL_Renderer* render;
SDL_Event event;

CirclePoint circlePoints[360 / ANGLE];
int numCircles = 0;
Circle* circles = NULL;

unsigned int nextTick = 0;

void growCircles() {
	unsigned int ticks = SDL_GetTicks();
	if(ticks >= nextTick) {
		nextTick = ticks + 1000 / GROWTH_PER_SECOND;

		for(int i = 0; i < numCircles; i++) {
			if(circles[i].r < 0xffffffff)
				circles[i].r += GROWTH_AMOUNT;
		}
	}
}

void drawCircle(Circle c) {
	SDL_SetRenderDrawColor(render, COLORS[c.color].r, COLORS[c.color].g, COLORS[c.color].b, COLORS[c.color].a);

	for(int i = 0; i < 360 / ANGLE - 1; i++) {
		SDL_RenderDrawLine(render,
			c.x + (int) (circlePoints[i].x * c.r), c.y + (int) (circlePoints[i].y * c.r),
			c.x + (int) (circlePoints[i + 1].x * c.r), c.y + (int) (circlePoints[i + 1].y * c.r));
	}

	SDL_RenderDrawLine(render,
			c.x + (int) (circlePoints[360 / ANGLE - 1].x * c.r), c.y + (int) (circlePoints[360 / ANGLE - 1].y * c.r),
			c.x + (int) (circlePoints[0].x * c.r), c.y + (int) (circlePoints[0].y * c.r));

}

void generateCirclePoints() {
	const double addAngle = ANGLE * M_PI / 180;
	double angle = addAngle;

	for(int i = 0; i < 360 / ANGLE; i++) {
		circlePoints[i].x = cos(angle);
		circlePoints[i].y = sin(angle);
		angle += addAngle;
	}
}

void init() {
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Failed to Initialize SDL2 :(\n");
		exit(1);
	}

	win = SDL_CreateWindow("Growing Circles - Ari Bishop",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			WIN_WIDTH, WIN_HEIGHT, 0);
	render = SDL_CreateRenderer(win, -1, 0);

	srand(time(NULL));
	generateCirclePoints();
}

void quit() {
	if(circles != NULL)
		free(circles);

	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(win);
	SDL_Quit();
}

int main() {
	init();
	int weena = 0;

	while(true) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				goto exit;
			}
			else if(event.type == SDL_KEYUP) {
				if(event.key.keysym.sym == SDLK_SPACE) {
					Circle* newCircles = calloc(numCircles + 1, sizeof(Circle));
					for(int i = 0; i < numCircles; i++) {
						newCircles[i] = circles[i];
					}

					if(circles != NULL)
						free(circles);

					circles = newCircles;
					circles[numCircles] = (Circle) {
						(rand() % (WIN_WIDTH - PADDING * 2 - 1)) + PADDING,
						(rand() % (WIN_HEIGHT - PADDING * 2 - 1)) + PADDING,
						50,
						(Color) ((rand() % 4) + 1)
					};
					numCircles++;
				}
			}
		}

		SDL_SetRenderDrawColor(render, 0, 0, 0, 0xff);
		SDL_RenderClear(render);

		growCircles();
		for(int i = 0; i < numCircles; i++) {
			drawCircle(circles[i]);
		}

		SDL_RenderPresent(render);
	}

	exit:
	quit();
	return 0;
}
