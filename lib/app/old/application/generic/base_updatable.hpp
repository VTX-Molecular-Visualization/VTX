#ifndef __VTX_APP_OLD_APPLICATION_GENERIC_BASE_UPDATABLE__
#define __VTX_APP_OLD_APPLICATION_GENERIC_BASE_UPDATABLE__

namespace VTX::App::Old::Application::Generic
{
	class BaseUpdatable
	{
	  public:
		virtual ~BaseUpdatable() = default;

		virtual void update( const float & p_deltaTime ) = 0;
	};
} // namespace VTX::App::Old::Application::Generic
#endif
