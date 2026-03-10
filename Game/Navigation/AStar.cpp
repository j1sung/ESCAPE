#include "Navigation/AStar.h"

#include <algorithm>
#include <cmath>

AStar::AStar()
{
}

AStar::~AStar()
{
	Reset();
}

std::vector<Node*> AStar::FindPath(
	const Vector2& startPosition,
	const Vector2& goalPosition,
	std::vector<std::vector<int>>& grid
)
{
	// 이전 탐색 정보 정리.
	Reset();

	// 시작/목표 노드 생성.
	startNode = new Node(startPosition.x, startPosition.y);
	goalNode = new Node(goalPosition.x, goalPosition.y);

	// 예외처리.
	if (!startNode || !goalNode || grid.empty() || grid[0].empty())
	{
		return { };
	}

	// 시작 노드를 열린 리스트에 추가 및 탐색 시작.
	openList.emplace_back(startNode);

	// 대각선 비용 상수.
	const float diagonalCost = 1.41421345f;

	// 비용 계산에 사용할 변수 값 설정.
	std::vector<Direction> directions =
	{
		// 하상우좌(상하좌우 뭐로 해도 됨) 이동
		{0,1,1.0f}, {0,-1,1.0f}, {1,0,1.0f}, {-1,0,1.0f},
		// 대각선 이동.
		//{1,1,diagonalCost}, {-1,-1, diagonalCost},
		//{-1,1, diagonalCost}, {1,-1,diagonalCost}
	};

	// 탐색 가능한 위치가 있으면 계속 진행.
	while (!openList.empty())
	{
		// 현재 열린 리스트에 있는 노드 중 fCost가 가장 낮은 노드 검색.
		Node* lowestNode = openList[0];

		// 가장 비용이 작은 노드 검색(선형 탐색).
		for (Node* const node : openList)
		{
			if (node->fCost < lowestNode->fCost)
			{
				lowestNode = node;
			}
		}

		// fCost가 낮은 노드를 현재 노드로 설정.
		Node* currentNode = lowestNode;

		// 현재 노드가 목표 노드인지 확인.
		if (IsDestination(currentNode))
		{
			std::vector<Node*> path = ConstructPath(currentNode);

			// 최종 경로만 5로 표시.
			for (Node* const node : path)
			{
				SetCellState(grid, node->position.x, node->position.y, 5);
			}

			return path;
		}

		// 방문 처리를 위해 열린 리스트에서 제거.
		for (auto iterator = openList.begin(); iterator != openList.end(); ++iterator)
		{
			if ((*iterator) == currentNode)
			{
				openList.erase(iterator);
				break;
			}
		}

		// 현재 노드를 방문 노드에 추가.
		closedList.emplace_back(currentNode);

		// 이웃 노드 방문(탐색).
		for (const Direction& direction : directions)
		{
			// 다음에 이동할 위치(이웃 노드의 위치).
			int newX = currentNode->position.x + direction.x;
			int newY = currentNode->position.y + direction.y;

			// 유효성 검증.
			if (!IsInRange(newX, newY, grid))
			{
				continue;
			}

			// 새 위치가 이동 가능한 곳인지 확인.
			// 장애물(못가는곳) = 1, 위험 지형 = 4.
			if (grid[newY][newX] == 1 || grid[newY][newX] == 4)
			{
				continue;
			}

			// 현재 노드를 기준으로 새 gCost 계산.
			float newGCost = currentNode->gCost + direction.cost;

			// 이미 방문한 곳인지 확인.
			if (HasVisited(newX, newY, newGCost))
			{
				continue;
			}

			// 방문을 위한 이웃 노드 생성.
			Node* neighborNode = new Node(newX, newY, currentNode);

			// 비용 계산.
			neighborNode->gCost = newGCost;
			neighborNode->hCost = CalculateHeuristic(neighborNode, goalNode);
			neighborNode->fCost = neighborNode->gCost + neighborNode->hCost;

			// 이웃 노드가 열린 리스트에 있는지 확인.
			Node* openListNode = nullptr;
			for (Node* const node : openList)
			{
				if (*node == *neighborNode)
				{
					openListNode = node;
					break;
				}
			}

			// 이웃 노드가 열린 리스트에 있으면 더 좋은 비용일 때만 처리.
			if (openListNode)
			{
				if (neighborNode->gCost < openListNode->gCost || neighborNode->fCost < openListNode->fCost)
				{
					openListNode->parentNode = neighborNode->parentNode;
					openListNode->gCost = neighborNode->gCost;
					openListNode->hCost = neighborNode->hCost;
					openListNode->fCost = neighborNode->fCost;
				}

				SafeDeletePathNode(neighborNode);
				continue;
			}

			// openList(열린 리스트)에 추가.
			openList.emplace_back(neighborNode);
		}
	}

	return { };
}

void AStar::Reset()
{
	for (Node* node : openList)
	{
		SafeDeletePathNode(node);
	}
	openList.clear();

	for (Node* node : closedList)
	{
		SafeDeletePathNode(node);
	}
	closedList.clear();

	SafeDeletePathNode(goalNode);
	startNode = nullptr;
}

std::vector<Node*> AStar::ConstructPath(Node* goalNode)
{
	// 경로를 저장할 배열 선언.
	std::vector<Node*> path;

	// 역추적하면서 path에 채우기.
	Node* currentNode = goalNode;
	while (currentNode)
	{
		path.emplace_back(currentNode);
		currentNode = currentNode->parentNode;
	}

	// 이렇게 얻은 결과는 순서가 거꾸로.
	std::reverse(path.begin(), path.end());
	return path;
}

float AStar::CalculateHeuristic(Node* currentNode, Node* goalNode)
{
	Vector2 diff = *currentNode - *goalNode;
	return static_cast<float>(std::sqrt(static_cast<float>(diff.x * diff.x + diff.y * diff.y)));
}

bool AStar::IsInRange(int x, int y, const std::vector<std::vector<int>>& grid)
{
	if (grid.empty() || grid[0].empty())
	{
		return false;
	}

	if (x < 0 || x >= static_cast<int>(grid[0].size()) || y < 0 || y >= static_cast<int>(grid.size()))
	{
		return false;
	}

	return true;
}

bool AStar::HasVisited(int x, int y, float gCost)
{
	for (Node* const node : openList)
	{
		if (node->position.x == x && node->position.y == y && gCost >= node->gCost)
		{
			return true;
		}
	}

	for (Node* const node : closedList)
	{
		if (node->position.x == x && node->position.y == y)
		{
			return true;
		}
	}

	return false;
}

bool AStar::IsDestination(const Node* const node)
{
	return *node == *goalNode;
}

void AStar::SetCellState(std::vector<std::vector<int>>& grid, int x, int y, int value)
{
	int& cell = grid[y][x];

	// 시작/목표/벽은 시각화 상태로 덮어쓰지 않음.
	if (cell == 1 || cell == 2 || cell == 3)
	{
		return;
	}

	cell = value;
}
