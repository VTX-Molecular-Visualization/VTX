#include "export.hpp"
#include "../tool/snapshoter.hpp"
#include "vtx_app.hpp"
namespace VTX
{
	namespace State
	{
		void Export::enter( void * const p_arg )
		{
			Model::Path * path = (Model::Path *)p_arg;

			if ( path->getViewpoints().size() < 2 )
			{
				VTXApp::get().goToState( ID::State::VISUALIZATION );
				return;
			}

			float totalTime	 = path->computeTotalTime();
			uint  totalFrame = uint( VIDEO_FPS * totalTime );

			if ( totalFrame == 0.f )
			{
				VTX_WARNING( "Total time must be > 0" );
				VTXApp::get().goToState( ID::State::VISUALIZATION );
				return;
			}

			VTX_INFO( "Exporting... total: " + std::to_string( totalFrame ) + " frames / " + std::to_string( totalTime )
					  + " seconds" );

			// std::string		 filename	= Util::Time::getTimestamp();
			ImGuiIO &		 io			= ImGui::GetIO();
			Tool::Snapshoter snapshoter = Tool::Snapshoter();
			Tool::Chrono	 chrono		= Tool::Chrono();

			chrono.start();
			for ( uint frame = 0; frame < totalFrame; ++frame )
			{
				float										 time = (float)frame / VIDEO_FPS;
				Model::Viewpoint::ViewpointInterpolationData data = path->getCurrentViewpointInterpolationData( time );
				VTXApp::get().getScene().getCamera().set(
					Util::Math::lerp( data.positionLhs, data.positionRhs, data.value ),
					Util::Math::lerp( data.rotationLhs, data.rotationRhs, data.value ) );

				// Update renderer.
				// VTXApp::get().getUI().draw();
				VTXApp::get().renderScene();
				VTXApp::get().getScene().update( 1.f / VIDEO_FPS );
				std::string counterStr = std::to_string( frame );
				IO::Path	path( VIDEO_DIR + "snapshot" + std::string( 4 - counterStr.length(), '0' ) + counterStr
								  + ".png" );
				snapshoter.takeSnapshot( path );
				VTX_INFO( std::to_string( ( uint )( (float)frame * 100 / totalFrame ) ) + "%" );
			}
			chrono.stop();
			VTX_INFO( "Export finished in " + std::to_string( chrono.elapsedTime() ) + " seconds" );
			VTXApp::get().goToState( ID::State::VISUALIZATION );
		}

		void Export::exit() {}

		void Export::update( const double p_deltaTime ) {}

	} // namespace State
} // namespace VTX
