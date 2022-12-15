#include "gpk_the_one.h"

::gpk::error_t				the1::theOneSetup		(::the1::STheOne & app, the1::POOL_GAME_MODE mode) { 
	gpk_necs(::the1::poolGameSetup(app.MainGame.Game, mode));
	//for(uint32_t iGame = 0; iGame < app.TestGames.size(); ++iGame) {
	//	gpk_necs(::the1::poolGameSetup(app.TestGames[iGame].Game, the1::POOL_GAME_MODE_Test2Balls));
	//}
	::the1::SPlayerUI				& playerUI				= app.MainGame.PlayerUI[app.MainGame.CurrentPlayer];
	//::the1::SCamera				& camera				= playerUI.Cameras.Selected ? playerUI.Cameras.Balls[playerUI.Cameras.Selected - 1] : playerUI.Cameras.Free;

	for(uint32_t iBall = 0; iBall < app.MainGame.Game.StartState.BallCount; ++iBall) {
		playerUI.Cameras.Balls[iBall].Position				= playerUI.Cameras.Free.Position;
		playerUI.Cameras.Balls[iBall].BallLockAtPosition	= true;
		playerUI.Cameras.Balls[iBall].BallLockAtTarget		= true;
		app.MainGame.Game.GetBallPosition(iBall, playerUI.Cameras.Balls[iBall].Target);
	}
	return 0; 
}


static	::gpk::error_t		updateInput				(::the1::STheOne & app, double secondsElapsed, ::gpk::view_array<const uint8_t> keyStates) { 
	if(keyStates[VK_ADD]) 
		app.MainGame.TimeScale					+= (float)secondsElapsed;
	else if(keyStates[VK_SUBTRACT]) 
		app.MainGame.TimeScale					= ::gpk::max(0.f, app.MainGame.TimeScale - (float)secondsElapsed);
	else if(keyStates['T']) 
		app.MainGame.TimeScale					= 1;

	secondsElapsed				*= app.MainGame.TimeScale;

	bool							reverse					= keyStates[VK_SHIFT];
	float							scale					= 10.0f * (reverse ? -1 : 1);

	::the1::SPlayerUI				& playerUI				= app.MainGame.PlayerUI[app.MainGame.CurrentPlayer];

	if(keyStates[VK_CONTROL]) {
		if(keyStates['0']) 
			playerUI.Cameras.Selected	= 0;
		else if(keyStates['9']) 
			playerUI.Cameras.Selected	= 1;
		if(keyStates['8']) 
			playerUI.Cameras.Selected	= 9;
		else {
			if(keyStates[VK_MENU]) {
				for(uint32_t iBall = 1; iBall < app.MainGame.Game.StartState.BallCount / 2U; ++iBall) {
					 if(keyStates['0' + iBall]) {
						 playerUI.Cameras.Selected	= iBall + 1;
						 break;
					 }
				}
			}
			else {
				for(uint32_t iBall = app.MainGame.Game.StartState.BallCount / 2 + 1; iBall < app.MainGame.Game.StartState.BallCount; ++iBall) {
					 if(keyStates['0' + iBall - app.MainGame.Game.StartState.BallCount / 2]) {
						 playerUI.Cameras.Selected	= iBall + 1;
						 break;
					 }
				}
			}
		}
	}


	::the1::SCamera					& cameraSelected		= playerUI.Cameras.Selected ? playerUI.Cameras.Balls[playerUI.Cameras.Selected - 1] : playerUI.Cameras.Free;
	if(keyStates[VK_CONTROL]) {
		if(0 == playerUI.Cameras.Selected) {
				 if(keyStates['Z']) cameraSelected.Target.z += float(secondsElapsed * scale);
			else if(keyStates['X']) cameraSelected.Target.x += float(secondsElapsed * scale);
			else if(keyStates['Y']) cameraSelected.Target.y += float(secondsElapsed * scale);
		}
	}
	else{
			 if(keyStates['Z']) cameraSelected.Position.z += float(secondsElapsed * scale);
		else if(keyStates['X']) cameraSelected.Position.x += float(secondsElapsed * scale);
		else if(keyStates['Y']) cameraSelected.Position.y += float(secondsElapsed * scale);

			 if(keyStates['R']) ::the1::poolGameReset(app.MainGame.Game, app.MainGame.Game.StartState.Mode);
		else if(keyStates['8']) ::the1::poolGameReset(app.MainGame.Game, ::the1::POOL_GAME_MODE_8Ball);
		else if(keyStates['9']) ::the1::poolGameReset(app.MainGame.Game, ::the1::POOL_GAME_MODE_9Ball);
		else if(keyStates['0']) ::the1::poolGameReset(app.MainGame.Game, ::the1::POOL_GAME_MODE_10Ball);
		else if(keyStates['2']) ::the1::poolGameReset(app.MainGame.Game, ::the1::POOL_GAME_MODE_Test2Balls);
	}

	return 0;
}

::gpk::error_t				the1::theOneUpdate		(::the1::STheOne & app, double secondsElapsed, ::gpk::view_array<const uint8_t> keyStates) { 
	::updateInput(app, secondsElapsed, keyStates);
	::the1::poolGameUpdate(app.MainGame.Game, secondsElapsed * app.MainGame.TimeScale);

	::the1::SPlayerUI				& playerUI				= app.MainGame.PlayerUI[app.MainGame.CurrentPlayer];
	for(uint32_t iBall = 0; iBall < app.MainGame.Game.StartState.BallCount; ++iBall) {
		::the1::SCamera					& cameraBall			= playerUI.Cameras.Balls[iBall];
		if(0 == iBall) {
			app.MainGame.Game.GetBallPosition(iBall, cameraBall.Target);
			cameraBall.Target			/= 2;		
		}
		else {
			app.MainGame.Game.GetBallPosition(0, cameraBall.Target);
			app.MainGame.Game.GetBallPosition(iBall, cameraBall.Position);
			auto							distance				=  cameraBall.Target - cameraBall.Position;
			auto							direction				=  ::gpk::SCoord3<float>{distance}.Normalize();
			cameraBall.Position			+= direction * -30;
			cameraBall.Position.y		+= 20;
		}
	}

	return 0; 
}

::gpk::error_t				the1::theOneDraw		(::the1::STheOne & app, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t> & backBuffer, double totalSeconds) { 
	const ::the1::SPlayerUI			& playerUI				= app.MainGame.PlayerUI[app.MainGame.CurrentPlayer];
	const ::the1::SCamera			& camera				= playerUI.Cameras.Selected ? playerUI.Cameras.Balls[playerUI.Cameras.Selected - 1] : playerUI.Cameras.Free;
	::the1::poolGameDraw(app.MainGame.Game, backBuffer, camera.Position, camera.Target, {0, 1, 0}, totalSeconds);
	return 0; 
}

