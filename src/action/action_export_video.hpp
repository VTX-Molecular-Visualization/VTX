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

				for ( uint frame = 0; frame < totalFrame; ++frame )
				{
					float time	= (float)frame / VIDEO_FPS;
					float total = 0.f;

					uint offset = 0;
					for ( ; offset < path->getCheckpoints().size(); ++offset )
					{
						Model::ModelCheckpoint * checkpoint = path->getCheckpoints()[ offset ];
						if ( offset >= 1 ) { total += checkpoint->getDuration(); }
						if ( total >= time ) { break; }
					}

					float value = 1.f - ( ( -( time - total ) ) / path->getCheckpoints()[ offset ]->getDuration() );

					VTXApp::get().getScene().getCamera().setConfiguration(
						Util::Math::lerpCameraConfiguration( path, offset, value ) );

					// Update renderer.
					VTXApp::get().getRenderer().render( VTXApp::get().getScene() );

					// std::string dirname = Util::Time::getTimestamp();
					IO::Path path( VIDEO_DIR + std::to_string( frame ) + ".jpg" );
					snapshoter.takeSnapshot( path );
				}
			};
		};
	} // namespace Action
} // namespace VTX
#endif
