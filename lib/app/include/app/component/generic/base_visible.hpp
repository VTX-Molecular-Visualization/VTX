#ifndef __VTX_APP_COMPONENT_GENERIC_BASE_VISIBLE__
#define __VTX_APP_COMPONENT_GENERIC_BASE_VISIBLE__

namespace VTX::App::Component::Generic
{
	class BaseVisible
	{
	  public:
		inline bool isVisible() const { return _visible; }
		inline void setVisible( const bool p_visible ) { _visible = p_visible; }

	  protected:
		bool _visible = true;
	};
} // namespace VTX::App::Component::Generic
#endif
