#include <iostream>
#include "Engine/SOD_Engine.h"
#include "Game/Game.h"
#include "Game/GameUI.h"
#include <Windows.h>

enum color
{
	RED,
	GREEN,
	BLUE
};

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


int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	Engine engine;
	engine.Initialize();

	AudioManager::AudioClip* clip = engine.audioManager.CreateAudioClip("Dash", "Assets/Audio/Dash.wav");
	AudioManager::AudioClip* clip2 = engine.audioManager.CreateAudioClip("Step", "Assets/Audio/Step.wav");
	AudioManager::AudioClip* clip3 = engine.audioManager.CreateAudioClip("Jump", "Assets/Audio/Jump2.wav");

	AudioManager::AudioClip* clip4 = engine.audioManager.CreateAudioClip("Jump2", "Assets/Audio/Jump.wav");

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
			auto& obj = std::get<std::optional<Player>>(game.m_gameEntities);
			std::cout << obj.value().GetEntity().GetComponent<Transform>()->position << "\n";
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

	return 0;
}