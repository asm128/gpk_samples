#include "gpk_engine_rendermesh.h"
#include "gpk_engine_renderbuffer.h"
#include "gpk_engine_rendersurface.h"
#include "gpk_engine_rendernode.h"
#include "gpk_bitmap_target.h"

#include <functional>

#ifndef GPK_ENGINE_SCENE_H
#define GPK_ENGINE_SCENE_H

namespace gpk 
{
	struct SEngineRenderCache {
		::gpk::SVSOutput					OutputVertexShader		= {};
		::gpk::SVSCache						CacheVertexShader		= {};
	};

	struct SEngineScene;

	struct SShaderManager {
		::gpk::array_obj<::std::function<int32_t(const ::gpk::SEngineScene & sceneResources, int32_t iRenderNode)>>	Shaders;
	};


	struct SEngineScene {
		::gpk::SRenderBufferManager			ManagedBuffers			= {};
		::gpk::SSurfaceManager				ManagedSurfaces			= {};
		::gpk::SMeshManager					ManagedMeshes			= {};
		::gpk::SShaderManager				ManagedShaders			= {};
		::gpk::SRenderNodeManager			ManagedRenderNodes		= {};

		::gpk::SEngineRenderCache			RenderCache				= {};
	};
	
	struct SEngineSceneConstants {
		::gpk::SMatrix4<float>				Projection			= {}; 
		::gpk::SNearFar						NearFar 			= {.1f, 500.0f}; 
		::gpk::SCoord3<float>				CameraPosition		= {}; 
		::gpk::SCoord3<float>				CameraFront			= {}; 
		::gpk::SCoord3<float>				LightPosition		= {}; 
		::gpk::SCoord3<float>				LightDirection		= {}; 
	};

	::gpk::error_t						drawScene									
		( ::gpk::view_grid<::gpk::SColorBGRA>	& backBufferColors
		, ::gpk::view_grid<uint32_t>			& backBufferDepth
		, ::gpk::SEngineRenderCache				& renderCache
		, const ::gpk::SEngineScene				& scene
		, const ::gpk::SEngineSceneConstants	& constants
		);

	template<typename _tCoord>
	static	::gpk::error_t				drawLine		(const ::gpk::SCoord2<uint16_t>& targetMetrics, const ::gpk::SLine3<_tCoord>& line, const ::gpk::SMatrix4<float> & mWVP, ::gpk::array_pod<::gpk::SCoord2<int16_t>>& out_Points) {
		::gpk::SCoord3<_tCoord>					vA				= mWVP.Transform(line.A); 
		::gpk::SCoord3<_tCoord>					vB				= mWVP.Transform(line.B);
		return ::gpk::drawLine(targetMetrics, ::gpk::SLine2<_tCoord>{{vA.x, vA.y}, {vB.x, vB.y}}, out_Points);
	}
} // namespace

#endif // GPK_ENGINE_SCENE_H