#ifndef OCTREE_MANAGER_H_
#define OCTREE_MANAGER_H_

#include <vector>
#include "graphics/geom.h"
#include "graphics/octree.h"
#include <set>

namespace graphics
{
	/*
	 * OctreeManager�� T�� ����Ʈ�� �� leaf�� �����ϴ� octree�� �����Ѵ�.
	 * �� Ŭ������ bounding box �ϳ��� �����ڿ��� ������ �ش� �ν��Ͻ��� Insert�� box3�� �Ѱ��ָ�
	 * �� box3�� bounding box�� normalize�ؼ� �� box3�� ������ ����ִ� octree��忡 Insert�� �Բ� �Ѱ��� TŸ���� ���ڸ� leaf�� �����Ѵ�.
	 * Search�� ����ϸ� ��ǥ�� octree leaf�� ����ϰ� �� leaf�� ����ִ� T ����Ʈ�� ��ȯ�Ѵ�.
	 */
	template< typename T>
	class OctreeManager
	{
	public:
		typedef std::vector<T> LeafItem;	
		/*
		 * exponent
		 * pow(2, exponent)�� octree ũ�Ⱑ �����ȴ�.
		 * box
		 * bounding box�� ũ��
		 */
		OctreeManager(int exponent, box3);
		const LeafItem& At(const vec3&) const;
		LeafItem& At(const vec3&);
		void At(const box3& bbox, std::set<T>& ret) const;
		void Insert(const box3& bbox, T id);
		Octree<LeafItem> octree() { return octree_; }
		void At(int i, int j, int k, std::set<T>& ret) const;
	public:
		int num_of_octree_node_;
		Octree< LeafItem > octree_;
		box3 bounding_box_;
		real xmin,ymin,zmin,xmax,ymax,zmax;
		real xwidth, ywidth, zwidth;
	};

#include "graphics/octree_manager.inl"
}

#endif