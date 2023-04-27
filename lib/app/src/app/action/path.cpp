#include "app/action/path.hpp"
#include "app/mvc.hpp"
#include "app/model/selection.hpp"
#include "app/model/viewpoint.hpp"
#include "app/old_app/selection/selection_manager.hpp"
// #include "state/export.hpp"
#include "app/old_app/vtx_app.hpp"
#include "app/worker/snapshoter.hpp"

namespace VTX::App::Action::Path
{
	void ChangeDuration::execute()
	{
		_path.setDuration( Util::Math::max<float>( _duration, 0.f ) );
		_path.refreshAllDurations();
	}

	void ChangeDurationMode::execute()
	{
		_path.setDurationMode( _mode );
		_path.refreshAllDurations();
	}

	void ChangeInterpolationMode::execute()
	{
		_path.setInterpolationMode( _mode );
		_path.refreshAllDurations();
	}

	void Play::execute() { VTXApp::get().goToState( ID::State::PLAY, _path ); }

	// void ExportVideo::execute()
	//{
	//	State::Export::Arg arg;
	//	arg.mode = _mode;
	//	arg.path = _path;

	//	VTXApp::get().goToState( ID::State::EXPORT, &arg );
	//}

	void ChangeIsLooping::execute() { _path.setIsLooping( _isLooping ); }

	void Clear::execute()
	{
		Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		selectionModel.unselectModels( _path.getViewpoints() );

		while ( _path.getViewpoints().size() )
		{
			Model::Viewpoint * const viewpointToDelete = _path.getViewpoints().back();
			_path.removeViewpoint( viewpointToDelete );

			VTX::MVC_MANAGER().deleteModel( viewpointToDelete );
		}
	}

} // namespace VTX::App::Action::Path
