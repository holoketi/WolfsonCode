#ifndef __sync_gpu_memory_h
#define __sync_gpu_memory_h

#include <set>


class RelativeMemoryManager;

class SyncGpuMemory {

public:

	SyncGpuMemory();

	void allocateChunkVBO(size_t page_num);
	void allocateChunkEBO(size_t page_num);
	void allocateChunkEdgeEBO(size_t page_num);
	void allocateChunkPointEBO(size_t page_num);

	void addPage(void* p);
	void addNeedSyncPage(void* p);
	virtual void sync();
	virtual void finish();

	// Rv allocates size 1 element only!
	bool allocVertexBufferElement(int& index, void*& physical_addr);
	void deallocVertexBufferElement(int index);
	void* addressOfVertexBufferElement(int index);

	bool allocElementBufferChunk(int count, int& index);
	void deallocElementBufferChunk(int index, size_t size);
	void* addressOfEBO(int index);


	bool allocEdgeElementBufferChunk(int count, int& index);
	void deallocEdgeElementBufferChunk(int index, size_t size);
	void* addressOfEdgeEBO(int index);

	bool allocPointElementBufferChunk(int count, int& index);
	void deallocPointElementBufferChunk(int index, size_t size);
	void* addressOfPointEBO(int index);

	bool isVBO(unsigned __int64 addr);
	bool isEBO(unsigned __int64 addr);
	bool isEdgeEBO(unsigned __int64 addr);
	bool isPointEBO(unsigned __int64 addr);

	unsigned int vbo() { return vbo_; }
	unsigned int ebo() { return ebo_; }
	unsigned int edge_ebo() { return edge_ebo_; }
	unsigned int point_ebo() { return point_ebo_; }

	virtual size_t sizeOfVertexBuffer() { return vbo_size_; }
	virtual size_t sizeOfElementBuffer() { return ebo_size_; }

	virtual void*  addressOfVertexData() { return host_memory_vbo_; }
	virtual void*  addressOfElementData() { return host_memory_ebo_; }
protected:

	void initializeVBO();

	std::set<void*> pages_;
	std::set<void*> need_sync_pages_;

	void*	host_memory_vbo_;
	size_t	mem_size_vbo_;
	size_t  vbo_size_;
	void*	host_memory_ebo_;
	size_t	mem_size_ebo_;
	size_t  ebo_size_;
	void*   host_memory_edge_ebo_;
	void*   host_memory_point_ebo_;
	size_t  mem_size_edge_ebo_;
	size_t  mem_size_point_ebo_;

	int		vbo_page_count_;
	int     ebo_page_count_;
	int     edge_ebo_page_count_;

	unsigned int vbo_;
	unsigned int ebo_;
	unsigned int edge_ebo_;
	unsigned int point_ebo_;

	RelativeMemoryManager* Rv_; //Relative VertexBufferObject
	RelativeMemoryManager* Re_; //Relative ElementBufferObject
	RelativeMemoryManager* Re_edge_; //Relative ElementBufferObject
	RelativeMemoryManager* Re_point_; //Relative ElementBufferObject
};

#endif