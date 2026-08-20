#include <iostream>
#include "Engine/SOD_Engine.h"
#include "Game/Game.h"
#include "Game/GameUI.h"
#include <Windows.h>

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

void UpdateSettings(Game& game, Engine& engine)
{
	Settings& settings = game.GetSettings();

	SDL_SetRenderVSync(engine.renderingSystem.renderer, settings.vsync);

	switch (settings.displayMode)
	{
	case Settings::DisplayMode::FULLSCREEN:
		SDL_SetWindowFullscreen(engine.application.GetWindow(), true);
		break;
	case Settings::DisplayMode::WINDOWED:
		SDL_SetWindowFullscreen(engine.application.GetWindow(), false);
		break;
	}
}


int main()
{
	if constexpr (DEBUGPRINT) {
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	}

	Engine engine;
	engine.Initialize();

	constexpr std::pair<const char*, const char*> audioClips[] =
	{
		{"Dash", "Assets/Audio/Dash.wav"},
		{"Step", "Assets/Audio/Step.wav"},
		{"Jump", "Assets/Audio/Jump2.wav"},
		{"Jump2", "Assets/Audio/Jump.wav"},
		{"Hover", "Assets/Audio/Hover.wav"},
		{"Click", "Assets/Audio/Hover.wav"},
		{"TorchLight", "Assets/Audio/TorchLight.wav" }
	};
	 
	for (auto& [name, filePath] : audioClips)
	{
		engine.audioManager.CreateAudioClip(name, filePath);
	}

	Game game(engine);

	// Game loop
	while (engine.isRunning)
	{
		float targetFrameTime = 1.0f / (float)game.GetSettings().FrameLimitToInt();

		// Start of frame
		engine.debugger.DebuggerStartTime();
		engine.DeltaTimeUpdate();
		engine.inputSystem.Process();
		engine.renderingSystem.ClearScreen();
		//

		// Debugger stuff
		if (game.GetSettings().debugMode &&
			engine.inputSystem.GetButtonDown(SDL_SCANCODE_0))
		{
			engine.debugger.enabled = !engine.debugger.enabled;
			engine.debugger.boxColliders.clear();
		}
		else if (!game.GetSettings().debugMode)
		{
			engine.debugger.enabled = false;
		}

		if (engine.debugger.enabled)
		{
			engine.debugger.DrawAllColliders(engine.entityManager);
		}

		game.Update();

		if (!game.playing)
		{
			UpdateSettings(game, engine);
		}

		if (engine.inputSystem.GetButtonDown(SDL_SCANCODE_L))
		{
			auto& obj = game.m_gameEntities.player;
			std::cout << obj.value().GetEntity().GetComponent<Transform>().position << "\n";
		}

		// End of frame
		engine.Update();
		engine.renderingSystem.RenderScreen(engine.entityManager);
		//

		// IGNORE BELOW THIS IS NOT IMPORTANT ONLY FPS LIMITER
		float frameTime =
			(float)(SDL_GetPerformanceCounter() - engine.debugger.GetStartTime()) /
			SDL_GetPerformanceFrequency();

		if (frameTime < targetFrameTime)
		{
			float delay = (targetFrameTime - frameTime) * 1000.0f;
			SDL_Delay((Uint32)delay);
		}
		if (engine.inputSystem.GetButtonDown(SDL_SCANCODE_ESCAPE))
			engine.Quit();

		engine.debugger.DebuggerEndTime();
	}

	if constexpr (DEBUGPRINT) {
		FILE* file;

		fopen_s(&file, "MemoryLeaks.txt", "w");

		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_WARN, file);

		_CrtDumpMemoryLeaks();

		fclose(file);
	}
	return 0;
}