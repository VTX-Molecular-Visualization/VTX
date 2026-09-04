#include "app/python_binding/preset/color_layout.hpp"
#include "app/action/color_layout.hpp"
#include "app/action/scene.hpp"
#include "app/python_binding/preset/helpers.hpp"
#include <pybind11/stl.h>
#include <python_binding/binding/helper.hpp>
#include <renderer/color.hpp>

namespace VTX::App::PythonBinding::Preset
{
	namespace py = pybind11;

	namespace
	{
		Index _resolveColorIndex( const py::ssize_t p_index )
		{
			const py::ssize_t size	= py::ssize_t( Renderer::Color::COLOR_LAYOUT_SIZE );
			const py::ssize_t index = p_index < 0 ? size + p_index : p_index;
			if ( index < 0 || index >= size )
			{
				throw py::index_error( "Color index is outside of the layout." );
			}
			return Index( index );
		}

		Util::Color::Rgba _getColor( const ColorLayoutHandle & p_preset, const py::ssize_t p_index )
		{ return getData( p_preset ).colors[ _resolveColorIndex( p_index ) ]; }

		void _setColor(
			const ColorLayoutHandle & p_preset,
			const py::ssize_t		  p_index,
			const Util::Color::Rgba & p_color
		)
		{
			validate( p_preset );
			executeAction<Action::ColorLayout::Change>(
				Entity( p_preset.entity ), _resolveColorIndex( p_index ), Util::Color::Rgba( p_color )
			);
		}

		void _setColors( const ColorLayoutHandle & p_preset, const Renderer::Color::LayoutArray & p_colors )
		{
			validate( p_preset );
			executeAction<Action::ColorLayout::ChangeAll>(
				Entity( p_preset.entity ), Renderer::Color::LayoutArray( p_colors )
			);
		}

		void _randomize( const ColorLayoutHandle & p_preset )
		{
			validate( p_preset );
			executeAction<Action::ColorLayout::Randomize>( Entity( p_preset.entity ) );
		}

		void _apply( const ColorLayoutHandle & p_preset )
		{
			validate( p_preset );
			executeAction<Action::Scene::SetColorLayout>( Entity( p_preset.entity ) );
		}
	} // namespace

	void bindColorLayout( Module & p_vtxModule, pybind11::class_<ColorLayoutHandle> & p_binding )
	{
		VTX::PythonBinding::Helper::declareEnum<Renderer::Color::E_LAYOUT_COLOR_INDEX>(
			p_vtxModule.pyModule(), "COLOR_LAYOUT_INDEX"
		);

		p_binding
			.def_property(
				"colors", []( const ColorLayoutHandle & p_preset ) { return getData( p_preset ).colors; }, &_setColors
			)
			.def_property_readonly(
				"colorCount", []( const ColorLayoutHandle & p_preset ) { return getData( p_preset ).colors.size(); }
			)
			.def( "getColor", &_getColor, py::arg( "index" ) )
			.def( "randomize", &_randomize )
			.def( "apply", &_apply, "Apply this color layout preset to the scene." )
			.def( "__len__", []( const ColorLayoutHandle & p_preset ) { return getData( p_preset ).colors.size(); } )
			.def( "__getitem__", &_getColor )
			.def( "__setitem__", &_setColor );
	}
} // namespace VTX::App::PythonBinding::Preset
