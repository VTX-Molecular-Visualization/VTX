#include "app/system/writer.hpp"
#include "app/ecs.hpp"
#include "app/helper/trajectory.hpp"
#include "app/services.hpp"
#include "app/system/selection.hpp"
#include "app/system/visibility.hpp"
#include <io/writer/system.hpp>
#include <latch>

namespace VTX::App::System
{
	struct SelectionWriter::_impl
	{
		_impl( FilePath p_dest ) : _dest( std::move( p_dest ) ) {}

		FilePath								   _dest;
		std::latch								   synchronizer { 1 };
		std::vector<IO::Writer::WriteArgs::System> systems;
	};

	class SelectedAtomFilter
	{
	  public:
		SelectedAtomFilter( const System::Selection & p_ ) : _selection( p_ ) {}

		bool operator()( const size_t & p_atomIdx ) const noexcept { return _selection.get().atoms.test( p_atomIdx ); }

	  private:
		std::reference_wrapper<const System::Selection> _selection;
	};

	SelectionWriter::SelectionWriter( FilePath p_dest ) : _ptr( new _impl( std::move( p_dest ) ) )
	{
		std::vector<Entity> systems;
		for ( auto it_selectionEntt : REG().group<System::Selection, Core::Struct::Topology>() )
		{
			_ptr->systems.emplace_back(
				IO::Writer::WriteArgs::System {
					.topology	= &REG().get<Core::Struct::Topology>( it_selectionEntt ),
					.atomFilter = SelectedAtomFilter( REG().get<System::Selection>( it_selectionEntt ) ) }
			);
			VTX::App::Helper::Trajectory::get( it_selectionEntt, _ptr->systems.back().trajectory );
		}
	}

	uint SelectionWriter::operator()( Util::StopToken p_token, Threading::OptionalThreadReference p_thread )
	{
		if ( p_token.stop_requested() )
		{
			return 0;
		}

		if ( p_thread )
		{
			p_thread.value().get().setProgressText( fmt::format( "Writting file {} ...", _ptr->_dest.string() ) );
		}

		VTX::IO::Writer::writeFile(
			VTX::IO::Writer::WriteArgs { .destination = _ptr->_dest, .topologies = std::move( _ptr->systems ) }
		);
		return 0;
	}

	void SelectionWriter::wait() noexcept { _ptr->synchronizer.wait(); }

	struct VisibleWriter::_impl
	{
		_impl( FilePath p_dest ) : _dest( std::move( p_dest ) ) {}

		FilePath								   _dest;
		std::latch								   synchronizer { 1 };
		std::vector<IO::Writer::WriteArgs::System> systems;
	};

	class VisibleAtomFilter
	{
	  public:
		VisibleAtomFilter( const System::Visibility & p_ ) : _visibility( p_ ) {}

		bool operator()( const size_t & p_atomIdx ) const noexcept { return _visibility.get().atoms.test( p_atomIdx ); }

	  private:
		std::reference_wrapper<const System::Visibility> _visibility;
	};

	VisibleWriter::VisibleWriter( FilePath p_dest ) : _ptr( new _impl( std::move( p_dest ) ) )
	{
		std::vector<Entity> systems;
		for ( auto it_selectionEntt : REG().group<System::Visibility, Core::Struct::Topology>() )
		{
			_ptr->systems.emplace_back(
				IO::Writer::WriteArgs::System {
					.topology	= &REG().get<Core::Struct::Topology>( it_selectionEntt ),
					.atomFilter = VisibleAtomFilter( REG().get<System::Visibility>( it_selectionEntt ) ) }
			);
			VTX::App::Helper::Trajectory::get( it_selectionEntt, _ptr->systems.back().trajectory );
		}
	}

	uint VisibleWriter::operator()( Util::StopToken p_token, Threading::OptionalThreadReference p_thread )
	{
		if ( p_token.stop_requested() )
		{
			return 0;
		}

		if ( p_thread )
		{
			p_thread.value().get().setProgressText( fmt::format( "Writting file {} ...", _ptr->_dest.string() ) );
		}

		VTX::IO::Writer::writeFile(
			VTX::IO::Writer::WriteArgs { .destination = _ptr->_dest, .topologies = std::move( _ptr->systems ) }
		);
		return 0;
	}

	void VisibleWriter::wait() noexcept { _ptr->synchronizer.wait(); }

} // namespace VTX::App::System
