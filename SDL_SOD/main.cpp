#include <iostream>
#include "Engine/SOD_Engine.h"
#include "Game/Game.h"

int main()
{
	Uint64 initStart = SDL_GetPerformanceCounter();

	Engine engine;
	engine.Initialize();

	bool enableDebugger = false;

	float fps = 0.0f;
	Uint64 lastTime = SDL_GetTicksNS();
	int frameCount = 0;

	constexpr float targetFrameTime = 1.0f / 6000.0f;

	AudioManager::AudioClip* clip = engine.audioManager.CreateAudioClip("Dash", "Assets/Audio/Dash.wav");
	AudioManager::AudioClip* clip2 = engine.audioManager.CreateAudioClip("Step", "Assets/Audio/Step.wav");
	AudioManager::AudioClip* clip3 = engine.audioManager.CreateAudioClip("Jump", "Assets/Audio/Jump2.wav");

	Uint64 initEnd = SDL_GetPerformanceCounter();

	float ms = (initEnd - initStart) * 1000.0f / SDL_GetPerformanceFrequency();
	std::cout << "It took " << ms << "ms to initialize the game & engine \n";

	Button button1;
	button1.screenPosition = { 200, 200 };
	button1.text = "I AM A BUTTON!!!";
	button1.stationaryColor = { 124, 200, 100, 255 };
	button1.width = 200;
	button1.height = 100;
	button1.highlightedColor = { 124, 200, 100, 150 };
	button1.pressedColor = { 124, 200, 100, 100 };
	engine.uiManager.AddButton("Button1", button1);

	Button button2;
	button2.screenPosition = { 1000, 200 };
	button2.text = "I AM A BUTTON TOO!!";
	button2.stationaryColor = { 200, 255, 0, 255 };
	button2.width = 100;
	button2.height = 50;
	button2.highlightedColor = { 200, 255, 0, 150 };
	button2.pressedColor = { 200, 255, 0, 100 };
	engine.uiManager.AddButton("Button2", button2);

	Game game;
	game.LoadTexturesAndPrefabs(engine.entityManager, engine.assetManager);
	game.LoadLevel(engine.entityManager, engine.assetManager, "Assets/Levels/Tutorial");

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