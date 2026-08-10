#include <iostream>
#include "Engine/SOD_Engine.h"
#include "Game/Game.h"

int frameLimit = 144;

void UpdateSettings(Game& game, Engine& engine)
{
	Settings& settings = game.GetSettings();

	switch (settings.frameLimit)
	{
	case Settings::FrameLimit::F30:
		frameLimit = 30;
		break;
	case Settings::FrameLimit::F60:
		frameLimit = 60;
		break;
	case Settings::FrameLimit::F90:
		frameLimit = 90;
		break;
	case Settings::FrameLimit::F120:
		frameLimit = 120;
		break;
	case Settings::FrameLimit::F144:
		frameLimit = 144;
		break;
	case Settings::FrameLimit::F200:
		frameLimit = 200;
		break;
	case Settings::FrameLimit::F240:
		frameLimit = 240;
		break;
	case Settings::FrameLimit::F360:
		frameLimit = 360;
		break;
	case Settings::FrameLimit::UNLIMITED:
		frameLimit = 99999;
		break;
	}

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
	Uint64 initStart = SDL_GetPerformanceCounter();

	Engine engine;
	engine.Initialize();

	bool enableDebugger = false;

	AudioManager::AudioClip* clip = engine.audioManager.CreateAudioClip("Dash", "Assets/Audio/Dash.wav");
	AudioManager::AudioClip* clip2 = engine.audioManager.CreateAudioClip("Step", "Assets/Audio/Step.wav");
	AudioManager::AudioClip* clip3 = engine.audioManager.CreateAudioClip("Jump", "Assets/Audio/Jump2.wav");

	Uint64 initEnd = SDL_GetPerformanceCounter();

	float ms = (initEnd - initStart) * 1000.0f / SDL_GetPerformanceFrequency();
	std::cout << "It took " << ms << "ms to initialize the game & engine \n";

	Game game(engine);

	// Game loop

	while (engine.isRunning)
	{
		float targetFrameTime = 1.0f / (float)frameLimit;

		engine.debugger.DebuggerStartTime();
		engine.DeltaTimeUpdate();
		engine.inputSystem.Process();
		engine.renderingSystem.ClearScreen();

		// Debugger stuff, get rid off on release
		if (engine.inputSystem.GetButtonDown(SDL_SCANCODE_0))
		{
			enableDebugger = !enableDebugger;
			engine.debugger.enabled = enableDebugger;
			engine.debugger.boxColliders.clear();
		}
		if (enableDebugger)
		{
			engine.debugger.enabled = true;
			engine.debugger.DrawAllColliders(engine.entityManager);
		}

		game.Update();

		if (!game.playing)
			UpdateSettings(game, engine);

		engine.Update();
		engine.renderingSystem.RenderScreen(engine.entityManager);

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

	return 0;
}