#include "application.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_bitmap_file.h"

#include "gpk_dialog_controls.h"
//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

 GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

::gpk::error_t			cleanup		(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.RootWindow); }
::gpk::error_t			setup		(::gme::SApplication & app)						{
	::gpk::SFramework				& framework					= app.Framework;
	::gpk::SWindow					& mainWindow				= framework.RootWindow;
	framework.Settings.GUIZoom											= false;
	app.Framework.GUI													= app.DialogMain.GUI;
	app.DialogMain.Input												= mainWindow.Input;
	mainWindow.Size														= {1024, 768};
	es_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));
	::gpk::SGUI						& gui						= *framework.GUI;
	gui.ColorModeDefault												= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault													= ::gpk::ASCII_COLOR_DARKGREEN * 16 + 7;
	{
		app.IdExit															= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement															& controlExit								= gui.Controls.Placement[app.IdExit];
		controlExit.Area													= {{}, {64, 20}};
		controlExit.Border													= {10, 10, 10, 10};
		controlExit.Margin													= {1, 1, 1, 1};
		controlExit.Align													= ::gpk::ALIGN_CENTER_BOTTOM;
		::gpk::SControlText														& controlText								= gui.Controls.Text[app.IdExit];
		controlText.Text													= "Exit";
		controlText.Align													= ::gpk::ALIGN_CENTER;
		::gpk::controlSetParent(gui, app.IdExit, -1);
	}

	{
		app.IdFrameRateUpdate												= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement															& controlExit								= gui.Controls.Placement[app.IdFrameRateUpdate];
		controlExit.Area													= {{}, {384, 20}};
		controlExit.Border													= {10, 10, 10, 10};
		controlExit.Margin													= {1, 1, 1, 1};
		controlExit.Align													= ::gpk::ALIGN_BOTTOM_LEFT;
		::gpk::SControlText														& controlText								= gui.Controls.Text[app.IdFrameRateUpdate];
		controlText.Align													= ::gpk::ALIGN_CENTER;
		::gpk::controlSetParent(gui, app.IdFrameRateUpdate, -1);
	}

	{
		app.IdFrameRateRender												= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement															& controlExit								= gui.Controls.Placement[app.IdFrameRateRender];
		controlExit.Area													= {{}, {384, 20}};
		controlExit.Border													= {10, 10, 10, 10};
		controlExit.Margin													= {1, 1, 1, 1};
		controlExit.Align													= ::gpk::ALIGN_BOTTOM_RIGHT;
		::gpk::SControlText														& controlText								= gui.Controls.Text[app.IdFrameRateRender];
		controlText.Align													= ::gpk::ALIGN_CENTER;
		::gpk::controlSetParent(gui, app.IdFrameRateRender, -1);
	}

	::gpk::pobj<::gpk::SDialogTuner<uint8_t>>								tuner						= {};
	app.NumericTuner													= ::gpk::tunerCreate(app.DialogMain, tuner);
	tuner->ValueLimits.Min												= 100;
	tuner->ValueLimits.Max												= 200;
	::gpk::SControlTable													& controlTable				= gui.Controls;
	controlTable.Placement[tuner->IdGUIControl].Area.Offset				= {128, 192};
	controlTable.Placement[tuner->IdGUIControl].Area.Size.x				= 128;
	controlTable.Placement[tuner->IdGUIControl].Area.Size.y				=  20;
	tuner->SetValue(0);

	::gpk::pobj<::gpk::SDialogCheckBox>									checkbox					= {};
	app.CheckBox														= ::gpk::checkBoxCreate(app.DialogMain, checkbox);
	controlTable.Placement[checkbox->IdGUIControl].Area.Offset			= {128, 256};


	::gpk::pobj<::gpk::SDialogViewport>									viewport					= {};
	app.Viewport														= ::gpk::viewportCreate(app.DialogMain, viewport);
	controlTable.Placement[viewport->IdGUIControl].Area.Offset		= {320, 128};
	controlTable.Placement[viewport->IdGUIControl].Area.Size			= {640, 480};
	controlTable.Images[viewport->IdClient].ImageInvertY			= true;


	//---------------------------

	stacxpr char													ragnaPath	[]								= "\\data_2005\\data\\";

	char																		temp		[512]							= {};
	::gpk::SRSWFileContents														& rswData									= app.RSWData;
	//sprintf_s(temp, "%s%s%s", ragnaPath, "", "in_sphinx1.rsw");						gpk_necall(::gpk::rswFileLoad(rswData						, ::gpk::view_const_string(temp)), "Error");
	sprintf_s(temp, "%s%s%s", ragnaPath, "", "alberta.rsw");					gpk_necall(::gpk::rswFileLoad(rswData		, ::gpk::view_const_string(temp)), "Error");
	sprintf_s(temp, "%s%s%s", ragnaPath, "", &rswData.GNDFilename[0]);			gpk_necall(::gpk::gndFileLoad(app.GNDData	, ::gpk::view_const_string(temp)), "Error");
	app.RSMData.resize(rswData.RSWModels.size());
	for(uint32_t iRSM = 0; iRSM < (uint32_t)rswData.RSWModels.size(); ++iRSM){
		::gpk::SRSMFileContents														& rsmData									= app.RSMData[iRSM];
		sprintf_s(temp, "%s%s%s", ragnaPath, "model\\", &rswData.RSWModels[iRSM].Filename[0]);
		ef_if(errored(::gpk::rsmFileLoad(rsmData, ::gpk::vcs(temp))), "Failed to load file: %s.", temp);
	}
	for(uint32_t iLight = 0; iLight < rswData.RSWLights.size(); ++iLight) {
		rswData.RSWLights[iLight].Position										*= 1.0 / app.GNDData.Metrics.TileScale;
		rswData.RSWLights[iLight].Position										+= ::gpk::n3f32{app.GNDData.Metrics.Size.x / 2.0f, 0.0f, (app.GNDData.Metrics.Size.y / 2.0f)};
		rswData.RSWLights[iLight].Position.y									*= -1;
	}

	app.TexturesGND.resize(app.GNDData.TextureNames.size());
	for(uint32_t iTex = 0; iTex < app.GNDData.TextureNames.size(); ++ iTex) {
		sprintf_s(temp, "%s%s%s", ragnaPath, "texture\\", &app.GNDData.TextureNames[iTex][0]);
		ef_if(errored(::gpk::bmpFileLoad(::gpk::view_const_string(temp), app.TexturesGND[iTex])), "Not found? %s.", temp);
	}

	app.GNDModel.Nodes.resize(app.GNDData.TextureNames.size() * 6);
	app.GNDModel.TileMapping.resize(app.GNDData.Metrics.Size);
	::gpk::grid<::gpk::STileGeometryGND>									tileGeometryView							= {app.GNDData.lstTileGeometryData.begin(), app.GNDData.Metrics.Size};
	for(uint32_t iTex = 0, texCount = app.GNDData.TextureNames.size(); iTex < texCount; ++iTex) {
		int32_t indexTop	= iTex + app.GNDData.TextureNames.size() * ::gpk::TILE_FACE_FACING_TOP		; gpk_necall(::gpk::gndGenerateFaceGeometry(app.GNDData.lstTileTextureData, app.GNDData.lstTileGeometryData, app.GNDData.Metrics,::gpk::TILE_FACE_FACING_TOP	, iTex, app.GNDModel.Nodes[indexTop		], app.GNDModel.TileMapping.View), "");
		int32_t indexFront	= iTex + app.GNDData.TextureNames.size() * ::gpk::TILE_FACE_FACING_FRONT	; gpk_necall(::gpk::gndGenerateFaceGeometry(app.GNDData.lstTileTextureData, app.GNDData.lstTileGeometryData, app.GNDData.Metrics,::gpk::TILE_FACE_FACING_FRONT	, iTex, app.GNDModel.Nodes[indexFront	], app.GNDModel.TileMapping.View), "");
		int32_t indexRight	= iTex + app.GNDData.TextureNames.size() * ::gpk::TILE_FACE_FACING_RIGHT	; gpk_necall(::gpk::gndGenerateFaceGeometry(app.GNDData.lstTileTextureData, app.GNDData.lstTileGeometryData, app.GNDData.Metrics,::gpk::TILE_FACE_FACING_RIGHT	, iTex, app.GNDModel.Nodes[indexRight	], app.GNDModel.TileMapping.View), "");
		//int32_t indexBottom	= iTex + app.GNDData.TextureNames.size() * ::gpk::TILE_FACE_FACING_BOTTOM	; gpk_necall(::gpk::gndGenerateFaceGeometry(app.GNDData.lstTileTextureData, app.GNDData.lstTileGeometryData, app.GNDData.Metrics,::gpk::TILE_FACE_FACING_BOTTOM	, iTex, app.GNDModel.Nodes[indexBottom	], app.GNDModel.TileMapping.View), "");
		int32_t indexBack	= iTex + app.GNDData.TextureNames.size() * ::gpk::TILE_FACE_FACING_BACK		; gpk_necall(::gpk::gndGenerateFaceGeometry(app.GNDData.lstTileTextureData, app.GNDData.lstTileGeometryData, app.GNDData.Metrics,::gpk::TILE_FACE_FACING_BACK	, iTex, app.GNDModel.Nodes[indexBack	], app.GNDModel.TileMapping.View), "");
		int32_t indexLeft	= iTex + app.GNDData.TextureNames.size() * ::gpk::TILE_FACE_FACING_LEFT		; gpk_necall(::gpk::gndGenerateFaceGeometry(app.GNDData.lstTileTextureData, app.GNDData.lstTileGeometryData, app.GNDData.Metrics,::gpk::TILE_FACE_FACING_LEFT	, iTex, app.GNDModel.Nodes[indexLeft	], app.GNDModel.TileMapping.View), "");
	}
	// Blend normals.
	for(uint32_t y = 0; y < app.GNDData.Metrics.Size.y - 1; ++y)
	for(uint32_t x = 0; x < app.GNDData.Metrics.Size.x - 1; ++x) {
		const ::gpk::STileGeometryGND	& tileGeometry0			= tileGeometryView[y][x];
		const ::gpk::STileGeometryGND	& tileGeometry1			= tileGeometryView[y][x + 1];
		const ::gpk::STileGeometryGND	& tileGeometry2			= tileGeometryView[y + 1][x];
		const ::gpk::STileGeometryGND	& tileGeometry3			= tileGeometryView[y + 1][x + 1];
		const ::gpk::STileMapping		& tileMapping0			= app.GNDModel.TileMapping.View[y + 0][x + 0];
		const ::gpk::STileMapping		& tileMapping1			= app.GNDModel.TileMapping.View[y + 0][x + 1];
		const ::gpk::STileMapping		& tileMapping2			= app.GNDModel.TileMapping.View[y + 1][x + 0];
		const ::gpk::STileMapping		& tileMapping3			= app.GNDModel.TileMapping.View[y + 1][x + 1];
		const bool						processTile0			= (tileGeometry0.SkinMapping.SkinIndexTop != -1);// && (app.GNDData.lstTileTextureData[tileGeometry0.SkinMapping.SkinIndexTop].TextureIndex != -1);
		const bool						processTile1			= (tileGeometry1.SkinMapping.SkinIndexTop != -1) && tileGeometry0.SkinMapping.SkinIndexFront == -1;// && (app.GNDData.lstTileTextureData[tileGeometry1.SkinMapping.SkinIndexTop].TextureIndex != -1);
		const bool						processTile2			= (tileGeometry2.SkinMapping.SkinIndexTop != -1) && tileGeometry0.SkinMapping.SkinIndexRight == -1;// && (app.GNDData.lstTileTextureData[tileGeometry2.SkinMapping.SkinIndexTop].TextureIndex != -1);
		const bool						processTile3			= (tileGeometry3.SkinMapping.SkinIndexTop != -1) && (tileGeometry0.SkinMapping.SkinIndexFront == -1 && tileGeometry0.SkinMapping.SkinIndexRight == -1);// && (app.GNDData.lstTileTextureData[tileGeometry3.SkinMapping.SkinIndexTop].TextureIndex != -1);
		const int32_t					texIndex0				= processTile0 ? app.GNDData.lstTileTextureData[tileGeometry0.SkinMapping.SkinIndexTop].TextureIndex : -1;
		const int32_t					texIndex1				= processTile1 ? app.GNDData.lstTileTextureData[tileGeometry1.SkinMapping.SkinIndexTop].TextureIndex : -1;
		const int32_t					texIndex2				= processTile2 ? app.GNDData.lstTileTextureData[tileGeometry2.SkinMapping.SkinIndexTop].TextureIndex : -1;
		const int32_t					texIndex3				= processTile3 ? app.GNDData.lstTileTextureData[tileGeometry3.SkinMapping.SkinIndexTop].TextureIndex : -1;
		::gpk::n3f32					normal					= {};
		uint32_t						divisor					= 0;
		if(processTile0) { ++divisor; ::gpk::SModelNodeGND & gndNode0 = app.GNDModel.Nodes[texIndex0]; normal += gndNode0.Normals[tileMapping0.VerticesTop[3]]; }
		if(processTile1) { ++divisor; ::gpk::SModelNodeGND & gndNode1 = app.GNDModel.Nodes[texIndex1]; normal += gndNode1.Normals[tileMapping1.VerticesTop[2]]; }
		if(processTile2) { ++divisor; ::gpk::SModelNodeGND & gndNode2 = app.GNDModel.Nodes[texIndex2]; normal += gndNode2.Normals[tileMapping2.VerticesTop[1]]; }
		if(processTile3) { ++divisor; ::gpk::SModelNodeGND & gndNode3 = app.GNDModel.Nodes[texIndex3]; normal += gndNode3.Normals[tileMapping3.VerticesTop[0]]; }
		if(divisor) {
			(normal /= divisor).Normalize();
			if(processTile0) { ::gpk::SModelNodeGND	& gndNode0 = app.GNDModel.Nodes[texIndex0]; gndNode0.Normals[tileMapping0.VerticesTop[3]] = normal; }
			if(processTile1) { ::gpk::SModelNodeGND	& gndNode1 = app.GNDModel.Nodes[texIndex1]; gndNode1.Normals[tileMapping1.VerticesTop[2]] = normal; }
			if(processTile2) { ::gpk::SModelNodeGND	& gndNode2 = app.GNDModel.Nodes[texIndex2]; gndNode2.Normals[tileMapping2.VerticesTop[1]] = normal; }
			if(processTile3) { ::gpk::SModelNodeGND	& gndNode3 = app.GNDModel.Nodes[texIndex3]; gndNode3.Normals[tileMapping3.VerticesTop[0]] = normal; }
		}
	}
	app.Scene.Camera.Points.Position	= {0, 30, -20};
	app.Scene.Camera.NearFar.Max		= 1000.0f;
	app.Scene.Camera.NearFar.Min		= 0.001f;

	::gpk::SGNDFileContents					& gndData									= app.GNDData;
		// -- Generate minimap
	::gpk::minmax<float>					heightMinMax								= {};
	for(uint32_t iTile = 0; iTile < gndData.lstTileGeometryData.size(); ++iTile)
		if(gndData.lstTileGeometryData[iTile].SkinMapping.SkinIndexTop != -1) {
			for(uint32_t iHeight = 0; iHeight < 4; ++iHeight) {
				heightMinMax.Max			= ::gpk::max(gndData.lstTileGeometryData[iTile].fHeight[iHeight] * -1, heightMinMax.Max);
				heightMinMax.Min			= ::gpk::min(gndData.lstTileGeometryData[iTile].fHeight[iHeight] * -1, heightMinMax.Min);
			}
		}
	::gpk::img8bgra				minimapTemp;
	minimapTemp.resize(gndData.Metrics.Size);

	::gpk::g8bgra				& minimapView								= minimapTemp.View;
	const float					heightRange									= heightMinMax.Max - heightMinMax.Min;
	for(uint32_t y = 0, yMax = minimapView.metrics().y; y < yMax; ++y)
	for(uint32_t x = 0, xMax = minimapView.metrics().x; x < xMax; ++x) {
		float						fAverageHeight								= 0;
		for(uint32_t iHeight = 0; iHeight < 4; ++iHeight)
			fAverageHeight			+= tileGeometryView[y][x].fHeight[iHeight] * -1;
		fAverageHeight			*= .25f;
		float						colorRatio										= (fAverageHeight - heightMinMax.Min) / heightRange;
		//minimapView[y][minimapView.metrics().x - 1 - x]							= ((fAverageHeight < 0) ? ::gpk::WHITE : ::gpk::BLUE) * colorRatio;
		minimapView[minimapView.metrics().y - 1 - y][x]							= ::gpk::WHITE * colorRatio;
	}

	app.TextureMinimap.resize(gndData.Metrics.Size * 4);
	::gpk::grid_scale(app.TextureMinimap.View, minimapTemp.View);

	app.ViewportMinimap												= ::gpk::viewportCreate(app.DialogMain, viewport);
	controlTable.Placement	[viewport->IdGUIControl].Area.Offset	= {0, 0};
	controlTable.Placement	[viewport->IdGUIControl].Align			= ::gpk::ALIGN_RIGHT;
	controlTable.Images	[viewport->IdClient].ImageInvertY		= true;
	::gpk::viewportAdjustSize(controlTable.Placement[viewport->IdGUIControl].Area.Size, minimapTemp.metrics().Cast<int16_t>() * 2);
	controlTable.Images	[viewport->IdClient].Image				= app.TextureMinimap.View;
	controlTable.Images	[viewport->IdClient].ImageAlign			= ::gpk::ALIGN_TOP_LEFT;

	::gpk::pobj<::gpk::SDialogSlider>									sliderH						= {};
	app.SliderH															= ::gpk::sliderCreate(app.DialogMain, sliderH);
	sliderH->ValueLimits.Min											= 0;
	sliderH->ValueLimits.Max											= app.TextureMinimap.View.metrics().x / 2;
	controlTable.Placement[sliderH->IdGUIControl].Area.Offset			= {0, 0};
	controlTable.Placement[sliderH->IdGUIControl].Area.Size.x			= controlTable.Placement[viewport->IdGUIControl].Area.Size.x;
	controlTable.Placement[sliderH->IdGUIControl].Area.Size.y			= 8;
	controlTable.Constraints[sliderH->IdGUIControl].DockToControl.Bottom= viewport->IdGUIControl;
	controlTable.Constraints[sliderH->IdGUIControl].DockToControl.Left	= viewport->IdGUIControl;
	//::gpk::controlSetParent(gui, sliderH->IdGUIControl, viewport->IdGUIControl);

	::gpk::pobj<::gpk::SDialogSlider>									sliderV						= {};
	app.SliderV															= ::gpk::sliderCreate(app.DialogMain, sliderV);
	sliderV->ValueLimits.Min											= 0;
	sliderV->ValueLimits.Max											= app.TextureMinimap.View.metrics().y / 2;
	sliderV->Vertical													= true;
	controlTable.Placement[sliderV->IdGUIControl].Area.Offset			= {0, 20};
	controlTable.Placement[sliderV->IdGUIControl].Area.Size.x			= 8;
	controlTable.Placement[sliderV->IdGUIControl].Area.Size.y			= controlTable.Placement[viewport->IdGUIControl].Area.Size.y - 20;
	controlTable.Constraints[sliderV->IdGUIControl].DockToControl.Top	= viewport->IdGUIControl;
	controlTable.Constraints[sliderV->IdGUIControl].DockToControl.Right	= viewport->IdGUIControl;
	::gpk::sliderSetValue(*sliderV, 0);
	return 0;
}

