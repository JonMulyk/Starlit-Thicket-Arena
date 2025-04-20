#include "GameMap.h"

GameMap::GameMap()
	: grid(GRID_SIZE, std::vector<int>(GRID_SIZE, 1))
{}

// --- Helpers
bool GameMap::outOfBound(int a) {
	return (a < 0 || a >= GRID_SIZE);
}

bool GameMap::outOfBound(int a, int b) {
	return outOfBound(a) || outOfBound(b);

}

int GameMap::toGridSpace(float a) {
	return (a + MAX_ACTUAL) * (GRID_SIZE - 1) / (2 * MAX_ACTUAL);
}

float GameMap::toGameSpace(int a) {
	return ((float(a) / float(GRID_SIZE - 1)) * 2.0f * MAX_ACTUAL) - MAX_ACTUAL;
}

// --- Graph updates
void GameMap::addBlock(int x, int y, int blocking) {
	// check invalid coordinates
	if (outOfBound(x) || outOfBound(y)) {
		return;
	}
	grid[x][y] = blocking;
}

void GameMap::updateGrid(int x0, int y0, int x1, int y1, int blocking) {
	// add start and end
	addBlock(x0, y0, blocking);
	addBlock(x1, y1, blocking);

	// convert start to float
	float x = x0;
	float y = y0;

	// get x and y distance
	int dx = x1 - x0;
	int dy = y1 - y0;

	// get step direction
	int stepX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
	int stepY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

	// get discrete step range
	float tDeltaX = (std::abs(dx) != 0) ? (1.0f / std::abs(dx)) : std::numeric_limits<float>::infinity();
	float tDeltaY = (std::abs(dy) != 0) ? (1.0f / std::abs(dy)) : std::numeric_limits<float>::infinity();

	// get first boundaries
	float tMaxX, tMaxY;
	if (dx == 0)
		tMaxX = std::numeric_limits<float>::infinity();
	else if (stepX > 0)
		tMaxX = ((x0 + 1) - x) * tDeltaX;
	else
		tMaxX = (x - x0) * tDeltaX;

	if (dy == 0)
		tMaxY = std::numeric_limits<float>::infinity();
	else if (stepY > 0)
		tMaxY = ((y0 + 1) - y) * tDeltaY;
	else
		tMaxY = (y - y0) * tDeltaY;

	// distance between two points (squared)
	float dd = dx * dx + dy * dy;

	// traverse till the checked cells equal dd
	while ((((x - x0) * dx + (y - y0) * dy) / dd) < 1.0f) {

		// check if boundary is x or y
		if (tMaxX < tMaxY) {
			// advance x
			x += stepX;
			tMaxX += tDeltaX;
		}
		else if (tMaxX > tMaxY) {
			// advance y
			y += stepY;
			tMaxY += tDeltaY;
		}
		else {
			// next boundary is a corner; advance both
			x += stepX;
			tMaxX += tDeltaX;
			y += stepY;
			tMaxY += tDeltaY;

			// add the adjacent boxes to a corner
			addBlock(x, y - stepY, blocking);
			addBlock(x - stepX, y, blocking);
		}

		// add new visited square
		addBlock(x, y, blocking);
	}
}

void GameMap::updateMap(physx::PxVec2 start, physx::PxVec2 end, int blocking) {
	int x0 = toGridSpace(start.x);
	int y0 = toGridSpace(start.y);
	int x1 = toGridSpace(end.x);
	int y1 = toGridSpace(end.y);
	updateGrid(x0, y0, x1, y1, blocking);
}

int GameMap::isBlocked(double x0, double z0) {
	// convert to grid space
	int x = toGridSpace(x0);
	int z = toGridSpace(z0);

	// check bounds
	if (outOfBound(x) || outOfBound(z)) {
		return true;
	}

	// return grid
	return grid[x][z];
}

void GameMap::printGraph() {
	for (int y = GRID_SIZE - 1; y >= 0; y--) {
		for (int x = 0; x < GRID_SIZE; x++) {
			if (grid[x][y] == 0) {
				std::cout << " ";
			}
			else {
				std::cout << "#";
			}
		}
		std::cout << std::endl;
	}
}

// reset graph
void GameMap::resetMap() {
	for (int i = 0; i < GRID_SIZE; i++) {
		for (int j = 0; j < GRID_SIZE; j++) {
			grid[i][j] = 1;
		}
	}
}

// --- A* Stuff
float GameMap::dirToAngle(int idx) {
	switch (idx) {
	case 0: return 0;
	case 1: return 45;
	case 2: return 90;
	case 3: return 135;
	case 4: return 180;
	case 5: return -135;
	case 6: return -90;
	case 7: return -45;
	}
}

int GameMap::heuristic(int x, int y, int goalX, int goalY) {
	return std::sqrt((x - goalX) * (x - goalX) + (y - goalY) * (y - goalY));
}

