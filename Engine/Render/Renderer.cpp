#include "Renderer.h"
#include "ScreenBuffer.h"
#include "Util/Util.h"

#include <iostream>

namespace Dark
{
	Renderer::Frame::Frame(int bufferCount)
	{
		// 배열 생성 및 초기화.
		charInfoArray = new CHAR_INFO[bufferCount];
		memset(charInfoArray, 0, sizeof(CHAR_INFO) * bufferCount);

		sortingOrderArray = new int[bufferCount];
		memset(sortingOrderArray, 0, sizeof(int) * bufferCount);
	}

	Renderer::Frame::~Frame()
	{
		SafeDeleteArray(charInfoArray);
		SafeDeleteArray(sortingOrderArray);
	}

	void Renderer::Frame::Clear(const Vector2& screenSize)
	{
		// 2차원 배열로 다루는 1차원 배열을 순회하면서
		// 빈 문자 (' ')를 설정.
		const int width = screenSize.x;
		const int height = screenSize.y;

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				// 배열 인덱스 구하기.
				const int index = (y * width) + x;

				// 글자 값 및 속성 설정.
				CHAR_INFO& info = charInfoArray[index];
				info.Char.AsciiChar = ' ';
				info.Attributes = 0;

				// 그리기 우선순위 초기화.
				sortingOrderArray[index] = -1;
			}
		}
	}

	// -------------- Frame -------------- //

	// 정적 변수 초기화.
	Renderer* Renderer::instance = nullptr;

	Renderer::Renderer(const Vector2& screenSize)
		:screenSize(screenSize)
	{
		instance = this;

		// 프레임 객체 생성.
		const int bufferCount = screenSize.x * screenSize.y;
		frame = new Frame(bufferCount);

		// 프레임 초기화.
		frame->Clear(screenSize);

		// 이중 버퍼 객체 생성 및 초기화.
		screenBuffers[0] = new ScreenBuffer(screenSize);
		screenBuffers[0]->Clear();

		screenBuffers[1] = new ScreenBuffer(screenSize);
		screenBuffers[1]->Clear();

		// 활성화 버퍼 설정.
		Present();
	}

	Renderer::~Renderer()
	{
		SafeDelete(frame);
		for (ScreenBuffer*& buffer : screenBuffers)
		{
			SafeDelete(buffer);
		}
	}

	// 2) 프레임 버퍼(Frame) 비우고, 큐를 순회하며 문자/색/우선순위 기록.
	void Renderer::Draw()
	{
		// 화면 지우기.
		Clear();

		// 전제조건: 레벨의 모든 액터가 렌더러에 Submit을 완료.
		// 렌더큐 순회하면서 프레임 채우기.
		for (const RenderCommand& command : renderQueue)
		{
			// 화면에 그릴 텍스트가 없으면 건너뜀.
			if (!command.text)
			{
				continue;
			}

			// 세로 기준 화면 벗어났는지 확인.
			if (command.position.y < 0
				|| command.position.y >= screenSize.y)
			{
				continue;
			}

			// 여기 부분에서 ＼ｎ문자 판별하는거 로직 전체 뜯어바꿈
			const char* text = command.text;
			const int baseY = command.position.y;

			const char* lineStart = text;
			int lineIndex = 0;

			while (lineStart)
			{
				const char* lineEnd = strchr(lineStart, '\n');
				const int lineLength = lineEnd
					? static_cast<int>(lineEnd - lineStart)
					: static_cast<int>(strlen(lineStart));

				const int y = baseY + lineIndex;

				if (y >= 0 && y < screenSize.y && lineLength > 0)
				{
					// x?? ???? ???? ????? ??.
					// ??? ?? ??: "abcde"
					const int startX = command.position.x;
					const int endX = command.position.x + lineLength - 1;

					if (!(endX < 0 || startX >= screenSize.x))
					{
						// ?? ??.
						const int visibleStart = startX < 0 ? 0 : startX;
						const int visibleEnd = endX >= screenSize.x ? screenSize.x - 1 : endX;

						for (int x = visibleStart; x <= visibleEnd; ++x)
						{
							const int sourceIndex = x - startX;
							const int index = (y * screenSize.x) + x;

							if (frame->sortingOrderArray[index] > command.sortingOrder)
							{
								continue;
							}

							frame->charInfoArray[index].Char.AsciiChar = lineStart[sourceIndex];
							frame->charInfoArray[index].Attributes = (WORD)command.color;
							frame->sortingOrderArray[index] = command.sortingOrder;
						}
					}
				}

				if (!lineEnd)
				{
					break;
				}

				lineStart = lineEnd + 1;
				++lineIndex;
			}

		}

		// 그리기.
		GetCurrentBuffer()->Draw(frame->charInfoArray);

		// 버퍼 교환.
		Present();

		// 렌더 큐 비우기.
		renderQueue.clear();
	}



	Renderer& Renderer::Get()
	{
		if (!instance)
		{
			std::cerr << "Error: Renderer::Get() - instance is null\n";

			__debugbreak();
		}

		return *instance;
	}

	void Renderer::Clear()
	{
		// 화면 지우기.
		// 1. 프레임(2차원 배열 데이터) 지우기.
		frame->Clear(screenSize);

		// 2. 콘솔 버퍼 지우기.
		GetCurrentBuffer()->Clear();
	}

	// 1) 액터들의 그리기 데이터를 renderQueue에 적재.
	void Renderer::Submit(const char* text, const Vector2& position, Color color, int sortingOrder)
	{
		// 렌더 데이터 생성 후 큐에 추가.
		RenderCommand command = {};
		command.text = text;
		command.position = position;
		command.color = color;
		command.sortingOrder = sortingOrder;

		renderQueue.emplace_back(command);
	}

	//// 바로 그리기
	//void Renderer::PresentImmediately()
	//{
	//	// 이거 중복 로직아님?
	//	Draw();
	//	GetCurrentBuffer()->Draw(frame->charInfoArray);
	//	Present();
	//}

	void Renderer::Present()
	{
		// 버퍼 교환.
		SetConsoleActiveScreenBuffer(GetCurrentBuffer()->GetBuffer());

		// 인덱스 교체.
		currentBufferIndex = 1 - currentBufferIndex;
	}

	ScreenBuffer* Renderer::GetCurrentBuffer()
	{
		return screenBuffers[currentBufferIndex];
	}
}