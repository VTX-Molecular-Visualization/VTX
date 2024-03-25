#ifndef __VTX_APP_COMPONENT_REPRESENTATION_COLOR_LAYOUT__
#define __VTX_APP_COMPONENT_REPRESENTATION_COLOR_LAYOUT__

#include "app/core/ecs/base_component.hpp"
#include <core/chemdb/color.hpp>
#include <util/callback.hpp>

namespace VTX::App::Component::Representation
{

	class ColorLayout : public Core::ECS::BaseComponent
	{
	  public:
		ColorLayout() = default;

		const VTX::Core::ChemDB::Color::ColorLayout & getLayout() const { return _layout; }

		void setColor( const size_t p_index, const Util::Color::Rgba & p_color );
		void setColors( const std::vector<Util::Color::Rgba> & p_colors );

		Util::Callback<> onColorChange;

	  private:
		VTX::Core::ChemDB::Color::ColorLayout _layout;
	};

} // namespace VTX::App::Component::Representation

#endif
