#include "gpk_model.h"
#include "gpk_geometry_lh.h"

#include "gpk_image.h"
#include "gpk_framework.h"

#include "gpk_udp_server.h"

#include "draw.h"

#ifndef CED_DEMO_08_H_298837492837
#define CED_DEMO_08_H_298837492837

struct SDrawCache {
	::gpk::array_pod<::gpk::SCoord2<int32_t>>			PixelCoords				= {};
	::gpk::array_pod<::gpk::STriangleWeights<float>>	PixelVertexWeights		= {};
	::gpk::array_pod<::gpk::SCoord3<float>>				LightPointsModel		= {};
	::gpk::array_pod<::gpk::SColorBGRA>					LightColorsModel		= {};
};

struct STextOverlay {
	::gpk::SCoord3<float>								LightVector0			= {-15, 50, -15};

	::gpk::SCoord3<float>								CameraTarget			= {0, 0, 0};
	::gpk::SCoord3<float>								CameraPosition			= {.0001f, 1000.1f, -0.00001f};
	::gpk::SCoord3<float>								CameraUp				= {0, 0, 1};
	::gpk::SGeometryQuads								GeometryLetters	[256]	= {};
	::gpk::SMatrix4<float>								MatrixProjection		= {};
	::SDrawCache										DrawCache				= {};

	static constexpr	const ::gpk::SCoord3<float>		ControlTranslation		= {0, 0, 20.0f};
	static constexpr	const ::gpk::SCoord2<uint32_t>	MetricsLetter			= {12, 12};
	static constexpr	const ::gpk::SCoord2<uint32_t>	MetricsMap				= {16, 16};
};

namespace klib
{
	//----------------------------------------------------------------------------------------------------------------------------------------------
	struct STacticalGame {
				// Game Flags tell us about the				current state of the application.
				::klib::GAME_FLAGS							Flags							= (::klib::GAME_FLAGS)(::klib::GAME_FLAGS_NETWORK_ENABLED | ::klib::GAME_FLAGS_TURN_BUSY);
				::klib::GAME_MODE							Mode							= ::klib::GAME_MODE_CAMPAIGN;	// This is the default because it's the only available mode at the moment
				::klib::SGameState							State							= {::klib::GAME_STATE_MENU_MAIN,};
				::klib::SGameState							PreviousState					= {::klib::GAME_STATE_MENU_MAIN,};

				::klib::SGamePlayer							Players[MAX_PLAYER_TYPES]		= {};

				::klib::STimer								FrameTimer						= {};

				// Tactical board.
				::klib::STacticalInfo						TacticalInfo					= {};

				// Displays.
				::klib::SWeightedDisplay					TacticalDisplay					= {};
				::klib::SWeightedDisplay					GlobalDisplay					= {};

				// Feedback messages.
				::klib::SGameMessages						Messages;
				::gpk::array_pod<::klib::SGameEvent>		Events;

				// For the special effect
				::klib::SEntityTables						EntityTables					= {};

				::std::mutex								PlayerMutex						= {};
				::std::mutex								ServerTimeMutex					= {};
				uint64_t									ServerTime						= 0;
				int64_t										Seed							= 0;

				void										ClearDisplays					()																						{
			TacticalDisplay	.Clear();
			GlobalDisplay	.Clear();
		}

		inline	void										LogAuxStateMessage				()	{ Messages.LogAuxStateMessage	(); }
		inline	void										LogAuxMessage					()	{ Messages.LogAuxMessage		(); }
		inline	void										LogAuxSuccess					()	{ Messages.LogAuxSuccess		(); }
		inline	void										LogAuxError						()	{ Messages.LogAuxError			(); }
		inline	void										LogAuxMiss						()	{ Messages.LogAuxMiss			(); }

		inline	void										LogStateMessage					()	{ Messages.LogStateMessage	(); }
		inline	void										LogMessage						()	{ Messages.LogMessage		(); }
		inline	void										LogSuccess						()	{ Messages.LogSuccess		(); }
		inline	void										LogError						()	{ Messages.LogError			(); }
		inline	void										LogMiss							()	{ Messages.LogMiss			(); }
		inline	void										ClearMessages					()	{ Messages.ClearMessages	(); }
	};	// struct
} // namespace

struct SApplication {
			::gpk::SFramework														Framework;
			::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>		Offscreen					= {};

			int32_t																	IdExit						= -1;

			::std::mutex															LockGUI;
			::std::mutex															LockRender;

			::STextOverlay															TextOverlay					= {};
			::gpk::array_obj<::gpk::ptr_obj<::klib::SGame>>							Game						= {};

			::gpk::SUDPServer														TacticalServer;
			typedef ::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>> TClientQueue;
			::gpk::array_obj<TClientQueue>											MessagesToProcess;

																					SApplication				(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
};

#endif // CED_DEMO_08_H_298837492837
