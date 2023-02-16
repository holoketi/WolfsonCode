#include "model/kernel.h"
#include "model/sync_gpu_memory.h"
#include <material/MaterialDatabase.h>

extern SyncGpuMemory* kSyncGpuMemory;
unsigned int kCurrentUndoContainerId;

void initKernel() {

	unsigned int space_id = CreateUndoContainer(1000);
	kCurrentUndoContainerId = space_id;
	OpenTransaction(space_id);


	kSyncGpuMemory = new SyncGpuMemory();
	kSyncGpuMemory->allocateChunkVBO(10000); // 100000 * 4096 = 400 mega
	kSyncGpuMemory->allocateChunkEBO(10000 / 3);
	kSyncGpuMemory->allocateChunkEdgeEBO(1000 / 3);
	kSyncGpuMemory->allocateChunkPointEBO(1000 / 3);


	model_kernel::InitializeMaterialDatabase();

	CommitTransaction(kCurrentUndoContainerId);
}


void beginTransaction()
{
	OpenTransaction(kCurrentUndoContainerId);
}
void endTransaction()
{
	CommitTransaction(kCurrentUndoContainerId);
	kSyncGpuMemory->sync();
	kSyncGpuMemory->finish();
}
