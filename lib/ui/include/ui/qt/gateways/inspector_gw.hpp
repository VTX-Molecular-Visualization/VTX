#ifndef __VTX_UI_QT_GATEWAYS_INSPECTOR__
#define __VTX_UI_QT_GATEWAYS_INSPECTOR__

#include "base_gw.hpp"
#include <app/application/scene.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/chemistry/trajectory.hpp>

namespace VTX::UI::QT::Gateways
{
	class InspectorGw : public BaseGw
	{
	  public:
		InspectorGw();
		~InspectorGw() {};
		void registerResource( QObject * treeWidget ) override;
		App::Component::Chemistry::Molecule & getMoleculeData(
			const VTX::App::Component::Scene::SceneItemComponent & p_item
		);
		App::Component::Chemistry::Trajectory & getTrajectoryData();
		void setPlayTrajectoryPlayer( const App::Component::Scene::SceneItemComponent & p_item );
		void setPauseTrajectoryPlayer( const App::Component::Scene::SceneItemComponent & p_item );
		void setStopTrajectoryPlayer( const App::Component::Scene::SceneItemComponent & p_item );

	  private:
		std::atomic_bool is_playing; // FIXME dev purpose to stop threads needs improvement
	};
} // namespace VTX::UI::QT::Gateways
#endif
