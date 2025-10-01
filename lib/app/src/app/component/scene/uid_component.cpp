#include "app/component/scene/uid_component.hpp"

namespace VTX::App::Component::Scene
{
	UIDComponent::UIDComponent( const App::Uid::uid & p_uid ) { referenceUID( p_uid ); }
	UIDComponent::UIDComponent( const Uid::UIDRange & p_range ) { referenceUID( p_range ); }

	void UIDComponent::referenceUID( const Uid::UIDRange & p_range ) { _uids.addRange( p_range ); }
	void UIDComponent::referenceUID( const Uid::uid & p_uid ) { _uids.addValue( p_uid ); }

	bool UIDComponent::contains( const App::Uid::uid & p_uid ) const { return _uids.contains( p_uid ); }
	bool UIDComponent::contains( const App::Uid::UIDRange & p_uidRange ) const { return _uids.contains( p_uidRange ); }

} // namespace VTX::App::Component::Scene
