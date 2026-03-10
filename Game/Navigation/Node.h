#pragma once

#include "Math/Vector2.h"

using namespace EscapeEngine;

class Node
{
public:
	Node(int x, int y, Node* parentNode = nullptr)
		: position(x, y), parentNode(parentNode)
	{

	}

	// 노드 위치 뺀 방향 구하는 연산자 오버로딩
	Vector2 operator-(const Node& other) const
	{
		return Vector2(position.x - other.position.x, position.y - other.position.y);
	}

	// 두 노드의 위치가 같은지 비교 연산자 오버로딩.
	bool operator==(const Node& other) const
	{
		return position.x == other.position.x && position.y == other.position.y;
	}

public:
	// 이 노드의 위치.
	Vector2 position;

	// 시작 위치에서 이 노드까지의 비용.
	float gCost = 0.0f;

	// 이 노드에서 목표지점까지의 예상 비용(휴리스틱).
	float hCost = 0.0f;

	// 최종 비용(gCost+hCost)
	float fCost = 0.0f;

	// 부모 노드 링크
	Node* parentNode = nullptr;
};
