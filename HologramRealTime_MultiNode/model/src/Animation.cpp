#include "model/Animation.h"
#include "model/AnimationChannel.h"

namespace model_kernel {


Animation::Animation(Animation* _parent)
: parent(_parent), children(), channels()
{
}

Animation::~Animation()
{
	for (int i = 0 ;i < channels.size() ; i++) {
		delete channels[i];
	}
//	childNodes.clear();
	parent = NULL;
}

Animation* Animation::Clone(Animation* _clone, bool cloneChildren) const
{
	Animation* clone = NULL;
	if (_clone == NULL) _clone = clone = new Animation( NULL);
	else clone = (Animation*) _clone;

	//Parent::Clone(_clone, cloneChildren);

	if (clone != NULL)
	{
		// Clone the channels
		for (AnimationChannelList::const_iterator it = channels.begin(); it != channels.end(); ++it)
		{
			AnimationChannel* clonedChannel = clone->AddChannel();
			(*it)->Clone(clonedChannel);
		}

		if (cloneChildren)
		{
			// Clone the animation tree children
			for (AnimationList::const_iterator it = children.begin(); it != children.end(); ++it)
			{
				Animation* clonedChild = clone->AddChild();
				(*it)->Clone(clonedChild, cloneChildren);
			}
		}
	}

	return _clone;
}

// Creates a new animation entity sub-tree contained within this animation entity tree.
Animation* Animation::AddChild()
{
	Animation* animation = new Animation(this);
	children.push_back(animation);
	return children.back();
}

// Adds a new animation channel to this animation entity.
AnimationChannel* Animation::AddChannel()
{
	AnimationChannel* channel = new AnimationChannel(this);
	channels.push_back(channel);
	return channels.back();
}

// Look for an animation children with the given COLLADA Id.
const Animation* Animation::FindDaeId(const WString& daeId) const
{ 
	WString mid = GetId();
	if (mid == daeId) return this;
	
	for (AnimationList::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		const Animation* found = (*it)->FindDaeId(daeId);
		if (found != NULL) return found;
	}
	return NULL;
}

void Animation::FindAnimationChannels(const WString& pointer, AnimationChannelList& targetChannels)
{

	for (size_t i = 0; i < GetChannelCount(); ++i)
	{
		WString target = channels[i]->getTarget();
		if (target == pointer)
		{
			targetChannels.push_back(channels[i]);
		}
	}

	// Look for channel(s) within the child animations
	for (size_t i = 0; i < GetChildrenCount(); ++i)
	{
		GetChild(i)->FindAnimationChannels(pointer, targetChannels);
	}
}

// Retrieve all the curves created under this animation element, in the animation tree
void Animation::GetCurves(AnimationCurveList& curves)
{
	// Retrieve the curves for this animation tree element
	for (AnimationChannelList::const_iterator it = channels.begin(); it != channels.end(); ++it)
	{
		size_t channelCurveCount = (*it)->GetCurveCount();
		for (size_t i = 0; i < channelCurveCount; ++i)
		{
			curves.push_back((*it)->GetCurve(i));
		}
	}

	// Retrieve the curves for the animation nodes under this one in the animation tree
	size_t childCount = children.size();
	for (size_t i = 0; i < childCount; ++i)
	{
		children[i]->GetCurves(curves);
	}
}


}