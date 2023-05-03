#include "ui/qt/state/export.hpp"
#include "ui/id.hpp"
#include <app/action/main.hpp>
#include <app/component/object3d/viewpoint.hpp>
#include <app/component/video/path.hpp>
#include <app/internal/worker/program_launcher.hpp>
#include <app/old_app/vtx_app.hpp>
#include <util/chrono.hpp>
#include <util/filesystem.hpp>

namespace VTX::UI::QT::State
{
	// Action loop
	void Export::enter( void * const p_arg )
	{
		_path		   = (App::Component::Video::Path *)p_arg;
		_directoryName = std::to_string( Util::Chrono::getTimestamp() );
		_directoryName.erase( remove_if( _directoryName.begin(), _directoryName.end(), isspace ),
							  _directoryName.end() );
		// VTXApp::get().getSetting().backup();

		float duration = _path->getDuration();
		_frameCount	   = uint( VTX::App::Application::Setting::VIDEO_FPS_DEFAULT * duration );

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
		/*
		BaseState::update( p_deltaTime );

		float			 time	   = _frame / (float)VTX::App::Application::Setting::VIDEO_FPS_DEFAULT;
		App::Component::Object3D::Viewpoint viewpoint = _path->getInterpolatedViewpoint( time );

		// Action.

		if ( _actions != _path->getCurrentActions( time ) )
		{
			_actions = _path->getCurrentActions( time );
			for ( const std::string & action : *_actions )
			{
				VTX_ACTION( action );
			}
		}


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
		VTXApp::get().getScene().update( 1.f / (float)VTX::App::Application::Setting::VIDEO_FPS_DEFAULT );
		VTXApp::get().renderScene();

		std::string counterStr = std::to_string( _frame );
		std::string fileName   = "frame" + std::string( 6 - counterStr.length(), '0' ) + counterStr;

		VTX_ACTION( new App::Action::Main::Snapshot( Worker::Snapshoter::MODE::GL,
												App::Internal::IO::Filesystem::getVideosPath( fileName + ".png" ) ) );

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
		*/
	}

	/*
	void Export::_generareVideo() const
	{
		if ( std::filesystem::exists( Util::Filesystem::FFMPEG_EXE_FILE ) == false )
		{
			throw Util::Exception::LibException( "ffmpeg is missing, frames are saved on disk" );
		}

		VTX_INFO( "Encoding video" );

		FilePath files = Util::Filesystem::getVideosBatchPath( _directoryName );
		files /= "frame%06d.png";
		std::string command = Util::Filesystem::FFMPEG_EXE_FILE.string() + " -f image2 -framerate "
							  + std::to_string( VTX::App::Application::Setting::VIDEO_FPS_DEFAULT ) + " -i " +
	files.string()
							  + " -vcodec libx264 -crf " + std::to_string(
	VTX::App::Application::Setting::VIDEO_CRF_DEFAULT ) + " "
							  + Util::Filesystem::getVideosPath( _directoryName + ".mp4" ).string();
		Internal::Worker::ProgramLauncher * worker = new Internal::Worker::ProgramLauncher( command );
		VTX_THREAD( worker );

		// Clean frames
		Util::Filesystem::removeAll( Util::Filesystem::getVideosBatchPath( _directoryName ) );
	}
	*/

} // namespace VTX::UI::QT::State
