#include <iostream>
#include "Engine/SOD_Engine.h"
#include "Game/Game.h"
#include "Game/GameUI.h"

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


int main()
{
	Engine engine;
	engine.Initialize();

	AudioManager::AudioClip* clip = engine.audioManager.CreateAudioClip("Dash", "Assets/Audio/Dash.wav");
	AudioManager::AudioClip* clip2 = engine.audioManager.CreateAudioClip("Step", "Assets/Audio/Step.wav");
	AudioManager::AudioClip* clip3 = engine.audioManager.CreateAudioClip("Jump", "Assets/Audio/Jump2.wav");

	AudioManager::AudioClip* clip4 = engine.audioManager.CreateAudioClip("Jump2", "Assets/Audio/Jump.wav");

	Game game(engine);

	auto& dialog = engine.dialogSystem.CreateDialog("Dialog");
	dialog.entireDialog.push_back("I am the mysterious Blob...");
	dialog.entireDialog.push_back("But friends call me BOB!");
	dialog.entireDialog.push_back("Since you're new around here.");
	dialog.entireDialog.push_back("I'll show you around!");
	dialog.entireDialog.push_back("Let me lead the way");
	dialog.position = { 200, 200 };
	dialog.audioClip = engine.audioManager.GetAudio("Jump2");

	auto& dialog2 = engine.dialogSystem.CreateDialog("Dialog2");
	dialog2.entireDialog.push_back("What did you just say Bob?");
	dialog2.entireDialog.push_back("You can't lead a fire sprit!");
	dialog2.entireDialog.push_back("That's my job! Ember's job!");
	dialog2.entireDialog.push_back("Ember has got this, Just follow me!");
	dialog2.position = { 200, 200 };
	dialog2.audioClip = engine.audioManager.GetAudio("Click");

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

		if (engine.inputSystem.GetButtonDown(SDL_SCANCODE_1))
		{
			dialog.activated = true;
		}
		if (engine.inputSystem.GetButtonDown(SDL_SCANCODE_2))
		{
			dialog2.activated = true;
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