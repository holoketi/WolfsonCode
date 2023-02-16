#ifndef __Animation_Channel_h
#define __Animation_Channel_h

#include <vector>
#include <model/kernel.h>

namespace model_kernel {

class Animation;
class AnimationCurve;

typedef std::vector<class AnimationCurve*, Allocator<class AnimationCurve*> > AnimationCurveList; /**< A dynamically-sized array of animation curves. */

/**
	A COLLADA animation channel.
	Each animation channel holds the animation curves for one animatable element,
	such as a single floating-point value, a 3D vector or a matrix.

	@see Animated
	@ingroup ocument
*/
class AnimationChannel
{
private:

	Animation* parent;
	StringType targetPointer;
	StringType targetQualifier;

	AnimationCurveList curves;

public:

	DECLARE_CLASS(AnimationChannel);
	/** Constructor: do not use directly.
		Instead, call the Animation::AddChannel function.
		@param document The COLLADA document that owns the animation channel.
		@param parent The animation sub-tree that contains the animation channel. */
	AnimationChannel(Animation* parent);

	/** Destructor. */
	virtual ~AnimationChannel();
	
		/** Copies the animation channel into a clone.
		The clone may reside in another document.
		@param clone The empty clone. If this pointer is NULL, a new animation channel
			will be created and you will need to release the returned pointer manually.
		@return The clone. */
	AnimationChannel* Clone(AnimationChannel* clone = NULL) const;

	/** Set target Pointer and Qualifier*/
	void SetTarget(WString& s) { targetPointer.resize(s.size()); for (int i = 0 ; i < s.size() ; i++) targetPointer[i] = s[i]; }
	void SetTargetQualifier(WString& s){ targetQualifier.resize(s.size()); for (int i = 0 ; i < s.size() ; i++) targetQualifier[i] = s[i]; }

	WString getTarget() { WString s; s.resize(targetPointer.size());  for (int i = 0 ; i < s.size() ; i++) s[i] = targetPointer[i]; return s; }
	WString getTargetQualifier() { WString s; s.resize(targetQualifier.size());  for (int i = 0 ; i < s.size() ; i++) s[i] = targetQualifier[i]; return s;}
	/** Retrieves the animation sub-tree that contains the animation channel.
		@return The parent animation sub-tree. */
	Animation* GetParent() { return parent; }
	const Animation* GetParent() const { return parent; } /**< See above. */

	void SetParent(Animation* a) { parent = a; }
	/** Retrieves the number of animation curves contained within the channel.
		@return The number of animation curves. */
	size_t GetCurveCount() const { return curves.size(); }

	/** Retrieves an animation curve contained within the channel.
		@param index The index of the animation curve.
		@return The animation curve at the given index. This pointer will be NULL
			if the index is out-of-bounds. */
	AnimationCurve* GetCurve(size_t index) { return curves.at(index); }
	const AnimationCurve* GetCurve(size_t index) const { return curves.at(index); } /**< See above. */

	/** Adds a new animation curve to this animation channel.
		@return The new animation curve. */
	AnimationCurve* AddCurve();
};
}
#endif