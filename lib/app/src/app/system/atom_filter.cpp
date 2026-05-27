#include "app/system/selection.hpp"
#include "app/system/visibility.hpp"
//
#include "app/system/atom_filter.hpp"

namespace VTX::App::System::atomFilters
{
	Visible::Visible( const Visibility & p_vis ) : _visibility( p_vis ) {}

	bool Visible::operator()( const size_t & p_ ) const { return _visibility.get().atoms.test( p_ ); }

	Selected::Selected( const Selection & p_ ) : _selection( p_ ) {}

	bool Selected::operator()( const size_t & p_ ) const { return _selection.get().atoms.test( p_ ); }

} // namespace VTX::App::System::atomFilters
