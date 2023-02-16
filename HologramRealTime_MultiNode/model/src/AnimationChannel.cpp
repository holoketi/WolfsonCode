#include "model/AnimationChannel.h"
#include "model/AnimationCurve.h"

namespace model_kernel {


AnimationChannel::AnimationChannel(Animation* _parent)
:	parent(_parent)
{
}

AnimationChannel::~AnimationChannel()
{
	for (int i = 0 ;i < curves.size() ; i++)
		delete curves[i];
	parent = NULL;
}

AnimationChannel* AnimationChannel::Clone(AnimationChannel* clone) const
{
	if (clone == NULL) clone = new AnimationChannel(NULL);

	// Clone the curves
	for (AnimationCurveList::const_iterator it = curves.begin(); it != curves.end(); ++it)
	{
		AnimationCurve* clonedCurve = clone->AddCurve();
		(*it)->Clone(clonedCurve, false);
	}

	return clone;
}

AnimationCurve* AnimationChannel::AddCurve()
{
	AnimationCurve* curve = new AnimationCurve(this);
	curves.push_back(curve);
	return curve;
}

}