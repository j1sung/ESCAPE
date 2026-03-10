#pragma once

#include "Common/RTTI.h"
#include "Math/Vector2.h"
#include "Math/Color.h"

namespace Dark
{
	// 전방 선언.
	class Level;

	class DARK_API Actor : public RTTI
	{
		RTTI_DECLARATIONS(Actor, RTTI)

	public:
		Actor(
			const char* image = "",
			const Vector2& position = Vector2::Zero,
			Color color = Color::White
		);
		~Actor();

		// 게임 플레이 이벤트.
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		// 삭제 요청 함수.
		void Destroy();

		// 삭제가 될 때 호출될 이벤트 함수.
		virtual void OnDestroy(); // Player가 오버라이딩 하기

		// 게임 종료 함수.
		//void QuitGame(); // -> MenuActor에서 이걸 쓸지 owner->DieMenu()를 할지 미정.

		// 충돌 여부 확인 함수.
		bool TestIntersect(const Actor* const other);

		// 액터의 이미지 값 변경 함수.
		void ChangeImage(const char* newImage);

		// 위치 변경 및 읽기 함수.
		void SetPosition(const Vector2& newPosition);
		inline Vector2 GetPosition() const { return position; }
		int GetRightmostX() const; // ai 코드

		// 오너십 추가/읽기 함수.
		inline void SetOwner(Level* newOwner) { owner = newOwner; }
		inline Level* GetOwner() const { return owner; }

		// Getter.
		inline bool HasBeganPlay() const
		{
			return hasBeganPlay;
		}

		inline bool IsActive() const
		{
			return isActive && !destroyRequested;
		}

		inline bool DestroyRequested() const
		{
			return destroyRequested;
		}

		inline int GetSortingOrder() const { return sortingOrder; }

		inline int GetWidth() const { return actorWidth; }
		inline int GetHeight() const { return actorHeight; }


		//inline ine GetWidth() const { return width; }

	protected:
		// 그릴 문자(이미지).
		char* image = nullptr;

		// 색상
		Color color = Color::White;

		// 위치.
		Vector2 position;

		// 오너십(Ownership).
		Level* owner = nullptr;

		// 이미 BeginPlay 이벤트를 받았는지 여부.
		bool hasBeganPlay = false;

		// 활성화 상태 여부.
		bool isActive = true;

		// 현재 프레임에 삭제 요청 받았는지 여부.
		bool destroyRequested = false;

		// 그리기 우선 순위 (값이 크면 우선순위가 높음).
		int sortingOrder = 0;

		// 문자열 너비.
		int actorWidth = 0;
		// 문자열 높이.
		int actorHeight = 0;
	};

}

