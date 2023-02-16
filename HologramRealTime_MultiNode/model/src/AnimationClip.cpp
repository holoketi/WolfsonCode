#include "model/AnimationClip.h"
#include "model/Animation.h"
#include "model/AnimationChannel.h"

namespace model_kernel {


AnimationClip::AnimationClip():
	start(0.0f)
,	end(0.0f)
,   animations()
,   channels()
{
}

AnimationClip::~AnimationClip()
{
	for (int i = 0 ;i < animations.size() ; i++)
		delete animations[i];
	animations.clear();
	channels.clear();
}

void AnimationClip::AddClipChannel(AnimationChannel* ch)
{
	channels.push_back(ch);
}

Animation* AnimationClip::AddInstanceAnimation(Animation* animation)
{
	animations.push_back(animation);
	return animation;
}

std::vector<AnimationChannel*> AnimationClip::FindChannelByTargetName(const WString& s)
{
	std::vector<AnimationChannel*> ret;
	for (int i = 0; i < channels.size() ; i++) {
		WString t = channels[i]->getTarget();
		size_t splitIndex =  t.find_first_of(L"/");
		t = t.substr(0, splitIndex);
		if (t == s) {
			ret.push_back(channels[i]);
		}
	}
	return ret;
}
Animation* AnimationClip::FindAnimationWithId(const WString& id)
{
	for (int i = 0 ; i < animations.size() ; i++) {
		Animation* anim = animations[i]->FindDaeId(id);
		if (anim) return anim;
	}
	return 0;
}

}