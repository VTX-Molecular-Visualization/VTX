#ifndef __VTX_ACTION_EXPORT_VIDEO__
#define __VTX_ACTION_EXPORT_VIDEO__

#ifdef _MSC_VER
#pragma once
#endif

#include "../tool/chrono.hpp"
#include "base_action.hpp"
#include "model/model_path.hpp"
#include "tool/snapshoter.hpp"
#include "util/math.hpp"
#include "util/time.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionExportVideo : public BaseAction
		{
		  public:
			virtual void execute() override
			{
				Model::ModelPath * path = VTXApp::get().getScene().getPaths()[ 0 ];

				if ( path->getCheckpoints().size() < 2 )
				{
					VTXApp::get().goToState( ID::State::VISUALIZATION );
					return;
				}

				float totalTime	 = path->computeTotalTime();
				uint  totalFrame = uint( VIDEO_FPS * totalTime );

				if ( totalFrame == 0.f )
				{
					VTXApp::get().goToState( ID::State::VISUALIZATION );
					return;
				}

				VTX_INFO( "Exporting... total: " + std::to_string( totalFrame ) + " frames / "
						  + std::to_string( totalTime ) + " seconds" );

				std::string		 filename	= Util::Time::getTimestamp();
				ImGuiIO &		 io			= ImGui::GetIO();
				Tool::Snapshoter snapshoter = Tool::Snapshoter();
				Tool::Chrono	 chrono		= Tool::Chrono();

				chrono.start();
				for ( uint frame = 0; frame < totalFrame; ++frame )
				{
					float												time = (float)frame / VIDEO_FPS;
					Model::ModelCheckpoint::CheckpointInterpolationData data
						= path->getCurrentCheckpointInterpolationData( time );
					VTXApp::get().getScene().getCamera().set(
						Util::Math::lerp( data.positionLhs, data.positionRhs, data.value ),
						Util::Math::lerp( data.rotationLhs, data.rotationRhs, data.value ) );

					// Update renderer.
					VTXApp::get().getRenderer().render( VTXApp::get().getScene() );

					IO::Path path( VIDEO_DIR + std::to_string( frame ) + ".jpg" );
					snapshoter.takeSnapshot( path );
					VTX_INFO(std::to_string((uint)((float)frame * 100 / totalFrame))+"%");
				}
				chrono.stop();
				VTX_INFO( "Export finished in " + std::to_string( chrono.elapsedTime() ) + " seconds" );

				// system( "../tool/ffmpeg.exe -r 1/5 -i * -c:v libx264 -vf fps=60 -pix_fmt yuv420p video.mp4" );
			};
		};
	} // namespace Action
} // namespace VTX
#endif
