#include "model/ImportedModel.h"
#include <model/rel_mem_manager.h>
#include <model/MemManager.h>
#include <model/sync_gpu_memory.h>
#include <graphics/unsigned.h>

#define RM_INCREMENT_COUNT 1000

namespace model_kernel {

ImportedModel::ImportedModel():render_table_()
{
}

ImportedModel::~ImportedModel()
{
	MaterialRenderTable::iterator i = render_table_.begin();
	for (MaterialRenderTable::iterator i = render_table_.begin() ; i != render_table_.end() ; i++) {
		RelativeMemoryManager* re = i->second;
		std::vector<graphics::ivec2> renderinfo = re->getRenderInfo();
		for (int k = 0 ; k < renderinfo.size() ; k++) {
			for (int j = renderinfo[k][0] ; j <= renderinfo[k][1] ; j++) {
				int* addr = (int*)kSyncGpuMemory->addressOfEBO(re->address(j));
				kSyncGpuMemory->deallocVertexBufferElement(addr[0]);
				kSyncGpuMemory->deallocVertexBufferElement(addr[1]);
				kSyncGpuMemory->deallocVertexBufferElement(addr[2]);
				memset(addr, 0, sizeof(int)*3);
				re->Deallocate(j, 1);			
			}
		}
	}
}

float*	ImportedModel::addressOfVertexBuffer(int index)
{
	return (float*)kSyncGpuMemory->addressOfVertexBufferElement(index);
}


float*  ImportedModel::getTriangleVertex(RelativeMemoryManager* rm, int tri_index, int vertex_index)
{
	int* addr = addressOfElementBuffer(rm, tri_index);
	//LOG("buffer index %d %d %d\n", addr[0], addr[1], addr[2]);
	return addressOfVertexBuffer(addr[vertex_index]);
}


std::vector<int> ImportedModel::triangleIndexes(RelativeMemoryManager* re)
{
	std::vector<graphics::ivec2> renderinfo = re->getRenderInfo();
	std::vector<int> ret;
	for (int k = 0 ; k < renderinfo.size() ; k++) {
		for (int j = renderinfo[k][0] ; j <= renderinfo[k][1] ; j++) {
			ret.push_back(j);
		}
	}
	return ret;
}

RelativeMemoryManager* ImportedModel::createRelativeMemoryManager()
{
	int index;
	bool result = kSyncGpuMemory->allocElementBufferChunk(1000,index);
	if (result) {
		RelativeMemoryManager* r = new RelativeMemoryManager();
		r->CreateSpace(0, RM_INCREMENT_COUNT);
		r->setBaseAddr(index);
		
		return r;
	}
	return 0;
}

RelativeMemoryManager* ImportedModel::getRelativeMemoryManager(GMaterial* mat)
{
	MaterialRenderTable::iterator found = render_table_.find(mat);
	if (found == render_table_.end()) 
	{ 
		return createRelativeMemoryManager(mat); 
	}

	return found->second;
}

int*	ImportedModel::addressOfElementBuffer(RelativeMemoryManager* rm, int index)
{
	return (int*)kSyncGpuMemory->addressOfEBO(rm->address(index));
}

int ImportedModel::addTriangle2Gpu(RelativeMemoryManager* rm, const vec3& v1, const vec3& v2, const vec3& v3, 
						const vec3& n1, const vec3& n2, const vec3& n3,
						const uint  col,
						const vec2& uv1, const vec2& uv2, const vec2& uv3,
						const uint vertex1, const uint vertex2, const uint vertex3)
{
	int index1, index2, index3;
	void* vaddr1, *vaddr2, *vaddr3;

	kSyncGpuMemory->allocVertexBufferElement(index1, vaddr1);
	kSyncGpuMemory->allocVertexBufferElement(index2, vaddr2);
	kSyncGpuMemory->allocVertexBufferElement(index3, vaddr3);
	size_t size;

	int ret = rm->Allocate(1, size);
	if (ret == -1) {
		//LOG("icrement %d -> %d\n", rm->size(), rm->size() + RM_INCREMENT_COUNT);
		increaseRelativeMemoryManager(rm, rm->size() + RM_INCREMENT_COUNT);
		ret = rm->Allocate(1, size);
	}

	int* addr = addressOfElementBuffer(rm, ret);
	addr[0] = index1;
	addr[1] = index2;
	addr[2] = index3;

	float* faddr = (float*)vaddr1;
	faddr[0] = v1[0];
	faddr[1] = v1[1];
	faddr[2] = v1[2];
	faddr[3] = n1[0];
	faddr[4] = n1[1];
	faddr[5] = n1[2];

	uint64* uaddr = (uint64*)&faddr[6];
	uaddr[0] = col;
	faddr[8] = uv1[0];
	faddr[9] = uv1[1];
	uaddr = (uint64*)&faddr[10];
	uaddr[0] = vertex1;


	faddr = (float*)vaddr2;
	faddr[0] = v2[0];
	faddr[1] = v2[1];
	faddr[2] = v2[2];
	faddr[3] = n2[0];
	faddr[4] = n2[1];
	faddr[5] = n2[2];
	uaddr = (uint64*)&faddr[6];
	uaddr[0] = col;
	faddr[8] = uv2[0];
	faddr[9] = uv2[1];
	uaddr = (uint64*)&faddr[10];
	uaddr[0] = vertex2;

	faddr = (float*)vaddr3;
	faddr[0] = v3[0];
	faddr[1] = v3[1];
	faddr[2] = v3[2];
	faddr[3] = n3[0];
	faddr[4] = n3[1];
	faddr[5] = n3[2];
	uaddr = (uint64*)&faddr[6];
	uaddr[0] = col;
	faddr[8] = uv3[0];
	faddr[9] = uv3[1];
	uaddr = (uint64*)&faddr[10];
	uaddr[0] = vertex3;

	return ret;
}


void ImportedModel::deleteTriangleFromGpu(RelativeMemoryManager* rm, int index)
{
	int* addr = addressOfElementBuffer(rm, index);
	kSyncGpuMemory->deallocVertexBufferElement(addr[0]);
	kSyncGpuMemory->deallocVertexBufferElement(addr[1]);
	kSyncGpuMemory->deallocVertexBufferElement(addr[2]);
	memset(addr, 0, sizeof(int)*3);
	rm->Deallocate(index, 1);

}

RelativeMemoryManager* ImportedModel::createRelativeMemoryManager(GMaterial* mat)
{
	MaterialRenderTable::iterator found = render_table_.find(mat);
	if (found != render_table_.end()) return found->second;

	int index;
	bool result = kSyncGpuMemory->allocElementBufferChunk(1000,index);
	if (result) {
		RelativeMemoryManager* r = new RelativeMemoryManager();
		r->CreateSpace(0, RM_INCREMENT_COUNT);
		r->setBaseAddr(index);
		render_table_[mat] = r;
		return r;
	}
	return 0;
}

std::vector<RelativeMemoryManager*> ImportedModel::getAllRelativeMemoryManager()
{
	std::vector<RelativeMemoryManager*> ret;
	MaterialRenderTable::iterator i = render_table_.begin();
	for (MaterialRenderTable::iterator i = render_table_.begin() ; i != render_table_.end() ; i++) {
		ret.push_back(i->second);
	}

	return ret;
}
std::vector<GMaterial*> ImportedModel::getAllMaterial()
{
	std::vector<GMaterial*> ret;
	for (MaterialRenderTable::iterator i = render_table_.begin() ; i != render_table_.end() ; i++) {
		ret.push_back(i->first);
	}

	return ret;
}


bool    ImportedModel::increaseRelativeMemoryManager(RelativeMemoryManager* rm, int count_of_element)
{
	if (rm->size() >= count_of_element) return true;

	int index;
	bool result = kSyncGpuMemory->allocElementBufferChunk(count_of_element,index);
	if (!result) return false;
	unsigned long long addr = rm->baseAddr();
	unsigned long long new_base_addr = index;
	uchar* ptr1 = (uchar*)kSyncGpuMemory->addressOfEBO(addr);
	uchar* ptr2 = (uchar*)kSyncGpuMemory->addressOfEBO(new_base_addr);
	memcpy(ptr2, ptr1, sizeof(int)*3*rm->size());
	memset(ptr1,0,sizeof(int)*3*rm->size());
	kSyncGpuMemory->deallocElementBufferChunk(addr, rm->size());
	rm->setBaseAddr(index);
	rm->setNewSize(count_of_element);
	return true;
}

}