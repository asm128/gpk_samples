// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"

#include "gpk_bitmap_target.h"
#include "gpk_bmg.h"
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

static				::gpk::error_t										updateSizeDependentResources				(::SApplication& applicationInstance)											{ 
	//static constexpr	const ::gpk::SCoord2<uint32_t>							GAME_SCREEN_SIZE							= {640, 360};
	::gpk::updateSizeDependentTarget(applicationInstance.Framework.MainDisplayOffscreen->Color.Texels, applicationInstance.Framework.MainDisplayOffscreen->Color.View, GAME_SCREEN_SIZE);
	return 0;
}

// --- Cleanup application resources.
					::gpk::error_t										cleanup										(::SApplication& applicationInstance)											{
	
	applicationInstance;
	g_ApplicationInstance													= 0;
	//error_printf("Error message test. Press F5 to continue if the debugger breaks execution at this point.");
	return 0;
}

static				::gpk::error_t										setupSprite									(::gpk::SImageProcessable<::gpk::SColorBGRA>& textureToProcess, ::gpk::SCoord2<int32_t>& textureCenter, const ::gpk::view_const_string& filename)	{ 
	bool																		failedFromFile								= errored(::gpk::pngFileLoad(filename, textureToProcess.Original));
	char_t																		filenameBMGCompo	[256]					= {};
	strcpy_s(filenameBMGCompo, filename.begin());
	filenameBMGCompo[strlen(filenameBMGCompo) - 3]							= 'b';
	filenameBMGCompo[strlen(filenameBMGCompo) - 2]							= 'm';
	filenameBMGCompo[strlen(filenameBMGCompo) - 1]							= 'g';
	const ::gpk::view_const_string												filenameBMG									= filenameBMGCompo;
	if(failedFromFile) {
		error_printf("Failed to load bitmap from file: %s.", filename);
		gerror_if(errored(::gpk::bmgFileLoad(filenameBMG, textureToProcess.Original)), "Failed to load image from disk: %s.", filenameBMG.begin());
	} 
	else {
		gerror_if(errored(::gpk::bmgFileWrite(filenameBMG, textureToProcess.Original.View)), "Failed to store file on disk: %s.", filenameBMG.begin());
	}
	textureCenter															= (textureToProcess.Original.View.metrics() / 2).Cast<int32_t>();
	textureToProcess.Processed.View											= textureToProcess.Original.View;
	return 0;
}

