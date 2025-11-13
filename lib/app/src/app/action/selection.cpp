#include "app/action/selection.hpp"
#include "app/system/selection.hpp"
#include <core/struct/system.hpp>

namespace
{
	using namespace VTX;
	using namespace App;

	void consolidate( const Core::Struct::System & p_system, System::Selection & p_selection )
	{
		if ( p_selection.isFullSystemSelected )
		{
			p_selection.chains.clear();
			p_selection.residues.clear();
			p_selection.atoms.clear();
			return;
		}

		//
	}
} // namespace

namespace VTX::App::Action::Selection
{
	void SelectSystem::execute( const ECS::Entity p_ent, const bool p_select )
	{
		auto & selection = REG().get<System::Selection>( p_ent );

		selection.isFullSystemSelected = p_select;
		selection.chains.clear();
		selection.residues.clear();
		selection.atoms.clear();
	}

	void SelectChain::execute( const ECS::Entity p_ent, const Index p_chainIndex, const bool p_select )
	{
		SelectChains s;
		s.execute( p_ent, { { p_chainIndex, p_chainIndex + 1 } }, p_select );
	}

	void SelectChains::execute(
		const ECS::Entity					 p_ent,
		const Util::Math::RangeList<Index> & p_chainIndices,
		const bool							 p_select
	)
	{
		const auto & system	   = REG().get<Core::Struct::System>( p_ent );
		auto &		 selection = REG().get<System::Selection>( p_ent );

		if ( p_select )
		{
			for ( const auto & range : p_chainIndices )
			{
				selection.chains.addRange( range );
			}
		}
		else
		{
			for ( const auto & range : p_chainIndices )
			{
				selection.chains.removeRange( range );
			}
		}
	}

} // namespace VTX::App::Action::Selection
