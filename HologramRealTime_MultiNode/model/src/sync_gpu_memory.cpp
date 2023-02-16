#include "model/kernel.h"
#include "model/sync_gpu_memory.h"
#include "model/MemManager.h"
#include <graphics/sys.h>
#include "model/rel_mem_manager.h"
#include <graphics/RenderEnv.h>
#include <graphics/unsigned.h>

#define PAGE_SIZE		4096


SyncGpuMemory::SyncGpuMemory()
	: mem_size_vbo_(0), mem_size_ebo_(0),pages_(), need_sync_pages_()
{
	vbo_ = 0;
	ebo_ = 0;
	edge_ebo_ = 0;
	point_ebo_ = 0;
}

void SyncGpuMemory::allocateChunkVBO(size_t page_num)
{
	host_memory_vbo_ = AllocateChunk(page_num*PAGE_SIZE, kCurrentUndoContainerId);
	GLenum error = 0;
	LOG("VBOpage count %d\n", pages_.size());
	vbo_page_count_ = pages_.size();
	mem_size_vbo_ = pages_.size() * PAGE_SIZE - 8;
	vbo_size_ = (mem_size_vbo_/VBO_ELEM_SIZE + 1)*VBO_ELEM_SIZE;
	Rv_ = new RelativeMemoryManager();
	LOG("VBO size %d\n", mem_size_vbo_);
	Rv_->CreateSpace(0, mem_size_vbo_/VBO_ELEM_SIZE);
	Rv_->setBaseAddr((unsigned __int64)host_memory_vbo_);
	LOG("VBO %p\n", host_memory_vbo_);
	initializeVBO();
}
void SyncGpuMemory::allocateChunkEBO(size_t page_num)
{
	host_memory_ebo_ = AllocateChunk(page_num*PAGE_SIZE, kCurrentUndoContainerId);
	//LOG("page count %d\n", pages_.size());
	mem_size_ebo_ = (pages_.size()-vbo_page_count_) * PAGE_SIZE - 8;
	ebo_size_ = (mem_size_ebo_/EBO_ELEM_SIZE + 1) * EBO_ELEM_SIZE;
	ebo_page_count_ = pages_.size();
	LOG("EBO page count %d\n", pages_.size()-vbo_page_count_);
	LOG("EBO size %d\n", mem_size_ebo_);
	Re_ = new RelativeMemoryManager();
	Re_->CreateSpace(0, mem_size_ebo_/EBO_ELEM_SIZE);
	Re_->setBaseAddr((unsigned __int64)host_memory_ebo_);
	LOG("EBO %p\n", (void*)Re_->baseAddr());

}

void SyncGpuMemory::allocateChunkEdgeEBO(size_t page_num)
{
	host_memory_edge_ebo_ = AllocateChunk(page_num*PAGE_SIZE, kCurrentUndoContainerId);
	//LOG("page count %d\n", pages_.size());
	mem_size_edge_ebo_ = (pages_.size()-ebo_page_count_) * PAGE_SIZE - 8;
	edge_ebo_page_count_ = pages_.size();
	LOG("Edge EBO page count %d\n", pages_.size()-ebo_page_count_);
	LOG("Edge EBO size %d\n", mem_size_edge_ebo_);
	Re_edge_ = new RelativeMemoryManager();
	Re_edge_->CreateSpace(0, mem_size_edge_ebo_/EDGE_ELEM_SIZE);
	Re_edge_->setBaseAddr((unsigned __int64)host_memory_edge_ebo_);
	LOG("edge EBO %p\n", (void*)Re_edge_->baseAddr());

}


void SyncGpuMemory::allocateChunkPointEBO(size_t page_num)
{
	host_memory_point_ebo_ = AllocateChunk(page_num*PAGE_SIZE, kCurrentUndoContainerId);
	//LOG("page count %d\n", pages_.size());
	mem_size_point_ebo_ = (pages_.size()-edge_ebo_page_count_) * PAGE_SIZE - 8;
	LOG("Point EBO page count %d\n", pages_.size()-edge_ebo_page_count_);
	LOG("Point EBO size %d\n", mem_size_point_ebo_);
	Re_point_ = new RelativeMemoryManager();
	Re_point_->CreateSpace(0, mem_size_point_ebo_/POINT_ELEM_SIZE);
	Re_point_->setBaseAddr((unsigned __int64)host_memory_point_ebo_);
	LOG("point EBO %p\n", (void*)Re_point_->baseAddr());

}

