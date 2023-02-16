#ifndef __Animation_h
#define __Animation_h

#include <vector>
#include "model/kernel.h"

namespace model_kernel {

class Animation;
class AnimationChannel;
class AnimationCurve;

typedef std::vector<class Animation*, Allocator<class Animation*> > AnimationList;
typedef std::vector<class AnimationChannel*, Allocator<class AnimationChannel*> > AnimationChannelList; /**< A dynamically-sized array of animation channels. */
typedef std::vector<class AnimationCurve*, Allocator<class AnimationCurve*> > AnimationCurveList; /**< A dynamically-sized array of animation curves. */

/**
	A COLLADA animation entity.
	An animation entity contains a list of child animation entities,
	in order to form a tree of animation entities.
	It also hold a list of animation channels, which hold the information
	to generate animation curves.

	In other words, the animation entity is a structural class
	used to group animation channels hierarchically.

	@ingroup ocument
*/
class Animation
{
private:


	StringType id;

	// Animation hierarchy
	Animation* parent;
	AnimationList children;
	AnimationChannelList channels;

public:

	DECLARE_CLASS(Animation);

	/** Constructor. Do not use directly.
		Instead, use the Library::AddEntity function
		or the AddChild function, depending on the
		hierarchical level of the animation entity.
		@param document The FCollada document that owns the animation entity.
		@param parent The parent animation entity. This pointer will be NULL for root animation entities. */
	Animation(Animation* parent = NULL);

	/** Destructor .*/
	virtual ~Animation();


	/** Retrieves the parent of the animation structure.
		@return The animation parent. This pointer will be NULL
			to indicate a root-level animation structure that is
			contained within the animation library. */
	inline Animation* GetParent() { return parent; }
	inline const Animation* GetParent() const { return parent; } /**< See above. */
    
	/** Copies the animation tree into a clone.
		The clone may reside in another document.
		@param clone The empty clone. If this pointer is NULL, a new animation tree
			will be created and you will need to release the returned pointer manually.
		@param cloneChildren Whether to recursively clone this entity's children.
		@return The clone. */
	virtual Animation* Clone(Animation* clone = NULL, bool cloneChildren = false) const;

	/** Retrieves the entity with the given COLLADA id.
		This function will look through the local sub-tree of animations
		for the given COLLADA id.
		@param daeId A COLLADA id.
		@return The animation entity that matches the COLLADA id. This pointer
			will be NULL if there are no animation entities that matches the COLLADA id. */
	virtual Animation* FindDaeId(const WString& daeId) { return const_cast<Animation*>(const_cast<const Animation*>(this)->FindDaeId(daeId)); }
	virtual const Animation* FindDaeId(const WString& daeId) const; /**< See above. */


	void SetId(const WString& s) {id.resize(s.size()); for (int i = 0 ; i < s.size() ; i++) id[i] = s[i]; }
	WString GetId() const { WString s; s.resize(id.size());  for (int i = 0 ; i < s.size() ; i++) s[i] = id[i]; return s; }

	/** Retrieves the number of animation entity sub-trees contained
		by this animation entity tree.
		@return The number of animation entity sub-trees. */
	inline size_t GetChildrenCount() const { return children.size(); }

	/** Retrieves an animation entity sub-tree contained by this
		animation entity tree.
		@param index The index of the sub-tree.
		@return The animation entity sub-tree at the given index. This pointer will
			be NULL if the index is out-of-bounds. */
	inline Animation* GetChild(size_t index) { return children.at(index); }
	inline const Animation* GetChild(size_t index) const { return children.at(index); } /**< See above. */

	/** Creates a new animation entity sub-tree contained within this animation entity tree.
		@return The new animation sub-tree. */
	Animation* AddChild();


	/** Retrieves the animation channels that target the given COLLADA target pointer.
		@param pointer A COLLADA target pointer.
		@param targetChannels A list of animation channels to fill in.
			This list is not cleared. */
	void FindAnimationChannels(const WString& pointer, AnimationChannelList& targetChannels);

	/** Retrieves the number of animation channels at this level within the animation tree.
		@return The number of animation channels. */
	size_t GetChannelCount() const { return channels.size(); }

	/** Retrieves an animation channel contained by this animation entity.
		@param index The index of the channel.
		@return The channel at the given index. This pointer will be NULL
			if the index is out-of-bounds. */
	AnimationChannel* GetChannel(size_t index) { return channels.at(index); }
	const AnimationChannel* GetChannel(size_t index) const { return channels.at(index); } /**< See above. */


	/** Adds a new animation channel to this animation entity.
		@return The new animation channel. */
	AnimationChannel* AddChannel();

	/** Retrieves all the curves created in the subtree of this animation element.
		@param curves A list of animation curves to fill in.
			This list is not cleared. */
	void GetCurves(AnimationCurveList& curves);
};

}

#endif