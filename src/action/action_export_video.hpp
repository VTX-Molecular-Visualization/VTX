#ifndef __VTX_ACTION_EXPORT_VIDEO__
#define __VTX_ACTION_EXPORT_VIDEO__

#ifdef _MSC_VER
#pragma once
#endif

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
				Tool::Snapshoter   snapshoter;

				if ( path->getCheckpoints().size() < 2 )
				{
					VTXApp::get().goToState( ID::State::VISUALIZATION );
					return;
				}

				uint totalFrame = VIDEO_FPS * path->computeTotalTime();

				if ( totalFrame == 0.f )
				{
					VTXApp::get().goToState( ID::State::VISUALIZATION );
					return;
				}

				VTX_DEBUG( "Exporting... total: " + std::to_string( totalFrame ) );

				std::string filename = Util::Time::getTimestamp();
				ImGuiIO &	io		 = ImGui::GetIO();

				for ( uint frame = 0; frame < totalFrame; ++frame )
				{
					float time = (float)frame / VIDEO_FPS;

					//					VTXApp::get().getScene().getCamera().setConfiguration(
					//		path->getCurrentCameraConfiguration( time ) );

					// Update renderer.
					VTXApp::get().getRenderer().render( VTXApp::get().getScene() );

					IO::Path path( VIDEO_DIR + std::to_string( frame ) + ".jpg" );
					snapshoter.takeSnapshot( path );
				}

				// system( "../tool/ffmpeg.exe -r 1/5 -i * -c:v libx264 -vf fps=60 -pix_fmt yuv420p video.mp4" );
			};
		};
	} // namespace Action
} // namespace VTX
#endif