void SyncGpuMemory::initializeVBO()
{
	// Add dummy vertex!
	int index;
	float* physical_addr;
	allocVertexBufferElement(index, (void*&)physical_addr);
	float* faddr = (float*)physical_addr;
	faddr[0] = 0;
	faddr[1] = 0;
	faddr[2] = 0;
	faddr[3] = 0;
	faddr[4] = 0;
	faddr[5] = 0;
	uint64* uaddr = (uint64*)&faddr[6];
	uaddr[0] = 0;
	faddr[8] = 0;
	faddr[9] = 0;
	uaddr = (uint64*)&faddr[10];
	uaddr[0] = 0;
}

bool SyncGpuMemory::allocVertexBufferElement(int& index, void*& physical_addr)
{
	size_t size;
	int ret = Rv_->Allocate(1, size);
	if (ret == -1) return false;
	index = ret;
	physical_addr = (void*)(Rv_->baseAddr() + index * VBO_ELEM_SIZE);
	return true;
}

void SyncGpuMemory::deallocVertexBufferElement(int index)
{
	Rv_->Deallocate(index, 1);
}

void* SyncGpuMemory::addressOfVertexBufferElement(int index)
{
	return (void*)(Rv_->baseAddr() + index * VBO_ELEM_SIZE);
}

bool SyncGpuMemory::allocElementBufferChunk(int count, int& index)
{
	size_t size;
	int ret = Re_->Allocate(count, size);
	if (ret == -1) return false;
	index = ret;
	return true;
}

void SyncGpuMemory::deallocElementBufferChunk(int index, size_t size)
{
	Re_->Deallocate(index, size);
}

void* SyncGpuMemory::addressOfEBO(int index)
{
	return (void*)(Re_->baseAddr() + index * EBO_ELEM_SIZE);
}


bool SyncGpuMemory::allocEdgeElementBufferChunk(int count, int& index)
{
	size_t size;
	int ret = Re_edge_->Allocate(count, size);
	if (ret == -1) return false;
	index = ret;
	return true;
}

void SyncGpuMemory::deallocEdgeElementBufferChunk(int index, size_t size)
{
	Re_edge_->Deallocate(index, size);
}

void* SyncGpuMemory::addressOfEdgeEBO(int index)
{
	return (void*)(Re_edge_->baseAddr() + index * EDGE_ELEM_SIZE);
}

bool SyncGpuMemory::allocPointElementBufferChunk(int count, int& index)
{
	size_t size;
	int ret = Re_point_->Allocate(count, size);
	if (ret == -1) return false;
	index = ret;
	return true;
}

void SyncGpuMemory::deallocPointElementBufferChunk(int index, size_t size)
{
	Re_point_->Deallocate(index, size);
}

void* SyncGpuMemory::addressOfPointEBO(int index)
{
	return (void*)(Re_point_->baseAddr() + index * POINT_ELEM_SIZE);
}



void SyncGpuMemory::addPage(void* p)
{
	pages_.insert(p);

	need_sync_pages_.insert(p);
}

void SyncGpuMemory::addNeedSyncPage(void* p)
{
	if (pages_.find(p) != pages_.end())
		need_sync_pages_.insert(p);
}


