#include "app/component/render/proxy_camera.hpp"
#include "app/application/system/ecs_system.hpp"
#include "app/component/render/camera.hpp"
#include "app/component/scene/transform_component.hpp"
#include <util/types.hpp>

namespace VTX::App::Component::Render
{
	ProxyCamera::ProxyCamera() {}
	ProxyCamera::~ProxyCamera()
	{
		if ( _proxyWrapper.isValid() )
		{
			//_proxyWrapper.accessor().proxy().onRemove();
		}
	}

	void ProxyCamera::setInRenderer( Renderer::Facade & p_renderer )
	{
		_generateProxy();
		_initCallbacks();

		p_renderer.setProxyCamera( _proxyWrapper.proxy() );
	}

	void ProxyCamera::_generateProxy()
	{
		Component::Render::Camera &	  cameraComp = MAIN_REGISTRY().getComponent<Component::Render::Camera>( *this );
		Component::Scene::Transform & transformComp
			= MAIN_REGISTRY().getComponent<Component::Scene::Transform>( *this );

		std::unique_ptr<VTX::Renderer::Proxy::Camera> proxyPtr
			= std::make_unique<VTX::Renderer::Proxy::Camera>( VTX::Renderer::Proxy::Camera {
				&cameraComp.getViewMatrix(),
				&cameraComp.getProjectionMatrix(),

				transformComp.getPosition(),
				VEC2I_ZERO,
				cameraComp.getNear(),
				cameraComp.getFar(),
				cameraComp.getProjection() == CAMERA_PROJECTION::PERSPECTIVE,
			} );

		_proxyWrapper.setProxy( proxyPtr );
	}
	void ProxyCamera::_initCallbacks()
	{
		Component::Render::Camera & cameraComp = MAIN_REGISTRY().getComponent<Component::Render::Camera>( *this );

		cameraComp.onMatrixViewChange +=
			[ this ]( const Mat4f & p_viewMatrix ) { _proxyWrapper.proxy().onMatrixView(); };
		cameraComp.onMatrixProjectionChange +=
			[ this ]( const Mat4f & p_projMatrix ) { _proxyWrapper.proxy().onMatrixProjection(); };

		cameraComp.onClipInfosChange +=
			[ this ]( float p_near, float p_far ) { _proxyWrapper.proxy().onCameraNearFar( p_near, p_far ); };
		cameraComp.onProjectionChange += [ this ]( CAMERA_PROJECTION p_projection )
		{ _proxyWrapper.proxy().onPerspective( p_projection == CAMERA_PROJECTION::PERSPECTIVE ); };

		Component::Scene::Transform & transformComp
			= MAIN_REGISTRY().getComponent<Component::Scene::Transform>( *this );
		transformComp.onTransform += [ this ]( const Util::Math::Transform & p_transform )
		{ _proxyWrapper.proxy().onCameraPosition( p_transform.getTranslationVector() ); };
	}

} // namespace VTX::App::Component::Render
