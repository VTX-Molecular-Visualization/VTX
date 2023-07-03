#include "app/old/internal/chemfiles/util.hpp"
#include "app/old/internal/algorithm/bond_order_guessing.hpp"
#include "app/old/internal/algorithm/bond_recomputation.hpp"

namespace VTX::App::Old::Internal::Chemfiles::Util
{
	chemfiles::Bond::BondOrder convertBondOrder( const VTX::Core::ChemDB::Bond::ORDER p_bondOrder )
	{
		switch ( p_bondOrder )
		{
		case VTX::Core::ChemDB::Bond::ORDER::SINGLE: return chemfiles::Bond::BondOrder::SINGLE; break;
		case VTX::Core::ChemDB::Bond::ORDER::DOUBLE: return chemfiles::Bond::BondOrder::DOUBLE; break;
		case VTX::Core::ChemDB::Bond::ORDER::TRIPLE: return chemfiles::Bond::BondOrder::TRIPLE; break;
		case VTX::Core::ChemDB::Bond::ORDER::QUADRUPLE: return chemfiles::Bond::BondOrder::QUADRUPLE; break;
		case VTX::Core::ChemDB::Bond::ORDER::QUINTUPLET: return chemfiles::Bond::BondOrder::QUINTUPLET; break;
		case VTX::Core::ChemDB::Bond::ORDER::AROMATIC: return chemfiles::Bond::BondOrder::AROMATIC; break;

		case VTX::Core::ChemDB::Bond::ORDER::UNKNOWN:
		default: return chemfiles::Bond::BondOrder::UNKNOWN; break;
		}
	}

	void recomputeBonds( chemfiles::Frame & p_frame, const VTX::App::Old::Component::Object3D::Helper::AABB & p_aabb )
	{
		VTX::App::Old::Internal::Algorithm::BondRecomputation::recomputeBonds( p_frame, p_aabb );
	}

	void recomputeBondOrders( chemfiles::Frame & p_frame )
	{
		VTX::App::Old::Internal::Algorithm::BondOrderGuessing::recomputeBondOrders( p_frame );
	}

	bool recomputeBondOrdersFromFile( chemfiles::Frame & p_frame )
	{
		return VTX::App::Old::Internal::Algorithm::BondOrderGuessing::recomputeBondOrdersFromFile( p_frame );
	}

} // namespace VTX::App::Old::Internal::Chemfiles::Util
