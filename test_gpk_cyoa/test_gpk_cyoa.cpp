
#include "test_gpk_cyoa.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_encoding.h"
#include "gpk_json_expression.h"
#include "gpk_storage.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Choose Your Own Adventure");

::gpk::error_t					resizeControlIdArray				(::gpk::SGUI & gui, ::gpk::array_pod<int32_t> & idArray, uint32_t newSize)						{
	for(uint32_t iButton = newSize; iButton < idArray.size(); ++iButton) {
		::gpk::controlDelete(gui, idArray[iButton]);
		idArray.remove(iButton--);
	}
	
	for(uint32_t iButton = idArray.size(); iButton < newSize; ++iButton) {
		idArray.push_back(::gpk::controlCreate(gui));
	}
	return 0;
}

::gpk::error_t					setupGUIForPage						(::gme::SApplication & app, const ::rgbg::SPage & page)						{
	::gpk::SFramework					& framework							= app.Framework;
	::gpk::SWindow						& mainWindow						= framework.MainDisplay;
	::gpk::SGUI							& gui								= *framework.GUI;

	::resizeControlIdArray(gui, app.IdJumps, page.PageJumps.size());
	::resizeControlIdArray(gui, app.IdLines, page.TextLines.size());
	
	{
		::gpk::SControl							controlExit							= gui.Controls.Controls		[app.IdExit];
		::gpk::SControlText						controlExitText						= gui.Controls.Text			[app.IdExit];
		::gpk::SControlConstraints				controlExitConstraints				= gui.Controls.Constraints	[app.IdExit];
		for(uint32_t iButton = 0; iButton < page.PageJumps.size(); ++iButton) {
			const int32_t							idJump								= app.IdJumps[iButton];
			gui.Controls.Controls	[idJump]	= controlExit;
			gui.Controls.Constraints[idJump]	= controlExitConstraints;
			gui.Controls.Text		[idJump]	= controlExitText;

			::gpk::SControl							& controlJump						= gui.Controls.Controls[idJump];
			controlJump.Area.Size.x				= (int16_t)mainWindow.Size.x >> 1;
			controlJump.Area.Offset.y			= (int16_t)(controlJump.Area.Size.y * iButton);
			controlJump.Border					= {1, 1, 1, 1};
			controlJump.Align					= ::gpk::ALIGN_CENTER_BOTTOM;
		
			::gpk::SControlText						& controlText						= gui.Controls.Text[idJump];
			controlText.Text					= ::gpk::vcs{page.PageJumps[page.PageJumps.size() - 1 - iButton].Text.data(), (uint32_t)-1};
			controlText.Align					= ::gpk::ALIGN_CENTER;

			::gpk::controlSetParent(gui, idJump, -1);
		}
	}


	{
		const ::gpk::SControl					& controlExit						= gui.Controls.Controls		[app.IdExit];
		const ::gpk::SControlText				& controlExitText					= gui.Controls.Text			[app.IdExit];
		const ::gpk::SControlConstraints		& controlExitConstraints			= gui.Controls.Constraints	[app.IdExit];
		for(uint32_t iButton = 0; iButton < page.TextLines.size(); ++iButton) {
			const int32_t								idLine								= app.IdLines[iButton];
			gui.Controls.Controls	[idLine]		= controlExit;
			gui.Controls.Constraints[idLine]		= controlExitConstraints;
			gui.Controls.Text		[idLine]		= controlExitText;
		
			::gpk::SControl								& controlJump						= gui.Controls.Controls[idLine];
			controlJump.Area.Size.x					= (int16_t)mainWindow.Size.x;
			controlJump.Area.Offset.y				= (int16_t)(controlJump.Area.Size.y * iButton);
			controlJump.Border						= {};
			controlJump.Align						= ::gpk::ALIGN_TOP_LEFT;
			
			::gpk::SControlText							& controlText						= gui.Controls.Text[idLine];
			controlText.Text						= ::gpk::vcs{page.TextLines[iButton].data(), (uint32_t)-1};
			controlText.Align						= ::gpk::ALIGN_CENTER;
		
			::gpk::SControlConstraints					& controlConstraints				= gui.Controls.Constraints[idLine];
			controlConstraints.AttachSizeToControl	= {idLine, -1};
		
			::gpk::controlSetParent(gui, idLine, -1);
		}
	}
	::gpk::guiUpdateMetrics(gui, mainWindow.Size, true);
	return 0;
}


			::gpk::error_t						cleanup								(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.MainDisplay); }
			::gpk::error_t						setup								(::gme::SApplication & app)						{
	::gpk::SFramework									& framework							= app.Framework;
	::gpk::SWindow										& mainWindow						= framework.MainDisplay;
	mainWindow.Size									= {1280, 720};
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window. %s.", "why?!");
	{ // Build the exit button
		::gpk::SGUI											& gui								= *framework.GUI;
		gui.ColorModeDefault							= ::gpk::GUI_COLOR_MODE_3D;
		gui.ThemeDefault								= ::gpk::ASCII_COLOR_DARKGREEN * 16 + 8;
		app.IdExit										= ::gpk::controlCreate(gui);
		::gpk::SControl										& controlExit						= gui.Controls.Controls[app.IdExit];
		controlExit.Area								= {{}, {64, 20}};
		controlExit.Border								= {10, 10, 10, 10};
		controlExit.Margin								= {1, 1, 1, 1};
		controlExit.Align								= ::gpk::ALIGN_BOTTOM_RIGHT;
		::gpk::SControlText									& controlText						= gui.Controls.Text[app.IdExit];
		controlText.Text								= "Exit";
		controlText.Align								= ::gpk::ALIGN_CENTER;
		//::gpk::SControlConstraints							& controlConstraints				= gui.Controls.Constraints[app.IdExit];
		//controlConstraints.AttachSizeToControl			= {app.IdExit, -1};
		::gpk::controlSetParent(gui, app.IdExit, -1);
	}

	//{
	//	::gpk::SPNGData										pngDataCacheForFasterLoad;
	//	::gpk::view_const_string							pathPNGSuite						= {};
	//
	//}
	

	app.CYOA.Pages.resize(1);
	app.PNGImages.resize(app.CYOA.Pages.size());
	::rgbg::loadPage("test_story", app.CYOA.Pages[0], 0);

	::setupGUIForPage(app, app.CYOA.Pages[0]);

	::gpk::SPNGData										pngDataCacheForFasterLoad;
	char					fileName[4096]	= {};
	char					folderName[4096]	= "test_story";
	sprintf_s(fileName, "%s/%u.png", folderName, app.CYOA.CurrentPage);
	gerror_if(errored(::gpk::pngFileLoad(pngDataCacheForFasterLoad, ::gpk::vcs{fileName}, app.PNGImages[app.CYOA.CurrentPage])), "Failed to load file: %s.", fileName);

	return 0;
}

			::gpk::error_t											update						(::gme::SApplication & app, bool exitSignal)	{
	//::gpk::STimer															timer;
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "%s", "Exit requested by runtime.");
	{
		::gpk::mutex_guard														lock						(app.LockRender);
		app.Framework.MainDisplayOffscreen									= app.Offscreen;
	}
	::gpk::SFramework														& framework					= app.Framework;
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "%s", "Exit requested by framework update.");

	::gpk::SGUI																& gui						= *framework.GUI;
	::gpk::array_pod<uint32_t>												controlsToProcess			= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);
	for(uint32_t iControl = 0, countControls = controlsToProcess.size(); iControl < countControls; ++iControl) {
		const uint32_t															idControl					= controlsToProcess		[iControl];
		const ::gpk::SControlState												& controlState				= gui.Controls.States	[idControl];
		if(controlState.Execute) {
			info_printf("Executed %u.", idControl);
			if(idControl == (uint32_t)app.IdExit)
				return 1;

			const int32_t				indexJump			= ::gpk::find((int32_t)idControl, ::gpk::view_array<const int32_t>{app.IdJumps});
			if(0 <= indexJump) {
				const ::rgbg::SPage			& currentPage		= app.CYOA.Pages[app.CYOA.CurrentPage];
				app.CYOA.CurrentPage	= currentPage.PageJumps[currentPage.PageJumps.size() - 1 - indexJump].Jump;

				if(app.CYOA.Pages.size() <= app.CYOA.CurrentPage) {
					app.CYOA.Pages.resize(app.CYOA.CurrentPage + 1);
					app.PNGImages.resize(app.CYOA.Pages.size());
				}

				::rgbg::SPage				& newPage			= app.CYOA.Pages[app.CYOA.CurrentPage];
				ree_if(errored(::rgbg::loadPage("test_story", newPage, app.CYOA.CurrentPage)), "%s", "");
				::setupGUIForPage(app, newPage);
				{
					::gpk::SPNGData				pngDataCacheForFasterLoad;
					char						fileName	[4096]	= {};
					char						folderName	[4096]	= "test_story";
					sprintf_s(fileName, "%s/%u.png", folderName, app.CYOA.CurrentPage);
					gerror_if(errored(::gpk::pngFileLoad(pngDataCacheForFasterLoad, ::gpk::vcs{fileName}, app.PNGImages[app.CYOA.CurrentPage])), "Failed to load file: %s.", fileName);
				}
			}
		}
	}
	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

			::gpk::error_t											draw					(::gme::SApplication & app)							{
	//::gpk::STimer															timer;
	(void)app;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>		target;
	target.create();
	target->resize(app.Framework.MainDisplay.Size, {}, 0xFFFFFFFFU);
	for(uint32_t y = 0; y < target->Color.View.metrics().y; y += 2)
	for(uint32_t x = 0; x < target->Color.View.metrics().x; x += 2) {
		target->Color.View[y][x]											= rand();
		target->Color.View[y][x].a											= 255;
	}

	::gpk::SCoord2<uint32_t>												position				= 
		{ (target->metrics().x >> 1) - (app.PNGImages[app.CYOA.CurrentPage].View.metrics().x >> 1)
		, uint16_t(20 + 20 * app.IdLines.size())//(target->metrics().y >> 1) - (app.PNGImages[app.CYOA.CurrentPage].View.metrics().y >> 1)
		};

	::gpk::grid_copy_blend(target->Color.View, app.PNGImages[app.CYOA.CurrentPage].View, position);

	{
		::gpk::mutex_guard														lock					(app.LockGUI);

		::gpk::guiDraw(*app.Framework.GUI, target->Color.View);

		if(target->metrics().x > 1590)
			app.Framework.GUI->SelectedFont		= 9;
		else if(target->metrics().x > 1280)
			app.Framework.GUI->SelectedFont		= 8;

		app.Framework.GUI->FontCharSize		= app.Framework.GUI->Fonts[app.Framework.GUI->SelectedFont].CharSize.Cast<uint16_t>();
		app.Framework.GUI->FontTexture		= app.Framework.GUI->Fonts[app.Framework.GUI->SelectedFont].Texture;
	}
	{
		::gpk::mutex_guard														lock					(app.LockRender);
		app.Offscreen														= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}