::gpk::error_t			update		(::gme::SApplication & app, bool exitSignal)	{
	::gpk::STimer															timer;
	rvis_if(::gpk::APPLICATION_STATE_EXIT, exitSignal);
	{
		::std::lock_guard														lock						(app.LockRender);
		app.Framework.RootWindow.BackBuffer									= app.Offscreen;
	}
	::gpk::SFramework			& framework		= app.Framework;
	rvis_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework));

	::gpk::SGUI					& gui			= *framework.GUI;
	::gpk::acid					toProcess		= {};
	if(1 == ::gpk::guiProcessControls(gui, [&app](::gpk::cid_t iControl) { return one_if(iControl == app.IdExit); }))
		return 1;

	app.DialogMain.Update();

	//----------------------------------------------
	::gpk::SFrameInfo			& frameInfo			= framework.FrameInfo;
	bool						updateProjection	= false;
	::gpk::SWindow				& mainWindow		= framework.RootWindow;
	if(mainWindow.Input->KeyboardCurrent.KeyState[VK_ADD		])	{ updateProjection = true; app.Scene.Camera.Angle += frameInfo.Seconds.LastFrame; }
	if(mainWindow.Input->KeyboardCurrent.KeyState[VK_SUBTRACT	])	{ updateProjection = true; app.Scene.Camera.Angle -= frameInfo.Seconds.LastFrame; }
	if(mainWindow.Input->KeyboardCurrent.KeyState['Q'])				{ updateProjection = true; app.Scene.Camera.Points.Position.x += (float)frameInfo.Seconds.LastFrame * 10; }
	if(mainWindow.Input->KeyboardCurrent.KeyState['E'])				{ updateProjection = true; app.Scene.Camera.Points.Position.x -= (float)frameInfo.Seconds.LastFrame * 10; }
	if(mainWindow.Input->KeyboardCurrent.KeyState['W'])				{ updateProjection = true; app.Scene.Camera.Points.Position.y += (float)frameInfo.Seconds.LastFrame * 10; }
	if(mainWindow.Input->KeyboardCurrent.KeyState['S'])				{ updateProjection = true; app.Scene.Camera.Points.Position.y -= (float)frameInfo.Seconds.LastFrame * 10; }
	if(mainWindow.Input->KeyboardCurrent.KeyState['A'])				{ updateProjection = true; app.Scene.Camera.Points.Position.z += (float)frameInfo.Seconds.LastFrame * 10; }
	if(mainWindow.Input->KeyboardCurrent.KeyState['D'])				{ updateProjection = true; app.Scene.Camera.Points.Position.z -= (float)frameInfo.Seconds.LastFrame * 10; }

	//------------------------------------------------ Camera
	::gpk::SCameraPoints														& camera									= app.Scene.Camera.Points;
	//camera.Position.RotateY(framework.Input->MouseCurrent.Deltas.x / 20.0f / (framework.Input->KeyboardCurrent.KeyState[VK_CONTROL] ? 2.0 : 1));
	//if(framework.Input->MouseCurrent.Deltas.z) {
	//	::gpk::n3f32														zoomVector									= camera.Position;
	//	zoomVector.Normalize();
	//	const double																zoomWeight									= framework.Input->MouseCurrent.Deltas.z * (framework.Input->KeyboardCurrent.KeyState[VK_SHIFT] ? 10 : 1) / 240.;
	//	camera.Position															+= zoomVector * zoomWeight * .5;
	//}
	camera.Target																= {app.TextureMinimap.metrics().x / 2.0f, 0, -(int32_t)app.TextureMinimap.metrics().y / 2.0f};
	//------------------------------------------------ Lights
	::gpk::n3f32														& lightDir									= app.LightDirection;
	lightDir.RotateY(frameInfo.Microseconds.LastFrame / 1000000.0f);
	lightDir.Normalize();

	::gpk::SRSWFileContents														& rswData									= app.RSWData;
	const ::gpk::n3f32													halfMapDir									= ::gpk::n3f32{app.GNDData.Metrics.Size.x / 2.0f, 0.0f, (app.GNDData.Metrics.Size.y / 2.0f)};
	for(uint32_t iLight = 0; iLight < rswData.RSWLights.size(); ++iLight) {
		rswData.RSWLights[iLight].Position										-= halfMapDir;
		rswData.RSWLights[iLight].Position.RotateY(frameInfo.Seconds.LastFrame);
		rswData.RSWLights[iLight].Position										+= halfMapDir;
	}
	//------------------------------------------------
	//app.GridPivot.Scale										= {2.f, 4.f, 2.f};
	app.GridPivot.Scale										= {1, -1, -1};
	app.GridPivot.Orientation.Normalize();

	::gpk::pobj<::gpk::SDialogSlider>									slider									= {};
	app.DialogMain.Controls[app.SliderH].as(slider);
	::gpk::pobj<::gpk::SDialogViewport>									viewport									= {};
	app.DialogMain.Controls[app.ViewportMinimap].as(viewport);
	gui.Controls.Images[viewport->IdClient].ImageOffset.x				= (int16_t)slider->ValueCurrent;
	::gpk::controlMetricsInvalidate(gui, slider->IdGUIControl);

	app.DialogMain.Controls[app.SliderV].as(slider);
	gui.Controls.Images[viewport->IdClient].ImageOffset.y				= (int16_t)slider->ValueCurrent;
	::gpk::controlMetricsInvalidate(gui, slider->IdGUIControl);


	timer.Frame();
	sprintf_s(app.StringFrameRateUpdate, "Last frame time (update): %fs.", (float)timer.LastTimeSeconds);
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	gui.Controls.Text[app.IdFrameRateUpdate].Text							= app.StringFrameRateUpdate;
	::gpk::controlMetricsInvalidate(gui, app.IdFrameRateUpdate);

	return 0;
}

