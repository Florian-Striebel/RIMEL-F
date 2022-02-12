







#if !defined(LLDB_API_SBBROADCASTER_H)
#define LLDB_API_SBBROADCASTER_H

#include "lldb/API/SBDefines.h"

namespace lldb {

class LLDB_API SBBroadcaster {
public:
SBBroadcaster();

SBBroadcaster(const char *name);

SBBroadcaster(const SBBroadcaster &rhs);

const SBBroadcaster &operator=(const SBBroadcaster &rhs);

~SBBroadcaster();

explicit operator bool() const;

bool IsValid() const;

void Clear();

void BroadcastEventByType(uint32_t event_type, bool unique = false);

void BroadcastEvent(const lldb::SBEvent &event, bool unique = false);

void AddInitialEventsToListener(const lldb::SBListener &listener,
uint32_t requested_events);

uint32_t AddListener(const lldb::SBListener &listener, uint32_t event_mask);

const char *GetName() const;

bool EventTypeHasListeners(uint32_t event_type);

bool RemoveListener(const lldb::SBListener &listener,
uint32_t event_mask = UINT32_MAX);



bool operator==(const lldb::SBBroadcaster &rhs) const;



bool operator!=(const lldb::SBBroadcaster &rhs) const;




bool operator<(const lldb::SBBroadcaster &rhs) const;

protected:
friend class SBCommandInterpreter;
friend class SBCommunication;
friend class SBDebugger;
friend class SBEvent;
friend class SBListener;
friend class SBProcess;
friend class SBTarget;

SBBroadcaster(lldb_private::Broadcaster *broadcaster, bool owns);

lldb_private::Broadcaster *get() const;

void reset(lldb_private::Broadcaster *broadcaster, bool owns);

private:
lldb::BroadcasterSP m_opaque_sp;
lldb_private::Broadcaster *m_opaque_ptr = nullptr;
};

}

#endif
