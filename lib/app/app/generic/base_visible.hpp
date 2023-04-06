#ifndef __VTX_BASE_VISIBLE__
#define __VTX_BASE_VISIBLE__

namespace VTX
{
	namespace Generic
	{
		class BaseVisible
		{
		  public:
			inline bool isVisible() const { return _visible; }
			inline void setVisible( const bool p_visible ) { _visible = p_visible; }

		  protected:
			bool _visible = true;
		};
	} // namespace Generic
} // namespace VTX
#endif
