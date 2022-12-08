#include "gpk_the_one.h"

::gpk::error_t		the1::theOneSetup		(::the1::STheOne & app, the1::POOL_GAME_MODE mode) { 
	gpk_necs(::the1::poolGameSetup(app.MainGame.Game, mode));
	for(uint32_t iGame = 0; iGame < app.TestGames.size(); ++iGame) {
		gpk_necs(::the1::poolGameSetup(app.TestGames[iGame].Game, the1::POOL_GAME_MODE_Test2Balls));
	}
	return 0; 
}

::gpk::error_t		the1::theOneUpdate		(::the1::STheOne & app, double secondsElapsed) { 
	::the1::poolGameUpdate(app.MainGame.Game, secondsElapsed);
	return 0; 
}

::gpk::error_t		the1::theOneDraw		(::the1::STheOne & app, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t> & backBuffer, double totalSeconds) { 
	::the1::poolGameDraw(app.MainGame.Game, backBuffer, totalSeconds);
	return 0; 
}

