#pragma once
#include "PxPhysicsAPI.h"
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

#define GRID_SIZE 200
#define MAX_ACTUAL 90

// Node containing information about traversal
struct Node {
	int x, y, sCost, hCost, tCost;
	float angle, xActual, yActual;
	std::shared_ptr<Node> parent;

	Node(float _angle, int _x, int _y, int _sCost, int _hCost, std::shared_ptr<Node> _parent = nullptr)
		: angle(_angle), x(_x), y(_y), sCost(_sCost), hCost(_hCost), parent(_parent) {
		tCost = sCost + hCost;
		xActual = 0;
		yActual = 0;
	}
};

// Overload operator to compare nodes
struct CompareNode {
	bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) const {
		return (a->tCost) > (b->tCost);
	}
};

class GameMap {
private:
	// --- Helpers
	std::vector<physx::PxVec2T<int>> dt = {
		{1,0}, {1,1}, {0,1}, {-1,1},
		{-1,0}, {-1,-1}, {0,-1}, {1,-1}
	};

    bool outOfBound(int a);
    bool outOfBound(int a, int b);
	int toGridSpace(float a);
	float toGameSpace(int a);

    // --- Clearance based Graph
    void addBlock(int x, int y, int blocking = 0);
	// a supercover DDA algorithm grid update
    void updateGrid(int x0, int y0, int x1, int y1, int blocking = 0);

    // --- A*
    // euclidean distance
    int heuristic(int x, int y, int goalX, int goalY);
    // convert coordinates to angle
    float dirToAngle(int i);
    // Convert pointer connected list to array
    std::vector<std::shared_ptr<Node>> reconstructPath(std::shared_ptr<Node> goalNode);

public:
	GameMap();

	// grid of clearance values
	std::vector<std::vector<int>> grid;

	// reset graph
	void resetMap();

	// --- Graph generation
	void updateMap(physx::PxVec2 start, physx::PxVec2 end, int blocking = 0);
	int isBlocked(double x0, double z0);
	void printGraph();

	// --- A*
	std::vector<std::shared_ptr<Node>> aStar(float angle, float startX, float startY, float goalX, float goalY);
	std::vector<std::shared_ptr<Node>> aStar(float angle, int startX, int startY, int goalX, int goalY);
	void printGraph(std::vector<std::shared_ptr<Node>> path);

	// --- Obstacle Avoidance
	float rayCast(int x0, int y0, float angle, float step);
	float rayCast(float x0, float y0, float angle, float step);
	physx::PxVec2 openArea(float x, float y);

	// -- Direct Attack
	bool castRayTo(float x0, float y0, float x1, float y1);

    // a supercover DDA algorithm for finding obstructions between two points
	bool checkCollisions(int x0, int y0, int x1, int y1);
};
