#pragma once
#include "PxPhysicsAPI.h"
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

#define GRID_SIZE 30
#define MAX_ACTUAL 45.f

// Node containing information about traversal
struct Node {
	int x, y, sCost, hCost, tCost;
	Node* parent;

	Node(int x0, int y0, int s, int h, Node* p = nullptr)
		: x(x0), y(y0), sCost(s), hCost(h), tCost(s + h), parent(p) {}
};

// Overload operator to compare nodes
struct CompareNode {
	bool operator()(const Node* a, const Node* b) const {
		return a->tCost > b->tCost;
	}
};


class GameMap {
public:
	GameMap();

	// propogation directions
	std::vector<physx::PxVec2T<int>> dt = {
		{1, 1}, {1, 0}, {1, -1}, {0, 1},
		{-1, 1}, {-1, 0}, {-1,-1}, {0, -1}
	};
	std::queue<physx::PxVec2T<int>> q;		// Queue of propogations
	std::vector<std::vector<int>> grid;		// Matrix of map
	const int REQUIRED_CLEARANCE = 5.f / MAX_ACTUAL * GRID_SIZE;

// --- Clearance based Graph
	// helper function to add blocking calls
	void addBlock(int x, int y);

	void updateGrid(physx::PxVec2 start, physx::PxVec2 end);

	void propogateWeights();

	void updateMap(physx::PxVec2 start, physx::PxVec2 end);

	int isBlocked(double x0, double z0);

	void printGraph();

// --- A* Stuff
	// Compute the manhattan distance between goal and location
	int heuristic(int x, int y, int goalX, int goalY);

	// Convert pointer connected list to array
	std::vector<Node*> reconstructPath(Node* goalNode);

	std::vector<Node*> aStar(int startX, int startY, int goalX, int goalY);

	void printGraph(std::vector<Node*> path);
};
