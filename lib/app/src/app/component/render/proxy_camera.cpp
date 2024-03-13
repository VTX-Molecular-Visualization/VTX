#include "app/component/render/proxy_camera.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/vtx_app.hpp"
#include <core/chemdb/atom.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>

namespace VTX::App::Component::Render
{
	ProxyCamera::ProxyCamera() {}

	void ProxyCamera::init()
	{
		_generateProxy();
		_initCallbacks();
	}

	void ProxyCamera::_generateProxy()
	{
		Component::Render::Camera &	  cameraComp = MAIN_REGISTRY().getComponent<Component::Render::Camera>( *this );
		Component::Scene::Transform & transformComp
			= MAIN_REGISTRY().getComponent<Component::Scene::Transform>( *this );

		_proxyPtr = std::make_unique<VTX::Renderer::Proxy::Camera>( VTX::Renderer::Proxy::Camera {
			&cameraComp.getViewMatrix(),
			&cameraComp.getProjectionMatrix(),

			transformComp.getPosition(),
			VEC2I_ZERO,
			cameraComp.getNear(),
			cameraComp.getFar(),
			cameraComp.getProjection() == CAMERA_PROJECTION::PERSPECTIVE,
		} );
	}
	void ProxyCamera::_initCallbacks()
	{
		Component::Render::Camera & cameraComp = MAIN_REGISTRY().getComponent<Component::Render::Camera>( *this );

		cameraComp.onMatrixViewChange += [ this ]( const Mat4f & p_viewMatrix ) { _proxyPtr->onMatrixView(); };
		cameraComp.onMatrixProjectionChange +=
			[ this ]( const Mat4f & p_projMatrix ) { _proxyPtr->onMatrixProjection(); };

		cameraComp.onClipInfosChange +=
			[ this ]( float p_near, float p_far ) { _proxyPtr->onCameraNearFar( p_near, p_far ); };
		cameraComp.onProjectionChange += [ this ]( CAMERA_PROJECTION p_projection )
		{ _proxyPtr->onPerspective( p_projection == CAMERA_PROJECTION::PERSPECTIVE ); };

		// Component::Scene::Transform & transformComp
		//	= MAIN_REGISTRY().getComponent<Component::Scene::Transform>( *this );
		// transformComp.onTransformChanged += [ this ]( const Util::Math::Transform & p_transform )
		//{ _proxyPtr->onCameraPosition( p_transform.getTranslationVector() ); };
	}

} // namespace VTX::App::Component::Render
