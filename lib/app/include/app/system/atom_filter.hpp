#ifndef __VTX_APP_SYSTEM_ATOM_FILTER__
#define __VTX_APP_SYSTEM_ATOM_FILTER__

#include <memory>

namespace VTX::App::System
{
	struct Visibility;
	struct Selection;
} // namespace VTX::App::System

/**
 * @brief Collections of classes to be used as atomFilters.
 * Initially intended to be used for writing.
 */
namespace VTX::App::System::atomFilters
{
	class Visible
	{
	  public:
		Visible( const Visibility & p_vis );

		bool operator()( const size_t & p_ ) const;

	  private:
		std::reference_wrapper<const Visibility> _visibility;
	};

	class Selected
	{
	  public:
		Selected( const Selection & );

		bool operator()( const size_t & ) const;

		std::reference_wrapper<const Selection> _selection;
	};
} // namespace VTX::App::System::atomFilters
#endif
