#include "gpk_the_one.h"

::gpk::error_t		the1::theOneSetup		(::the1::STheOne & app, the1::POOL_GAME_MODE mode) { 
	gpk_necs(::the1::poolGameSetup(app.MainGame.Game, mode));
	//for(uint32_t iGame = 0; iGame < app.TestGames.size(); ++iGame) {
	//	gpk_necs(::the1::poolGameSetup(app.TestGames[iGame].Game, the1::POOL_GAME_MODE_Test2Balls));
	//}
	for(uint32_t iBall = 0; iBall < app.MainGame.Game.StartState.BallCount; ++iBall) {
		app.MainGame.CameraBalls[iBall].Position	= app.MainGame.CameraPlayer.Position;
		app.MainGame.Game.GetBallPosition(iBall, app.MainGame.CameraBalls[iBall].Target);
	}
	return 0; 
}

::gpk::error_t		the1::theOneUpdate		(::the1::STheOne & app, double secondsElapsed) { 
	::the1::poolGameUpdate(app.MainGame.Game, secondsElapsed);
	for(uint32_t iBall = 0; iBall < app.MainGame.Game.StartState.BallCount; ++iBall) {
		app.MainGame.Game.GetBallPosition(iBall, app.MainGame.CameraBalls[app.MainGame.Game.StartState.BallOrder[iBall]].Target);
		app.MainGame.CameraBalls[app.MainGame.Game.StartState.BallOrder[iBall]].Target /= 2;
	}
	return 0; 
}

::gpk::error_t		the1::theOneDraw		(::the1::STheOne & app, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t> & backBuffer, double totalSeconds) { 
	const ::the1::SCamera	& camera				= app.MainGame.Camera ? app.MainGame.CameraBalls[app.MainGame.Camera - 1] : app.MainGame.CameraPlayer;
	::the1::poolGameDraw(app.MainGame.Game, backBuffer, camera.Position, camera.Target, {0, 1, 0}, totalSeconds);
	return 0; 
}

