// Verlet Intergration By Hunter Turner (HuntaBadday)

// Requires raylib for graphics
#include "raylib.h"
#include <math.h>

#define width 500
#define height 500

#define maxAmount 256

float posx[maxAmount];
float posy[maxAmount];
float oldx[maxAmount];
float oldy[maxAmount];

int p0[maxAmount];
int p1[maxAmount];
int stickLength[maxAmount];

// A function that is being a bitch when after main
double distance(double x1, double y1, double x2, double y2){
	double x = x2 - x1;
	double y = y2 - y1;
	return hypot(x, y);
}

//////////////////////////////////////////////////////////
// VARIABLES AND STUFF
//////////////////////////////////////////////////////////

#define bounce 0.99 // Multiplier to velocity on each bounce
#define gravity 0.5 // Gravity
#define friction 0.999 // Air friction multiplier
#define stiffness 5 // Stick stiffness

// Amount of points and linkages
int amountOfPoints = 1;
int amountOfSticks = 0;

// Index counter for generating objects
int pointIndex = 1;
int stickIndex = 0;

int xpoints[maxAmount] = {100}; // X positions
int ypoints[maxAmount] = {100}; // Y positions
int xvels[maxAmount] =   {0}; // X velocities
int yvels[maxAmount] =   {0}; // Y velocities

int startConnect[maxAmount] = {0}; // Index of first point for a linkage
int endConnect[maxAmount] =   {0}; // Index of second point for a linkage

int pointsToLock[] = {0}; // Index of point to lock in place

int main(){
	// Create rope
	for(int i = 0; i<10; i++){
		xpoints[pointIndex] = 125 + i*25;
		ypoints[pointIndex] = 100;
		xvels[pointIndex] = 0;
		yvels[pointIndex] = 0;
		startConnect[stickIndex] = pointIndex-1;
		endConnect[stickIndex++] = pointIndex++;
		amountOfPoints++;
		amountOfSticks++;
	}
	
	// Create ball at the end
	xpoints[pointIndex] = 375;
	ypoints[pointIndex] = 125;
	startConnect[stickIndex] = pointIndex-1;
	endConnect[stickIndex++] = pointIndex++;
	amountOfSticks++;
	amountOfPoints++;
	
	xpoints[pointIndex] = 400;
	ypoints[pointIndex] = 100;
	startConnect[stickIndex] = pointIndex-1;
	endConnect[stickIndex++] = pointIndex++;
	amountOfSticks++;
	amountOfPoints++;
	
	xpoints[pointIndex] = 375;
	ypoints[pointIndex] = 75;
	startConnect[stickIndex] = pointIndex-1;
	endConnect[stickIndex++] = pointIndex++;
	amountOfSticks++;
	amountOfPoints++;
	
	startConnect[stickIndex] = pointIndex-1;
	endConnect[stickIndex++] = pointIndex-4;
	amountOfSticks++;
	
	startConnect[stickIndex] = pointIndex-1;
	endConnect[stickIndex++] = pointIndex-3;
	amountOfSticks++;
	
	startConnect[stickIndex] = pointIndex-2;
	endConnect[stickIndex++] = pointIndex-4;
	amountOfSticks++;
	
	setup();
	
	// Initialize Raylib window
	InitWindow(width, height, "Verlet Integration");
	SetTargetFPS(60);
	
	// Mian loop
	while(!WindowShouldClose()){
		// If space is pressed then reset all points
		if(IsKeyPressed(KEY_SPACE)){
			setup();
		}
		
		// Do physics
		updatePoints();
		for(int i = 0; i<stiffness; i++){
			// Stuff to keep the dot in place
			updateSticks();
			constrainPoints();
		}
		// Start drawing
		BeginDrawing();
			ClearBackground(BLACK);
			renderSticks();
			//renderPoints();
			DrawFPS(3, 3);
		EndDrawing();
	}
}

// Set point positions and velocities
void setup(){
	for(int i = 0; i<amountOfPoints; i++){
		posx[i] = xpoints[i];
		posy[i] = ypoints[i];
		oldx[i] = xpoints[i] - xvels[i];
		oldy[i] = ypoints[i] - yvels[i];
	}
	for(int i = 0; i<amountOfSticks; i++){
		p0[i] = startConnect[i];
		p1[i] = endConnect[i];
		stickLength[i] = (int)distance(posx[startConnect[i]], posy[startConnect[i]], posx[endConnect[i]], posy[endConnect[i]]);
	}
}

// Render points to screen
void renderPoints(){
	for(int i = 0; i<amountOfPoints; i++){
		DrawCircle(posx[i], posy[i], 2, WHITE);
	}
}

// Thing that does physics stuff
void updatePoints(){
	for(int i = 0; i<amountOfPoints; i++){
		float vx = posx[i] - oldx[i];
		float vy = posy[i] - oldy[i];
		oldx[i] = posx[i];
		oldy[i] = posy[i];
		posx[i] += vx * friction;
		posy[i] += vy * friction;
		posy[i] += gravity;
	}
}

// This is what keeps the points inside the window
void constrainPoints(){
	for(int i = 0; i<amountOfPoints; i++){
		float vx = posx[i] - oldx[i];
		float vy = posy[i] - oldy[i];
		
		if(posx[i] > width){
			posx[i] = width;
			oldx[i] = posx[i] + vx * bounce;
		} else if(posx[i] < 0){
			posx[i] = 0;
			oldx[i] = posx[i] + vx * bounce;
		}
		if(posy[i] > height){
			posy[i] = height;
			oldy[i] = posy[i] + vy * bounce;
		} else if(posy[i] < 0){
			posy[i] = 0;
			oldy[i] = posy[i] + vy * bounce;
		}
		
	}
	// Code to lock points in place when set in pointsToLock[]
	if(sizeof(pointsToLock) > 0){
		int size = sizeof(pointsToLock)/sizeof(pointsToLock[0]);
		for(int i = 0; i<size; i++){
			posx[pointsToLock[i]] = xpoints[pointsToLock[i]];
			posy[pointsToLock[i]] = ypoints[pointsToLock[i]];
		}
	}
}

// Render the lines between the points
void renderSticks(){
	for(int i = 0; i<amountOfSticks; i++){
		DrawLine(posx[p0[i]], posy[p0[i]], posx[p1[i]], posy[p1[i]], WHITE);
	}
}

// This is what keeps the points the right distance from eachother
void updateSticks(){
	if(amountOfSticks == 0){
		return;
	}
	for(int i = 0; i<amountOfSticks; i++){
		float dx = posx[p1[i]] - posx[p0[i]];
		float dy = posy[p1[i]] - posy[p0[i]];
		float distance = sqrt(dx * dx + dy * dy);
		float difference = stickLength[i] - distance;
		float percent = difference / distance / 2;
		float offsetx = dx * percent;
		float offsety = dy * percent;
		
		posx[p0[i]] -= offsetx;
		posy[p0[i]] -= offsety;
		posx[p1[i]] += offsetx;
		posy[p1[i]] += offsety;
	}
}