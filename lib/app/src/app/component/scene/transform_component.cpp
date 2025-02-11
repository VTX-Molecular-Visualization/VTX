#include "app/component/scene/transform_component.hpp"
#include <util/math.hpp>

namespace VTX::App::Component::Scene
{
	Transform::Transform()
	{
		setTransform( nullptr );
		_updateRotation();
	}
	Transform::Transform( Util::Math::Transform * p_transformPtr )
	{
		setTransform( p_transformPtr );
		_updateRotation();
	}

	void Transform::setTransform( Util::Math::Transform * const p_transformPtr )
	{
		if ( p_transformPtr != nullptr )
		{
			_transformReference = p_transformPtr;
			_transformStorage	= nullptr;
		}
		else
		{
			_transformStorage	= std::make_unique<Util::Math::Transform>();
			_transformReference = _transformStorage.get();
		}
	};

	void Transform::setPosition( const Vec3f & p_position )
	{
		_transformReference->setTranslation( p_position );
		onTransform( *_transformReference );
	}
	void Transform::setPosition( const Mat4f & p_positionMat )
	{
		_transformReference->setTranslation( p_positionMat );
		onTransform( *_transformReference );
	}
	void Transform::localMove( const Vec3f & p_translation )
	{
		Vec3f localTranslation = RIGHT_AXIS * p_translation + UP_AXIS * p_translation + FRONT_AXIS * p_translation;

		localTranslation = Util::Math::castMat3( _rotation ) * localTranslation;

		_transformReference->setTranslation( _transformReference->getTranslationVector() + localTranslation );
		onTransform( *_transformReference );
	}
	void Transform::globalMove( const Vec3f & p_translation )
	{
		_transformReference->setTranslation( _transformReference->getTranslationVector() + p_translation );
		onTransform( *_transformReference );
	}
	void Transform::moveFront( const float p_distance ) { localMove( FRONT_AXIS * p_distance ); }
	void Transform::moveRight( const float p_distance ) { localMove( RIGHT_AXIS * p_distance ); }
	void Transform::moveUp( const float p_distance ) { localMove( UP_AXIS * p_distance ); }

	void Transform::setRotation( const Vec3f & p_euler )
	{
		_transformReference->setRotation( p_euler );
		_updateRotation();

		onTransform( *_transformReference );
	}
	void Transform::setRotation( const Quatf & p_quat )
	{
		_transformReference->setRotation( p_quat );
		_updateRotation( p_quat );

		onTransform( *_transformReference );
	}
	void Transform::setRotation( const Mat4f & p_rotationMat )
	{
		_transformReference->setRotation( p_rotationMat );
		_updateRotation();

		onTransform( *_transformReference );
	}

	void Transform::localRotate( const Vec3f & p_euler )
	{
		const Quatf newRotation = _rotation * Util::Math::eulerToQuaternion( p_euler );
		setRotation( newRotation );
	}
	void Transform::globalRotate( const float p_angle, const Vec3f & p_axis )
	{
		_transformReference->rotate( p_angle, p_axis );
		_updateRotation();

		onTransform( *_transformReference );
	}
	void Transform::rotatePitch( const float p_angle ) { localRotate( RIGHT_AXIS * p_angle ); }
	void Transform::rotateYaw( const float p_angle ) { localRotate( UP_AXIS * p_angle ); }
	void Transform::rotateRoll( const float p_angle ) { localRotate( FRONT_AXIS * p_angle ); }

	void Transform::setScale( const float & p_scale )
	{
		_transformReference->setScale( p_scale );
		onTransform( *_transformReference );
	}
	void Transform::setScale( const Vec3f & p_scaleVec )
	{
		_transformReference->setScale( p_scaleVec );
		onTransform( *_transformReference );
	}
	void Transform::setScale( const Mat4f & p_scaleMat )
	{
		_transformReference->setScale( p_scaleMat );
		onTransform( *_transformReference );
	}

	Vec3f Transform::getPosition() const { return _transformReference->getTranslationVector(); }
	Vec3f Transform::getFront() const { return Util::Math::castMat3( getRotation() ) * FRONT_AXIS; }
	Vec3f Transform::getRight() const { return Util::Math::castMat3( getRotation() ) * RIGHT_AXIS; }
	Vec3f Transform::getUp() const { return Util::Math::castMat3( getRotation() ) * UP_AXIS; }

	Quatf Transform::getRotation() const { return _rotation; }
	Vec3f Transform::getEuler() const
	{
		return Util::Math::rotationMatrixToEuler( _transformReference->getRotation() );
	}

	Vec3f Transform::getScale() const { return _transformReference->getScaleVector(); }

	void Transform::set( const Vec3f & p_position, const Quatf & p_rotation )
	{
		_transformReference->setTranslation( p_position );
		_transformReference->setRotation( p_rotation );
		_updateRotation( p_rotation );

		onTransform( *_transformReference );
	}
	void Transform::set( const Vec3f & p_position, const Quatf & p_rotation, const Vec3f & p_scale )
	{
		_transformReference->setTranslation( p_position );
		_transformReference->setRotation( p_rotation );
		_transformReference->setScale( p_scale );

		_updateRotation( p_rotation );

		onTransform( *_transformReference );
	}

	void Transform::setRotationAround( const Quatf & p_rotation, const Vec3f & p_target, const float p_distance )
	{
		const Quatf rotation = Util::Math::normalize( p_rotation );
		const Vec3f position = rotation * Vec3f( 0.f, 0.f, p_distance ) + p_target;

		set( position, rotation );
	}

	void Transform::rotateAround( const Quatf & p_rotation, const Vec3f & p_target, const float p_distance )
	{
		const Quatf rotation = Util::Math::normalize( getRotation() * p_rotation );
		const Vec3f position = rotation * Vec3f( 0.f, 0.f, p_distance ) + p_target;

		set( position, rotation );
	}

	void Transform::lookAt( const Vec3f & p_target, const Vec3f & p_up )
	{
		const Mat4f rotationMat = Util::Math::lookAt( getPosition(), p_target, p_up );
		setRotation( rotationMat );
	}

	void Transform::_updateRotation() { _rotation = Util::Math::toQuat( _transformReference->getRotation() ); }
	void Transform::_updateRotation( const Quatf & p_rotation ) { _rotation = p_rotation; }

	void Transform::applyTransform( const Util::Math::Transform & p_transform )
	{
		*_transformReference = p_transform;
		onTransform( *_transformReference );
	}

} // namespace VTX::App::Component::Scene
