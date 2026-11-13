#ifndef VTX_APP_PYTHON_BINDING_TOPOLOGY_ACTIONS_HPP
#define VTX_APP_PYTHON_BINDING_TOPOLOGY_ACTIONS_HPP

#include "app/action/color.hpp"
#include "app/action/representation.hpp"
#include "app/action/selection.hpp"
#include "app/action/visibility.hpp"
#include "app/helper/preset.hpp"
#include "app/python_binding/module.hpp"
#include "app/python_binding/preset/helpers.hpp"
#include "app/python_binding/preset/types.hpp"
#include "app/python_binding/topology/helpers.hpp"
#include "app/python_binding/topology/types.hpp"
#include <optional>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <renderer/color.hpp>
#include <renderer/representation.hpp>
#include <string>
#include <tuple>
#include <utility>

namespace VTX::App::PythonBinding::Topology
{
	/**
	 * @brief Execute action.
	 */
	template<typename Action, typename... Args>
	void executeSystemAction( const Entity p_entity, Args &&... p_args )
	{
		THREAD().synchronize(
			[ & ]()
			{
				getTopology( p_entity );
				Action action;
				Util::ActionManager::run( action, p_entity, std::forward<Args>( p_args )... );
			}
		);
	}

	/**
	 * @brief Set the visibility from App::Action.
	 */
	template<SystemItem ITEM>
	void setVisible( const Entity p_entity, const RangeList & p_ranges, const bool p_visible )
	{ executeSystemAction<App::Action::Visibility::SetVisible<ITEM>>( p_entity, p_ranges, p_visible ); }

	inline void setVisible(
		const Entity	  p_entity,
		const SystemItem  p_item,
		const RangeList & p_ranges,
		const bool		  p_visible
	)
	{ executeSystemAction<App::Action::Visibility::SetVisibleItem>( p_entity, p_item, p_ranges, p_visible ); }

	/**
	 * @brief Set selected from App::Action.
	 */
	template<SystemItem ITEM>
	void setSelected( const Entity p_entity, const RangeList & p_ranges, const bool p_selected, const bool p_append )
	{ executeSystemAction<App::Action::Selection::SetSelected<ITEM>>( p_entity, p_ranges, p_selected, p_append ); }

	inline void setSelected(
		const Entity	  p_entity,
		const SystemItem  p_item,
		const RangeList & p_ranges,
		const bool		  p_selected,
		const bool		  p_append
	)
	{
		executeSystemAction<App::Action::Selection::SetSelectedItem>(
			p_entity, p_item, p_ranges, p_selected, p_append
		);
	}

	using ColorIndex = Renderer::ColorIndex;
	using Target	 = std::tuple<Entity, SystemItem, RangeList>;

	/**
	 * @brief Verify existence.
	 */
	inline void validateColorIndex( const bool p_custom, const std::optional<ColorIndex> p_colorIndex )
	{
		if ( p_custom != p_colorIndex.has_value() )
		{
			throw pybind11::value_error( "A color index is required only for custom color schemes." );
		}
		if ( p_colorIndex
			 && ( *p_colorIndex < Renderer::Color::LAYOUT_OFFSET_CUSTOM
				  || *p_colorIndex >= Renderer::Color::LAYOUT_OFFSET_CUSTOM + Renderer::Color::LAYOUT_COUNT_CUSTOM ) )
		{
			throw pybind11::value_error( "Invalid custom color index." );
		}
	}

	/**
	 * @brief Verify existence.
	 */
	inline void validateColorScheme(
		const Renderer::E_COLOR_SCHEME	p_scheme,
		const std::optional<ColorIndex> p_colorIndex
	)
	{
		validateColorIndex(
			p_scheme == Renderer::E_COLOR_SCHEME::CUSTOM || p_scheme == Renderer::E_COLOR_SCHEME::CARBON_CUSTOM,
			p_colorIndex
		);
	}

	/**
	 * @brief Set color scheme.
	 */
	inline void setColorScheme(
		const Target &					p_target,
		const Renderer::E_COLOR_SCHEME	p_scheme,
		const std::optional<ColorIndex> p_colorIndex
	)
	{
		const auto & [ entity, item, ranges ] = p_target;
		validateColorScheme( p_scheme, p_colorIndex );
		executeSystemAction<App::Action::Color::AddItem>( entity, item, p_scheme, ranges, p_colorIndex );
	}

	/**
	 * @brief Set color scheme.
	 */
	inline void setSecondaryStructureColorScheme(
		const Target &									   p_target,
		const Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE p_scheme,
		const std::optional<ColorIndex>					   p_colorIndex
	)
	{
		const auto & [ entity, item, ranges ] = p_target;
		validateColorIndex( p_scheme == Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE::CUSTOM, p_colorIndex );
		executeSystemAction<App::Action::Color::AddSecondaryStructureItem>(
			entity, item, p_scheme, ranges, p_colorIndex
		);
	}