void SyncGpuMemory::sync()
{
	if (!need_sync_pages_.size()) return;

	
	GLenum error = 0;
	if (!vbo_) {
		glGenBuffers(1, &vbo_);
		error = glGetError();
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, vbo_size_, NULL, GL_STATIC_READ);
		
		if (error) {
			LOG("error allocating buffer %d\n", error);
		}
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &ebo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_size_, NULL, GL_STATIC_READ);
		
		if (error) {
			LOG("error allocating buffer %d\n", error);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenBuffers(1, &edge_ebo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edge_ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mem_size_edge_ebo_, NULL, GL_STATIC_READ);

		if (error) {
			LOG("error allocating buffer %d\n", error);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		glGenBuffers(1, &point_ebo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, point_ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mem_size_point_ebo_, NULL, GL_STATIC_READ);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	LOG("sync %d pages\n", need_sync_pages_.size());

	std::set<void*>::iterator i = need_sync_pages_.begin();
	GLenum err = 0;
	int success = 0;

	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);

	for ( ; i != need_sync_pages_.end() ; i++) {
		unsigned __int64 addr = (unsigned __int64)*i;

		if (isVBO(addr)) {
			if (addr+8 == ((unsigned __int64)host_memory_vbo_)) {
				glBufferSubData(GL_ARRAY_BUFFER, 0, PAGE_SIZE-8, (void*)(addr + 8));
			}
			else {
				glBufferSubData(GL_ARRAY_BUFFER, addr-(unsigned __int64)host_memory_vbo_, PAGE_SIZE, (void*)(addr));
			}
			err = glGetError();
			if (err) {
				LOG("error occured %d %p, %p\n", err, *i, (void*)((unsigned __int64)host_memory_vbo_ + mem_size_vbo_));
			}
			else {
				success++;
			}
		}
		else if (isEBO(addr)) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
			if (addr+8 == ((unsigned __int64)host_memory_ebo_)) {
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, PAGE_SIZE-8, (void*)(addr + 8));
			}
			else {
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, addr-(unsigned __int64)host_memory_ebo_, PAGE_SIZE, (void*)(addr));
			}
			err = glGetError();
			if (err) {
				LOG("error %d occured %p, %p\n", err, *i, (void*)((unsigned __int64)host_memory_ebo_ + mem_size_ebo_));
			}
			else {
				success++;
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		else if (isEdgeEBO(addr)) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edge_ebo_);
			if (addr+8 == ((unsigned __int64)host_memory_edge_ebo_)) {
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, PAGE_SIZE-8, (void*)(addr + 8));
			}
			else {
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, addr-(unsigned __int64)host_memory_edge_ebo_, PAGE_SIZE, (void*)(addr));
			}
			err = glGetError();
			if (err) {
				LOG("error %d occured %p, %p\n", err, *i, (void*)((unsigned __int64)host_memory_edge_ebo_ + mem_size_edge_ebo_));
			}
			else {
				success++;
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		else if (isPointEBO(addr)) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, point_ebo_);
			if (addr+8 == ((unsigned __int64)host_memory_point_ebo_)) {
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, PAGE_SIZE-8, (void*)(addr + 8));
			}
			else {
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, addr-(unsigned __int64)host_memory_point_ebo_, PAGE_SIZE, (void*)(addr));
			}
			err = glGetError();
			if (err) {
				LOG("error %d occured %p, %p\n", err, *i, (void*)((unsigned __int64)host_memory_point_ebo_ + mem_size_point_ebo_));
			}
			else {
				success++;
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}

	LOG("success transfer %d\n", success);
	success = 0;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
}

bool SyncGpuMemory::isVBO(unsigned __int64 addr)
{
	return (((addr + 8) >= ((unsigned __int64)host_memory_vbo_)) && ((addr + 8)<((unsigned __int64)host_memory_vbo_ + mem_size_vbo_))) ? true : false;
}

bool SyncGpuMemory::isEBO(unsigned __int64 addr)
{
	return (((addr + 8) >= ((unsigned __int64)host_memory_ebo_)) && ((addr + 8)<((unsigned __int64)host_memory_ebo_ + mem_size_ebo_))) ? true : false;
}

bool SyncGpuMemory::isEdgeEBO(unsigned __int64 addr)
{
	return (((addr + 8) >= ((unsigned __int64)host_memory_edge_ebo_)) && ((addr + 8)<((unsigned __int64)host_memory_edge_ebo_ + mem_size_edge_ebo_))) ? true : false;
}

bool SyncGpuMemory::isPointEBO(unsigned __int64 addr)
{
	return   (((addr + 8) >= ((unsigned __int64)host_memory_point_ebo_)) && ((addr + 8)<((unsigned __int64)host_memory_point_ebo_ + mem_size_point_ebo_))) ? true : false;
}

void SyncGpuMemory::finish()
{
	//need_sync_pages_.clear();
	glFlush();
}