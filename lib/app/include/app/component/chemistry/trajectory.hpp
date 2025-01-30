#ifndef __VTX_APP_COMPONENT_CHEMISTRY_TRAJECTORY__
#define __VTX_APP_COMPONENT_CHEMISTRY_TRAJECTORY__

#include "_fwd.hpp"
#include "app/component/chemistry/system.hpp"
#include "app/core/ecs/ecs_system.hpp"
#include "app/core/player/base_player.hpp"
#include "app/core/player/concepts.hpp"
#include "app/vtx_app.hpp"
#include "enum_trajectory.hpp"
#include <util/callback.hpp>

namespace VTX::App::Component::Chemistry
{
	class Trajectory : public Core::ECS::BaseComponent
	{
	  public:
		Trajectory();
		Trajectory( System * const p_system, const FilePath & p_path );

		const System * const getConstSystemPtr() const { return _systemPtr; }
		System * const		 getSystemPtr() const { return _systemPtr; }

		size_t getCurrentFrame() const;
		void   setCurrentFrame( const size_t p_frameIndex );

		size_t getFrameCount() const;

		App::Core::Player::BasePlayer & getPlayer() const { return *_player; }

		template<Core::Player::ConceptPlayer P>
		void setPlayer()
		{
			// Delete old player.
			if ( _player )
			{
				_player.reset();
				APP::onUpdate -= _currentUpdateCallback;
			}

			// Create and connect new player.
			_player				   = std::make_unique<P>();
			_currentUpdateCallback = APP::onUpdate += [ this ]( const float p_deltaTime, const float p_elapsedTime )
			{ static_cast<P *>( _player.get() )->update( p_deltaTime, p_elapsedTime ); };

			_player->setCount( _systemPtr->getTrajectory().getFrameCount() );

			/* _player->onFrameChange += [ this ]( const size_t p_frameIndex )
			{
				_systemPtr->getTrajectory()._currentFrameIndex = p_frameIndex;
				onFrameChange( p_frameIndex );
			};*/
			_player->onFrameChange += [ this ]( const VTX::Core::Struct::Frame & p_frame )
			{
				//_systemPtr->getTrajectory()._currentFrameIndex = p_frameIndex; currently done in player
				onFrameChange( p_frame );
			};
		}

		// Util::Callback<size_t> onFrameChange;
		Util::Callback<const std::vector<Vec3f> &> onFrameChange;
		// TODO:
		// Util::Callback<> onPlayerChange;

		const FilePath & getPath() const { return _path; }

	  private:
		void _referenceUpdateFunction();

		System *								  _systemPtr = nullptr;
		std::unique_ptr<Core::Player::BasePlayer> _player;
		Util::CallbackId						  _currentUpdateCallback;

		const FilePath _path;
	};
} // namespace VTX::App::Component::Chemistry
#endif
