#ifndef __VTX_ACTION_PATH__
#define __VTX_ACTION_PATH__

#include "base_action.hpp"
#include "model/path.hpp"
#include "path/path_enum.hpp"
#include "model/selection.hpp"
#include "model/viewpoint.hpp"
#include "mvc/mvc_manager.hpp"
#include "selection/selection_manager.hpp"
#include "state/export.hpp"
#include "vtx_app.hpp"
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
					_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
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
				explicit ChangeDurationMode( Model::Path & p_path, const VTX::Path::DURATION_MODE p_mode ) :
					_path( p_path ), _mode( p_mode )
				{
					_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
				}

				virtual void execute() override
				{
					_path.setDurationMode( _mode );
					_path.refreshAllDurations();
				}

			  private:
				Model::Path &					 _path;
				const VTX::Path::DURATION_MODE _mode;
			};

			class ChangeInterpolationMode : public BaseAction
			{
			  public:
				explicit ChangeInterpolationMode( Model::Path & p_path, const VTX::Path::INTERPOLATION_MODE p_mode ) :
					_path( p_path ), _mode( p_mode )
				{
					_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
				}

				virtual void execute() override
				{
					_path.setInterpolationMode( _mode );
					_path.refreshAllDurations();
				}

			  private:
				Model::Path &						  _path;
				const VTX::Path::INTERPOLATION_MODE _mode;
			};

			class Play : public BaseAction
			{
			  public:
				explicit Play( Model::Path * const p_path ) : _path( p_path ) {}
				virtual void execute() override { VTXApp::get().goToState( ID::State::PLAY, _path ); };

			  private:
				Model::Path * const _path;
			};

			/*
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
			*/

			class ChangeIsLooping : public BaseAction
			{
			  public:
				explicit ChangeIsLooping( Model::Path & p_path, const bool p_isLooping ) :
					_path( p_path ), _isLooping( p_isLooping )
				{
					_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
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

			class Clear : public BaseAction
			{
			  public:
				explicit Clear( Model::Path & p_path ) : _path( p_path )
				{
					_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
				}

				virtual void execute() override
				{
					Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
					selectionModel.unselectModels( _path.getViewpoints() );

					while ( _path.getViewpoints().size() )
					{
						Model::Viewpoint * const viewpointToDelete = _path.getViewpoints().back();
						_path.removeViewpoint( viewpointToDelete );

						MVC::MvcManager::get().deleteModel( viewpointToDelete );
					}
				}

			  private:
				Model::Path & _path;
			};

		} // namespace Path
	}	  // namespace Action
} // namespace VTX
#endif