	/**
	 * @brief Get representation preset.
	 */
	inline Preset::RepresentationHandle getRepresentationPreset( const std::string & p_name )
	{
		const std::optional<Entity> preset
			= synchronizeCopy( [ &p_name ]()
							   { return Helper::Preset::getByName<Renderer::Representation>( p_name ); } );
		if ( not preset )
		{
			throw pybind11::value_error( "Representation preset not found: " + p_name );
		}
		return { *preset };
	}

	/**
	 * @brief Set representation.
	 */
	inline void setRepresentation( const Target & p_target, const Preset::RepresentationHandle & p_preset )
	{
		THREAD().synchronize(
			[ &p_target, &p_preset ]()
			{
				const auto & [ entity, item, ranges ] = p_target;
				Preset::validate( p_preset );
				executeSystemAction<App::Action::Representation::AddItem>(
					entity, item, Entity( p_preset.entity ), ranges
				);
			}
		);
	}

	/**
	 * @brief Bind topology items.
	 */
	template<SystemItem ITEM, typename T>
	pybind11::class_<T> bindState( pybind11::class_<T> p_class )
	{
		namespace py = pybind11;

		constexpr auto getRanges = []( const T & p_object )
		{
			if constexpr ( requires { p_object.index; } )
			{
				return RangeList( p_object.index );
			}
			else
			{
				return RangeList();
			}
		};
		constexpr auto getIndex = []( const T & p_object ) -> std::optional<Index>
		{
			if constexpr ( requires { p_object.index; } )
			{
				return p_object.index;
			}
			else
			{
				return std::nullopt;
			}
		};

		p_class
			.def(
				"setVisible",
				[ getRanges ]( const T & p_object, const bool p_visible )
				{ setVisible<ITEM>( p_object.entity, getRanges( p_object ), p_visible ); },
				py::arg( "visible" ) = true
			)
			.def(
				"setSelected",
				[ getRanges ]( const T & p_object, const bool p_selected, const bool p_append )
				{ setSelected<ITEM>( p_object.entity, getRanges( p_object ), p_selected, p_append ); },
				py::arg( "selected" ) = true,
				py::arg( "append" )	  = false
			)
			.def_property_readonly(
				"visibleState",
				[ getIndex ]( const T & p_object )
				{ return getVisibleState( p_object.entity, ITEM, getIndex( p_object ) ); }
			)
			.def_property_readonly(
				"selectionState",
				[ getIndex ]( const T & p_object )
				{ return getSelectionState( p_object.entity, ITEM, getIndex( p_object ) ); }
			)
			.def(
				"isVisible",
				[ getIndex ]( const T & p_object ) { return isVisible( p_object.entity, ITEM, getIndex( p_object ) ); }
			)
			.def(
				"isFullyVisible",
				[ getIndex ]( const T & p_object )
				{ return isFullyVisible( p_object.entity, ITEM, getIndex( p_object ) ); }
			)
			.def(
				"isSelected",
				[ getIndex ]( const T & p_object ) { return isSelected( p_object.entity, ITEM, getIndex( p_object ) ); }
			)
			.def(
				"isFullySelected",
				[ getIndex ]( const T & p_object )
				{ return isFullySelected( p_object.entity, ITEM, getIndex( p_object ) ); }
			);

		return p_class;
	}

	/**
	 * @brief Bind style items (color scheme, representation).
	 */
	template<typename T, typename TargetGetter>
	pybind11::class_<T> bindStyle( pybind11::class_<T> p_class, const TargetGetter p_getTarget )
	{
		namespace py = pybind11;

		p_class
			.def(
				"setColorScheme",
				[ p_getTarget ](
					const T &						p_object,
					const Renderer::E_COLOR_SCHEME	p_scheme,
					const std::optional<ColorIndex> p_colorIndex
				) { setColorScheme( p_getTarget( p_object ), p_scheme, p_colorIndex ); },
				"Set the color scheme. A color index is required for custom schemes.",
				py::arg( "scheme" ),
				py::arg( "colorIndex" ) = py::none()
			)
			.def(
				"setSecondaryStructureColorScheme",
				[ p_getTarget ](
					const T &										   p_object,
					const Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE p_scheme,
					const std::optional<ColorIndex>					   p_colorIndex
				) { setSecondaryStructureColorScheme( p_getTarget( p_object ), p_scheme, p_colorIndex ); },
				"Set the secondary structure color scheme. A color index is required for the custom scheme.",
				py::arg( "scheme" ),
				py::arg( "colorIndex" ) = py::none()
			)
			.def(
				"setRepresentation",
				[ p_getTarget ]( const T & p_object, const std::string & p_name )
				{ setRepresentation( p_getTarget( p_object ), getRepresentationPreset( p_name ) ); },
				"Apply a representation preset by name.",
				py::arg( "name" )
			)
			.def(
				"setRepresentation",
				[ p_getTarget ]( const T & p_object, const Preset::RepresentationHandle & p_preset )
				{ setRepresentation( p_getTarget( p_object ), p_preset ); },
				"Apply a representation preset.",
				py::arg( "preset" )
			);

		return p_class;
	}
} // namespace VTX::App::PythonBinding::Topology

#endif
