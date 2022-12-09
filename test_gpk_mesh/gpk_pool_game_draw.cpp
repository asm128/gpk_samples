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
	::gpk::SCoord3<float>							lightPos						= {10, 10, 0};
	lightPos.RotateY(-totalSeconds);

	const ::gpk::SCoord2<uint16_t>					offscreenMetrics				= backBuffer.Color.View.metrics().Cast<uint16_t>();
	::gpk::SNearFar									nearFar							= {0.01f , 500.0f};

	::gpk::SCoord3<float>							cameraFront						= (cameraTarget - cameraPosition).Normalize();

	::gpk::SMatrix4<float>							mView							= {};
	::gpk::SMatrix4<float>							mPerspective					= {};
	::gpk::SMatrix4<float>							mViewport						= {};
	mView.LookAt(cameraPosition, cameraTarget, cameraUp);
	mPerspective.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar.Near, nearFar.Far);
	mViewport.ViewportLH(offscreenMetrics, nearFar);

	::gpk::SMatrix4<float>							mViewPerspectiveScreen			= mView * mPerspective * mViewport;

	::gpk::STimer									timer;
	::gpk::SEngine									& engine						= pool.Engine;

	::gpk::array_pod<::gpk::SCoord2<int16_t>>		& wireframePixelCoords			= engine.Scene->RenderCache.CacheVertexShader.WireframePixelCoords;
	for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
		for(uint32_t iDelta = ::gpk::max(0, (int32_t)pool.PositionDeltas[iBall].size() - 20); iDelta < pool.PositionDeltas[iBall].size(); ++iDelta) {
			::gpk::SLine3<float>							screenDelta				= pool.PositionDeltas[iBall][iDelta];
			screenDelta.A								= mViewPerspectiveScreen.Transform(screenDelta.A);
			screenDelta.B								= mViewPerspectiveScreen.Transform(screenDelta.B);
			wireframePixelCoords.clear();
			::gpk::drawLine(offscreenMetrics, screenDelta, wireframePixelCoords);
			for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
				::gpk::SCoord2<int16_t>							coord					= wireframePixelCoords[iCoord];
				backBuffer.Color.View[coord.y][coord.x]		= pool.StartState.BallColors[pool.StartState.BallOrder[iBall]];
			}
		}
	}

	const gpk::SCoord3<float>						halfDimensions			= pool.StartState.TableDimensions * .5;

	wireframePixelCoords.clear();
	const	::gpk::SCoord3<float>	limitsBottom[4]	= 
		{ { halfDimensions.x, .0f,  halfDimensions.z}
		, {-halfDimensions.x, .0f,  halfDimensions.z}
		, { halfDimensions.x, .0f, -halfDimensions.z}
		, {-halfDimensions.x, .0f, -halfDimensions.z}
		};
	const	::gpk::SCoord3<float>	limitsTop	[4]	= 
		{ limitsBottom[0] + ::gpk::SCoord3<float>{0, pool.StartState.TableDimensions.y, 0}
		, limitsBottom[1] + ::gpk::SCoord3<float>{0, pool.StartState.TableDimensions.y, 0}
		, limitsBottom[2] + ::gpk::SCoord3<float>{0, pool.StartState.TableDimensions.y, 0}
		, limitsBottom[3] + ::gpk::SCoord3<float>{0, pool.StartState.TableDimensions.y, 0}
		};

	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsBottom[3], limitsBottom[2]}, mViewPerspectiveScreen, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsBottom[3], limitsBottom[1]}, mViewPerspectiveScreen, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsBottom[0], limitsBottom[1]}, mViewPerspectiveScreen, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsBottom[0], limitsBottom[2]}, mViewPerspectiveScreen, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsTop[3], limitsTop[2]}, mViewPerspectiveScreen, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsTop[3], limitsTop[1]}, mViewPerspectiveScreen, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsTop[0], limitsTop[1]}, mViewPerspectiveScreen, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsTop[0], limitsTop[2]}, mViewPerspectiveScreen, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsBottom[0], limitsTop[0]}, mViewPerspectiveScreen, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsBottom[1], limitsTop[1]}, mViewPerspectiveScreen, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsBottom[2], limitsTop[2]}, mViewPerspectiveScreen, wireframePixelCoords);
	::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{limitsBottom[3], limitsTop[3]}, mViewPerspectiveScreen, wireframePixelCoords);

	for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
		::gpk::SCoord2<int16_t>								coord		= wireframePixelCoords[iCoord];
		::gpk::SColorFloat									color		= 
			{ (float)(totalSeconds - iCoord / 1.0f / totalSeconds)
			, (float)(totalSeconds - iCoord / 2.0f / totalSeconds)
			, (float)(totalSeconds - iCoord / 3.0f / totalSeconds)
			};
		backBuffer.Color.View[coord.y][coord.x]		= color;
	}

	::gpk::drawScene(backBuffer.Color.View, backBuffer.DepthStencil.View, engine.Scene->RenderCache, *engine.Scene
		, mViewPerspectiveScreen, nearFar
		, cameraPosition, cameraFront, lightPos, {0, -1, 0});
	timer.Frame();
	info_printf("Render scene in %f seconds", timer.LastTimeSeconds);
	return 0;
}
