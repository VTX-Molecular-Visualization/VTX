#include "app/python_binding/trajectory.hpp"
#include "app/action/trajectory.hpp"
#include "app/helper/trajectory.hpp"
#include <pybind11/pybind11.h>
#include <python_binding/binding/helper.hpp>
#include <stdexcept>
#include <util/constants.hpp>

namespace VTX::App::PythonBinding
{
	namespace py = pybind11;

	namespace
	{
		using FrameView = App::Helper::Trajectory::FrameView;

		FrameView _getFramePositions( const Frame & p_frame )
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
					const FrameView positions = _getFramePositions( p_frame );
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
					const FrameView positions = _getFramePositions( p_frame );
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
				"loadedFrameCount",
				[]( const Trajectory & p_trajectory )
				{ return App::Helper::Trajectory::getLoadedFrameCount( p_trajectory.entity ); }
			)
			.def_property_readonly(
				"availableFrames",
				[]( const Trajectory & p_trajectory )
				{ return App::Helper::Trajectory::getAvailableFrames( p_trajectory.entity ); }
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
			.def_property_readonly(
				"requestedFrameIndex",
				[]( const Trajectory & p_trajectory )
				{
					const uint index = App::Helper::Trajectory::getRequestedFrameIndex( p_trajectory.entity );
					if ( index == TypeMax<uint> )
					{
						throw std::runtime_error( "Trajectory has no requested frame." );
					}

					return index;
				}
			)
			.def_property_readonly(
				"playMode",
				[]( const Trajectory & p_trajectory )
				{ return App::Helper::Trajectory::getPlayMode( p_trajectory.entity ); }
			)
			.def_property_readonly(
				"paused",
				[]( const Trajectory & p_trajectory )
				{ return App::Helper::Trajectory::isPaused( p_trajectory.entity ); }
			)
			.def_property_readonly(
				"speed",
				[]( const Trajectory & p_trajectory )
				{ return App::Helper::Trajectory::getPlayingSpeed( p_trajectory.entity ); }
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
				{ return App::Helper::Trajectory::getLoadedFrameCount( p_trajectory.entity ); }
			)
			.def(
				"__getitem__",
				[]( const Trajectory & p_trajectory, const py::ssize_t p_index )
				{
					const std::size_t index
						= _resolveIndex( p_index, App::Helper::Trajectory::getLoadedFrameCount( p_trajectory.entity ) );
					if ( index > TypeMax<uint>
						 || not App::Helper::Trajectory::isFrameAvailable(
							 p_trajectory.entity, static_cast<uint>( index )
						 ) )
					{
						throw py::index_error( "Frame is unavailable." );
					}

					return Frame { p_trajectory.entity, index };
				}
			)
			.def(
				"toggleStartPause",
				[]( const Trajectory & p_trajectory )
				{ executeAction<App::Action::Trajectory::ToggleStartPause>( p_trajectory.entity ); }
			)
			.def(
				"play",
				[]( const Trajectory & p_trajectory )
				{ executeAction<App::Action::Trajectory::SetPaused>( p_trajectory.entity, false ); }
			)
			.def(
				"pause",
				[]( const Trajectory & p_trajectory )
				{ executeAction<App::Action::Trajectory::SetPaused>( p_trajectory.entity, true ); }
			)
			.def(
				"stop",
				[]( const Trajectory & p_trajectory )
				{ executeAction<App::Action::Trajectory::Stop>( p_trajectory.entity ); }
			)
			.def(
				"jumpTo",
				[]( const Trajectory & p_trajectory, const uint p_index )
				{ executeAction<App::Action::Trajectory::JumpTo>( p_trajectory.entity, p_index ); },
				py::arg( "index" )
			)
			.def(
				"setPlayMode",
				[]( const Trajectory & p_trajectory, const App::System::TRAJECTORY_PLAY_MODE p_mode )
				{ executeAction<App::Action::Trajectory::ChangePlayer>( p_trajectory.entity, p_mode ); },
				py::arg( "mode" )
			)
			.def(
				"setSpeed",
				[]( const Trajectory & p_trajectory, const float p_speed )
				{ executeAction<App::Action::Trajectory::ChangeSpeed>( p_trajectory.entity, p_speed ); },
				py::arg( "speed" )
			);
	}
} // namespace VTX::App::PythonBinding
