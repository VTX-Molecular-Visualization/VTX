#include "app/core/action/action_system.hpp"
#include <exception>
#include <typeinfo>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>

namespace VTX::App::Core::Action
{
	ActionSystem::ActionSystem()  = default;
	ActionSystem::~ActionSystem() = default;
	void ActionSystem::execute( std::unique_ptr<BaseAction> & p_actionPtr )
	{
		VTX_DEBUG( "ActionSystem::execute( {} )", typeid( *p_actionPtr ).name() );
		try
		{
			auto duration = Util::CHRONO_CPU( [ &p_actionPtr ]() { p_actionPtr->execute(); } );
			VTX_DEBUG(
				"ActionSystem::execute( {} ) - done ({})",
				typeid( *p_actionPtr ).name(),
				Util::String::durationToStr( duration )
			);
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Error with action of type {} : {}", typeid( *p_actionPtr ).name(), p_e.what() );
			return;
		}
	}

	void ActionSystem::execute( std::unique_ptr<BaseActionUndonable> & p_actionPtr )
	{
		VTX_DEBUG( "ActionSystem::execute( {} )", typeid( *p_actionPtr ).name() );
		try
		{
			auto duration = Util::CHRONO_CPU( [ &p_actionPtr ]() { p_actionPtr->execute(); } );
			VTX_DEBUG(
				"ActionSystem::execute( {} ) - done ({})",
				typeid( *p_actionPtr ).name(),
				Util::String::durationToStr( duration )
			);
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Error with action of type {} : {}", typeid( *p_actionPtr ).name(), p_e.what() );
			return;
		}

		// Handle undo.
		_bufferUndo.push_front( std::move( p_actionPtr ) );
		_purgeBuffer();

		_bufferRedo.clear();
	}

	bool ActionSystem::canUndo() const { return _bufferUndo.size() > 0; }

	void ActionSystem::undo()
	{
		if ( !canUndo() )
			return;

		VTX_DEBUG( "Undo ( {} more )", ( _bufferUndo.size() - 1 ) );
		std::unique_ptr<BaseActionUndonable> & actionToUndo = _bufferUndo.front();

		actionToUndo->undo();
		_bufferRedo.push_front( std::move( actionToUndo ) );
		_bufferUndo.pop_front();

		// VTX_EVENT( "UNDO_ACTION", *actionToUndo );
		// switch ( actionToUndo->getTag() )
		//{
		// case ACTION_TAG::MODIFY_SCENE: App::Old::APP::getScenePathData().decrementSceneModifications();
		// break; case ACTION_TAG::NONE: default: break;
		// }
	}

	bool ActionSystem::canRedo() const { return _bufferRedo.size() > 0; }

	void ActionSystem::redo()
	{
		if ( !canRedo() )
			return;

		VTX_DEBUG( "Redo ( {} more ).", ( _bufferRedo.size() - 1 ) );

		std::unique_ptr<BaseActionUndonable> & actionToRedo = _bufferRedo.front();
		actionToRedo->redo();
		_bufferUndo.push_front( std::move( actionToRedo ) );
		_bufferRedo.pop_front();

		// VTX_EVENT( "REDO_ACTION", *actionToUndo );
		// switch ( actionToRedo->getTag() )
		//{
		// case ACTION_TAG::MODIFY_SCENE: App::Old::APP::getScenePathData().incrementSceneModifications();
		// break; case ACTION_TAG::NONE: default: break;
		//}
	}

	void ActionSystem::setBufferSize( const uint p_bufferSize )
	{
		_bufferSize = p_bufferSize;
		_purgeBuffer();
	}

	void ActionSystem::_purgeBuffer()
	{
		if ( _bufferSize == 0 )
			return;

		while ( _bufferUndo.size() > _bufferSize )
		{
			// const std::unique_ptr<BaseActionUndonable> & purgedAction = _bufferUndo.back();
			//  VTX_EVENT( "PURGE_ACTION", *purgedAction );
			//  If we loose the last action which modify scene before saving, the scene can't return to unmodified
			//  state if ( App::Old::APP::getScenePathData().getSceneModificationsCounter() < 0 )
			//{
			//	App::Old::APP::getScenePathData().forceSceneModifications();
			// }

			_bufferUndo.pop_back();
		}
	}
} // namespace VTX::App::Core::Action
