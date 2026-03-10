#pragma once

#include "Navigation/Node.h"

#include <vector>

template<typename T>
void SafeDeletePathNode(T*& t)
{
	if (t)
	{
		delete t;
		t = nullptr;
	}
}

// class는 처리하는 객체 처리
class AStar
{
	// 방향 처리를 위한 구조체.
	// 데이터는 public으로 해도 됨 -> 구조체는 데이터 관리에 씀
	struct Direction
	{
		// 위치.
		int x = 0;
		int y = 0;

		// 이동 비용.
		float cost = 0.0f;
	};

public:
	AStar();
	~AStar();

	// 탐색 시작 함수.
	void Initialize(
		const Vector2& startPosition,
		const Vector2& goalPosition
	);

	// 탐색을 한 단계 진행하는 함수.
	bool Step(std::vector<std::vector<int>>& grid);

	// 경로 검색(탐색) 함수.
	// startPosition: 시작 지점.
	// goalPosition: 목표 지점.
	// grid: 탐색할 맵(2차원 배열).
	std::vector<Node*> FindPath(
		const Vector2& startPosition,
		const Vector2& goalPosition,
		std::vector<std::vector<int>>& grid
	);

	// 내부 데이터 초기화 함수.
	void Reset();

	// 상태 확인 함수.
	inline bool IsSearching() const { return isSearching; }
	inline bool IsFinished() const { return isFinished; }
	inline bool HasFoundPath() const { return hasFoundPath; }
	inline const std::vector<Node*>& GetPath() const { return path; }

private:
	// 탐색을 완료한 후에 최적 경로 반환하는 함수.
	// 부모링크를 따라가면서 역추적.
	std::vector<Node*> ConstructPath(Node* goalNode);

	// 휴리스틱(hCost) 계산 함수.
	float CalculateHeuristic(Node* currentNode, Node* goalNode);

	// 탐색하려는 위치(노드)가 그리드 범위 안에 있는지 확인.
	bool IsInRange(int x, int y, const std::vector<std::vector<int>>& grid);

	// 이미 방문했는지 확인하는 함수.
	bool HasVisited(int x, int y, float gCost);

	// 탐색하려는 노드가 목표 노드인지 확인하는 함수.
	bool IsDestination(const Node* const node);

	// 셀 시각화 상태 갱신 함수.
	void SetCellState(std::vector<std::vector<int>>& grid, int x, int y, int value);

private:
	// 열린 리스트.
	std::vector<Node*> openList;

	// 닫힌 리스트.
	std::vector<Node*> closedList;

	// 시작 노드.
	Node* startNode = nullptr;

	// 목표 노드.
	Node* goalNode = nullptr;

	// 최종 경로.
	std::vector<Node*> path;

	// 탐색 상태.
	bool isSearching = false;
	bool isFinished = false;
	bool hasFoundPath = false;
};
