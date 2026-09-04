#include "app/system/writer.hpp"
#include "app/ecs.hpp"
#include "app/helper/trajectory.hpp"
#include "app/services.hpp"
#include "app/system/selection.hpp"
#include "app/system/visibility.hpp"
#include <io/writer/system.hpp>

namespace VTX::App::System
{
	struct SelectionWriter::_impl
	{
		_impl( FilePath p_dest ) : _dest( std::move( p_dest ) ) {}

		FilePath								   _dest;
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

	SelectionWriter::~SelectionWriter() = default;

	void SelectionWriter::operator()()
	{
		VTX::IO::Writer::writeFile(
			VTX::IO::Writer::WriteArgs { .destination = _ptr->_dest, .topologies = std::move( _ptr->systems ) }
		);
	}

	struct VisibleWriter::_impl
	{
		_impl( FilePath p_dest ) : _dest( std::move( p_dest ) ) {}

		FilePath								   _dest;
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

	VisibleWriter::~VisibleWriter() = default;

	void VisibleWriter::operator()()
	{
		VTX::IO::Writer::writeFile(
			VTX::IO::Writer::WriteArgs { .destination = _ptr->_dest, .topologies = std::move( _ptr->systems ) }
		);
	}

} // namespace VTX::App::System
