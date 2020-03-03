#ifndef __VTX_BASE_SELECTABLE__
#define __VTX_BASE_SELECTABLE__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Generic
	{
		class BaseSelectable
		{
		  public:
			virtual ~BaseSelectable() = default;

			bool		 isSelected() const { return _isSelected; }
			virtual void setSelected( const bool p_selected ) { _isSelected = p_selected; }

		  private:
			bool _isSelected = false;
		};
	} // namespace Generic
} // namespace VTX
#endif
