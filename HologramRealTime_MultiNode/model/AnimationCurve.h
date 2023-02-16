#ifndef __Animation_Curve_h
#define __Animation_Curve_h

#include <vector>
#include <graphics/unsigned.h>
#include <model/kernel.h>


namespace Infinity
{
	/** An infinity type.
		They determine what happens when evaluating an animation curve outside of its bounds. */
	enum Infinity
	{
		CONSTANT = 0, /**< Uses the output value of the closest animation key. This is the default infinity type. */
		LINEAR, /**< Takes the distance between the closest animation key input value and the evaluation time.
					Multiplies this distance against the instant slope at the closest animation key and offsets the
					result with the closest animation key output value. */
		CYCLE, /**< Iteratively removes or adds the animation curve time length to the evaluation time until it is
					within the animation curve time interval and evaluates it. */
		CYCLE_RELATIVE, /**< Iteratively removes or adds the animation curve time length to the evaluation time until it is
							within the animation curve time interval and evaluates it. Adds to the evaluation output the
							number of iteration done multiplied by the difference between the animation curve
							start and end key outputs. */
		OSCILLATE, /**< Iteratively removes or adds the animation curve time length to the evaluation time until it is
						within the animation curve time interval. If the number of iterations done is even, evaluate the
						new evaluation time, otherwise evaluate (animation curve time length - evaluation time). */

		UNKNOWN, /**< An unknown infinity type. */
		DEFAULT = CONSTANT
	};

	/** Converts the FCollada infinity type string into a infinity type.
		@param value The FCollada infinity type string.
		@return The infinity type. */
	Infinity FromString(const char* value);
	
	/** Converts the infinity type into its FCollada infinity type string.
		@param infinity The infinity type.
		@return The infinity type string. */
	const char* ToString(Infinity infinity);
	inline Infinity FromString(const std::string& value) { return FromString(value.c_str()); } /**< See above. */
};


/** Contains the animation curve interpolation types and their conversion functions. */
namespace Interpolation
{
	/** An animation curve interpolation type. */
	enum Interpolation
	{
		STEP = 0, /**< No interpolation. 
					Uses the output value of the previous key until the next key time is reached. */
		LINEAR, /**< Linear interpolation. Uses the average slope between the previous key and the next key. */
		BEZIER, /**< Bezier interpolation. Uses two 2D control points for each segment, wrongly called in-tangent and out-tangent in COLLADA. */
		TCB, /**< TCB interpolation. Uses Tension, Continuity and Bias values to generate Hermite tangents.
				This interpolation type is not standard COLLADA. */

		UNKNOWN, /**< An unknown interpolation type. */
		DEFAULT = STEP,
	};

	/** Converts the COLLADA interpolation type string into an interpolation type.
		@param value The COLLADA interpolation type string.
		@return The interpolation type. */
	Interpolation FromString(const std::string& value);

	/** Converts the interpolation type into its COLLADA type string.
		@param value The interpolation type.
		@return The COLLADA interpolation type string. */
	const char* ToString(const Interpolation& value);
};

namespace model_kernel {


class AnimationChannel;
class AnimationKey;


typedef std::vector<class AnimationKey*, Allocator<class AnimationKey*> > AnimationKeyList; /**< A dynamically-sized array of curve keys. */

/**
	A COLLADA single-dimensional animation curve.
	An animation curve holds the keyframes necessary
	to animate an animatable floating-point value.

	There are multiple interpolation mechanisms supported by COLLADA.
	FCollada supports the CONSTANT, LINEAR and BEZIER interpolations.

	@see Interpolation Infinity
	@ingroup ocument
*/
class AnimationCurve
{
private:

	// The channel owning this curve.
	AnimationChannel* parent;

	// Curve information
	AnimationKeyList keys;
	Infinity::Infinity preInfinity, postInfinity;
	
	float currentOffset;

public:

