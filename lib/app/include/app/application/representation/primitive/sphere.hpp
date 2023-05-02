#ifndef __VTX_APP_APPLICATION_REPRESENTATION_PRIMITIVE_SPHERE__
#define __VTX_APP_APPLICATION_REPRESENTATION_PRIMITIVE_SPHERE__

namespace VTX::App::Application::Representation::Primitive
{
	struct Sphere
	{
	  public:
		Sphere();

		float radiusFixed;
		float radiusAdd;
		bool  isRadiusFixed;
	};

} // namespace VTX::App::Application::Representation::Primitive
#endif
