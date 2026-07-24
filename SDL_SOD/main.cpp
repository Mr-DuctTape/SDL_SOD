#include <iostream>
#include "Engine/SOD_Engine.h"
#include "Game/Game.h"

int main()
{
	Engine engine;
	engine.Initialize();

	bool enableDebugger = false;

	float fps = 0.0f;
	Uint64 lastTime = SDL_GetTicksNS();
	int frameCount = 0;

	constexpr float targetFrameTime = 1.0f / 6000.0f;

	AudioManager::AudioClip* clip = engine.audioManager.CreateAudioClip("Dash", "Assets/Audio/DashSound.wav");
	AudioManager::AudioClip* clip2 = engine.audioManager.CreateAudioClip("Step", "Assets/Audio/Step.wav");
	AudioManager::AudioClip* clip3 = engine.audioManager.CreateAudioClip("Jump", "Assets/Audio/Jumo.wav");
	AudioManager::AudioClip* clip4 = engine.audioManager.CreateAudioClip("Music", "Assets/Audio/sample.wav");

	Game game;
	game.LoadTexturesAndPrefabs(engine.entityManager, engine.assetManager);
	game.LoadLevel(engine.entityManager, engine.assetManager, "Assets/Levels/Level1");

	// Game loop
	while (engine.isRunning)
	{
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
		if(engine.inputSystem.GetButtonDown(SDL_SCANCODE_B))
			engine.audioManager.PlayAudioClip(*clip2);

		if (enableDebugger)
		{
			engine.debugger.enabled = true;
			engine.debugger.DrawAllColliders(engine.entityManager);
		}

		game.Update(engine.renderingSystem, engine.entityManager, engine.audioManager, engine.inputSystem, engine.deltaTime);


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