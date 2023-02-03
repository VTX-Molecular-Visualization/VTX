#ifndef __VTX_BASE_RENDERABLE__
#define __VTX_BASE_RENDERABLE__

namespace VTX
{
	namespace Object3D
	{
		class Camera;
	}

	namespace Generic
	{
		class BaseRenderable
		{
		  public:
			virtual ~BaseRenderable() = default;

			virtual void render( const Object3D::Camera & ) const = 0;
			virtual void init() {};
		};
	} // namespace Generic
} // namespace VTX
#endif
