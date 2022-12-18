// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"

#include "gpk_bitmap_target.h"
#include "gpk_grid_scale.h"
#include "gpk_grid_copy.h"
#include "gpk_view_bit.h"
#include "gpk_gui_text.h"
#include "gpk_png.h"
#include "gpk_json_expression.h"
#include "gpk_storage.h"

#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "Gontrill v0.1");

static ::SApplication::TParticleSystem::TIntegrator::TParticle			particleDefinitions	[::PARTICLE_TYPE_COUNT]	= {};

static				void												setupParticles								()																				{
	particleDefinitions	[::PARTICLE_TYPE_PROJECTILE		].Position				=
	particleDefinitions	[::PARTICLE_TYPE_SHIP_THRUST	].Position				=
	particleDefinitions	[::PARTICLE_TYPE_STAR			].Position				= {};

	particleDefinitions	[::PARTICLE_TYPE_PROJECTILE		].SetMass				(1);
	particleDefinitions	[::PARTICLE_TYPE_SHIP_THRUST	].SetMass				(1);
	particleDefinitions	[::PARTICLE_TYPE_STAR			].SetMass				(1);

	particleDefinitions	[::PARTICLE_TYPE_PROJECTILE		].Damping				= 1.0f;
	particleDefinitions	[::PARTICLE_TYPE_SHIP_THRUST	].Damping				= 0.80f;
	particleDefinitions	[::PARTICLE_TYPE_STAR			].Damping				= 1.0f;

	particleDefinitions	[::PARTICLE_TYPE_PROJECTILE		].Forces.Velocity		= {};
	particleDefinitions	[::PARTICLE_TYPE_SHIP_THRUST	].Forces.Velocity		= {};
	particleDefinitions	[::PARTICLE_TYPE_STAR			].Forces.Velocity		= {};
}

					::SApplication										* g_ApplicationInstance						= 0;

static				::gpk::error_t										updateSizeDependentResources				(::SApplication& app)											{
	//static constexpr	const ::gpk::SCoord2<uint32_t>							GAME_SCREEN_SIZE							= {640, 360};
	::gpk::updateSizeDependentTarget(app.Framework.BackBuffer->Color.Texels, app.Framework.BackBuffer->Color.View, GAME_SCREEN_SIZE);
	return 0;
}

// --- Cleanup application resources.
					::gpk::error_t										cleanup										(::SApplication& app)											{

	app;
	g_ApplicationInstance													= 0;
	//error_printf("Error message test. Press F5 to continue if the debugger breaks execution at this point.");
	return 0;
}

static				::gpk::error_t										setupSprite									(::gpk::SImageProcessable<::gpk::SColorBGRA>& textureToProcess, ::gpk::SCoord2<int32_t>& textureCenter, const ::gpk::view_const_string& filename)	{
	gpk_necall(::gpk::pngFileLoad(filename, textureToProcess.Original), "%s", "Failed to load sprite image.");
	textureCenter															= (textureToProcess.Original.View.metrics() / 2).Cast<int32_t>();
	textureToProcess.Processed.View											= textureToProcess.Original.View;
	return 0;
}

