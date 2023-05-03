#ifndef __VTX_APP_ACTION_PATH__
#define __VTX_APP_ACTION_PATH__

#include "app/action.hpp"
#include "app/core/action/base_action.hpp"
#include "app/component/video/path.hpp"
#include "app/component/video/enum_path.hpp"
// #include "app/render/worker/snapshoter.hpp"

namespace VTX::App::Action::Path
{
	class ChangeDuration : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeDuration( App::Component::Video::Path & p_path, const float p_duration ) :
			_path( p_path ), _duration( p_duration )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
	 App::Component::Video::Path & _path;
		const float	  _duration;
	};

	class ChangeDurationMode : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeDurationMode( App::Component::Video::Path & p_path, const App::Component::Video::PATH_DURATION_MODE p_mode ) :
			_path( p_path ), _mode( p_mode )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
	 App::Component::Video::Path &				   _path;
		const App::Component::Video::PATH_DURATION_MODE _mode;
	};

	class ChangeInterpolationMode : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeInterpolationMode( App::Component::Video::Path & p_path, const App::Component::Video::PATH_INTERPOLATION_MODE p_mode ) :
			_path( p_path ), _mode( p_mode )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
	 App::Component::Video::Path &						_path;
		const App::Component::Video::PATH_INTERPOLATION_MODE _mode;
	};

	class Play : public App::Core::Action::BaseAction
	{
	  public:
		explicit Play( App::Component::Video::Path * const p_path ) : _path( p_path ) {}
		virtual void execute() override;

	  private:
	 App::Component::Video::Path * const _path;
	};

	// class ExportVideo : public App::Core::Action::BaseAction
	//{
	//   public:
	//	explicit ExportVideo( App::Component::Video::Path * const p_path, Worker::Snapshoter::MODE p_mode ) :
	//		_path( p_path ), _mode( p_mode )
	//	{
	//	}

	//	virtual void execute() override;

	//  private:
	//	const Worker::Snapshoter::MODE _mode;
	// App::Component::Video::Path * const			   _path;
	//};

	class ChangeIsLooping : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeIsLooping( App::Component::Video::Path & p_path, const bool p_isLooping ) :
			_path( p_path ), _isLooping( p_isLooping )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
	 App::Component::Video::Path & _path;
		const bool	  _isLooping;
	};

	class SetSelected : public App::Core::Action::BaseAction
	{
	  public:
	  private:
	};

	class Clear : public App::Core::Action::BaseAction
	{
	  public:
		explicit Clear( App::Component::Video::Path & p_path ) : _path( p_path )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
	 App::Component::Video::Path & _path;
	};

} // namespace VTX::App::Action::Path
#endif
