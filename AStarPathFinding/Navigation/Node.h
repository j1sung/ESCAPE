#pragma once

// 콘솔 좌표에서 위치를 나타낼 클래스.
class Position
{
public:
	int x = 0;
	int y = 0;

	Position(int x, int y)
		:x(x), y(y)
	{

	}
};

class Node
{
public:
	Node(int x, int y, Node* parentNode = nullptr)
		: position(x, y), parentNode(parentNode)
	{
		
	}

	// 노드 위치 뺀 방향 구하는 연산자 오버로딩
	Position operator-(const Node& other) const
	{
		return Position(position.x - other.position.x, position.y - other.position.y);
	}

	// 두 노드의 위치가 같은지 비교 연산자 오버로딩.
	bool operator==(const Node& other) const
	{
		return position.x == other.position.x && position.y == other.position.y;
	}

public:
	// 이 노드의 위치.
	Position position;

	// 시작 위치에서 이 노드까지의 비용.
	float gCost = 0.0f;

	// 이 노드에서 목표지점까지의 예상 비용(휴리스틱).
	float hCost = 0.0f;

	// 최종 비용(gCost+hCost)
	float fCost = 0.0f;

	// 부모 노드 링크
	Node* parentNode = nullptr;
};