std::vector<std::shared_ptr<Node>> GameMap::reconstructPath(std::shared_ptr<Node> goalNode) {
	std::vector<std::shared_ptr<Node>> path;

	while (goalNode != nullptr) {
		// Udate actual positions in world space.
		goalNode->xActual = toGameSpace(goalNode->x);
		goalNode->yActual = toGameSpace(goalNode->y);

		path.push_back(goalNode);
		goalNode = goalNode->parent;
	}

	std::reverse(path.begin(), path.end());
	return path;
}

std::vector<std::shared_ptr<Node>> GameMap::aStar(float angle, float startX, float startY, float goalX, float goalY) {
	// convert to grid space
	int x0 = toGridSpace(startX);
	int y0 = toGridSpace(startY);
	int x1 = toGridSpace(goalX);
	int y1 = toGridSpace(goalY);

	// bound
	x0 = physx::PxClamp(x0, 0, GRID_SIZE - 1);
	y0 = physx::PxClamp(y0, 0, GRID_SIZE - 1);
	x1 = physx::PxClamp(x1, 0, GRID_SIZE - 1);
	y1 = physx::PxClamp(y1, 0, GRID_SIZE - 1);

	return aStar(angle, x0, y0, x1, y1);
}

std::vector<std::shared_ptr<Node>> GameMap::aStar(float angle, int startX, int startY, int goalX, int goalY) {
	using namespace std;
	angle = angle * 180. / 3.14159;

	// Data structures for open list, closed list and cumulative cost.
	priority_queue<shared_ptr<Node>, vector<shared_ptr<Node>>, CompareNode> openList;
	vector<vector<bool>> closedList(GRID_SIZE, vector<bool>(GRID_SIZE, false));
	vector<vector<int>> costSoFar(GRID_SIZE, vector<int>(GRID_SIZE, numeric_limits<int>::max()));

	// Create start node
	int hCost = heuristic(startX, startY, goalX, goalY);
	shared_ptr<Node> startNode = make_shared<Node>(angle, startX, startY, 0, hCost);
	openList.push(startNode);
	costSoFar[startX][startY] = 0;

	while (!openList.empty()) {
		shared_ptr<Node> current = openList.top();
		openList.pop();

		// If goal reached, reconstruct path and return
		if (current->x == goalX && current->y == goalY) {
			return reconstructPath(current);
		}

		closedList[current->x][current->y] = true;

		// get the valid direction range angle +/-45
		int dir[3];

		if (abs(current->angle) < 22.5) {
			dir[0] = 0;
			dir[1] = 7;
			dir[2] = 1;
		}
		else if (abs(current->angle) > 157.5) {
			dir[0] = 4;
			dir[1] = 3;
			dir[2] = 5;
		}
		else if (current->angle > 0) {
			if (current->angle < 67.5) {
				dir[0] = 1;
				dir[1] = 0;
				dir[2] = 2;
			}
			else if (current->angle < 112.5) {
				dir[0] = 2;
				dir[1] = 1;
				dir[2] = 3;
			}
			else {
				dir[0] = 3;
				dir[1] = 2;
				dir[2] = 4;
			}
		}
		else {
			if (current->angle > -67.5) {
				dir[0] = 7;
				dir[1] = 6;
				dir[2] = 0;
			}
			else if (current->angle > -112.5) {
				dir[0] = 6;
				dir[1] = 5;
				dir[2] = 7;
			}
			else {
				dir[0] = 5;
				dir[1] = 4;
				dir[2] = 6;
			}
		}
		
		// Explore neighbors
		for (int i = 0; i < 3; i++) {
			int newX = current->x + dt[dir[i]].x;
			int newY = current->y + dt[dir[i]].y;

			// Check bounds.
			if (newX < 0 || newX >= GRID_SIZE || newY < 0 || newY >= GRID_SIZE)
				continue;

			// check for blocking
			if (grid[newX][newY] == 0)
				continue;

			// Check if already visited.
			if (closedList[newX][newY])
				continue;

			int newCost = current->sCost + 1;
			if (newCost < costSoFar[newX][newY]) {
				costSoFar[newX][newY] = newCost;
				int h = heuristic(newX, newY, goalX, goalY);

				// Create neighbor with current as parent.
				shared_ptr<Node> neighbor = make_shared<Node>(dirToAngle(dir[i]), newX, newY, newCost, h, current);
				openList.push(neighbor);
			}
		}
	}
	// If no path is found, return empty vector.
	return vector<shared_ptr<Node>>();
}