	DECLARE_CLASS(AnimationCurve);
	/** Constructor: do not use directly.
		Instead, use the AnimationChannel::AddCurve function.
		You should also attach the new curve to an animated
		element using the Animated::SetCurve function.
		@param document The COLLADA document that owns the animation curve.
		@param parent The animation channel that contains the curve. */
	AnimationCurve(AnimationChannel* parent);

	/** Destructor. */
	virtual ~AnimationCurve();

	/** Retrieves the animation channel that contains this animation curve.
		@return The parent animation channel. */
	inline AnimationChannel* GetParent() { return parent; }
	inline const AnimationChannel* GetParent() const { return parent; } /**< See above. */


	/** Retrieves the number of keys within the animation curve.
		@return The number of keys. */
	inline size_t GetKeyCount() const { return keys.size(); }

	/** Sets the number of keys within the animation curve.
		@param count The new number of keys in the curve.
		@param interpolation If creating new keys, the interpolation type
			for the new keys. */
	void SetKeyCount(size_t count, Interpolation::Interpolation interpolation);

	/** Retrieves one key in the animation curve.
		@param index The index of the key to retrieve.
		@return The key. */
	inline AnimationKey* GetKey(size_t index) {  return keys.at(index); }
	inline const AnimationKey* GetKey(size_t index) const { return keys.at(index); } /**< See above. */

	/** Appends a key to the animation curve.
		@param interpolation The interpolation type for the new key.
		@return The new key. */
	AnimationKey* AddKey(Interpolation::Interpolation interpolation);

	/** Adds a new key to the animation curve at the given time.
		@param interpolation The interpolation type for the new key.
		@param input The input (x) value of the new key.
		@return The new key. */
	AnimationKey* AddKey(Interpolation::Interpolation interpolation, float input) { size_t blah; return AddKey(interpolation, input, blah); }

	/** Adds a new key to the animation curve at the given time.
		@param interpolation The interpolation type for the new key.
		@param input The input (x) value of the new key.
		@param index [OUT] The index in the array of the new key
		@return The new key. */
	AnimationKey* AddKey(Interpolation::Interpolation interpolation, float input, size_t& index);

	/** Removes the given key from this curves list and deletes it
		@param key The key to find and delete
		@return True on success, false if the key is not found */
	bool DeleteKey(AnimationKey* key);

	/** Retrieves the type of behavior for the curve if the input value is
		outside the input interval defined by the curve keys and less than any key input value.
		@see Infinity
		@return The pre-infinity behavior of the curve. */
	inline Infinity::Infinity GetPreInfinity() const { return preInfinity; }

	/** Sets the behavior of the curve if the input value is
		outside the input interval defined by the curve keys and less than any key input value.
		@see Infinity
		@param infinity The pre-infinity behavior of the curve. */
	inline void SetPreInfinity(Infinity::Infinity infinity) { preInfinity = infinity;  }

	/** Retrieves the type of behavior for the curve if the input value is
		outside the input interval defined by the curve keys and greater than any key input value.
		@see Infinity
		@return The post-infinity behavior of the curve. */
	inline Infinity::Infinity GetPostInfinity() const { return postInfinity; }

	/** Sets the behavior of the curve if the input value is
		outside the input interval defined by the curve keys and greater than any key input value.
		@see Infinity
		@param infinity The post-infinity behavior of the curve. */
	inline void SetPostInfinity(Infinity::Infinity infinity) { postInfinity = infinity; }



	/** Clones the animation curve. The animation clips can be cloned as well,
		but this may lead to an infinite recursion because cloning the clips
		will also clone its curves.
		@param clone The cloned animation curve. If this pointer is NULL, a new
			animation curve will be created for you. You will then need to release
			the pointer.
		@param includeClips True if want to also clone the animation clips.
		@return The cloned animation curve. */
	AnimationCurve* Clone(AnimationCurve* clone = NULL, bool includeClips = true) const;


	/** Evaluates the animation curve.
		@param input An input value.
		@return The sampled value of the curve at the given input value. */
	float Evaluate(float input) const;

};


}
#endif