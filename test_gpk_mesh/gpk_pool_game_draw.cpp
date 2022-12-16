#include "gpk_pool_game.h"

#include "gpk_timer.h"
#include "gpk_noise.h"

::gpk::error_t								transformFragments				()	{ return 0; }

::gpk::error_t								the1::poolGameDraw				
	( ::the1::SPoolGame									& pool
	, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>	& backBuffer
	, const ::gpk::SCoord3<float>						& cameraPosition
	, const ::gpk::SCoord3<float>						& cameraTarget
	, const ::gpk::SCoord3<float>						& cameraUp
	, double											totalSeconds
	) {
	::gpk::SCoord3<float>							lightPos						= {15, 10, 0};
	lightPos.RotateY(-totalSeconds);

	const ::gpk::SCoord2<uint16_t>					offscreenMetrics				= backBuffer.Color.View.metrics().Cast<uint16_t>();

	::gpk::SCoord3<float>							cameraFront						= (cameraTarget - cameraPosition).Normalize();

	::gpk::SEngineSceneConstants					constants						= {};
	constants.NearFar							= {0.1f , 200.0f};
	constants.CameraPosition					= cameraPosition;
	constants.CameraFront						= cameraFront;
	constants.LightPosition						= lightPos;
	constants.LightDirection					= {0, -1, 0};

	constants.View.LookAt(cameraPosition, cameraTarget, cameraUp);
	constants.Perspective.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, constants.NearFar.Near, constants.NearFar.Far);
	constants.Screen.ViewportLH(offscreenMetrics, constants.NearFar);
	constants.Projection						= constants.View * constants.Perspective * constants.Screen;

	::gpk::STimer									timer;
	::gpk::SEngine									& engine						= pool.Engine;

	::gpk::array_pod<::gpk::SCoord2<int16_t>>		& wireframePixelCoords			= engine.Scene->RenderCache.CacheVertexShader.WireframePixelCoords;
	for(uint32_t iBall = 0; iBall < pool.StateCurrent.BallCount; ++iBall) {
		for(uint32_t iDelta = ::gpk::max(0, (int32_t)pool.PositionDeltas[iBall].size() - 20); iDelta < pool.PositionDeltas[iBall].size(); ++iDelta) {
			::gpk::SLine3<float>							screenDelta				= pool.PositionDeltas[iBall][iDelta];
			screenDelta.A								= constants.Projection.Transform(screenDelta.A);
			screenDelta.B								= constants.Projection.Transform(screenDelta.B);
			wireframePixelCoords.clear();
			::gpk::drawLine(offscreenMetrics, screenDelta, wireframePixelCoords);
			for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
				::gpk::SCoord2<int16_t>							coord					= wireframePixelCoords[iCoord];
				backBuffer.Color.View[coord.y][coord.x]		= pool.StateCurrent.BallColors[iBall];
			}
		}
	}

	const gpk::SCoord2<float>						halfDimensions					= pool.StateCurrent.Table.Size * .5;

	wireframePixelCoords.clear();
	const	::gpk::SCoord3<float>					limitsBottom[4]					= 
		{ { halfDimensions.x, .0f,  halfDimensions.y}
		, {-halfDimensions.x, .0f,  halfDimensions.y}
		, { halfDimensions.x, .0f, -halfDimensions.y}
		, {-halfDimensions.x, .0f, -halfDimensions.y}
		};
	const	::gpk::SCoord3<float>	limitsTop	[4]	= 
		{ limitsBottom[0] + ::gpk::SCoord3<float>{0, pool.StateCurrent.Table.Height, 0}
		, limitsBottom[1] + ::gpk::SCoord3<float>{0, pool.StateCurrent.Table.Height, 0}
		, limitsBottom[2] + ::gpk::SCoord3<float>{0, pool.StateCurrent.Table.Height, 0}
		, limitsBottom[3] + ::gpk::SCoord3<float>{0, pool.StateCurrent.Table.Height, 0}
		};

	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsBottom[3], limitsBottom[2]}, constants.Projection, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsBottom[3], limitsBottom[1]}, constants.Projection, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsBottom[0], limitsBottom[1]}, constants.Projection, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsBottom[0], limitsBottom[2]}, constants.Projection, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsTop[3], limitsTop[2]}, constants.Projection, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsTop[3], limitsTop[1]}, constants.Projection, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsTop[0], limitsTop[1]}, constants.Projection, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsTop[0], limitsTop[2]}, constants.Projection, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsBottom[0], limitsTop[0]}, constants.Projection, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsBottom[1], limitsTop[1]}, constants.Projection, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsBottom[2], limitsTop[2]}, constants.Projection, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsBottom[3], limitsTop[3]}, constants.Projection, wireframePixelCoords);

	for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
		::gpk::SCoord2<int16_t>								coord		= wireframePixelCoords[iCoord];
		::gpk::SColorFloat									color		= 
			{ (float)(totalSeconds - iCoord / 1.0f / totalSeconds)
			, (float)(totalSeconds - iCoord / 2.0f / totalSeconds)
			, (float)(totalSeconds - iCoord / 3.0f / totalSeconds)
			};
		backBuffer.Color.View[coord.y][coord.x]		= color;
	}

	::gpk::drawScene(backBuffer.Color.View, backBuffer.DepthStencil.View, engine.Scene->RenderCache, *engine.Scene, constants);
	timer.Frame();
	info_printf("Render scene in %f seconds", timer.LastTimeSeconds);
	return 0;
}
