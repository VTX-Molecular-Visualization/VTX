#ifndef __VTX_APP_COMPONENT_CHEMISTRY_TRAJECTORY__
#define __VTX_APP_COMPONENT_CHEMISTRY_TRAJECTORY__

#include "_fwd.hpp"
#include "app/core/trajectory_player/base_player.hpp"
#include "enum_trajectory.hpp"

namespace VTX::App::Component::Chemistry
{
	class Trajectory
	{
	  public:
		Trajectory();
		Trajectory( Molecule * const p_molecule );

		const Molecule * const getConstMoleculePtr() const { return _moleculePtr; }
		Molecule * const	   getMoleculePtr() const { return _moleculePtr; }

		size_t getCurrentFrame() const;
		size_t getFrameCount() const;

		App::Core::TrajectoryPlayer::BasePlayer & getPlayer() const { return *_player; }
		void setPlayer( std::unique_ptr<App::Core::TrajectoryPlayer::BasePlayer> & p_player );

	  private:
		void _update( const float p_deltaTime );

		void _referenceUpdateFunction();

		Molecule *												 _moleculePtr = nullptr;
		std::unique_ptr<App::Core::TrajectoryPlayer::BasePlayer> _player	  = nullptr;
	};
} // namespace VTX::App::Component::Chemistry
#endif
