#include "ui/old_ui/state/export.hpp"
#include <app/action/main.hpp>
#include <app/internal/worker/program_launcher.hpp>
#include <app/model/path.hpp>
#include <app/model/viewpoint.hpp>
#include <app/old_app/object3d/camera.hpp>
#include <app/old_app/object3d/scene.hpp>
#include <app/old_app/vtx_app.hpp>
#include <util/chrono.hpp>

namespace VTX
{
	namespace State
	{
		// Action loop
		void Export::enter( void * const p_arg )
		{
			_path		   = (Model::Path *)p_arg;
			_directoryName = std::to_string( Util::Chrono::getTimestamp() );
			_directoryName.erase( remove_if( _directoryName.begin(), _directoryName.end(), isspace ),
								  _directoryName.end() );
			// VTXApp::get().getSetting().backup();

			float duration = _path->getDuration();
			_frameCount	   = uint( Setting::VIDEO_FPS_DEFAULT * duration );

			if ( _frameCount == 0u || _path->getViewpoints().size() < 2 )
			{
				VTX_WARNING( "Total time must be > 0" );
				VTXApp::get().goToState( ID::State::VISUALIZATION );
				return;
			}

			VTX_INFO( "Exporting... total: " + std::to_string( _frameCount ) + " frames / " + std::to_string( duration )
					  + " seconds" );
		}

		void Export::exit()
		{
			_actions	= nullptr;
			_frame		= 0u;
			_frameCount = 0u;
			// VTXApp::get().getSetting().recover();
		}

		void Export::update( const float & p_deltaTime )
		{
			BaseState::update( p_deltaTime );

			float			 time	   = _frame / (float)Setting::VIDEO_FPS_DEFAULT;
			Model::Viewpoint viewpoint = _path->getInterpolatedViewpoint( time );

			// Action.
			/*
			if ( _actions != _path->getCurrentActions( time ) )
			{
				_actions = _path->getCurrentActions( time );
				for ( const std::string & action : *_actions )
				{
					VTX_ACTION( action );
				}
			}
			*/

			// Update renderer.
			if ( viewpoint.getController() == ID::Controller::TRACKBALL )
			{
				VTXApp::get().getScene().getCamera().setRotationAround(
					Quatf( viewpoint.getRotation() ), viewpoint.getTarget(), viewpoint.getDistance() );
			}
			else
			{
				VTXApp::get().getScene().getCamera().set( viewpoint.getPosition(), viewpoint.getRotation() );
			}
			VTXApp::get().getScene().update( 1.f / (float)Setting::VIDEO_FPS_DEFAULT );
			VTXApp::get().renderScene();

			std::string counterStr = std::to_string( _frame );
			std::string fileName   = "frame" + std::string( 6 - counterStr.length(), '0' ) + counterStr;

			// VTX_ACTION( new App::Action::Main::Snapshot( Worker::Snapshoter::MODE::GL,
			//										IO::Filesystem::getVideosPath( fileName + ".png" ) ) );

			VTX_INFO( std::to_string( (uint)( _frame * 100 / _frameCount ) ) + "%" );

			if ( _frame == _frameCount - 1 )
			{
				try
				{
					//_generareVideo();
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( p_e.what() );
				}

				VTXApp::get().goToState( ID::State::VISUALIZATION );
			}
			else
			{
				_frame++;
			}
		}

		/*
		void Export::_generareVideo() const
		{
			if ( std::filesystem::exists( IO::Filesystem::FFMPEG_EXE_FILE ) == false )
			{
				throw LibException( "ffmpeg is missing, frames are saved on disk" );
			}

			VTX_INFO( "Encoding video" );

			FilePath files = IO::Filesystem::getVideosBatchPath( _directoryName );
			files /= "frame%06d.png";
			std::string command = IO::Filesystem::FFMPEG_EXE_FILE.string() + " -f image2 -framerate "
								  + std::to_string( Setting::VIDEO_FPS_DEFAULT ) + " -i " + files.string()
								  + " -vcodec libx264 -crf " + std::to_string( Setting::VIDEO_CRF_DEFAULT ) + " "
								  + IO::Filesystem::getVideosPath( _directoryName + ".mp4" ).string();
			Worker::ProgramLauncher * worker = new Worker::ProgramLauncher( command );
			VTX_THREAD( worker );

			// Clean frames
			Util::Filesystem::removeAll( IO::Filesystem::getVideosBatchPath( _directoryName ) );
		}
		*/

	} // namespace State
} // namespace VTX
