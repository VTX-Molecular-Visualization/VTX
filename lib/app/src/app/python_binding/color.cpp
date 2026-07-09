#include "app/python_binding/color.hpp"
#include "app/action/color.hpp"
#include "app/ecs.hpp"
#include "app/system/color.hpp"
#include <core/struct/topology.hpp>
#include <optional>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <python_binding/binding/entity_caster.hpp>
#include <python_binding/binding/helper.hpp>
#include <python_binding/wrapper/arg.hpp>
#include <renderer/color.hpp>

namespace VTX::App::PythonBinding
{
	namespace
	{
		using SystemItem = Core::Struct::E_SYSTEM_ITEM;
		using RangeList	 = Core::Struct::IndexRangeList;
		using ColorIndex = Renderer::ColorIndex;

		void _validateSystem( const Entity p_ent )
		{
			if ( not REG().all_of<Core::Struct::Topology, App::System::Color>( p_ent ) )
			{
				throw pybind11::value_error( "Invalid system id." );
			}
		}

		void _validateColorIndex( const bool p_custom, const std::optional<ColorIndex> p_colorIndex )
		{
			if ( p_custom != p_colorIndex.has_value() )
			{
				throw pybind11::value_error( "A color index is required only for custom color schemes." );
			}
			if ( p_colorIndex
				 && ( *p_colorIndex < Renderer::Color::LAYOUT_OFFSET_CUSTOM
					  || *p_colorIndex
							 >= Renderer::Color::LAYOUT_OFFSET_CUSTOM + Renderer::Color::LAYOUT_COUNT_CUSTOM ) )
			{
				throw pybind11::value_error( "Invalid custom color index." );
			}
		}
	} // namespace

	void ColorBinder::bind( Module & p_vtxModule )
	{
		pybind11::module_ * module = nullptr;
		p_vtxModule.getPythonModule( &module );

		VTX::PythonBinding::Helper::declareEnum<Renderer::E_COLOR_SCHEME>( *module, "COLOR_SCHEME" );
		VTX::PythonBinding::Helper::declareEnum<Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE>(
			*module, "SECONDARY_STRUCTURE_COLOR_SCHEME"
		);

		p_vtxModule.def(
			"setColorScheme",
			[]( const Entity					p_ent,
				const Renderer::E_COLOR_SCHEME	p_scheme,
				const SystemItem				p_item,
				const RangeList &				p_ranges,
				const std::optional<ColorIndex> p_colorIndex )
			{
				_validateSystem( p_ent );
				_validateColorIndex(
					p_scheme == Renderer::E_COLOR_SCHEME::CUSTOM || p_scheme == Renderer::E_COLOR_SCHEME::CARBON_CUSTOM,
					p_colorIndex
				);
				executeAction<App::Action::Color::AddItem>( p_ent, p_item, p_scheme, p_ranges, p_colorIndex );
			},
			"Set the color scheme of system items.",
			VTX::PythonBinding::Wrapper::Arg( "systemId" ),
			VTX::PythonBinding::Wrapper::Arg( "scheme" ),
			VTX::PythonBinding::Wrapper::VArg<SystemItem>( "item", SystemItem::SYSTEM ),
			VTX::PythonBinding::Wrapper::VArg<RangeList>( "ranges", RangeList() ),
			VTX::PythonBinding::Wrapper::VArg<std::optional<ColorIndex>>( "colorIndex", std::optional<ColorIndex>() )
		);
		p_vtxModule.def(
			"setColorSchemeSelected",
			[]( const Renderer::E_COLOR_SCHEME p_scheme, const std::optional<ColorIndex> p_colorIndex )
			{
				_validateColorIndex(
					p_scheme == Renderer::E_COLOR_SCHEME::CUSTOM || p_scheme == Renderer::E_COLOR_SCHEME::CARBON_CUSTOM,
					p_colorIndex
				);
				executeAction<App::Action::Color::AddSelected>( p_scheme, p_colorIndex );
			},
			"Set the color scheme of the current selection.",
			VTX::PythonBinding::Wrapper::Arg( "scheme" ),
			VTX::PythonBinding::Wrapper::VArg<std::optional<ColorIndex>>( "colorIndex", std::optional<ColorIndex>() )
		);
		p_vtxModule.def(
			"setSecondaryStructureColorScheme",
			[]( const Entity									   p_ent,
				const Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE p_scheme,
				const SystemItem								   p_item,
				const RangeList &								   p_ranges,
				const std::optional<ColorIndex>					   p_colorIndex )
			{
				_validateSystem( p_ent );
				_validateColorIndex( p_scheme == Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE::CUSTOM, p_colorIndex );
				executeAction<App::Action::Color::AddSecondaryStructureItem>(
					p_ent, p_item, p_scheme, p_ranges, p_colorIndex
				);
			},
			"Set the secondary structure color scheme of system items.",
			VTX::PythonBinding::Wrapper::Arg( "systemId" ),
			VTX::PythonBinding::Wrapper::Arg( "scheme" ),
			VTX::PythonBinding::Wrapper::VArg<SystemItem>( "item", SystemItem::SYSTEM ),
			VTX::PythonBinding::Wrapper::VArg<RangeList>( "ranges", RangeList() ),
			VTX::PythonBinding::Wrapper::VArg<std::optional<ColorIndex>>( "colorIndex", std::optional<ColorIndex>() )
		);
	}
} // namespace VTX::App::PythonBinding
