#include "app/util/chemfiles.hpp"
#include "app/internal/algorithm/bond_order_guessing.hpp"
#include "app/internal/algorithm/bond_recomputation.hpp"

namespace VTX::Util::App::Chemfiles
{
	chemfiles::Bond::BondOrder convertBondOrder( const VTX::App::Internal::ChemDB::Bond::ORDER p_bondOrder )
	{
		switch ( p_bondOrder )
		{
		case VTX::App::Internal::ChemDB::Bond::ORDER::SINGLE: return chemfiles::Bond::BondOrder::SINGLE; break;
		case VTX::App::Internal::ChemDB::Bond::ORDER::DOUBLE: return chemfiles::Bond::BondOrder::DOUBLE; break;
		case VTX::App::Internal::ChemDB::Bond::ORDER::TRIPLE: return chemfiles::Bond::BondOrder::TRIPLE; break;
		case VTX::App::Internal::ChemDB::Bond::ORDER::QUADRUPLE: return chemfiles::Bond::BondOrder::QUADRUPLE; break;
		case VTX::App::Internal::ChemDB::Bond::ORDER::QUINTUPLET: return chemfiles::Bond::BondOrder::QUINTUPLET; break;
		case VTX::App::Internal::ChemDB::Bond::ORDER::AROMATIC: return chemfiles::Bond::BondOrder::AROMATIC; break;

		case VTX::App::Internal::ChemDB::Bond::ORDER::UNKNOWN:
		default: return chemfiles::Bond::BondOrder::UNKNOWN; break;
		}
	}

	void recomputeBonds( chemfiles::Frame & p_frame, const VTX::App::Component::Object3D::Helper::AABB & p_aabb )
	{
		VTX::App::Internal::Algorithm::BondRecomputation::recomputeBonds( p_frame, p_aabb );
	}

	void recomputeBondOrders( chemfiles::Frame & p_frame )
	{
		VTX::App::Internal::Algorithm::BondOrderGuessing::recomputeBondOrders( p_frame );
	}

	bool recomputeBondOrdersFromFile( chemfiles::Frame & p_frame )
	{
		return VTX::App::Internal::Algorithm::BondOrderGuessing::recomputeBondOrdersFromFile( p_frame );
	}

} // namespace VTX::Util::App::Chemfiles
