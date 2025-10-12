#ifndef __VTX_APP_LIBRARY_PRESET_COLOR_LAYOUT__
#define __VTX_APP_LIBRARY_PRESET_COLOR_LAYOUT__

#include "app/library/base_preset.hpp"
#include <core/struct/color_layout.hpp>

namespace VTX::App::Library::Preset
{
	class ColorLayout : public BasePreset<VTX::Core::Struct::ColorLayout>
	{
	  public:
		ColorLayout();

		void save() override;
		void load() override;

		void setColor( const Index p_index, const Util::Color::Rgba & p_color );
		void setColors( const VTX::Core::Struct::ColorLayoutArray & p_colors );

		const Util::Color::Rgba & getChainColor( const Index p_index ) const;
		const Util::Color::Rgba & getResidueColor( const Index p_index ) const;

		Util::Callback<const Index> onChange;
		Util::Callback<>			onChangeAll;
	};
} // namespace VTX::App::Library::Preset

#endif
