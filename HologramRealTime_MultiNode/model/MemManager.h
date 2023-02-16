#ifndef MEM_MANAGER_H_INCLUDED
#define MEM_MANAGER_H_INCLUDED

#ifndef _WIN32
#include <graphics/sys.h>
#endif

#include <string>


typedef unsigned int UndoContainerId;

#ifdef _WIN32
// setup
int MemAccessViolationSignalHandler(void* e);
#else
void MemAccessViolationSignalHandler(int sig, siginfo_t *si, void *context);
#endif



UndoContainerId CreateUndoContainer(size_t initial_size);
int	Destroy(UndoContainerId sid);
int Clear(UndoContainerId sid);

int ClearAll();

// transactions
int OpenTransaction(UndoContainerId sid);

int CommitTransaction(UndoContainerId sid);


// allocation
void* Allocate(size_t size, UndoContainerId sid);
void* AllocateChunk(size_t size, UndoContainerId sid);


void  Deallocate(void* p); // does space lookup, slower
void  Deallocate(void* p, UndoContainerId sid);


extern int kMaxUndoSize;
class SyncGpuMemory;
extern SyncGpuMemory* kSyncGpuMemory;

#endif //