static				::gpk::error_t										setupSprites								(::SApplication& app)											{ 
	::gpk::view_const_string												pathPNGSuite									= {};
	{
		const ::gpk::SJSONReader												& jsonReader									= app.Framework.ReaderJSONConfig;
		const int32_t															indexObjectConfig								= ::gpk::jsonArrayValueGet(*jsonReader.Tree[0], 0);	// Get the first JSON {object} found in the [document]
		gpk_necall(::gpk::jsonExpressionResolve("assets.images.path", jsonReader, indexObjectConfig, pathPNGSuite), "Failed to get path of image files! Last contents found: %s.", pathPNGSuite.begin());
		info_printf("Path to PNG test files: %s.", pathPNGSuite.begin());
		::gpk::view_const_string												fileNamePNG										= {};
		const int32_t															indexJSONNodeArrayPNGFileNames					= ::gpk::jsonExpressionResolve("application.test_gontrill.images", jsonReader, indexObjectConfig, fileNamePNG);
		const uint32_t															countFilesToLoad								= (uint32_t)::gpk::jsonArraySize(*jsonReader.Tree[indexJSONNodeArrayPNGFileNames]);
		::gpk::SPNGData															pngDataCacheForFasterLoad;
		::gpk::array_pod<char_t>												fullPathPNG										= {};
		char																	subscriptExpression	[64]						= {};
		for(uint32_t iFile = 0; iFile < ::gpk::min(countFilesToLoad, (uint32_t)GAME_TEXTURE_COUNT); ++iFile) {
			const uint32_t															lenExpression									= sprintf_s(subscriptExpression, "[%u]", iFile);	//application.test_gontrill.images[%u]
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

					::gpk::error_t										mainWindowCreate							(::gpk::SDisplay& mainWindow, HINSTANCE hInstance);
					::gpk::error_t										setup										(::SApplication& applicationInstance)											{ 
	//_CrtSetBreakAlloc(120);
	g_ApplicationInstance													= &applicationInstance;
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	gerror_if(errored(::gpk::mainWindowCreate(framework.MainDisplay, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?????!?!?!?!?");
	::setupParticles();
	ree_if	(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	ree_if	(errored(::setupSprites					(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	::gpk::view_grid<::gpk::SColorBGRA>											& fontAtlasView								= applicationInstance.Textures[GAME_TEXTURE_FONT_ATLAS].Processed.View;
	const ::gpk::SCoord2<uint32_t>												& fontAtlasMetrics							= fontAtlasView.metrics();
	for(uint32_t y = 0, yMax = fontAtlasMetrics.y; y < yMax; ++y) 
	for(uint32_t x = 0, xMax = fontAtlasMetrics.x; x < xMax; ++x) {
		::gpk::SColorBGRA															& curTexel									= fontAtlasView[y][x];
		if(curTexel.r == 0x00 && curTexel.g == 0x00 && curTexel.b == 0x00)
			curTexel																= {0xFF, 0x00, 0xFF, 0xFF};
	}

	::gpk::SCoord2<uint32_t>													metricsScreen								= framework.MainDisplayOffscreen->Color.View.metrics();
	::SGame																		& gameInstance								= applicationInstance.Game;
	for(uint32_t iShip = 0, shipCount = applicationInstance.Game.ShipsPlaying; iShip < shipCount; ++iShip) {
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
	applicationInstance.PSOffsetFromShipCenter								= {-applicationInstance.TextureCenters[GAME_TEXTURE_SHIP0].x};
	return 0;
}

					::gpk::error_t										drawBackground								(::SApplication& applicationInstance);	
					::gpk::error_t										drawShots									(::SApplication& applicationInstance);	
					::gpk::error_t										drawThrust									(::SApplication& applicationInstance);	
					::gpk::error_t										drawPowerups								(::SApplication& applicationInstance);	
					::gpk::error_t										drawShips									(::SApplication& applicationInstance);	
					::gpk::error_t										drawCrosshair								(::SApplication& applicationInstance);	
					::gpk::error_t										drawCollisions								(::SApplication& applicationInstance);	
					::gpk::error_t										draw										(::SApplication& applicationInstance)											{	
	gerror_if(errored(::drawBackground	(applicationInstance)), "Why??");	// --- Draw stars
	gerror_if(errored(::drawPowerups		(applicationInstance)), "Why??");	// --- Draw powerups
	gerror_if(errored(::drawShips		(applicationInstance)), "Why??");	// --- Draw ship
	gerror_if(errored(::drawCrosshair	(applicationInstance)), "Why??");	// --- Draw crosshair
	gerror_if(errored(::drawThrust		(applicationInstance)), "Why??");	// --- Draw propulsion engine
	gerror_if(errored(::drawShots		(applicationInstance)), "Why??");	// --- Draw lasers
	gerror_if(errored(::drawCollisions	(applicationInstance)), "Why??");	// --- Draw lasers

	static constexpr	const ::gpk::SCoord2<int32_t>							sizeCharCell								= {9, 16};
	uint32_t																	lineOffset									= 0;
	static	const ::gpk::view_const_string										textLine0									= "W: Up, S: Down, A: Left, D: Right";
	static	const ::gpk::view_const_string										textLine1									= "T: Shoot. Y: Thrust. U: Handbrake.";
	static	const ::gpk::view_const_string										textLine2									= "Press ESC to exit or P to (un)pause.";
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	::gpk::view_grid<::gpk::SColorBGRA>											& offscreenView								= framework.MainDisplayOffscreen->Color.View;
	::gpk::view_grid<::gpk::SColorBGRA>											& fontAtlasView								= applicationInstance.Textures[GAME_TEXTURE_FONT_ATLAS].Processed.View;
	const ::gpk::SCoord2<uint32_t>												& offscreenMetrics							= offscreenView.metrics();
	::gpk::textLineDrawAlignedFixedSizeLit(offscreenView, applicationInstance.TextureFontMonochrome.View, fontAtlasView.metrics(), lineOffset++, offscreenMetrics, sizeCharCell, textLine0, ::gpk::SColorBGRA{0, applicationInstance.Framework.FrameInfo.FrameNumber % 0xFF, 0xFFU, 0xFFU});	
	::gpk::textLineDrawAlignedFixedSizeLit(offscreenView, applicationInstance.TextureFontMonochrome.View, fontAtlasView.metrics(), lineOffset++, offscreenMetrics, sizeCharCell, textLine1, ::gpk::SColorBGRA{applicationInstance.Framework.FrameInfo.FrameNumber % 0xFFU, 0xFFU, 0, 0xFFU});	
	::gpk::textLineDrawAlignedFixedSizeRGBA(offscreenView, fontAtlasView, lineOffset = offscreenMetrics.y / 16 - 1, offscreenMetrics, sizeCharCell, textLine2);	
	::gpk::textLineDrawAlignedFixedSizeLit(offscreenView, applicationInstance.TextureFontMonochrome.View, fontAtlasView.metrics(), --lineOffset, offscreenMetrics, sizeCharCell, weaponProperties[applicationInstance.Game.Ships.Weapon[0].IndexProperties].Name, ::gpk::SColorBGRA{applicationInstance.Framework.FrameInfo.FrameNumber % 0xFFU, 0xFFU, 0, 0xFFU});	
	if(applicationInstance.Debugging) {
		::gpk::STimer																& timer										= framework.Timer;
		::gpk::SDisplay																& mainWindow								= framework.MainDisplay;
		char																		buffer		[512]							= {};
		int32_t																		lineLen										= sprintf_s(buffer, "[%u x %u]. Projecitle fx count: %u. Thrust fx count: %u."
			, mainWindow.Size.x, mainWindow.Size.y
			, applicationInstance.ParticleSystemProjectiles	.Instances.size()
			, applicationInstance.ParticleSystemThrust		.Instances.size()
			);
		::gpk::textLineDrawAlignedFixedSizeRGBA(offscreenView, fontAtlasView, --lineOffset, offscreenMetrics, sizeCharCell, {buffer, (uint32_t)lineLen});	
		lineLen																	= sprintf_s(buffer, "Stars fx count: %u. Debris fx count: %u. Projectiles fx count: %u."
			, applicationInstance.ParticleSystemStars		.Instances.size()
			, applicationInstance.ParticleSystemDebris		.Instances.size()
			, applicationInstance.ParticleSystemProjectiles	.Instances.size()
			);
		::gpk::textLineDrawAlignedFixedSizeRGBA(offscreenView, fontAtlasView, --lineOffset, offscreenMetrics, sizeCharCell, {buffer, (uint32_t)lineLen});	
		lineLen																	= sprintf_s(buffer, "Enemy count: %u. Projectile count: %u. Powerup count: %u."
			, applicationInstance.Game.CountEnemies
			, applicationInstance.Game.CountProjectiles
			, applicationInstance.Game.CountPowerups
			);
		::gpk::textLineDrawAlignedFixedSizeRGBA(offscreenView, fontAtlasView, --lineOffset, offscreenMetrics, sizeCharCell, {buffer, (uint32_t)lineLen});	
		lineLen																	= sprintf_s(buffer, "FPS: %g. Last frame seconds: %g.", 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
		::gpk::textLineDrawAlignedFixedSizeRGBA(offscreenView, fontAtlasView, --lineOffset, offscreenMetrics, sizeCharCell, {buffer, (uint32_t)lineLen});	
	}
	return 0;
}

					::gpk::error_t										removeDeadStuff								(::SApplication& applicationInstance);
					::gpk::error_t										updateInput									(::SApplication& applicationInstance);
					::gpk::error_t										updateShots									(::SApplication& applicationInstance, const ::gpk::view_array<::SApplication::TParticleSystem::TIntegrator::TParticle> & particleDefinitions);
					::gpk::error_t										updateSpawn									(::SApplication& applicationInstance, const ::gpk::view_array<::SApplication::TParticleSystem::TIntegrator::TParticle> & particleDefinitions);
					::gpk::error_t										updateShips									(::SApplication& applicationInstance);
					::gpk::error_t										updateEnemies								(::SApplication& applicationInstance);
					::gpk::error_t										updateParticles								(::SApplication& applicationInstance);
 					::gpk::error_t										updateGUI									(::SApplication& applicationInstance);
					::gpk::error_t										update										(::SApplication& applicationInstance, bool systemRequestedExit)					{ 
	retval_ginfo_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	::gpk::error_t																frameworkResult								= ::gpk::updateFramework(framework);
	ree_if	(errored(frameworkResult), "Unknown error.");
	rvi_if	(1, frameworkResult == 1, "Framework requested close. Terminating execution.");

	ree_if	(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	gerror_if(errored(::updateInput					(applicationInstance)), "Unknown error.");
	if(applicationInstance.Paused)
		return 0;

	// update background
	const float																	windDirection								= (float)(sin(framework.FrameInfo.Seconds.Total / 10.0) * .5 + .5);
	applicationInstance.ColorBackground.g									= (uint8_t)(windDirection * (applicationInstance.ColorBackground.b / 3.0));
	applicationInstance.ColorBackground.r									= (uint8_t)(windDirection * (applicationInstance.ColorBackground.b / 3.0));

	gerror_if(errored(::removeDeadStuff	(applicationInstance)), "Unknown error.");
	gerror_if(errored(::updateParticles	(applicationInstance)), "Unknown error.");
	gerror_if(errored(::updateSpawn		(applicationInstance, particleDefinitions)), "Unknown error.");
	gerror_if(errored(::updateShips		(applicationInstance)), "Unknown error.");
	gerror_if(errored(::updateEnemies	(applicationInstance)), "Unknown error.");
	gerror_if(errored(::updateShots		(applicationInstance, particleDefinitions)), "Unknown error.");
	gerror_if(errored(::updateGUI		(applicationInstance)), "Unknown error.");
	return 0;
}
