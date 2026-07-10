#ifndef __VTX_APP_PYTHON_BINDING_TOPOLOGY_COLLECTION__
#define __VTX_APP_PYTHON_BINDING_TOPOLOGY_COLLECTION__

#include "app/python_binding/topology/actions.hpp"
#include <optional>
#include <pybind11/pybind11.h>

namespace VTX::App::PythonBinding::Topology
{
	/**
	 * @brief Make a handle from an entity and an index.
	 */
	template<typename T>
	T makeHandle( const Entity p_entity, const Index p_index )
	{
		return { p_entity, p_index };
	}

	/**
	 * @brief Get index at a specific position in a RangeList.
	 */
	inline Index getValueAt( const RangeList & p_ranges, const size_t p_index )
	{
		size_t current = 0;

		for ( const Index value : p_ranges )
		{
			if ( current == p_index )
			{
				return value;
			}
			++current;
		}

		throw pybind11::index_error( "Index out of bounds" );
	}

	/**
	 * @brief Resolve a python index.
	 */
	inline size_t resolveIndex( const pybind11::ssize_t p_index, const size_t p_size )
	{
		// Python can use negative indices to access elements from the end of a collection.
		const pybind11::ssize_t signedSize = pybind11::ssize_t( p_size );
		const pybind11::ssize_t index	   = p_index < 0 ? signedSize + p_index : p_index;

		if ( index < 0 || index >= signedSize )
		{
			throw pybind11::index_error( "Index out of bounds" );
		}

		return size_t( index );
	}

	/**
	 * @brief Slice a RangeList using a python slice.
	 */
	inline RangeList sliceRanges( const RangeList & p_ranges, const pybind11::slice & p_slice )
	{
		size_t start = 0;
		size_t stop	 = 0;
		size_t step	 = 0;
		size_t count = 0;

		if ( not p_slice.compute( p_ranges.size(), &start, &stop, &step, &count ) )
		{
			throw pybind11::error_already_set();
		}

		// Fast path for simple slices.
		if ( step == 1 && p_ranges.rangeCount() == 1 )
		{
			return RangeList( p_ranges.rangeBegin()->subRange(
				static_cast<Range::Count>( start ), static_cast<Range::Count>( count )
			) );
		}

		// Rebuild RangeList from the slice.
		RangeList result;
		size_t	  position = start;

		for ( size_t i = 0; i < count; ++i )
		{
			result.addValue( getValueAt( p_ranges, position ) );
			position += step;
		}

		return result;
	}

	/**
	 * @brief Bind a collection of handles.
	 */
	template<typename T>
	void bindCollection(
		pybind11::module_ &				p_module,
		const char * const				p_name,
		const std::optional<SystemItem> p_item = std::nullopt
	)
	{
		namespace py		 = pybind11;
		using CollectionType = Collection<T>;

		auto cls = py::class_<CollectionType>( p_module, p_name, py::module_local() );

		cls.def( "__len__", []( const CollectionType & p_collection ) { return p_collection.ranges.size(); } )
			.def(
				"__iter__",
				[]( const CollectionType & p_collection )
				{
					py::list handles;

					for ( const Index index : p_collection.ranges )
					{
						handles.append( makeHandle<T>( p_collection.entity, index ) );
					}

					return py::iter( handles );
				}
			)
			.def(
				"__getitem__",
				[]( const CollectionType & p_collection, const py::ssize_t p_index )
				{
					const size_t index = resolveIndex( p_index, p_collection.ranges.size() );
					return makeHandle<T>( p_collection.entity, getValueAt( p_collection.ranges, index ) );
				}
			)
			.def(
				"__getitem__",
				[]( const CollectionType & p_collection, const py::slice & p_slice )
				{ return CollectionType { p_collection.entity, sliceRanges( p_collection.ranges, p_slice ) }; }
			);

		if ( p_item )
		{
			cls.def(
				   "setVisible",
				   [ p_item ]( const CollectionType & p_collection, const bool p_visible )
				   { setVisible( p_collection.entity, *p_item, p_collection.ranges, p_visible ); },
				   py::arg( "visible" ) = true
			)
				.def(
					"setSelected",
					[ p_item ]( const CollectionType & p_collection, const bool p_selected, const bool p_append )
					{ setSelected( p_collection.entity, *p_item, p_collection.ranges, p_selected, p_append ); },
					py::arg( "selected" ) = true,
					py::arg( "append" )	  = false
				);
		}
	}
} // namespace VTX::App::PythonBinding::Topology

#endif
