#ifndef __Anim_Transform_h
#define __Anim_Transform_h

#include "graphics/vec.h"
#include "graphics/quater.h"
#include "graphics/misc.h"
#include "model/kernel.h"
#include "model/Matrix4.h"

namespace model_kernel {


class Transform 
{
public:
	/** The COLLADA transform types. */
	enum Type
	{
		TRANSLATION, /**< A translation(FCDTTranslation). */
		ROTATION, /**< A rotation(FCDTRotation). */
		SCALE, /**< A non-uniform scale(FCDTScale). */
		MATRIX, /**< A matrix multiplication(FCDTMatrix). */
	};

DECLARE_CLASS(Transform);

private:
	StringType sid;

public:
	/** Constructor: do not use directly.
		Instead, use the FCDSceneNode::AddTransform function.
		@param document The COLLADA document that owns the transform.
		@param parent The visual scene node that contains the transform.
			Set this pointer to NULL if this transform is not owned by a
			visual scene node. */
	Transform();

	/** Destructor. */
	virtual ~Transform();

	/** Creates a copy of a transformation.
		@param clone The transform that will be the clone.
		@return The cloned transformation. */
	virtual Transform* Clone(Transform* clone = NULL) const = 0;

	/** Retrieves the class type of the transformation.
		The class type should be used to up-case the transformation pointer.
		@return The class type. */
	virtual Type GetType() const = 0;

	/** Converts the transformation into a matrix.
		Useful for visual scene nodes with a weird transformation stack.
		@return A matrix equivalent of the transformation. */
	virtual user::Matrix4 ToMatrix() const = 0;

	/** Retrieves the wanted sub-id for this transform.
		A wanted sub-id will always be exported, even if the transform is not animated.
		But the wanted sub-id may be modified if it isn't unique within the scope.
		@return The sub-id. */
	inline const WString GetSubId() const { WString s; s.resize(sid.size());  for (int i = 0 ; i < s.size() ; i++) s[i] = sid[i]; return s; } /**< See above. */

	/** Sets the wanted sub-id for this transform.
		A wanted sub-id will always be exported, even if the transform is not animated.
		But the wanted sub-id may be modified if it isn't unique within the scope.
		@param subId The wanted sub-id. */
	void SetSubId(const WString& s) { sid.resize(s.size()); for (int i = 0 ; i < s.size() ; i++) sid[i] = s[i]; }
	
};

/**
	A COLLADA translation.
	A translation is a simple 3D displacement.

	@ingroup FCDocument
*/
class Translation : public Transform
{
private:

	graphics::vec3 translation;

public:
	/** Constructor: do not use directly.
		Instead, use the FCDSceneNode::AddTransform function with
		the TRANSLATION transformation type.
		@param document The COLLADA document that owns the translation.
		@param parent The visual scene node that contains the translation.
			Set this pointer to NULL if the translation is not owned
			by a visual scene node. */
	Translation();
	
	/** Destructor. */
	virtual ~Translation();

	/** Retrieves the transformation class type for the translation.
		@return The transformation class type: TRANSLATION. */
	virtual Type GetType() const { return TRANSLATION; }

	/** Retrieves the translation 3D displacement vector.
		This displacement vector may be animated.
		@return The displacement vector. */
	inline graphics::vec3& GetTranslation() { return translation; }
	inline const graphics::vec3& GetTranslation() const { return translation; } /**< See above. */

	/** Sets the translation 3D displacement vector.
		@param _translation The displacement vector. */
	inline void SetTranslation(const graphics::vec3& _translation) { translation = _translation; }

	/** Sets the translation 3D displacement vector.
		@param x The x-component displacement.
		@param y The y-component displacement.
		@param z The z-component displacement. */
	inline void SetTranslation(float x, float y, float z) { translation = graphics::vec3(x, y, z);  }

	/** Converts the translation into a matrix.
		@return A matrix equivalent of the translation. */
	virtual user::Matrix4 ToMatrix() const;

	/** Creates a copy of a translation.
		@param clone The transform that will be the clone.
		@return The cloned transformation. */
	virtual Transform* Clone(Transform* clone = NULL) const;
};

/**
	A COLLADA non-uniform scale.
	A non-uniform scale contains three scale factors.
	@ingroup FCDocument
*/
class Scale : public Transform
{
private:
	graphics::vec3 scale;

public:
	/** Constructor: do not use directly.
		Instead, use the FCDSceneNode::AddTransform function with
		the SCALE transformation type.
		@param document The COLLADA document that owns the non-uniform scale.
		@param parent The visual scene node that contains the non-uniform scale.
			Set this pointer to NULL if the non-uniform scale is not owned
			by a visual scene node. */
	Scale();

	/** Destructor. */
	virtual ~Scale();

	/** Retrieves the transformation class type for the non-uniform scale.
		@return The class type: SCALE. */
	virtual Type GetType() const { return SCALE; }

	/** Retrieves the factors of the non-uniform scale.
		These factors may be animated.
		@return The scale factors. */
	inline graphics::vec3& GetScale() { return scale; }
	inline const graphics::vec3& GetScale() const { return scale; } /**< See above. */

	/** Sets the factors of the non-uniform scale.
		@param _scale The scale factors. */
	inline void SetScale(const graphics::vec3& _scale) { scale = _scale;  }