void GameMap::printGraph(std::vector<std::shared_ptr<Node>> path) {
	bool* tmp = new bool[GRID_SIZE * GRID_SIZE];

	for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
		tmp[i] = false;
	}

	for (auto p : path) {
		tmp[p->x + p->y * GRID_SIZE] = true;
	}

	for (int y = GRID_SIZE - 1; y >= 0; y--) {
		for (int x = 0; x < GRID_SIZE; x++) {
			if (tmp[x + y * GRID_SIZE] == false) {
				std::cout << ". ";
			}
			else {
				std::cout << "# ";
			}
		}
		std::cout << std::endl;
	}

	delete[] tmp;
}

// -- Obstacle Avoidance
float GameMap::rayCast(int x0, int y0, float angle, float step) {
	float dist = 0.0f;

	while(true) {
		int x = x0 + int(round(dist * cos(angle)));
		int y = y0 + int(round(dist * sin(angle)));

		// Check bounds
		if (outOfBound(x) || outOfBound(y)) {
			break;
		}

		// check or obstacle
		if (grid[x][y] == 0) {
			break;
		}

		dist += step;
	}

	return dist;
}

float GameMap::rayCast(float x0, float y0, float angle, float step) {
	return rayCast(toGridSpace(x0), toGridSpace(y0), angle, step);
}

physx::PxVec2 GameMap::openArea(float x0, float y0) {
	int x = toGridSpace(x0);
	int y = toGridSpace(y0);

	const float PI = 3.14159265f;
	float dist = 0.0f;
	float angle = 0.0f;

	// check angle at 1 deg increments
	for (float theta = -PI/2; theta <= PI/2; theta += ( PI * 1.f / 180.f)) {
		float len = rayCast(x, y, theta, 1.f);
		if (len > dist) {
			dist = len;
			angle = theta;
		}
	}

	return physx::PxVec2(
		toGameSpace(x + round(dist * cos(angle))),
		toGameSpace(y + round(dist * sin(angle)))
	);
}

bool GameMap::checkCollisions(int x0, int y0, int x1, int y1) {
	// check if start or end is out of bounds
	if (outOfBound(x0, y0) || outOfBound(x1, y1))
		return false;

	// convert start to float
	float x = x0;
	float y = y0;

	// get x and y distance
	int dx = x1 - x0;
	int dy = y1 - y0;

	// get step direction
	int stepX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
	int stepY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

	// get discrete step range
	float tDeltaX = (std::abs(dx) != 0) ? (1.0f / std::abs(dx)) : std::numeric_limits<float>::infinity();
	float tDeltaY = (std::abs(dy) != 0) ? (1.0f / std::abs(dy)) : std::numeric_limits<float>::infinity();

	// get first boundaries
	float tMaxX, tMaxY;
	if (dx == 0)
		tMaxX = std::numeric_limits<float>::infinity();
	else if (stepX > 0)
		tMaxX = ((x0 + 1) - x) * tDeltaX;
	else
		tMaxX = (x - x0) * tDeltaX;

	if (dy == 0)
		tMaxY = std::numeric_limits<float>::infinity();
	else if (stepY > 0)
		tMaxY = ((y0 + 1) - y) * tDeltaY;
	else
		tMaxY = (y - y0) * tDeltaY;

	// distance between two points (squared)
	float dd = dx * dx + dy * dy;

	// traverse till the checked cells equal dd
	while ((((x - x0) * dx + (y - y0) * dy) / dd) < 1.0f) {

		// check if boundary is x or y
		if (tMaxX < tMaxY) {
			// advance x
			x += stepX;
			tMaxX += tDeltaX;
		}
		else if (tMaxX > tMaxY) {
			// advance y
			y += stepY;
			tMaxY += tDeltaY;
		}
		else {
			// next boundary is a corner; advance both
			x += stepX;
			tMaxX += tDeltaX;
			y += stepY;
			tMaxY += tDeltaY;

			// check bounds
			if (outOfBound(int(std::floor(x)), int(std::floor(y))) ||
				outOfBound(int(std::floor(x - stepX)), int(std::floor(y - stepY)))) {
				return false;
			}

			// check for collision
			if (grid[int(std::floor(x))][int(std::floor(y - stepY))] == 0 ||
				grid[int(std::floor(x - stepX))][int(std::floor(y))] == 0)
				return false;
		}

		// check bound
		if (outOfBound(int(std::floor(x)), int(std::floor(y)))) {
			return false;
		}

		// check collision
		if (grid[int(std::floor(x))][int(std::floor(y))] == 0) {
			return false;
		}
	}

	// no collision found
	return true;
}

// -- Direct Attack
bool GameMap::castRayTo(float x0, float y0, float x1, float y1) {
	return checkCollisions(
		float(toGridSpace(x0)), float(toGridSpace(y0)),
		float(toGridSpace(x1)), float(toGridSpace(y1))
	);
}

bool GameMap::isOccupied(float x, float z) const
{
	int gx = toGridSpace(x);
	int gz = toGridSpace(z);

	if (outOfBound(gx) || outOfBound(gz))
		return true;

	return grid[gx][gz] == 0;
}