static				::gpk::error_t										setupSprites								(::SApplication& app)											{
	::gpk::view_const_string												pathPNGSuite									= {};
	{
		const ::gpk::SJSONReader												& jsonReader									= app.Framework.JSONConfig.Reader;
		gpk_necall(::gpk::jsonExpressionResolve(::gpk::vcs{"assets.images.path"}, jsonReader, 0, pathPNGSuite), "Failed to get path of image files! Last contents found: %s.", pathPNGSuite.begin());
		info_printf("Path to PNG test files: %s.", ::gpk::toString(pathPNGSuite).begin());
		::gpk::view_const_string												fileNamePNG										= {};
		const int32_t															indexJSONNodeArrayPNGFileNames					= ::gpk::jsonExpressionResolve(::gpk::vcs{"application.test_gontrill.images"}, jsonReader, 0, fileNamePNG);
		const uint32_t															countFilesToLoad								= (uint32_t)::gpk::jsonArraySize(*jsonReader.Tree[indexJSONNodeArrayPNGFileNames]);
		::gpk::SPNGData															pngDataCacheForFasterLoad;
		::gpk::array_pod<char_t>												fullPathPNG										= {};
		char																	subscriptExpression	[64]						= {};
		for(uint32_t iFile = 0; iFile < ::gpk::min(countFilesToLoad, (uint32_t)GAME_TEXTURE_COUNT); ++iFile) {
			const uint32_t															lenExpression									= sprintf_s(subscriptExpression, "['%u']", iFile);	//application.test_gontrill.images[%u]
			::gpk::jsonExpressionResolve({subscriptExpression, lenExpression}, jsonReader, indexJSONNodeArrayPNGFileNames, fileNamePNG);
			fullPathPNG.clear();
			::gpk::pathNameCompose(pathPNGSuite, fileNamePNG, fullPathPNG);
			::setupSprite(app.Textures[iFile], app.TextureCenters[iFile], {fullPathPNG.begin(), fullPathPNG.size()});
		}
	}

	const ::gpk::view_grid<::gpk::SColorBGRA>									& fontAtlas									= app.Textures[GAME_TEXTURE_FONT_ATLAS].Processed.View;
	const ::gpk::SCoord2<uint32_t>												& textureFontMetrics						= fontAtlas.metrics();
	app.TextureFontMonochrome.resize(textureFontMetrics);
	for(uint32_t y = 0, yMax = textureFontMetrics.y; y < yMax; ++y)
	for(uint32_t x = 0, xMax = textureFontMetrics.x; x < xMax; ++x) {
		const ::gpk::SColorBGRA														& pixelToTest								= fontAtlas[y][x];
		app.TextureFontMonochrome.View[y * textureFontMetrics.x + x]
		=	0 != pixelToTest.r
		||	0 != pixelToTest.g
		||	0 != pixelToTest.b
		;
	}
	app.StuffToDraw.TexturesPowerup0.push_back(app.Textures[GAME_TEXTURE_POWCORESQUARE		].Processed.View);
	app.StuffToDraw.TexturesPowerup0.push_back(app.Textures[GAME_TEXTURE_POWICON			].Processed.View);
	app.StuffToDraw.TexturesPowerup1.push_back(app.Textures[GAME_TEXTURE_POWCOREDIAGONAL	].Processed.View);
	app.StuffToDraw.TexturesPowerup1.push_back(app.Textures[GAME_TEXTURE_POWICON			].Processed.View);
	return 0;
}

					::gpk::error_t										mainWindowCreate							(::gpk::SWindow& mainWindow, HINSTANCE hInstance);
					::gpk::error_t										setup										(::SApplication& app)											{
	//_CrtSetBreakAlloc(120);
	g_ApplicationInstance													= &app;
	::gpk::SFramework															& framework									= app.Framework;
	framework.RootWindow.Size												= {1280, 720};
	gerror_if(errored(::gpk::mainWindowCreate(framework.RootWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?!");
	::setupParticles();
	ree_if	(errored(::updateSizeDependentResources	(app)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	ree_if	(errored(::setupSprites					(app)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	::gpk::view_grid<::gpk::SColorBGRA>											& fontAtlasView								= app.Textures[GAME_TEXTURE_FONT_ATLAS].Processed.View;
	const ::gpk::SCoord2<uint32_t>												& fontAtlasMetrics							= fontAtlasView.metrics();
	for(uint32_t y = 0, yMax = fontAtlasMetrics.y; y < yMax; ++y)
	for(uint32_t x = 0, xMax = fontAtlasMetrics.x; x < xMax; ++x) {
		::gpk::SColorBGRA															& curTexel									= fontAtlasView[y][x];
		if(curTexel.r == 0x00 && curTexel.g == 0x00 && curTexel.b == 0x00)
			curTexel																= {0xFF, 0x00, 0xFF, 0xFF};
	}

	::gpk::SCoord2<uint32_t>													metricsScreen								= framework.BackBuffer->Color.View.metrics();
	::SGame																		& gameInstance								= app.Game;
	for(uint32_t iShip = 0, shipCount = app.Game.ShipsPlaying; iShip < shipCount; ++iShip) {
		gameInstance.Ships.Alive		[iShip]									= 1;
		gameInstance.Ships.Position		[iShip]									= metricsScreen.Cast<float>() / 4 + ::gpk::SCoord2<float>{0, (float)iShip * 64};
		gameInstance.Ships.Weapon		[iShip]									= {(int32_t)iShip};
		gameInstance.Ships.Health		[iShip].Health							= 5000;
		gameInstance.Ships.Health		[iShip].Shield							= 5000;
		gameInstance.PositionCrosshair	[iShip]									= gameInstance.Ships.Position[iShip] + ::gpk::SCoord2<float>{96, };
		gameInstance.Powerups.Alive		[iShip]									= 1;
		gameInstance.Powerups.Position	[iShip]									= metricsScreen.Cast<float>() / 4 * 3 + ::gpk::SCoord2<float>{0, (float)iShip * 64};
		gameInstance.Powerups.Family	[iShip]									= (POWERUP_FAMILY)iShip;
		if(gameInstance.Powerups.Family	[iShip] == POWERUP_FAMILY_WEAPON)
			gameInstance.Powerups.Type		[iShip].TypeWeapon						= (WEAPON_TYPE)(rand() % WEAPON_TYPE_COUNT);
		else
			gameInstance.Powerups.Type		[iShip].TypeHealth						= (HEALTH_TYPE)(rand() % 2);
		++gameInstance.CountPowerups;
	}
	//gameInstance.PositionPowerup											= framework.Offscreen.View.metrics().Cast<float>() / 4U * 3U;
	app.PSOffsetFromShipCenter								= {-app.TextureCenters[GAME_TEXTURE_SHIP0].x};
	return 0;
}

					::gpk::error_t										drawBackground								(::SApplication& app);
					::gpk::error_t										drawShots									(::SApplication& app);
					::gpk::error_t										drawThrust									(::SApplication& app);
					::gpk::error_t										drawPowerups								(::SApplication& app);
					::gpk::error_t										drawShips									(::SApplication& app);
					::gpk::error_t										drawCrosshair								(::SApplication& app);
					::gpk::error_t										drawCollisions								(::SApplication& app);
					::gpk::error_t										draw										(::SApplication& app)											{
	gerror_if(errored(::drawBackground	(app)), "Why??");	// --- Draw stars
	gerror_if(errored(::drawPowerups	(app)), "Why??");	// --- Draw powerups
	gerror_if(errored(::drawShips		(app)), "Why??");	// --- Draw ship
	gerror_if(errored(::drawCrosshair	(app)), "Why??");	// --- Draw crosshair
	gerror_if(errored(::drawThrust		(app)), "Why??");	// --- Draw propulsion engine
	gerror_if(errored(::drawShots		(app)), "Why??");	// --- Draw lasers
	gerror_if(errored(::drawCollisions	(app)), "Why??");	// --- Draw debris particles

	static constexpr	const ::gpk::SCoord2<int32_t>							sizeCharCell								= {9, 16};
	uint32_t																	lineOffset									= 0;
	static	const ::gpk::view_const_string										textLine0									= "W: Up, S: Down, A: Left, D: Right";
	static	const ::gpk::view_const_string										textLine1									= "T: Shoot. Y: Thrust. U: Handbrake.";
	static	const ::gpk::view_const_string										textLine2									= "Press ESC to exit or P to (un)pause.";
	::gpk::SFramework															& framework									= app.Framework;
	::gpk::view_grid<::gpk::SColorBGRA>											& offscreenView								= framework.BackBuffer->Color.View;
	::gpk::view_grid<::gpk::SColorBGRA>											& fontAtlasView								= app.Textures[GAME_TEXTURE_FONT_ATLAS].Processed.View;
	const ::gpk::SCoord2<uint32_t>												& offscreenMetrics							= offscreenView.metrics();
	::gpk::textLineDrawAlignedFixedSizeLit(offscreenView, app.TextureFontMonochrome.View, fontAtlasView.metrics(), lineOffset++, offscreenMetrics, sizeCharCell, textLine0, ::gpk::SColorBGRA{0, app.Framework.FrameInfo.FrameNumber % 0xFF, 0xFFU, 0xFFU});
	::gpk::textLineDrawAlignedFixedSizeLit(offscreenView, app.TextureFontMonochrome.View, fontAtlasView.metrics(), lineOffset++, offscreenMetrics, sizeCharCell, textLine1, ::gpk::SColorBGRA{app.Framework.FrameInfo.FrameNumber % 0xFFU, 0xFFU, 0, 0xFFU});
	::gpk::textLineDrawAlignedFixedSizeRGBA(offscreenView, fontAtlasView, lineOffset = offscreenMetrics.y / 16 - 1, offscreenMetrics, sizeCharCell, textLine2);
	::gpk::textLineDrawAlignedFixedSizeLit(offscreenView, app.TextureFontMonochrome.View, fontAtlasView.metrics(), --lineOffset, offscreenMetrics, sizeCharCell, weaponProperties[app.Game.Ships.Weapon[0].IndexProperties].Name, ::gpk::SColorBGRA{app.Framework.FrameInfo.FrameNumber % 0xFFU, 0xFFU, 0, 0xFFU});
	if(app.Debugging) {
		::gpk::STimer																& timer										= framework.Timer;
		::gpk::SWindow																& mainWindow								= framework.RootWindow;
		char																		buffer		[512]							= {};
		int32_t																		lineLen										= sprintf_s(buffer, "[%u x %u]. Projecitle fx count: %u. Thrust fx count: %u."
			, mainWindow.Size.x, mainWindow.Size.y
			, app.ParticleSystemProjectiles	.Instances.size()
			, app.ParticleSystemThrust		.Instances.size()
			);
		::gpk::textLineDrawAlignedFixedSizeRGBA(offscreenView, fontAtlasView, --lineOffset, offscreenMetrics, sizeCharCell, {buffer, (uint32_t)lineLen});
		lineLen																	= sprintf_s(buffer, "Stars fx count: %u. Debris fx count: %u. Projectiles fx count: %u."
			, app.ParticleSystemStars		.Instances.size()
			, app.ParticleSystemDebris		.Instances.size()
			, app.ParticleSystemProjectiles	.Instances.size()
			);
		::gpk::textLineDrawAlignedFixedSizeRGBA(offscreenView, fontAtlasView, --lineOffset, offscreenMetrics, sizeCharCell, {buffer, (uint32_t)lineLen});
		lineLen																	= sprintf_s(buffer, "Enemy count: %u. Projectile count: %u. Powerup count: %u."
			, app.Game.CountEnemies
			, app.Game.CountProjectiles
			, app.Game.CountPowerups
			);
		::gpk::textLineDrawAlignedFixedSizeRGBA(offscreenView, fontAtlasView, --lineOffset, offscreenMetrics, sizeCharCell, {buffer, (uint32_t)lineLen});
		lineLen																	= sprintf_s(buffer, "FPS: %g. Last frame seconds: %g.", 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
		::gpk::textLineDrawAlignedFixedSizeRGBA(offscreenView, fontAtlasView, --lineOffset, offscreenMetrics, sizeCharCell, {buffer, (uint32_t)lineLen});
	}
	return 0;
}

					::gpk::error_t										removeDeadStuff								(::SApplication& app);
					::gpk::error_t										updateInput									(::SApplication& app);
					::gpk::error_t										updateShots									(::SApplication& app, const ::gpk::view_array<::SApplication::TParticleSystem::TIntegrator::TParticle> & particleDefinitions);
					::gpk::error_t										updateSpawn									(::SApplication& app, const ::gpk::view_array<::SApplication::TParticleSystem::TIntegrator::TParticle> & particleDefinitions);
					::gpk::error_t										updateShips									(::SApplication& app);
					::gpk::error_t										updateEnemies								(::SApplication& app);
					::gpk::error_t										updateParticles								(::SApplication& app);
 					::gpk::error_t										updateGUI									(::SApplication& app);
					::gpk::error_t										update										(::SApplication& app, bool systemRequestedExit)					{
	retval_ginfo_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");
	::gpk::SFramework															& framework									= app.Framework;
	::gpk::error_t																frameworkResult								= ::gpk::updateFramework(framework);
	ree_if	(errored(frameworkResult), "Unknown error.");
	rvi_if	(1, frameworkResult == 1, "Framework requested close. Terminating execution.");

	ree_if	(errored(::updateSizeDependentResources	(app)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	gerror_if(errored(::updateInput					(app)), "Unknown error.");
	if(app.Paused)
		return 0;

	// update background
	const float																	windDirection								= (float)(sin(framework.FrameInfo.Seconds.Total / 10.0) * .5 + .5);
	app.ColorBackground.g									= (uint8_t)(windDirection * (app.ColorBackground.b / 3.0));
	app.ColorBackground.r									= (uint8_t)(windDirection * (app.ColorBackground.b / 3.0));

	gerror_if(errored(::removeDeadStuff	(app)), "Unknown error.");
	gerror_if(errored(::updateParticles	(app)), "Unknown error.");
	gerror_if(errored(::updateSpawn		(app, particleDefinitions)), "Unknown error.");
	gerror_if(errored(::updateShips		(app)), "Unknown error.");
	gerror_if(errored(::updateEnemies	(app)), "Unknown error.");
	gerror_if(errored(::updateShots		(app, particleDefinitions)), "Unknown error.");
	gerror_if(errored(::updateGUI		(app)), "Unknown error.");
	return 0;
}
