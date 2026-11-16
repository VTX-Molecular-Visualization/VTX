#include "app/python_binding/trajectory.hpp"
#include "app/action/trajectory.hpp"
#include "app/helper/trajectory.hpp"
#include "app/python_binding/identity.hpp"
#include <cmath>
#include <core/struct/trajectory.hpp>
#include <pybind11/pybind11.h>
#include <python_binding/binding/helper.hpp>
#include <stdexcept>
#include <util/constants.hpp>
#include <utility>

namespace VTX::App::PythonBinding
{
	namespace py = pybind11;

	namespace
	{
		struct TrajectoryState
		{
			std::size_t					frameCount = 0;
			App::Trajectory::FrameRange availableFrames;
			uint						currentFrameIndex	= 0;
			uint						requestedFrameIndex = 0;
			App::Trajectory::PLAY_MODE	playMode			= App::Trajectory::PLAY_MODE::NONE;
			bool						paused				= true;
			float						speed				= 0.f;
		};

		void _validateTrajectory( const Entity p_entity )
		{
			if ( not REG().valid( p_entity ) || not REG().all_of<Core::Struct::Trajectory>( p_entity ) )
			{
				throw py::value_error( "System has no trajectory data." );
			}
		}

		void _validatePlayer( const Entity p_entity )
		{
			_validateTrajectory( p_entity );
			if ( not REG().all_of<App::Trajectory::Player>( p_entity ) )
			{
				throw py::value_error( "System trajectory is not playable." );
			}
		}

		template<typename Action, typename... Args>
		void _executeTrajectoryAction( const Trajectory & p_trajectory, Args &&... p_args )
		{
			THREAD().synchronize(
				[ & ]()
				{
					_validatePlayer( p_trajectory.entity );
					Action action;
					Util::ActionManager::run( action, p_trajectory.entity, std::forward<Args>( p_args )... );
				}
			);
		}

		TrajectoryState _getTrajectoryState( const Entity p_entity )
		{
			return synchronizeCopy(
				[ p_entity ]
				{
					_validateTrajectory( p_entity );

					const Core::Struct::Trajectory & trajectory = REG().get<Core::Struct::Trajectory>( p_entity );
					TrajectoryState					 state;
					state.frameCount	  = trajectory.frameCount;
					state.availableFrames = App::Helper::Trajectory::getAvailableFrames( p_entity );
					if ( const auto * const player = REG().try_get<App::Trajectory::Player>( p_entity ) )
					{
						state.currentFrameIndex	  = player->currentFrameIndex;
						state.requestedFrameIndex = player->requestedFrameIndex;
						state.playMode			  = player->playMode;
						state.paused			  = player->paused;
						state.speed				  = player->playingSpeed;
					}
					return state;
				}
			);
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

		std::size_t _getFrameAtomCount( const Frame & p_frame ) { return p_frame.positions.size(); }

		Vec3f _getFrameAtomPosition( const Frame & p_frame, const py::ssize_t p_atom )
		{ return p_frame.positions[ _resolveIndex( p_atom, p_frame.positions.size() ) ]; }

		template<typename Func>
		Frame _readFrame( const Func & p_read )
		{
			try
			{
				return synchronizeCopy( p_read );
			}
			catch ( const std::out_of_range & )
			{
				throw py::index_error( "Frame index out of bounds." );
			}
			catch ( const std::invalid_argument & )
			{
				throw py::value_error( "Invalid trajectory." );
			}
		}

		Frame _getFrame( const Entity p_entity, const std::size_t p_index )
		{
			if ( p_index > TypeMax<uint> )
			{
				throw py::index_error( "Frame index out of bounds." );
			}

			return _readFrame(
				[ p_entity, p_index ]
				{
					return Frame { p_index,
								   App::Helper::Trajectory::getFrame( p_entity, static_cast<uint>( p_index ) ) };
				}
			);
		}
	} // namespace

	Frame getCurrentFrame( const Entity p_entity )
	{
		return _readFrame(
			[ p_entity ]
			{
				_validateTrajectory( p_entity );
				const auto * const player = REG().try_get<App::Trajectory::Player>( p_entity );
				const uint		   index  = player ? player->currentFrameIndex : 0;
				return Frame { index, App::Helper::Trajectory::getFrame( p_entity, index ) };
			}
		);
	}

