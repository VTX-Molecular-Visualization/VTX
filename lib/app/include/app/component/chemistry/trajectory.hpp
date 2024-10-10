#ifndef __VTX_APP_COMPONENT_CHEMISTRY_TRAJECTORY__
#define __VTX_APP_COMPONENT_CHEMISTRY_TRAJECTORY__

#include "_fwd.hpp"
#include "app/application/system/ecs_system.hpp"
#include "app/core/player/base_player.hpp"
#include "enum_trajectory.hpp"
#include <util/callback.hpp>

namespace VTX::App::Component::Chemistry
{
	class Trajectory : public Core::ECS::BaseComponent
	{
	  private:
		inline static const Application::System::ECSSystem::ComponentStaticIDRegistration<Trajectory> registration {
			"Chemistry::TrajectoryComponent"
		};

	  public:
		Trajectory();
		Trajectory( Molecule * const p_molecule );

		const Molecule * const getConstMoleculePtr() const { return _moleculePtr; }
		Molecule * const	   getMoleculePtr() const { return _moleculePtr; }

		size_t getCurrentFrame() const;
		void   setCurrentFrame( const size_t p_frameIndex );

		size_t getFrameCount() const;

		App::Core::Player::BasePlayer & getPlayer() const { return *_player; }
		void							setPlayer( App::Core::Player::BasePlayer * const p_player );

		Util::Callback<size_t> onFrameChange;

	  private:
		void _update( const float p_deltaTime );
		void _referenceUpdateFunction();

		Molecule *						_moleculePtr = nullptr;
		App::Core::Player::BasePlayer * _player		 = nullptr;
	};
} // namespace VTX::App::Component::Chemistry
#endif
