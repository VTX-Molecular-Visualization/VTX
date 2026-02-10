#include "app/action/scene.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/camera.hpp"
#include "app/action/color.hpp"
#include "app/action/representation.hpp"
#include "app/action/visibility.hpp"
#include "app/events.hpp"
#include "app/preset/name.hpp"
#include "app/scene/color_layout.hpp"
#include "app/scene/graphics_config.hpp"
#include "app/scene/tag_root.hpp"
#include "app/services.hpp"
#include "app/system/color.hpp"
#include "app/system/deleted.hpp"
#include "app/system/load.hpp"
#include "app/system/metadata.hpp"
#include "app/system/representation.hpp"
#include "app/system/selection.hpp"
#include "app/system/trajectory_preparation.hpp"
#include "app/system/uid.hpp"
#include "app/system/visibility.hpp"
#include "app/threading/base_thread.hpp"
#include "app/threading/thread_manager.hpp"
#include "app/uid/uid_manager.hpp"
#include <core/chemdb/atom.hpp>
#include <core/struct/system.hpp>
#include <io/reader/system.hpp>
#include <renderer/renderer.hpp>
#include <renderer/representation.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>
#include <util/math/aabb.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Action::Scene
{

	void LoadSystem::execute( FilePath p_path /*, const std::string * const p_buffer*/ )
	{
		using namespace Core::Struct;

		auto & reg = REG();

		// Create entity.
		ECS::Entity entity			  = reg.create();
		auto &		pendingSystemData = reg.emplace<System::PendingSystem>( entity );

		THREAD().createThread(
			[ entity,
			  path = std::move( p_path ),
			  &pendingSystemData ]( VTX::Util::StopToken p_stopToken, Threading::BaseThread & ) -> uint
			{
				pendingSystemData.path = std::move( path );
				pendingSystemData.loader.emplace();
				pendingSystemData.loader->readFile( path, pendingSystemData.system );
				pendingSystemData.pdbIdCode = pendingSystemData.loader->getChemfilesReader().getPdbIdCode();

				if ( p_stopToken.stop_requested() )
					return 0;

				pendingSystemData.topologyReady = true;
				pendingSystemData.trajectoryDecision.wait();

				if ( p_stopToken.stop_requested() )
					return 0;

				auto visitor = [ loader = &pendingSystemData.loader.value() ]( auto && traj )
				{ System::prepare( traj, std::move( *loader ) ); };
				std::visit( visitor, pendingSystemData.trajectoryData );
				pendingSystemData.trajectoryReady = true;
				return 0;
			}

		);
		return;
	}

	void DeleteSystem::execute( const ECS::Entity p_e ) { REG().destroy( p_e ); }

	void Clear::execute()
	{
		REG().view<System::Metadata>().each( [ & ]( auto p_e, auto & ) { REG().destroy( p_e ); } );
	}

	void SetColorLayout::execute( const ECS::Entity p_e )
	{
		using CL = App::Scene::ColorLayout;

		auto & reg	 = REG();
		auto   scene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot>();
		auto   view	 = reg.view<CL>();

		assert( view.size() <= 1 );

		reg.remove<CL>( scene );
		reg.emplace<CL>( scene, p_e );
	}

	void SetGraphicsConfig::execute( const ECS::Entity p_e )
	{
		using GC = App::Scene::GraphicsConfig;

		auto & reg	 = REG();
		auto   scene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot>();
		auto   view	 = reg.view<GC>();

		assert( view.size() <= 1 );

		reg.remove<GC>( scene );
		reg.emplace<GC>( scene, p_e );
	}

} // namespace VTX::App::Action::Scene
