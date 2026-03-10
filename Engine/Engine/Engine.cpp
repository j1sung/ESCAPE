#include "Engine/Engine.h"
#include "Level/Level.h"
#include "Core/Input.h"
#include "Util/Util.h"
#include "Render/Renderer.h"

#include <iostream>
#include <Windows.h>

namespace Dark
{
	// 정적 변수 초기화.
	Engine* Engine::instance = nullptr;

	Engine::Engine()
	{
		// 정적 변수 값 초기화.
		instance = this;

		// 입력 관리자 생성.
		input = new Input();

		// 설정 파일 로드.
		LoadSetting();

		// 렌더러 객체 생성.
		renderer = new Renderer(Vector2(setting.width, setting.height));

		// 커서 끄기.
		//Util::TurnOffCursor();

		// 랜덤 종자값 설정.
		Util::SetRandomSeed();

	}
	Engine::~Engine()
	{
		// 메인 레벨 제거.
		SafeDelete(mainLevel);

		if (input)
		{
			delete input;
			input = nullptr;
		}

		// 렌더러 객체 제거.
		SafeDelete(renderer);
	}

	void Engine::Run()
	{
		// 고정밀 타이머 주파수 얻기(한번만) -> ex) 1초 1천만 tick 정도
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);

		// 프레임 계산용 변수.
		int64_t currentTime = 0;
		int64_t previousTime = 0;

		// 하드웨어 고정밀 타이머로 시간 얻기.
		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);

		// 엔진 시작 직전에는 두 시간 값을 같게 맞춤. 
		// -> time.QuadPart로 같게 해야함! 0이면 다음 시간부터 차이 너무 커서 튐!
		currentTime = time.QuadPart;
		previousTime = currentTime;

		// famerate 0 방지 초기화
		setting.framerate = setting.framerate == 0.0f ? 60.0f : setting.framerate;

		float oneFrameTime = 1.0f / setting.framerate;

		// 엔진 루프
		while (!isQuit)
		{
			// 현재 시간 구하기.
			QueryPerformanceCounter(&time);
			currentTime = time.QuadPart;

			// tick 차이 (정수) - 프레임 시간 계산.
			int64_t deltaTicks = currentTime - previousTime;

			// 초 단위 변환. -> float로 썼음 (double 아닌 이유? 속도)
			float deltaTime = 
				static_cast<float>(deltaTicks) / static_cast<float>(frequency.QuadPart);

			// 스파이크 방지 -> 0.1f(100ms, 10fps)
			Util::ClampMax<float>(deltaTime, 0.1f);

			// 고정 프레임 기법.
			if (deltaTime >= oneFrameTime)
			{
				input->ProcessInput();

				// 프레임 처리.
				BeginPlay();
				Tick(deltaTime);
				Draw();

				// 이전 시간 값 갱신.
				previousTime = currentTime;

				input->SavePreviousInputStates();

				// 레벨에 요청된 추가/제거 처리.
				if (mainLevel)
				{
					mainLevel->ProcessAddAndDestroyActors();
				}
				
				// 레벨 전환 처리
				if (nextLevel)
				{
					// 기존 레벨 제거.
					SafeDelete(mainLevel);

					// 전환할 레벨을 메인 레벨로 지정.
					mainLevel = nextLevel;

					// 포인터 정리.
					nextLevel = nullptr;
				}
			}
		}
		// 정리.
		Shutdown();
		
	}

	void Engine::QuitEngine()
	{
		isQuit = true;
	}

	void Engine::SetNewLevel(Level* newLevel)
	{
		mainLevel = newLevel;
	}

	Engine& Engine::Get()
	{
		// 예외 처리.
		if (!instance)
		{
			std::cerr << "Error: Engine::Get(). instance is null\n";
			__debugbreak();
		}
		return *instance;
	}

	void Engine::Shutdown()
	{
		// 정리 작업.
		std::cout << "Engine has been shutdown....\n";

		// 커서 켜기.
		//Util::TurnOnCursor();
	}

	void Engine::LoadSetting()
	{
		// 엔진 설정 파일 열기.
		FILE* file = nullptr;
		fopen_s(&file, "../Config/Setting.txt", "rt");

		// 예외 처리
		if (!file)
		{
			std::cerr << "Error: Failed to open engine setting file.\n";
			__debugbreak();
			return;
		}

		// 파일에서 읽은 데이터를 담을 버퍼.
		char buffer[2048] = {};

		// 파일에서 읽기.
		// buffer는 file에서 복사해옴.
		// readSize == 읽은 바이트 수 (\0은 없어서 필요하면 붙여야 함)
		size_t readSize = fread(buffer, sizeof(char), 2048, file);
		
		// 문자열 자르기 (파싱).
		// 첫번째 문자열 분리할 때는 첫 파라미터 전달.
		char* context = nullptr;
		char* token = nullptr;

		// \n 기준으로 분리해서 그 앞은 token으로 받고, 뒤의 시작주소를 context로 넣음
		// \n이 \0으로 변경되어 문자열로 들어감.
		token = strtok_s(buffer, "\n", &context);

		// 반복해서 자르기.
		while (token)
		{
			// 설정 텍스트에서 파라미터 이름만 읽기.
			char header[10] = {};

			// 문자열 읽기 함수 활용.
			// 이때 "%s"로 읽으면 스페이스가 있으면 거기까지 읽음.
			// token에서 공백전까지 읽고 header에 넣음.
			sscanf_s(token, "%s", header, 10); // ex) "framerate = 60"이면, 'framerate'만 읽음

			// 문자열 비교 및 값 읽기.
			if (strcmp(header, "framerate") == 0) // C 문자열 비교 strcmp, 같으면 0 반환.
			{
				sscanf_s(token, "framerate = %f", &setting.framerate);
			}
			else if (strcmp(header, "width") == 0)
			{
				sscanf_s(token, "width = %d", &setting.width);
			}
			else if (strcmp(header, "height") == 0)
			{
				sscanf_s(token, "height = %d", &setting.height);
			}

			// 개행 문자로 문자열 분리.
			// 다음 위치부터(&context에 저장되어있음) 다시 파싱.
			token = strtok_s(nullptr, "\n", &context);
		}

		// 파일 닫기.
		fclose(file);
	}



	// Awake -> 씬에서 액터들 한번만 실행
	void Engine::BeginPlay()
	{
		// 레벨이 있다면 이벤트 전달.
		if (!mainLevel)
		{
			return;
		}
		
		mainLevel->BeginPlay();
	}

	void Engine::Tick(float deltaTime)
	{
		// 레벨에 이벤트 전달.
		if (!mainLevel)
		{
			return;
		}

		mainLevel->Tick(deltaTime);
	}

	void Engine::Draw()
	{
		// 레벨에 이벤트 흘리기.
		if (!mainLevel)
		{
			return;
		}

		// 레벨의 모든 액터가 렌더 데이터 제출.
		mainLevel->Draw();

		// 렌더러에 그리기 명령 전달.
		renderer->Draw();
	}
}