#include "user/Vector4.h"
#include "user/Hash.h"
namespace user {

void Vector4::append(user::Hash& h) const
{
	h.append(x);
	h.append(y);
	h.append(z);
	h.append(w);
}

}