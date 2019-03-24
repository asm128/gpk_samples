#include "gpk_array.h"

#ifndef TRACK_H_9283749823
#define TRACK_H_9283749823

#pragma pack(push, 1)
enum TRACK_EVENT_TYPE : int32_t
	{ TRACK_EVENT_TYPE_ENEMY			= 0
	, TRACK_EVENT_TYPE_SOUND_START
	, TRACK_EVENT_TYPE_SOUND_STOP
	, TRACK_EVENT_TYPE_PARTICLE
	, TRACK_EVENT_TYPE_MODE_AUTO
	, TRACK_EVENT_TYPE_MODE_MANUAL
	, TRACK_EVENT_TYPE_COUNT
	, TRACK_EVENT_TYPE_INVALID			= -1
	};

struct STrackEvent {
						float										TimeStart							;
						float										Duration							;
						int32_t										PayloadId							;
						TRACK_EVENT_TYPE							Type								;
};
#pragma pack(pop)

struct STrackEventTable {
						::gpk::array_pod<float				>		EventTimeStart						;
						::gpk::array_pod<float				>		EventDuration						;
						::gpk::array_pod<int32_t			>		EventPayloadId						;
						::gpk::array_pod<TRACK_EVENT_TYPE	>		EventType							;

						void										Clear								()									noexcept		{ ::gpk::clear(EventTimeStart, EventDuration, EventPayloadId, EventType); }
						::gpk::error_t								PushEvent							(const STrackEvent& eventToPush)					{
		if(-1 == EventTimeStart	.push_back(eventToPush.TimeStart	)) { ::gpk::resize(EventType.size(), EventTimeStart, EventDuration, EventPayloadId, EventType); error_printf("Out of memory?"); return -1; }
		if(-1 == EventDuration	.push_back(eventToPush.Duration		)) { ::gpk::resize(EventType.size(), EventTimeStart, EventDuration, EventPayloadId, EventType); error_printf("Out of memory?"); return -1; }
		if(-1 == EventPayloadId	.push_back(eventToPush.PayloadId	)) { ::gpk::resize(EventType.size(), EventTimeStart, EventDuration, EventPayloadId, EventType); error_printf("Out of memory?"); return -1; }
		if(-1 == EventType		.push_back(eventToPush.Type			)) { ::gpk::resize(EventType.size(), EventTimeStart, EventDuration, EventPayloadId, EventType); error_printf("Out of memory?"); return -1; }
		return 0;
	}
};

#pragma pack(push, 1)
struct STrackState {
						float										TimeElapsed;
						int32_t										LastEvent;
};
#pragma pack(pop)

//struct STrack {
//						float										TimeStart							;
//						float										Duration							;
//						STrackEventTable							Table								;
//};

#endif // TRACK_H_9283749823
