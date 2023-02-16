#include <model/AnimationCurve.h>
#include <model/AnimationKey.h>

namespace model_kernel {

template <class T, class T2, class T3>
inline T Clamp(T val, T2 mn, T3 mx) { return (T) ((val > (T) mx) ? (T) mx : (val < (T) mn) ? (T) mn : val); }
// Uses iterative method to accurately pin-point the 't' of the Bezier 
// equation that corresponds to the current time.
static float FindT(float cp0x, float cp1x, float cp2x, float cp3x, float input, float initialGuess)
{
	float localTolerance = 0.001f;
	float highT = 1.0f;
	float lowT = 0.0f;

	//Optimize here, start with a more intuitive value than 0.5
	float midT = 0.5f;
	if (initialGuess <= 0.1) midT = 0.1f; //clamp to 10% or 90%, because if miss, the cost is too high.
	else if (initialGuess >= 0.9) midT = 0.9f;
	else midT = initialGuess;
	bool once = true;
	while ((highT-lowT) > localTolerance) {
		if (once) once = false;
		else midT = (highT - lowT) / 2.0f + lowT;
		float ti = 1.0f - midT; // (1 - t)
		float calculatedTime = cp0x*ti*ti*ti + 3*cp1x*midT*ti*ti + 3*cp2x*midT*midT*ti + cp3x*midT*midT*midT; 
		if (fabsf(calculatedTime - input) <= localTolerance) break; //If we 'fall' very close, we like it and break.
		if (calculatedTime > input) highT = midT;
		else lowT = midT;
	}
	return midT;
}

static void ComputeTCBTangent(const AnimationKey* previousKey, const AnimationKey* currentKey, const AnimationKey* nextKey, float tens, float cont, float bias, graphics::vec2& leftTangent, graphics::vec2& rightTangent)
{
	if(currentKey == NULL) return;

	// Calculate the intervals and allow for time differences of both sides.
	graphics::vec2 pCurrentMinusPrevious;
	graphics::vec2 pNextMinusCurrent;

	//If the previous key or the last key is NULL, do make one up...
	if (!previousKey) {
		if (nextKey) pCurrentMinusPrevious[0] = nextKey->input - currentKey->input;
		else pCurrentMinusPrevious[0] = 0.5f; //Case where there is only one TCB key.. should not happen.
		pCurrentMinusPrevious[1] = 0.0f;
	}
	else {
		pCurrentMinusPrevious[0] = previousKey->input - currentKey->input;
		pCurrentMinusPrevious[1] = previousKey->output - currentKey->output;
	}
	if (!nextKey) {
		if (previousKey) pNextMinusCurrent[0] = currentKey->input - previousKey->input;
		else pNextMinusCurrent[0] = 0.5f; //Case where there is only one TCB key.. ?
		pNextMinusCurrent[1] = 0.0f;
	}
	else {
		pNextMinusCurrent[0] = nextKey->input - currentKey->input;
		pNextMinusCurrent[1] = nextKey->output - currentKey->output;
	}

	//Calculate the constants applied that contain the continuity, tension, and bias.
	float k1 = ((1.0f - tens) * (1.0f - cont) * (1.0f + bias))/2;
	float k2 = ((1.0f - tens) * (1.0f + cont) * (1.0f - bias))/2;
	float k3 = ((1.0f - tens) * (1.0f + cont) * (1.0f + bias))/2;
	float k4 = ((1.0f - tens) * (1.0f - cont) * (1.0f - bias))/2;

	leftTangent[0] = k1 * pCurrentMinusPrevious[0] + k2 * pNextMinusCurrent[0];
	leftTangent[1] = k1 * pCurrentMinusPrevious[1] + k2 * pNextMinusCurrent[1];

	rightTangent[0] = k3 * pCurrentMinusPrevious[0] + k4 * pNextMinusCurrent[0];
	rightTangent[1] = k3 * pCurrentMinusPrevious[1] + k4 * pNextMinusCurrent[1];
}



AnimationCurve::AnimationCurve(AnimationChannel* _parent)
 :	parent(_parent),
 preInfinity(Infinity::CYCLE), postInfinity(Infinity::CYCLE)
{
}

AnimationCurve::~AnimationCurve()
{
	for (int i = 0 ; i < keys.size() ; i++) {
		delete keys[i];
	}
	keys.clear();

	parent = NULL;
}

void AnimationCurve::SetKeyCount(size_t count, Interpolation::Interpolation interpolation)
{
	size_t oldCount = GetKeyCount();
	if (oldCount < count)
	{
		keys.reserve(count);
		for (; oldCount < count; ++oldCount) AddKey(interpolation);
	}
	else if (count < oldCount)
	{
		for (AnimationKeyList::iterator it = keys.begin() + count; it != keys.end(); ++it) delete (*it);
		keys.resize(count);
	}

}

AnimationKey* AnimationCurve::AddKey(Interpolation::Interpolation interpolation)
{
	AnimationKey* key;
	switch (interpolation)
	{
	case Interpolation::STEP: key = (AnimationKey*) KernelAlloc(sizeof(AnimationKey)); break;
	case Interpolation::LINEAR: key = (AnimationKey*) KernelAlloc(sizeof(AnimationKey)); break;
	case Interpolation::BEZIER: key = (AnimationKey*) KernelAlloc(sizeof(AnimationKeyBezier)); break;
	case Interpolation::TCB: key = (AnimationKey*) KernelAlloc(sizeof(AnimationKeyTCB)); break;
	}
	key->interpolation = (uint) interpolation;
	keys.push_back(key);
	return key;
}


// Insert a new key into the ordered array at a certain time
AnimationKey* AnimationCurve::AddKey(Interpolation::Interpolation interpolation, float input, size_t& index)
{
	AnimationKey* key;
	switch (interpolation)
	{
	case Interpolation::STEP: key = (AnimationKey*) KernelAlloc(sizeof(AnimationKey)); break;
	case Interpolation::LINEAR: key = (AnimationKey*) KernelAlloc(sizeof(AnimationKey)); break;
	case Interpolation::BEZIER: key = (AnimationKey*) KernelAlloc(sizeof(AnimationKeyBezier)); break;
	case Interpolation::TCB: key = (AnimationKey*) KernelAlloc(sizeof(AnimationKeyTCB)); break;
	}
	key->interpolation = (uint) interpolation;
	key->input = input;
	AnimationKeyList::iterator insertIdx = keys.begin();
	AnimationKeyList::iterator finalIdx = keys.end();

	// TODO: Not cabbage search :-)
	for (index = 0; insertIdx != finalIdx; insertIdx++, index++)
	{
		if ((*insertIdx)->input > input) break;
	}

	keys.insert(insertIdx, key);
	return key;
}

bool AnimationCurve::DeleteKey(AnimationKey* key)
{
	AnimationKeyList::iterator kitr;
	for (kitr = keys.begin(); kitr != keys.end() ; kitr++) {
		if (*kitr == key) break;
	}
	if (kitr == keys.end()) return false;

	keys.erase(kitr);
	delete key;
	return true;
}


AnimationCurve* AnimationCurve::Clone(AnimationCurve* clone, bool includeClips) const
{
	if (clone == NULL) clone = new AnimationCurve(parent);


	// Pre-buffer the list of keys and clone them.
	clone->keys.clear();
	clone->keys.reserve(keys.size());
	for (AnimationKeyList::const_iterator it = keys.begin(); it != keys.end(); ++it)
	{
		AnimationKey* key = clone->AddKey((Interpolation::Interpolation) (*it)->interpolation);
		key->input = (*it)->input;
		key->output = (*it)->output;
		if ((*it)->interpolation == Interpolation::BEZIER)
		{
			AnimationKeyBezier* bkey1 = (AnimationKeyBezier*) (*it);
			AnimationKeyBezier* bkey2 = (AnimationKeyBezier*) key;
			bkey2->inTangent = bkey1->inTangent;
			bkey2->outTangent = bkey1->outTangent;
		}
		else if ((*it)->interpolation == Interpolation::TCB)
		{
			AnimationKeyTCB* tkey1 = (AnimationKeyTCB*) (*it);
			AnimationKeyTCB* tkey2 = (AnimationKeyTCB*) key;
			tkey2->tension = tkey1->tension;
			tkey2->continuity = tkey1->continuity;
			tkey2->bias = tkey1->bias;
			tkey2->easeIn = tkey1->easeIn;
			tkey2->easeOut = tkey1->easeOut;
		}
	}

	clone->preInfinity = preInfinity;
	clone->postInfinity = postInfinity;


	return clone;
}


// Main workhorse for the animation system:
// Evaluates the curve for a given input
float AnimationCurve::Evaluate(float input) const
{
	// Check for empty curves and poses (curves with 1 key).
	if (keys.size() == 0) return 0.0f;
	if (keys.size() == 1) return keys.front()->output;

	float inputStart = keys.front()->input;
	float inputEnd = keys.back()->input;
	float inputSpan = inputEnd - inputStart;
	float outputStart = keys.front()->output;
	float outputEnd = keys.back()->output;
	float outputSpan = outputEnd - outputStart;

	// Account for pre-infinity mode
	float outputOffset = 0.0f;
	if (input < inputStart)
	{
		float inputDifference = inputStart - input;
		switch (preInfinity)
		{
		case Infinity::CONSTANT: return outputStart;
		case Infinity::LINEAR: return outputStart + inputDifference * (keys[1]->output - outputStart) / (keys[1]->input - inputStart);
		case Infinity::CYCLE: { float cycleCount = ceilf(inputDifference / inputSpan); input += cycleCount * inputSpan; break; }
		case Infinity::CYCLE_RELATIVE: { float cycleCount = ceilf(inputDifference / inputSpan); input += cycleCount * inputSpan; outputOffset -= cycleCount * outputSpan; break; }
		case Infinity::OSCILLATE: { float cycleCount = ceilf(inputDifference / (2.0f * inputSpan)); input += cycleCount * 2.0f * inputSpan; input = inputEnd - fabsf(input - inputEnd); break; }
		case Infinity::UNKNOWN: default: return outputStart;
		}
	}

	// Account for post-infinity mode
	else if (input >= inputEnd)
	{
		float inputDifference = input - inputEnd;
		switch (postInfinity)
		{
		case Infinity::CONSTANT: return outputEnd;
		case Infinity::LINEAR: return outputEnd + inputDifference * (keys[keys.size() - 2]->output - outputEnd) / (keys[keys.size() - 2]->input - inputEnd);
		case Infinity::CYCLE: { float cycleCount = ceilf(inputDifference / inputSpan); input -= cycleCount * inputSpan; break; }
		case Infinity::CYCLE_RELATIVE: { float cycleCount = ceilf(inputDifference / inputSpan); input -= cycleCount * inputSpan; outputOffset += cycleCount * outputSpan; break; }
		case Infinity::OSCILLATE: { float cycleCount = ceilf(inputDifference / (2.0f * inputSpan)); input -= cycleCount * 2.0f * inputSpan; input = inputStart + fabsf(input - inputStart); break; }
		case Infinity::UNKNOWN: default: return outputEnd;
		}
	}

	// Find the current interval
	AnimationKeyList::const_iterator it, start = keys.begin(), terminate = keys.end();
	it = start;

	while (it != terminate)
	{ 
		if ((*it)->input > input) {
			terminate = it;
			break;
		}
		else start = it;
		it++;
	}
	// Linear search is more efficient on the last interval
	for (it = start; it != terminate; ++it)
	{
		if ((*it)->input >= input) break;
	}
	if (it == keys.begin()) return outputOffset + outputStart;

	// Get the keys and values for this interval
	const AnimationKey* startKey = *(it - 1);
	const AnimationKey* endKey = *it;
	float inputInterval = endKey->input - startKey->input;
	float outputInterval = endKey->output - startKey->output;

	// Interpolate the output.
	// Similar code is found in AnimationMultiCurve.cpp. If you update this, update the other one too.
	float output;
	switch (startKey->interpolation)
	{
	case Interpolation::LINEAR: {
		output = startKey->output + (input - startKey->input) / inputInterval * outputInterval;
		break; }

	case Interpolation::BEZIER: {
		if (endKey->interpolation == Interpolation::LINEAR) {
			output = startKey->output + (input - startKey->input) / inputInterval * outputInterval;
			break;
		}
		if (endKey->interpolation == Interpolation::DEFAULT || 
			endKey->interpolation == Interpolation::STEP ||
			endKey->interpolation == Interpolation::UNKNOWN) {
			output = startKey->output;
			break;
		}
		//Code that applies to both whether the endKey is Bezier or TCB.
		AnimationKeyBezier* bkey1 = (AnimationKeyBezier*) startKey;
		graphics::vec2 inTangent;
		if (endKey->interpolation == Interpolation::BEZIER) {
			inTangent = ((AnimationKeyBezier*) endKey)->inTangent;
		}
		else if (endKey->interpolation == Interpolation::TCB) {
			AnimationKeyTCB* tkey2 = (AnimationKeyTCB*) endKey;
			graphics::vec2 tempTangent;
			tempTangent[0] = tempTangent[1] = 0.0f;
			const AnimationKey* nextKey = (it + 1) < keys.end() ? (*(it + 1)) : NULL;
			ComputeTCBTangent(startKey, endKey, nextKey, tkey2->tension, tkey2->continuity, tkey2->bias, inTangent, tempTangent);
			//Change this when we've figured out the values of the vectors from TCB...
			inTangent[0] = endKey->input + inTangent[0]; 
			inTangent[1] = endKey->output + inTangent[1];
		}
		float t = (input - startKey->input) / inputInterval;
		t = FindT(bkey1->input, bkey1->outTangent[0], inTangent[0], endKey->input, input, t);
 		float b = bkey1->outTangent[1];
		float c = inTangent[1];
		float ti = 1.0f - t;
		float br = 3.0f;
		float cr = 3.0f;

		br = inputInterval / (bkey1->outTangent[0] - startKey->input);
		cr = inputInterval / (endKey->input - inTangent[0]);
		br = Clamp(br, 0.01f, 100.0f);
		cr = Clamp(cr, 0.01f, 100.0f);

		output = startKey->output * ti * ti * ti + br * b * ti * ti * t + cr * c * ti * t * t + endKey->output * t * t * t;
		break; }
	case Interpolation::TCB: {
		if (endKey->interpolation == Interpolation::LINEAR) {
			output = startKey->output + (input - startKey->input) / inputInterval * outputInterval;
			break;
		}
		if (endKey->interpolation == Interpolation::DEFAULT || 
			endKey->interpolation == Interpolation::STEP ||
			endKey->interpolation == Interpolation::UNKNOWN) {
			output = startKey->output;
			break;
		}
		// Calculate the start key's out-tangent.
		AnimationKeyTCB* tkey1 = (AnimationKeyTCB*) startKey;
		graphics::vec2 startTangent, tempTangent, endTangent;
		startTangent[0] = startTangent[1] = tempTangent[0] = tempTangent[1] = endTangent[0] = endTangent[1] = 0.0f;
		const AnimationKey* previousKey = (it - 1) > keys.begin() ? (*(it - 2)) : NULL;
		ComputeTCBTangent(previousKey, startKey, endKey, tkey1->tension, tkey1->continuity, tkey1->bias, tempTangent, startTangent);

		// Calculate the end key's in-tangent.
		float by = 0.0f, cy= 0.0f; //will be used in the Bezier equation.
		float bx = 0.0f, cx = 0.0f; //will be used in FindT.. x equivalent of the point at b and c
		if (endKey->interpolation == Interpolation::TCB) {
			AnimationKeyTCB* tkey2 = (AnimationKeyTCB*) endKey;
			const AnimationKey* nextKey = (it + 1) < keys.end() ? (*(it + 1)) : NULL;
			ComputeTCBTangent(startKey, endKey, nextKey, tkey2->tension, tkey2->continuity, tkey2->bias, endTangent, tempTangent);
			cy = endKey->output + endTangent[1]; //Assuming the tangent is GOING from the point.
			cx = endKey->output + endTangent[0];
		}
		else if (endKey->interpolation == Interpolation::BEZIER) {
			AnimationKeyBezier* tkey2 = (AnimationKeyBezier*) endKey;
			endTangent = tkey2->inTangent;
			cy = endTangent[1];
			cx = endTangent[0];
		}
		float t = (input - inputStart) / inputInterval;
		by = startKey->output - startTangent[1]; //Assuming the tangent is GOING from the point.
		bx = startKey->input - startTangent[0];

		t = FindT(tkey1->input, bx, cx, endKey->input, input, t);

		float ti = 1.0f - t;
		output = startKey->output*ti*ti*ti +
			3*by*t*ti*ti +
			3*cy*t*t*ti +
			endKey->output*t*t*t;
		break; }
	case Interpolation::STEP:
	case Interpolation::UNKNOWN:
	default:
		output = startKey->output;
		break;
	}
	return outputOffset + output;
}
}