::gpk::error_t				drawGND
	( ::gpk::SRenderCache						& renderCache
	, ::gpk::SSceneTransforms					& transforms
	, ::gpk::SSceneCamera						& camera
	, ::gpk::SWindow::TOffscreen				& target
	, const ::gpk::SModelPivot<float>			& modelPivot
	, const ::gpk::n3f32						& lightDir
	, const ::gpk::SModelGND					& modelGND
	, const ::gpk::SRSWWorldLight				& directionalLight
	, const ::gpk::view<const ::gpk::img8bgra>	& textures
	, const ::gpk::view<const ::gpk::SLightInfoRSW>	& lights
	, bool											wireframe
	);
::gpk::error_t			draw		(::gme::SApplication & app)							{
	::gpk::STimer															timer;
	::gpk::SFramework				& framework									= app.Framework;
	::gpk::SGUI						& gui										= *framework.GUI;
	static ::gpk::rtbgra8d32				buffer3D;
	::gpk::pobj<::gpk::SDialogViewport>									viewport									= {};
	app.DialogMain.Controls[app.Viewport].as(viewport);
	const ::gpk::n2<uint32_t>											& offscreenMetrics							= gui.Controls.Placement[viewport->IdClient].Area.Size.Cast<uint32_t>();

	buffer3D.resize(offscreenMetrics, {0, 0, 0, 0}, 0xFFFFFFFF);
	gui.Controls.Images[viewport->IdClient].Image						= buffer3D.Color.View;
	::gpk::pobj<::gpk::SDialogCheckBox>			checkbox;
	app.DialogMain.Controls[app.CheckBox].as(checkbox);

	int32_t 																pixelsDrawn0								= drawGND(app.RenderCache, app.Scene.Transforms, app.Scene.Camera, buffer3D, app.GridPivot, app.LightDirection, app.GNDModel, app.RSWData.Light, app.TexturesGND, app.RSWData.RSWLights, checkbox->Checked);
	es_if(errored(pixelsDrawn0));
	::gpk::pobj<::gpk::rtbgra8d32>	target;
	target.create();
	target->resize(app.Framework.RootWindow.Size, ::gpk::LIGHTGRAY, 0xFFFFFFFFU);
	{
		::std::lock_guard														lock					(app.Framework.LockGUI);
		::gpk::guiDraw(*app.Framework.GUI, target->Color.View);
	}
	{
		::std::lock_guard														lock					(app.Framework.LockGUI);
		::gpk::guiDraw(*app.DialogMain.GUI, target->Color.View);
	}
	{
		::std::lock_guard														lock					(app.LockRender);
		app.Offscreen														= target;
	}
	timer.Frame();
	sprintf_s(app.StringFrameRateRender, "Last frame time (render): %fs.", (float)timer.LastTimeSeconds);
	gui.Controls.Text[app.IdFrameRateRender].Text							= app.StringFrameRateRender;
	::gpk::controlMetricsInvalidate(gui, app.IdFrameRateRender);
	return 0;
}
