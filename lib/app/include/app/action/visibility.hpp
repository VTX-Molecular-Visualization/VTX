#ifndef __VTX_APP_ACTION_VISIBILLITY__
#define __VTX_APP_ACTION_VISIBILLITY__

#include "app/core/action/base_action.hpp"

namespace VTX::App::Action::Visibility
{
	enum struct E_ITEM_TYPE
	{
		SYSTEM,
		CATEGORY,
		CHAIN,
		RESIDUE,
		ATOM
	};

	template<E_ITEM_TYPE T>
	class SetVisible final : public App::Core::Action::BaseAction
	{
	  public:
		explicit SetVisible(
			App::Component::Chemistry::System * const p_system,
			const bool								  p_visible,
			const std::optional<size_t>				  p_id
		) : _system( p_system ), _visible( p_visible ), _id( p_id )
		{
		}

		void execute() override
		{
			assert( _system );

			if constexpr ( T == E_ITEM_TYPE::SYSTEM )
			{
				_system->setVisible( _visible );
			}
			else if constexpr ( T == E_ITEM_TYPE::CATEGORY ) {}
			else if constexpr ( T == E_ITEM_TYPE::CHAIN )
			{
				assert( _id.has_value() );
				auto * const chain = _system->getChain( _id.value() );
				assert( chain );
				chain->setVisible( _visible );
			}
			else if constexpr ( T == E_ITEM_TYPE::RESIDUE )
			{
				assert( _id.has_value() );
				auto * const residue = _system->getResidue( _id.value() );
				assert( residue );
				residue->setVisible( _visible );
			}
			else if constexpr ( T == E_ITEM_TYPE::ATOM )
			{
				assert( _id.has_value() );
				auto * const atom = _system->getAtom( atom_index_t( _id.value() ) );
				assert( atom );
				atom->setVisible( _visible );
			}
		}

	  private:
		App::Component::Chemistry::System * const _system;
		const bool								  _visible;
		const std::optional<size_t>				  _id;
	};

} // namespace VTX::App::Action::Visibility
#endif
