#include "app/python_binding/trajectory.hpp"
#include "app/helper/trajectory.hpp"
#include <pybind11/pybind11.h>
#include <python_binding/binding/helper.hpp>
#include <span>
#include <stdexcept>
#include <util/constants.hpp>

namespace VTX::App::PythonBinding
{
	namespace py = pybind11;

	namespace
	{
		std::span<const Vec3f> _getFramePositions( const Frame & p_frame )
		{
			if ( not App::Helper::Trajectory::isFrameAvailable( p_frame.entity, static_cast<uint>( p_frame.index ) ) )
			{
				throw py::index_error( "Frame is unavailable." );
			}

			return App::Helper::Trajectory::getAtomPositions( p_frame.entity, static_cast<uint>( p_frame.index ) );
		}

		std::size_t _resolveIndex( const py::ssize_t p_index, const std::size_t p_size )
		{
			const py::ssize_t size	= static_cast<py::ssize_t>( p_size );
			const py::ssize_t index = p_index < 0 ? size + p_index : p_index;

			if ( index < 0 || index >= size )
			{
				throw py::index_error( "Index out of bounds." );
			}

			return static_cast<std::size_t>( index );
		}
	} // namespace

	void TrajectoryBinder::bind( Module & p_vtxModule )
	{
		py::module_ & module = p_vtxModule.pyModule();
		VTX::PythonBinding::Helper::declareEnum<App::System::TRAJECTORY_PLAY_MODE>( module, "TRAJECTORY_PLAY_MODE" );

		py::class_<Frame>( module, "Frame", py::module_local() )
			.def_property_readonly( "index", []( const Frame & p_frame ) { return p_frame.index; } )
			.def_property_readonly(
				"atomCount", []( const Frame & p_frame ) { return _getFramePositions( p_frame ).size(); }
			)
			.def(
				"getAtomPosition",
				[]( const Frame & p_frame, const std::size_t p_atom )
				{
					const std::span<const Vec3f> positions = _getFramePositions( p_frame );
					if ( p_atom >= positions.size() )
					{
						throw py::index_error( "Atom index is unavailable." );
					}

					return positions[ p_atom ];
				},
				py::arg( "index" )
			)
			.def( "__len__", []( const Frame & p_frame ) { return _getFramePositions( p_frame ).size(); } )
			.def(
				"__getitem__",
				[]( const Frame & p_frame, const py::ssize_t p_index )
				{
					const std::span<const Vec3f> positions = _getFramePositions( p_frame );
					return positions[ _resolveIndex( p_index, positions.size() ) ];
				}
			);

		py::class_<Trajectory>( module, "Trajectory", py::module_local() )
			.def_property_readonly(
				"frameCount",
				[]( const Trajectory & p_trajectory )
				{ return App::Helper::Trajectory::getFrameCount( p_trajectory.entity ); }
			)
			.def_property_readonly(
				"currentFrameIndex",
				[]( const Trajectory & p_trajectory )
				{
					const uint index = App::Helper::Trajectory::getCurrentFrameIndex( p_trajectory.entity );
					if ( index == TypeMax<uint> )
					{
						throw std::runtime_error( "Trajectory has no current frame." );
					}

					return index;
				}
			)
			.def(
				"isMultiFrame",
				[]( const Trajectory & p_trajectory )
				{ return App::Helper::Trajectory::hasMultiFrameTrajectory( p_trajectory.entity ); }
			)
			.def(
				"getCurrentFrame",
				[]( const Trajectory & p_trajectory )
				{
					const uint index = App::Helper::Trajectory::getCurrentFrameIndex( p_trajectory.entity );
					if ( index == TypeMax<uint> )
					{
						throw std::runtime_error( "Trajectory has no current frame." );
					}

					return Frame { p_trajectory.entity, index };
				}
			)
			.def(
				"getFrame",
				[]( const Trajectory & p_trajectory, const std::size_t p_index )
				{
					if ( p_index > TypeMax<uint>
						 || not App::Helper::Trajectory::isFrameAvailable(
							 p_trajectory.entity, static_cast<uint>( p_index )
						 ) )
					{
						throw py::index_error( "Frame is unavailable." );
					}

					return Frame { p_trajectory.entity, p_index };
				},
				py::arg( "index" )
			)
			.def(
				"__len__",
				[]( const Trajectory & p_trajectory )
				{ return App::Helper::Trajectory::getFrameCount( p_trajectory.entity ); }
			)
			.def(
				"__getitem__",
				[]( const Trajectory & p_trajectory, const py::ssize_t p_index )
				{
					const std::size_t index
						= _resolveIndex( p_index, App::Helper::Trajectory::getFrameCount( p_trajectory.entity ) );
					if ( index > TypeMax<uint>
						 || not App::Helper::Trajectory::isFrameAvailable(
							 p_trajectory.entity, static_cast<uint>( index )
						 ) )
					{
						throw py::index_error( "Frame is unavailable." );
					}

					return Frame { p_trajectory.entity, index };
				}
			);
	}
} // namespace VTX::App::PythonBinding
