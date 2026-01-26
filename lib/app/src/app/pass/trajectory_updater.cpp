#include "app/pass/trajectory_updater.hpp"
#include "app/services.hpp"
#include "app/system/trajectory.hpp"
#include "app/system/uid.hpp"
#include <renderer/renderer.hpp>

namespace VTX::App::Pass
{
	TrajectoryUpdater::TrajectoryUpdater()
	{
		REG().on_update<System::TrajectoryFullBuffer>().connect<&TrajectoryUpdater::_onTrajectoryFullBufferCreation>(
			this
		);
		REG().on_destroy<System::TrajectoryFullBuffer>().connect<&TrajectoryUpdater::_onTrajectoryDestruction>( this );
	}

	void TrajectoryUpdater::_onTrajectoryFullBufferCreation( ECS::Entity p_entity )
	{
		if ( not _players.contains( p_entity ) )
			_players.emplace( p_entity, _Player() );
		auto &						localPlayerStruct = _players[ p_entity ];
		System::GenericTrajectory * trajPtr			  = nullptr;
		System::get( p_entity, trajPtr );
		if ( trajPtr )
		{
			if ( trajPtr->playMode != localPlayerStruct.currentPlayMode )
			{
				localPlayerStruct.currentPlayMode = trajPtr->playMode;
				switch ( localPlayerStruct.currentPlayMode )
				{
					using namespace VTX::Util;
				case System::TrajectoryPlayMode::forward:
					localPlayerStruct.currentPlayer
						= Players::Forward( trajPtr->trajectorySize, trajPtr->currentFrameIndex );
					break;
				default:
					localPlayerStruct.currentPlayer
						= Players::Forward( trajPtr->trajectorySize, trajPtr->currentFrameIndex );
					break;
				}
			}
		}
	}
	void TrajectoryUpdater::_onTrajectoryDestruction( ECS::Entity p_entity )
	{
		if ( _players.contains( p_entity ) )
			_players.erase( p_entity );
	}

	bool TrajectoryUpdater::_tryUpdateFrame(
		const ECS::Entity &			   entity,
		System::TrajectoryFullBuffer & p_traj
	) noexcept
	{
		if ( p_traj.lastFrameAvailable < p_traj.genericData.requestedFrameIndex )
			return false;

		RENDERER().setSystemPosition(
			REG().get<System::UID>( entity ).system, p_traj.frameCollection[ p_traj.genericData.requestedFrameIndex ]
		);
		return true;
	}
	namespace
	{
		bool tryUpdateFrame( const ECS::Entity & entity, System::TrajectoryFullBuffer & p_traj ) noexcept
		{
			if ( p_traj.lastFrameAvailable < p_traj.genericData.requestedFrameIndex )
				return false;

			RENDERER().setSystemPosition(
				REG().get<System::UID>( entity ).system,
				p_traj.frameCollection[ p_traj.genericData.requestedFrameIndex ]
			);
			return true;
		}
		/**
		 * @brief Return true if the frame should be updated.
		 * @param p_traj trajectory data
		 * @param p_elapsedTime elapsed time since software start
		 * @return
		 */
		bool shouldUpdateFrame( const System::GenericTrajectory & p_traj, const float & p_elapsedTime )
		{
			return p_traj.currentFrameIndex != p_traj.requestedFrameIndex
				   && p_traj.lastFrameUpdateTime + p_traj.playingSpeed > p_elapsedTime;
		}
		template<typename TrajectoryT>
		System::GenericTrajectory & genericData( TrajectoryT & p_ )
		{
			return p_.genericData;
		}
		/**
		 * @brief Update frame for every trajectory of the input type
		 * @tparam TrajectoryT type of trajectory
		 * @param p_elapsedTime Time since program start
		 */
		template<typename TrajectoryT>
		void updateTrajectoresPosition(
			std::unordered_map<ECS::Entity, TrajectoryUpdater::_Player> & players,
			const float													  p_elapsedTime
		)
		{
			for ( ECS::Entity it_entity : REG().view<TrajectoryT>() )
			{
				const TrajectoryT &			traj		   = REG().get<TrajectoryT>( it_entity );
				System::GenericTrajectory * genericTrajPtr = nullptr;
				System::get( it_entity, genericTrajPtr );
				if ( genericTrajPtr == nullptr )
					continue;
				if ( not players.contains( it_entity ) )
					continue;
				auto & player = players[ it_entity ].currentPlayer;

				if ( genericTrajPtr->paused )
					continue;

				uint nextStep = 0;
				player.next( nextStep );
				if ( genericTrajPtr->currentFrameIndex == nextStep )
					continue;
				if ( genericTrajPtr->lastFrameUpdateTime + genericTrajPtr->playingSpeed > p_elapsedTime )
					continue;

				player.increment();

				REG().patch<TrajectoryT>(
					it_entity,
					[ &nextStep, &it_entity, &p_elapsedTime ]( TrajectoryT & traj )
					{
						System::GenericTrajectory & trajGenericData = genericData( traj );
						trajGenericData.requestedFrameIndex			= nextStep;
						if ( tryUpdateFrame( it_entity, traj ) )
						{
							trajGenericData.requestedFrameIndex;
							trajGenericData.lastFrameUpdateTime = p_elapsedTime;
						}
					}
				);
			}
		}
	} // namespace

	void TrajectoryUpdater::update( const float p_delta, const float p_elapsedTime )
	{
		updateTrajectoresPosition<System::TrajectoryFullBuffer>( _players, p_elapsedTime );
	}

} // namespace VTX::App::Pass
