#include "application.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_encoding.h"
#include "gpk_json_expression.h"
#include "gpk_storage.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

			::gpk::error_t											cleanup								(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.MainDisplay); }
			::gpk::error_t											setup								(::gme::SApplication & app)						{
	::gpk::SFramework														& framework							= app.Framework;
	::gpk::SDisplay															& mainWindow						= framework.MainDisplay;
	framework.Input.create();
	mainWindow.Size														= {1280, 720};
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window. %s.", "why?????!?!?!?!?");
	{ // Build the exit button
		::gpk::SGUI																& gui								= *framework.GUI;
		gui.ColorModeDefault												= ::gpk::GUI_COLOR_MODE_3D;
		gui.ThemeDefault													= ::gpk::ASCII_COLOR_DARKGREEN * 16 + 7;
		app.IdExit															= ::gpk::controlCreate(gui);
		::gpk::SControl															& controlExit						= gui.Controls.Controls[app.IdExit];
		controlExit.Area													= {{}, {64, 20}};
		controlExit.Border													= {10, 10, 10, 10};
		controlExit.Margin													= {1, 1, 1, 1};
		controlExit.Align													= ::gpk::ALIGN_BOTTOM_RIGHT;
		::gpk::SControlText														& controlText						= gui.Controls.Text[app.IdExit];
		controlText.Text													= "Exit";
		controlText.Align													= ::gpk::ALIGN_CENTER;
		::gpk::SControlConstraints												& controlConstraints				= gui.Controls.Constraints[app.IdExit];
		controlConstraints.AttachSizeToControl								= {app.IdExit, -1};
		::gpk::controlSetParent(gui, app.IdExit, -1);
	}

	{
		app.PNGImages.resize(::gpk::size(filenames));
		::gpk::SPNGData															pngDataCacheForFasterLoad;
		::gpk::view_const_string												pathPNGSuite						= {};
		{
			const ::gpk::SJSONReader												& jsonReader						= framework.JSONConfig.Reader;
			gpk_necall(::gpk::jsonExpressionResolve("assets.pngsuite.path", jsonReader, 0, pathPNGSuite), "Failed to get path of PNG files! Last contents found: %s.", pathPNGSuite.begin());
			info_printf("Path to PNG test files: %s.", pathPNGSuite.begin());
			::gpk::view_const_string												fileNamePNG							= {};
			const int32_t															indexJSONNodeArrayPNGFileNames		= ::gpk::jsonExpressionResolve("application.gpk_test_png.images", jsonReader, 0, fileNamePNG);
			const uint32_t															countFilesToLoad					= (uint32_t)::gpk::jsonArraySize(*jsonReader.Tree[indexJSONNodeArrayPNGFileNames]);
			gpk_necall(app.PNGImages.resize(countFilesToLoad), "Failed to resize array for %u PNG files.", countFilesToLoad);
			::gpk::array_pod<char_t>												expression							= {};
			::gpk::array_pod<char_t>												fullPathPNG							= {};
			char																	subscriptExpression	[64]			= {};
			for(uint32_t iFile = 0; iFile < countFilesToLoad; ++iFile) {
				const uint32_t															lenExpression						= sprintf_s(subscriptExpression, "['%u']", iFile);
				::gpk::jsonExpressionResolve({subscriptExpression, lenExpression}, jsonReader, indexJSONNodeArrayPNGFileNames, fileNamePNG);
				fullPathPNG.clear();
				::gpk::pathNameCompose(pathPNGSuite, fileNamePNG, fullPathPNG);
				gerror_if(errored(::gpk::pngFileLoad(pngDataCacheForFasterLoad, {fullPathPNG.begin(), fullPathPNG.size()}, app.PNGImages[iFile])), "Failed to load file: %s.", fullPathPNG.begin());
			}
		}
		{
			// ---- Test our recently developed RLE algorithm.
			::gpk::array_pod<uint32_t>												sizesUncompressed;
			::gpk::array_pod<byte_t>												rleBuffer;
			::gpk::array_pod<uint32_t>												sizesRLE;
			uint32_t																sizeTotalUncompressed				= 0;
			uint32_t																sizeTotalRLE						= 0;
			for(uint32_t iFile = 0, countFilesToLoad = app.PNGImages.size(); iFile < countFilesToLoad; ++iFile) {
				::gpk::view_array<::gpk::SColorBGRA>	viewToRLE{app.PNGImages[iFile].View.begin(), app.PNGImages[iFile].View.metrics().x * app.PNGImages[iFile].View.metrics().y};
				sizesUncompressed.push_back(viewToRLE.size());
				::gpk::rleEncode(viewToRLE, rleBuffer);
				sizesRLE.push_back(rleBuffer.size());
				const uint32_t															sizePNGInBytes			= viewToRLE.size() * sizeof(::gpk::SColorBGRA);
				info_printf("--- RLE compression stats:"
					"\nsizePNGRLE          : %u"
					"\nsizePNGUncompressed : %u"
					"\nratio               : %f"
					, rleBuffer.size()
					, sizePNGInBytes
					, (float)rleBuffer.size() / sizePNGInBytes
					);
				sizeTotalRLE														+= rleBuffer.size();
				sizeTotalUncompressed												+= sizePNGInBytes;
				rleBuffer.clear();
			}
			info_printf("--- RLE compression stats:"
				"\nsizeTotalRLE          : %u"
				"\nsizeTotalUncompressed : %u"
				"\nratio                 : %f"
				, sizeTotalRLE
				, sizeTotalUncompressed
				, (float)sizeTotalRLE / sizeTotalUncompressed
				);
		}
	}
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
		uint32_t																idControl					= controlsToProcess		[iControl];
		const ::gpk::SControlState												& controlState				= gui.Controls.States	[idControl];
		if(controlState.Execute) {
			info_printf("Executed %u.", idControl);
			if(idControl == (uint32_t)app.IdExit)
				return 1;
		}
	}
	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

			::gpk::error_t											draw					(::gme::SApplication & app)							{
	//::gpk::STimer															timer;
	app;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>		target;
	target.create();
	target->resize(app.Framework.MainDisplay.Size, {}, 0xFFFFFFFFU);
	for(uint32_t y = 0; y < target->Color.View.metrics().y; ++y)
	for(uint32_t x = 0; x < target->Color.View.metrics().x; ++x) {
		target->Color.View[y][x]											= rand();
		target->Color.View[y][x].a											= 255;
	}


	for(uint32_t iFile = 0; iFile < app.PNGImages.size(); ++iFile) {
		const uint32_t															offsetX					= (iFile * 64);
		::gpk::SCoord2<uint32_t>												position				= {offsetX % (target->Color.View.metrics().x - 64), offsetX / (target->Color.View.metrics().x - 64) * 64};
		::gpk::grid_copy_blend(target->Color.View, app.PNGImages[iFile].View, position);
		//::gpk::grid_scale_alpha(target->Color.View, app.PNGImages[iFile].View, position.Cast<int32_t>(), app.PNGImages[iFile].View.metrics().Cast<int32_t>() * (1 + (.01 * iFile)));
	}

	//::gpk::array_pod<ubyte_t>												bytesPNG				= 0;
	//::gpk::pngFileWrite(target->Color.View, bytesPNG);
	//FILE																	* fp					= 0;
	//fopen_s(&fp, "png_test_generated.png", "wb");
	//if(fp) {
	//	fwrite(bytesPNG.begin(), 1, bytesPNG.size(), fp);
	//	fclose(fp);
	//}

	//::gpk::clearTarget(*target);
	{
		::gpk::mutex_guard														lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(*app.Framework.GUI, 0, target->Color.View);
	}
	{
		::gpk::mutex_guard														lock					(app.LockRender);
		app.Offscreen														= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}
