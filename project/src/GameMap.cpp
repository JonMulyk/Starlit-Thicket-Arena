#include "GameMap.h"


GameMap::GameMap() : grid(GRID_SIZE, std::vector<int>(GRID_SIZE, GRID_SIZE)) {}

// --- Clearance based graph finding
void GameMap::addBlock(int x, int y) {
	// check invalid coordinates
	if (x < 0 || y < 0 || x >= GRID_SIZE || y >= GRID_SIZE) return;

	// add if currently it is not blocked
	if (grid[x][y] != 0) {
		grid[x][y] = 0;
		q.push({ x, y });
	}
}

void GameMap::updateGrid(physx::PxVec2 start, physx::PxVec2 end) {
	// Convert game space to grid space
	physx::PxVec2T<int> p1 = {
		int((start.x + MAX_ACTUAL) * GRID_SIZE / (2 * MAX_ACTUAL)),
		int((start.y + MAX_ACTUAL) * GRID_SIZE / (2 * MAX_ACTUAL)),
	};

	physx::PxVec2T<int> p2 = {
		int((end.x + MAX_ACTUAL) * GRID_SIZE / (2 * MAX_ACTUAL)),
		int((end.y + MAX_ACTUAL) * GRID_SIZE / (2 * MAX_ACTUAL)),
	};

	// p1 and p2 are out of bounds end
	if (p1.x < 0 || p1.x >= GRID_SIZE || p1.y < 0 || p1.y >= GRID_SIZE)
		if (p2.x < 0 || p2.x >= GRID_SIZE || p2.y < 0 || p2.y >= GRID_SIZE)
			return;


	// add start
	addBlock(p1.x, p1.y);

	// end if line is a point
	if (p1 == p2) {
		return;
	}

	// add end
	addBlock(p2.x, p2.y);

	// take discrete jumps
	physx::PxVec2 v = { float(p2.x - p1.x), float(p2.y - p1.y) };

	if (abs(v.x) > abs(v.y)) {
		int sign = (v.x >= 0) ? 1 : -1;
		v = v / abs(v.x);

		for (int i = 0; p1.x + i != p2.x; i += sign) {
			addBlock(p1.x + i, p1.y + int(i * v.y));
		}
	}
	else {
		int sign = (v.y >= 0) ? 1 : -1;
		v = v / abs(v.y);

		for (int i = 0; p1.y + i != p2.y; i += sign) {
			addBlock(p1.x + int(i * v.x), p1.y + i);
		}
	}
}

void GameMap::propogateWeights() {
	while (!q.empty()) {
		// get current position
		physx::PxVec2T<int> cur = q.front();
		q.pop();

		// go through each direction
		for (int dir = 0; dir < 8; dir++) {
			physx::PxVec2T<int> n = cur + dt[dir];

			// Check boundaries.
			if (n.x >= 0 && n.x < GRID_SIZE && n.y >= 0 && n.y < GRID_SIZE) {
				// If the neighbor is open and we found a shorter distance.
				if (grid[n.x][n.y] > grid[cur.x][cur.y] + 1) {
					grid[n.x][n.y] = grid[cur.x][cur.y] + 1;
					q.push(n);
				}
			}
		}
	}
}

void GameMap::updateMap(physx::PxVec2 start, physx::PxVec2 end) {
	updateGrid(start, end);
	propogateWeights();
}

int GameMap::isBlocked(double x0, double z0) {
	int x = (x0 + MAX_ACTUAL) * GRID_SIZE / (2 * MAX_ACTUAL);
	int z = (z0 + MAX_ACTUAL) * GRID_SIZE / (2 * MAX_ACTUAL);

	if (x < 0 || x >= GRID_SIZE) return true;
	if (z < 0 || z >= GRID_SIZE) return true;

	return grid[x][z];
}

void GameMap::printGraph() {
	for (int y = GRID_SIZE - 1; y >= 0; y--) {
		for (int x = GRID_SIZE - 1; x >= 0; x--) {
			if (grid[x][y] == 0) {
				std::cout << ".  ";
			}
			else if (grid[x][y] < 10) {
				std::cout << grid[x][y] << "  ";
			}
			else {
				std::cout << grid[x][y] << " ";
			}
		}
		std::cout << std::endl;
	}
}

// --- A* Stuff
int GameMap::heuristic(int x, int y, int goalX, int goalY) {
	return std::abs(x - goalX) + std::abs(y - goalY);
}

// Convert pointer connected list to array
std::vector<Node*> GameMap::reconstructPath(Node* goalNode) {
	std::vector<Node*> path;

	while (goalNode != nullptr) {
		path.push_back(goalNode);
		goalNode = goalNode->parent;
	}

	std::reverse(path.begin(), path.end());
	return path;
}

std::vector<Node*> GameMap::aStar(int startX, int startY, int goalX, int goalY) {
	// Create data structures for openlist, closed list and cumulative cost
	std::priority_queue<Node*, std::vector<Node*>, CompareNode> openList;
	std::vector<std::vector<bool>> closedList(GRID_SIZE, std::vector<bool>(GRID_SIZE, false));
	std::vector<std::vector<int>> costSoFar(GRID_SIZE, std::vector<int>(GRID_SIZE, std::numeric_limits<int>::max()));
	Node* current = nullptr;

	// Create start node
	int hCost = heuristic(startX, startY, goalX, goalY);
	Node* startNode = new Node(startX, startY, 0, hCost);
	openList.push(startNode);
	costSoFar[startX][startY] = 0;

	while (!openList.empty()) {
		current = openList.top();
		openList.pop();

		// If goal reached, reconstruct path
		if (current->x == goalX && current->y == goalY) {
			return reconstructPath(current);
		}

		closedList[current->x][current->y] = true;

		// Explore neighbors
		for (int dir = 0; dir < 8; ++dir) {
			int newX = current->x + dt[dir].x;
			int newY = current->y + dt[dir].y;

			// Check bounds
			if (newX < 0 || newX >= GRID_SIZE || newY < 0 || newY >= GRID_SIZE)
				continue;

			// Check clearance
			if (grid[newX][newY] < REQUIRED_CLEARANCE)
				continue;

			// Check if its already been passed
			if (closedList[newX][newY])
				continue;

			// Cost: here assuming cost 1 per step; adjust if needed
			int newCost = current->sCost + 1;

			if (newCost < costSoFar[newX][newY]) {
				costSoFar[newX][newY] = newCost;
				int h = heuristic(newX, newY, goalX, goalY);
				Node* neighbor = new Node(newX, newY, newCost, h, current);
				openList.push(neighbor);
			}
		}
	}
	// If no path is found, return empty path
	return std::vector<Node*>();
}

void GameMap::printGraph(std::vector<Node*> path) {
	bool tmp[GRID_SIZE][GRID_SIZE] = { false };

	Node* curr = nullptr;

	for (auto p : path) {
		tmp[p->x][p->y] = true;
	}

	for (int y = GRID_SIZE - 1; y >= 0; y--) {
		for (int x = GRID_SIZE - 1; x >= 0; x--) {
			if (tmp[x][y] == false) {
				std::cout << ".. ";
			}
			else {
				std::cout << "## ";
			}
		}
		std::cout << std::endl;
	}
}
