// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "test_gpk_mesh.h"

#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_matrix.h"
#include "gpk_png.h"

#include "gpk_app_impl.h"
#include "gpk_bitmap_target.h"

static constexpr	const uint32_t										ASCII_SCREEN_WIDTH							= 132	;
static constexpr	const uint32_t										ASCII_SCREEN_HEIGHT							= 50	;

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "The One");

static				::gpk::error_t										updateSizeDependentResources				(::SApplication& app)											{
	const ::gpk::SCoord2<uint32_t>												newSize										= app.Framework.MainDisplay.Size;
	::gpk::updateSizeDependentTarget(app.Framework.MainDisplayOffscreen->Color, newSize);
	return 0;
}

// --- Cleanup application resources.
					::gpk::error_t										cleanup										(::SApplication& app)											{
	::gpk::SWindowPlatformDetail												& displayDetail								= app.Framework.MainDisplay.PlatformDetail;
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	::UnregisterClass(displayDetail.WindowClassName, displayDetail.WindowClass.hInstance);
	return 0;
}

					::gpk::error_t										setup										(::SApplication& app)											{
	::gpk::SFramework															& framework									= app.Framework;
	::gpk::SWindow																& mainWindow								= framework.MainDisplay;
	mainWindow.Size															= {1280, 720};
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?!");

	::the1::theOneSetup(app.TheOne);

	//app.EntityCamera					= app.Engine.CreateCamera	();
	//app.EntityLightDirectional		= app.Engine.CreateLight	(::gpk::LIGHT_TYPE_Directional	);
	//app.EntityLightPoint				= app.Engine.CreateLight	(::gpk::LIGHT_TYPE_Point		);
	//app.EntityLightSpot				= app.Engine.CreateLight	(::gpk::LIGHT_TYPE_Spot			);
	//app.EntityBox					= app.Engine.CreateBox		();
	//app.EntitySphere				= app.Engine.CreateSphere	();

	//if(-1 != app.EntityCamera				)	app.Engine.SetPosition(app.EntityCamera				, {0, 0, 0});
	//if(-1 != app.EntityLightDirectional		)	app.Engine.SetPosition(app.EntityLightDirectional	, {0, 0, 0});
	//if(-1 != app.EntityLightPoint			)	app.Engine.SetPosition(app.EntityLightPoint			, {0, 0, 0});
	//if(-1 != app.EntityLightSpot			)	app.Engine.SetPosition(app.EntityLightSpot			, {0, 0, 0});
	//if(-1 != app.EntityBox					)	app.Engine.SetPosition(app.EntityBox				, {0, 0.5f, 0});
	//if(-1 != app.EntitySphere				)	app.Engine.SetPosition(app.EntitySphere				, {0, 0.5f, 2});

	ree_if	(errored(::updateSizeDependentResources	(app)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	return 0;
}

::gpk::error_t										update										(::SApplication& app, bool systemRequestedExit)					{
	retval_ginfo_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");
	::gpk::error_t											frameworkResult								= ::gpk::updateFramework(app.Framework);
	ree_if(errored(frameworkResult), "Unknown error.");
	rvi_if(1, frameworkResult == 1, "Framework requested close. Terminating execution.");
	ree_if(errored(::updateSizeDependentResources(app)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	static float timeScale = 1;
	::gpk::SFramework									& framework									= app.Framework;
	::gpk::SFrameInfo									& frameInfo									= framework.FrameInfo;
	{
		::gpk::STimer										timer;
		::the1::theOneUpdate(app.TheOne, frameInfo.Seconds.LastFrame * timeScale);

		timer.Frame();
		info_printf("Update engine in %f seconds", timer.LastTimeSeconds);
	}

	//-----------------------------
	::gpk::STimer																& timer										= app.Framework.Timer;
	::gpk::SWindow																& mainWindow								= app.Framework.MainDisplay;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowTextA(windowHandle, buffer);


	bool					reverse					= app.Framework.Input->KeyboardCurrent.KeyState[VK_SHIFT];
	float					scale					= 10.0f * (reverse ? -1 : 1);


	if(app.Framework.Input->KeyboardCurrent.KeyState[VK_CONTROL]) {
		if(app.Framework.Input->KeyboardCurrent.KeyState['0']) 
			app.TheOne.MainGame.Camera = 0;
		else if(app.Framework.Input->KeyboardCurrent.KeyState['9']) 
			app.TheOne.MainGame.Camera = 1;
		if(app.Framework.Input->KeyboardCurrent.KeyState['8']) 
			app.TheOne.MainGame.Camera = 9;
		else {
			if(app.Framework.Input->KeyboardCurrent.KeyState[VK_MENU]) {
				for(uint32_t iBall = 1; iBall < app.TheOne.MainGame.Game.StartState.BallCount / 2; ++iBall) {
					 if(app.Framework.Input->KeyboardCurrent.KeyState['0' + iBall]) {
						 app.TheOne.MainGame.Camera = iBall + 1;
						 break;
					 }
				}
			}
			else {
				for(uint32_t iBall = app.TheOne.MainGame.Game.StartState.BallCount / 2 + 1; iBall < app.TheOne.MainGame.Game.StartState.BallCount; ++iBall) {
					 if(app.Framework.Input->KeyboardCurrent.KeyState['0' + iBall - app.TheOne.MainGame.Game.StartState.BallCount / 2]) {
						 app.TheOne.MainGame.Camera = iBall + 1;
						 break;
					 }
				}
			}
		}
	}


	::the1::SCamera				& camera				= app.TheOne.MainGame.Camera ? app.TheOne.MainGame.CameraBalls[app.TheOne.MainGame.Camera - 1] : app.TheOne.MainGame.CameraPlayer;
	if(app.Framework.Input->KeyboardCurrent.KeyState[VK_CONTROL]) {
		if(0 == app.TheOne.MainGame.Camera) {
				 if(app.Framework.Input->KeyboardCurrent.KeyState['Z']) camera.Target.z += float(frameInfo.Seconds.LastFrame * scale);
			else if(app.Framework.Input->KeyboardCurrent.KeyState['X']) camera.Target.x += float(frameInfo.Seconds.LastFrame * scale);
			else if(app.Framework.Input->KeyboardCurrent.KeyState['Y']) camera.Target.y += float(frameInfo.Seconds.LastFrame * scale);
		}
	}
	else{
			 if(app.Framework.Input->KeyboardCurrent.KeyState['Z']) camera.Position.z += float(frameInfo.Seconds.LastFrame * scale);
		else if(app.Framework.Input->KeyboardCurrent.KeyState['X']) camera.Position.x += float(frameInfo.Seconds.LastFrame * scale);
		else if(app.Framework.Input->KeyboardCurrent.KeyState['Y']) camera.Position.y += float(frameInfo.Seconds.LastFrame * scale);

			 if(app.Framework.Input->KeyboardCurrent.KeyState['R']) ::the1::poolGameReset(app.TheOne.MainGame.Game, app.TheOne.MainGame.Game.StartState.Mode);
		else if(app.Framework.Input->KeyboardCurrent.KeyState['8']) ::the1::poolGameReset(app.TheOne.MainGame.Game, ::the1::POOL_GAME_MODE_8Ball);
		else if(app.Framework.Input->KeyboardCurrent.KeyState['9']) ::the1::poolGameReset(app.TheOne.MainGame.Game, ::the1::POOL_GAME_MODE_9Ball);
		else if(app.Framework.Input->KeyboardCurrent.KeyState['0']) ::the1::poolGameReset(app.TheOne.MainGame.Game, ::the1::POOL_GAME_MODE_10Ball);
		else if(app.Framework.Input->KeyboardCurrent.KeyState['2']) ::the1::poolGameReset(app.TheOne.MainGame.Game, ::the1::POOL_GAME_MODE_Test2Balls);
	}
	if(app.Framework.Input->KeyboardCurrent.KeyState[VK_ADD]) 
		timeScale			+= .1f;
	if(app.Framework.Input->KeyboardCurrent.KeyState[VK_SUBTRACT]) 
		timeScale			= ::gpk::max(0.f, timeScale - .1f);
	
	return 0;
}

					::gpk::error_t										draw										(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework															& framework									= app.Framework;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>			backBuffer;
	backBuffer->resize(framework.MainDisplayOffscreen->Color.metrics(), 0xFF008000, (uint32_t)-1);

	::the1::theOneDraw(app.TheOne, *backBuffer, framework.FrameInfo.Seconds.Total);

	//memcpy(framework.MainDisplayOffscreen->Color.View.begin(), backBuffer->Color.View.begin(), backBuffer->Color.View.byte_count());
	::gpk::grid_mirror_y(framework.MainDisplayOffscreen->Color.View, backBuffer->Color.View);
	//framework.MainDisplayOffscreen = backBuffer;
	//------------------------------------------------
	return 0;
}

