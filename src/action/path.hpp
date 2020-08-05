#ifndef __VTX_ACTION_PATH__
#define __VTX_ACTION_PATH__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/path.hpp"
#include "state/export.hpp"
#include "worker/snapshoter.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Path
		{
			class ChangeDuration : public BaseAction
			{
			  public:
				explicit ChangeDuration( Model::Path & p_path, const float p_duration ) :
					_path( p_path ), _duration( p_duration )
				{
				}

				virtual void execute() override
				{
					_path.setDuration( Util::Math::max<float>( _duration, 0.f ) );
					_path.refreshAllDurations();
				}

			  private:
				Model::Path & _path;
				const float	  _duration;
			};

			class ChangeDurationMode : public BaseAction
			{
			  public:
				explicit ChangeDurationMode( Model::Path & p_path, const Model::Path::DURATION_MODE p_mode ) :
					_path( p_path ), _mode( p_mode )
				{
				}

				virtual void execute() override
				{
					_path.setDurationMode( _mode );
					_path.refreshAllDurations();
				}

			  private:
				Model::Path &					 _path;
				const Model::Path::DURATION_MODE _mode;
			};

			class ChangeInterpolationMode : public BaseAction
			{
			  public:
				explicit ChangeInterpolationMode( Model::Path & p_path, const Model::Path::INTERPOLATION_MODE p_mode ) :
					_path( p_path ), _mode( p_mode )
				{
				}

				virtual void execute() override
				{
					_path.setInterpolationMode( _mode );
					_path.refreshAllDurations();
				}

			  private:
				Model::Path &						  _path;
				const Model::Path::INTERPOLATION_MODE _mode;
			};

			class Play : public BaseAction
			{
			  public:
				explicit Play( Model::Path * const p_path ) : _path( p_path ) {}
				virtual void execute() override { VTXApp::get().goToState( ID::State::PLAY, _path ); };

			  private:
				Model::Path * const _path;
			};

			class ExportVideo : public BaseAction
			{
			  public:
				explicit ExportVideo( Model::Path * const p_path, Worker::Snapshoter::MODE p_mode ) :
					_path( p_path ), _mode( p_mode )
				{
				}

				virtual void execute() override
				{
					State::Export::Arg arg;
					arg.mode = _mode;
					arg.path = _path;

					VTXApp::get().goToState( ID::State::EXPORT, &arg );
				};

			  private:
				const Worker::Snapshoter::MODE _mode;
				Model::Path * const			   _path;
			};

			class ChangeIsLooping : public BaseAction
			{
			  public:
				explicit ChangeIsLooping( Model::Path & p_path, const bool p_isLooping ) :
					_path( p_path ), _isLooping( p_isLooping )
				{
				}

				virtual void execute() override { _path.setIsLooping( _isLooping ); }

			  private:
				Model::Path & _path;
				const bool	  _isLooping;
			};

			class SetSelected : public BaseAction
			{
			  public:
			  private:
			};

		} // namespace Path
	}	  // namespace Action
} // namespace VTX
#endif
