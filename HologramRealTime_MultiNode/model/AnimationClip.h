#ifndef __Animation_Clip_h
#define __Animation_Clip_h

#include "model/Animation.h"

namespace model_kernel {



/**
	A COLLADA animation clip.

	Animation clips are used to group together animation segments.
	Animation clips are typically used to form complex animation sequences
	where all the curves should only be used simultaneously.

	@ingroup FCDocument
*/
class AnimationClip
{
private:

	AnimationChannelList channels;
	AnimationList animations;

	StringType id;

	float start;
	float end;

	StringType name;

public:
	DECLARE_CLASS(AnimationClip);
	/** Constructor.
		@param document The COLLADA document that holds this animation clip. */
	AnimationClip();

	/** Destructor. */
	virtual ~AnimationClip();

	void SetId(const WString& s) {id.resize(s.size()); for (int i = 0 ; i < s.size() ; i++) id[i] = s[i]; }
	WString GetId() const { WString s; s.resize(id.size());  for (int i = 0 ; i < s.size() ; i++) s[i] = id[i]; return s; }

	/** Inserts an existing curve within this animation clip.
		@param curve An animation curve to be used within this clip. */
	void AddClipChannel(AnimationChannel* ch);

	std::vector<AnimationChannel*> FindChannelByTargetName(const WString& s);
	/** Retrieves the start time marker position for this animation clip.
		When using the animation clip, all the animation curves will need
		to be synchronized in order for the animation to start at the start time.
		@return The start time marker position, in seconds. */
	float GetStart() const { return start; }

	/** Sets the start time marker position for this animation clip.
		@param _start The new start time marker position. */
	void SetStart(float _start) { start = _start; } 

	/** Retrieves the end time marker position for this animation clip.
		When using the animation clip, all the animation curves will need
		to be synchronized in order for the animation to complete at the end time.
		@return The end time marker position, in seconds. */
	float GetEnd() const { return end; }

	/** Sets the end time marker position for this animation clip.
		@param _end The end time marker position. */
	void SetEnd(float _end) { end = _end;  }

	/** Retrieves the number of instanced animations within this animation clip.
        @return The number of instanced animations. */
	inline size_t GetAnimationCount() const { return animations.size(); }

    /** Retrieves a given animation instanced by this clip.
        @param index The index of the animation to retrieve.
        @return The animation object at the given index. */
	inline Animation* GetAnimation(size_t index) const {return (Animation*) animations[index]; };
  
    Animation* FindAnimationWithId(const WString& id);

    /** [INTERNAL] Adds an animation instance.
        @param animation The animation to instance.
        @return The animation instance. */
	Animation* AddInstanceAnimation(Animation* animation);
};


}

#endif