#ifndef __VTX_UI_WIDGET_INSPECTOR_SECTION_FLAG__
#define __VTX_UI_WIDGET_INSPECTOR_SECTION_FLAG__

namespace VTX::UI::Widget::Inspector
{
	enum class SectionFlag : int
	{
		TRANSFORM		 = 1 << 0,
		REPRESENTATION	 = 1 << 1,
		TRAJECTORY		 = 1 << 2,
		TRAJECTORY_TIMER = 1 << 3,
		AUTO_ROTATE		 = 1 << 3,
		INFOS			 = 1 << 4,

		NONE = 0,
		ALL	 = 0xFFFF
	};

	inline SectionFlag operator|( const SectionFlag p_lhs, const SectionFlag p_rhs )
	{
		return static_cast<SectionFlag>( static_cast<int>( p_lhs ) | static_cast<int>( p_rhs ) );
	}
	inline SectionFlag & operator|=( SectionFlag & lhs, const SectionFlag rhs )
	{
		lhs = lhs | rhs;
		return lhs;
	}
	inline SectionFlag operator&( const SectionFlag lhs, const SectionFlag rhs )
	{
		return static_cast<SectionFlag>( static_cast<int>( lhs ) & static_cast<int>( rhs ) );
	}
	inline SectionFlag & operator&=( SectionFlag & lhs, const SectionFlag rhs )
	{
		lhs = lhs & rhs;
		return lhs;
	}

} // namespace VTX::UI::Widget::Inspector

#endif
