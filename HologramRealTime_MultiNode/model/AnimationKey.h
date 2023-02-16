#ifndef _ANIMATION_KEY_H_
#define _ANIMATION_KEY_H_

#include <graphics/unsigned.h>
#include <graphics/vec.h>

namespace model_kernel {
/**
	A simple animation key.
	This class is the base for the more complex one-dimensional keys
	and it is used directly for linear and step keys.

	Do not create directly.
	Instead call FCDAnimationCurve::AddKey(FUDaeInterpolation::LINEAR)
	or FCDAnimationCurve::AddKey(FUDaeInterpolation::STEP).
*/
class AnimationKey
{
public:
	/** The key input. Typically, this will be a time value, in seconds.
		For driven curves, the dimension of this value will depend on the driver. */
	float input;

	/** The key output. */
	float output;

	/** The key interpolation type.
		@see FUDaeInterpolation::Interpolation */
	uint interpolation;
};

/**
	An animation key with tangents values.
	This class is used for bezier keys and soon: for hermite keys as well.
	
	Do not create directly.
	Instead call FCDAnimationCurve::AddKey(FUDaeInterpolation::BEZIER).
*/
class AnimationKeyBezier : public AnimationKey
{
public:
	graphics::vec2 inTangent; /**< The incoming tangent value. */
	graphics::vec2 outTangent; /**< The outcoming tangent value. */
};

/**
	An animation key with tension, continuity and bias values.
	This class is used for 3dsMax TCB keys.
	
	Do not create directly.
	Instead call FCDAnimationCurve::AddKey(FUDaeInterpolation::TCB).
*/
class AnimationKeyTCB : public AnimationKey
{
public:
	float tension; /**< The tension. */
	float continuity; /**< The continuity. */
	float bias; /**< The bias. */

	float easeIn; /**< The ease-in factor. */
	float easeOut; /**< The ease-out factor. */
};

}
#endif