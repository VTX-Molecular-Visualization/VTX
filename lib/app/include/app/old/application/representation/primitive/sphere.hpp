#ifndef __VTX_APP_OLD_APPLICATION_REPRESENTATION_PRIMITIVE_SPHERE__
#define __VTX_APP_OLD_APPLICATION_REPRESENTATION_PRIMITIVE_SPHERE__

namespace VTX::App::Old::Application::Representation::Primitive
{
	struct Sphere
	{
	  public:
		Sphere();

		float radiusFixed;
		float radiusAdd;
		bool  isRadiusFixed;
	};

} // namespace VTX::App::Old::Application::Representation::Primitive
#endif
