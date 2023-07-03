#include "ui/old_ui/state/export.hpp"
#include "ui/id.hpp"
#include <app/old/action/main.hpp>
#include <app/old/application/scene.hpp>
#include <app/old/component/object3d/viewpoint.hpp>
#include <app/old/component/render/camera.hpp>
#include <app/old/component/video/path.hpp>
#include <app/old/internal/worker/program_launcher.hpp>
#include <app/old/vtx_app.hpp>
#include <util/chrono.hpp>

namespace VTX
{
	namespace State
	{
		// Action loop
		void Export::enter( void * const p_arg )
		{
			_path		   = (App::Old::Component::Video::Path *)p_arg;
			_directoryName = std::to_string( Util::Chrono::getTimestamp() );
			_directoryName.erase( remove_if( _directoryName.begin(), _directoryName.end(), isspace ),
								  _directoryName.end() );
			// App::Old::VTXApp::get().getSetting().backup();

			float duration = _path->getDuration();
			_frameCount	   = uint( VTX::App::Old::Application::Setting::VIDEO_FPS_DEFAULT * duration );

			if ( _frameCount == 0u || _path->getViewpoints().size() < 2 )
			{
				VTX_WARNING( "Total time must be > 0" );
				App::Old::VTXApp::get().goToState( UI::ID::State::VISUALIZATION );
				return;
			}

			VTX_INFO( "Exporting... total: {} frames / {} seconds.", _frameCount, duration );
		}

		void Export::exit()
		{
			_actions	= nullptr;
			_frame		= 0u;
			_frameCount = 0u;
			// App::Old::VTXApp::get().getSetting().recover();
		}

		void Export::update( const float & p_deltaTime )
		{
			BaseState::update( p_deltaTime );

			float time = _frame / (float)VTX::App::Old::Application::Setting::VIDEO_FPS_DEFAULT;
			App::Old::Component::Object3D::Viewpoint viewpoint = _path->getInterpolatedViewpoint( time );

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
			if ( viewpoint.getController() == UI::ID::Controller::TRACKBALL )
			{
				App::Old::VTXApp::get().getScene().getCamera().setRotationAround(
					Quatf( viewpoint.getRotation() ), viewpoint.getTarget(), viewpoint.getDistance() );
			}
			else
			{
				App::Old::VTXApp::get().getScene().getCamera().set( viewpoint.getPosition(), viewpoint.getRotation() );
			}
			App::Old::VTXApp::get().getScene().update( 1.f / (float)VTX::App::Old::Application::Setting::VIDEO_FPS_DEFAULT );
			App::Old::VTXApp::get().renderScene();

			std::string counterStr = std::to_string( _frame );
			std::string fileName   = "frame" + std::string( 6 - counterStr.length(), '0' ) + counterStr;

			// VTX_ACTION( new App::Old::Action::Main::Snapshot( Worker::Snapshoter::MODE::GL,
			//										App::Old::Internal::IO::Filesystem::getVideosPath( fileName + ".png" ) )
			//);

			VTX_INFO( "{}%", ( _frame * uint( 100 ) / _frameCount ) );

			if ( _frame == _frameCount - 1 )
			{
				try
				{
					//_generareVideo();
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "{}", p_e.what() );
				}

				App::Old::VTXApp::get().goToState( UI::ID::State::VISUALIZATION );
			}
			else
			{
				_frame++;
			}
		}

		/*
		void Export::_generareVideo() const
		{
			if ( std::filesystem::exists( App::Old::Internal::IO::Filesystem::FFMPEG_EXE_FILE ) == false )
			{
				throw LibException( "ffmpeg is missing, frames are saved on disk" );
			}

			VTX_INFO( "Encoding video" );

			FilePath files = App::Old::Internal::IO::Filesystem::getVideosBatchPath( _directoryName );
			files /= "frame%06d.png";
			std::string command = App::Old::Internal::IO::Filesystem::FFMPEG_EXE_FILE.string() + " -f image2 -framerate "
								  + std::to_string( VTX::App::Old::Application::Setting::VIDEO_FPS_DEFAULT ) + " -i " +
		files.string()
								  + " -vcodec libx264 -crf " + std::to_string(
		VTX::App::Old::Application::Setting::VIDEO_CRF_DEFAULT ) + " "
								  + App::Old::Internal::IO::Filesystem::getVideosPath( _directoryName + ".mp4" ).string();
			Internal::Worker::ProgramLauncher * worker = new Internal::Worker::ProgramLauncher( command );
			VTX_THREAD( worker );

			// Clean frames
			Util::Filesystem::removeAll( App::Old::Internal::IO::Filesystem::getVideosBatchPath( _directoryName ) );
		}
		*/

	} // namespace State
} // namespace VTX