	void TrajectoryBinder::bind( Module & p_vtxModule )
	{
		const py::module_ & module = p_vtxModule.pyModule();
		VTX::PythonBinding::Helper::declareEnum<App::Trajectory::PLAY_MODE>( module, "TRAJECTORY_PLAY_MODE" );

		py::class_<Frame>( module, "Frame", py::module_local() )
			.def_property_readonly( "index", []( const Frame & p_frame ) { return p_frame.index; } )
			.def_property_readonly( "atomCount", &_getFrameAtomCount )
			.def(
				"getAtomPosition",
				[]( const Frame & p_frame, const std::size_t p_atom )
				{ return _getFrameAtomPosition( p_frame, static_cast<py::ssize_t>( p_atom ) ); },
				py::arg( "index" )
			)
			.def( "__len__", &_getFrameAtomCount )
			.def(
				"__getitem__",
				[]( const Frame & p_frame, const py::ssize_t p_index )
				{ return _getFrameAtomPosition( p_frame, p_index ); }
			);

		bindIdentity( py::class_<Trajectory>( module, "Trajectory", py::module_local() ) )
			.def_property_readonly(
				"frameCount",
				[]( const Trajectory & p_trajectory ) { return _getTrajectoryState( p_trajectory.entity ).frameCount; }
			)
			.def_property_readonly(
				"loadedFrameCount",
				[]( const Trajectory & p_trajectory )
				{ return _getTrajectoryState( p_trajectory.entity ).availableFrames.getCount(); }
			)
			.def_property_readonly(
				"availableFrames",
				[]( const Trajectory & p_trajectory )
				{ return _getTrajectoryState( p_trajectory.entity ).availableFrames; }
			)
			.def_property_readonly(
				"currentFrameIndex",
				[]( const Trajectory & p_trajectory )
				{ return _getTrajectoryState( p_trajectory.entity ).currentFrameIndex; }
			)
			.def_property_readonly(
				"requestedFrameIndex",
				[]( const Trajectory & p_trajectory )
				{ return _getTrajectoryState( p_trajectory.entity ).requestedFrameIndex; }
			)
			.def_property_readonly(
				"playMode",
				[]( const Trajectory & p_trajectory ) { return _getTrajectoryState( p_trajectory.entity ).playMode; }
			)
			.def_property_readonly(
				"paused",
				[]( const Trajectory & p_trajectory ) { return _getTrajectoryState( p_trajectory.entity ).paused; }
			)
			.def_property_readonly(
				"speed",
				[]( const Trajectory & p_trajectory ) { return _getTrajectoryState( p_trajectory.entity ).speed; }
			)
			.def_property_readonly(
				"currentFrame", []( const Trajectory & p_trajectory ) { return getCurrentFrame( p_trajectory.entity ); }
			)
			.def(
				"getFrame",
				[]( const Trajectory & p_trajectory, const std::size_t p_index )
				{ return _getFrame( p_trajectory.entity, p_index ); },
				py::arg( "index" )
			)
			.def(
				"__len__",
				[]( const Trajectory & p_trajectory ) { return _getTrajectoryState( p_trajectory.entity ).frameCount; }
			)
			.def(
				"__getitem__",
				[]( const Trajectory & p_trajectory, const py::ssize_t p_index )
				{
					const std::size_t frameCount = _getTrajectoryState( p_trajectory.entity ).frameCount;
					return _getFrame( p_trajectory.entity, _resolveIndex( p_index, frameCount ) );
				}
			)
			.def(
				"toggleStartPause",
				[]( const Trajectory & p_trajectory )
				{ _executeTrajectoryAction<App::Action::Trajectory::ToggleStartPause>( p_trajectory ); }
			)
			.def(
				"play",
				[]( const Trajectory & p_trajectory )
				{ _executeTrajectoryAction<App::Action::Trajectory::SetPaused>( p_trajectory, false ); }
			)
			.def(
				"pause",
				[]( const Trajectory & p_trajectory )
				{ _executeTrajectoryAction<App::Action::Trajectory::SetPaused>( p_trajectory, true ); }
			)
			.def(
				"stop",
				[]( const Trajectory & p_trajectory )
				{ _executeTrajectoryAction<App::Action::Trajectory::Stop>( p_trajectory ); }
			)
			.def(
				"jumpTo",
				[]( const Trajectory & p_trajectory, const uint p_index )
				{
					THREAD().synchronize(
						[ &p_trajectory, p_index ]()
						{
							_validatePlayer( p_trajectory.entity );
							if ( p_index >= REG().get<Core::Struct::Trajectory>( p_trajectory.entity ).frameCount )
							{
								throw py::index_error( "Frame index out of bounds." );
							}
							App::Action::Trajectory::JumpTo action;
							Util::ActionManager::run( action, p_trajectory.entity, p_index );
						}
					);
				},
				py::arg( "index" )
			)
			.def(
				"setPlayMode",
				[]( const Trajectory & p_trajectory, const App::Trajectory::PLAY_MODE p_mode )
				{ _executeTrajectoryAction<App::Action::Trajectory::ChangePlayer>( p_trajectory, p_mode ); },
				py::arg( "mode" )
			)
			.def(
				"setSpeed",
				[]( const Trajectory & p_trajectory, const float p_speed )
				{
					if ( not std::isfinite( p_speed ) )
					{
						throw py::value_error( "Speed must be a finite number." );
					}
					_executeTrajectoryAction<App::Action::Trajectory::ChangeSpeed>( p_trajectory, p_speed );
				},
				py::arg( "speed" )
			);
	}
} // namespace VTX::App::PythonBinding
