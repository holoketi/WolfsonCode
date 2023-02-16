#include "user/Vector3.h"
#include "user/Hash.h"
namespace user {

void Vector3::append(user::Hash& h) const
{
	h.append(x);
	h.append(y);
	h.append(z);
}

}