	/** Sets the factors of the non-uniform scale.
		@param x The x-component scale factor.
		@param y The y-component scale factor.
		@param z The z-component scale factor. */
	inline void SetScale(float x, float y, float z) { scale = graphics::vec3(x, y, z);  }

	/** Converts the non-uniform scale into a matrix.
		@return A matrix equivalent of the non-uniform scale. */
	virtual user::Matrix4 ToMatrix() const;




	/** Creates a copy of a non-uniform scale.
		@param clone The transform that will be the clone.
		@return The cloned transformation. */
	virtual Transform* Clone(Transform* clone = NULL) const;
};

/**
	A COLLADA angle-axis rotation.
	This rotation defines an axis around which the 3D points
	are rotated by a given angle.
	@todo (clock-wise/counter-clock-wise?)
	@ingroup FCDocument
*/
class Rotation : public Transform
{
private:

	graphics::vec3 axis; /**< The axis of rotation. */
	double angle; /**< The angle of rotation. degree*/


public:
	/** Constructor: do not use directly.
		Instead, use the FCDSceneNode::AddTransform function with
		the transformation type: ROTATION.
		@param document The COLLADA document that owns the rotation.
		@param parent The visual scene node that contains the rotation.
			Set this pointer to NULL if the rotation is not owned
			by a visual scene node. */
	Rotation();
	
	/** Destructor. */
	virtual ~Rotation();

	/** Retrieves the transformation class type for the rotation.
		@return The class type: ROTATION. */
	virtual Type GetType() const { return ROTATION; }

	/** Retrieves the angle-axis value of the rotation.
		@return The angle-axis value. */
	inline graphics::vec3& GetAngleAxis() { return axis; }
	inline const graphics::vec3& GetAngleAxis() const { return axis; } /**< See above. */

	/** Sets the angle-axis value of the rotation.
		@param aa The new angle-axis value. */
	inline void SetAngleAxis(const graphics::vec3& aa, double ang) {axis = aa; angle = ang; }

	/** Retrieves the rotation axis.
		This 3D vector may be animated.
		@return The rotation axis. */
	inline graphics::vec3& GetAxis() { return axis; }
	inline const graphics::vec3& GetAxis() const { return axis; } /**< See above. */

	/** Sets the rotation axis.
		@param axis The rotation axis. */
	inline void SetAxis(const graphics::vec3& a) { axis = a; }

	/** Sets the rotation axis.
		@param x The x-component of the rotation axis.
		@param y The y-component of the rotation axis.
		@param z The z-component of the rotation axis. */
	inline void SetAxis(float x, float y, float z) { axis = graphics::vec3(x, y, z); }

	/** Retrieves the rotation angle.
		This angle may be animated.
		@return The rotation angle, in degrees. */
	inline double& GetAngle() { return angle; }
	inline const double& GetAngle() const { return angle; } /**< See above. */

	/** Sets the rotation angle.
		@param a The rotation angle, in degrees. */
	inline void SetAngle(float a) { angle = a; }

	/** Sets the rotation components
		@param axis The rotation axis.
		@param angle The rotation angle, in degrees. */
	inline void SetRotation(const graphics::vec3& aa, double ang) { SetAngleAxis(aa, ang); }

	/** Retrieves the rotation orientation.
		@return The rotation orientation quaternion. */
	inline graphics::quater GetOrientation() { return graphics::orient(graphics::radian(angle), axis); }

	/** Sets the rotation orientation.
		@param q The rotation orientation quaternion. */
	inline void SetOrientation(const quater& q) { graphics::get_rotation(q, angle, axis); angle = graphics::degree(angle); }

	/** Converts the rotation into a matrix.
		@return A matrix equivalent of the rotation. */
	virtual user::Matrix4 ToMatrix() const;


		/** Creates a copy of a rotation.
		@param clone The transform that will be the clone.
		@return The cloned transformation. */
	virtual Transform* Clone(Transform* clone = NULL) const;
};

/**
	A COLLADA matrix transformation.
	This transformation contains a matrix that should be
	multiplied to the local transformation matrix.
	@ingroup FCDocument
*/
class Matrix: public Transform
{
private:

	user::Matrix4 transform;

public:
	/** Constructor: do not use directly.
		Instead, use the FCDSceneNode::AddTransform function with
		the transformation type: MATRIX.
		@param document The COLLADA document that owns the transformation.
		@param parent The visual scene node that contains the transformation. */
	Matrix();

	/** Destructor. */
	virtual ~Matrix();
	
	/** Retrieves the transformation class type for the transformation.
		@return The class type: MATRIX. */
	virtual Type GetType() const { return MATRIX; }

	/** Retrieves the matrix for the transformation.
		All 16 values of the matrix may be animated.
		@return The transformation matrix. */
	inline user::Matrix4& GetTransform() { return transform; }
	inline const user::Matrix4& GetTransform() const { return transform; } /**< See above. */

	/** Sets the matrix for the transformation.
		@param mx The transformation matrix. */
	inline void SetTransform(const user::Matrix4& mx) { transform = mx;  }

	/** Converts the transformation into a matrix.
		For matrix transformations, that's simply the transformation matrix.
		@return The transformation matrix. */
	virtual user::Matrix4 ToMatrix() const { return transform; }


	/** Creates a copy of a matrix transformation.
		@param clone The transform that will be the clone.
		@return The cloned transformation. */
	virtual Transform* Clone(Transform* clone = NULL) const;
};

}
#endif