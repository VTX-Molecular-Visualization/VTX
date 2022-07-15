#include "util/chemfiles.hpp"
#include "util/bond_guessing/bond_order_guessing.hpp"
#include "util/bond_guessing/bond_recomputation.hpp"

namespace VTX::Util::Chemfiles
{
	chemfiles::Bond::BondOrder convertBondOrder( const Model::Bond::ORDER p_bondOrder )
	{
		switch ( p_bondOrder )
		{
		case Model::Bond::ORDER::SINGLE: return chemfiles::Bond::BondOrder::SINGLE; break;
		case Model::Bond::ORDER::DOUBLE: return chemfiles::Bond::BondOrder::DOUBLE; break;
		case Model::Bond::ORDER::TRIPLE: return chemfiles::Bond::BondOrder::TRIPLE; break;
		case Model::Bond::ORDER::QUADRUPLE: return chemfiles::Bond::BondOrder::QUADRUPLE; break;
		case Model::Bond::ORDER::QUINTUPLET: return chemfiles::Bond::BondOrder::QUINTUPLET; break;
		case Model::Bond::ORDER::AROMATIC: return chemfiles::Bond::BondOrder::AROMATIC; break;

		case Model::Bond::ORDER::UNKNOWN:
		default: return chemfiles::Bond::BondOrder::UNKNOWN; break;
		}
	}

	void recomputeBonds( chemfiles::Frame & p_frame, const VTX::Object3D::Helper::AABB & p_aabb )
	{
		BondGuessing::BondRecomputation::recomputeBonds( p_frame, p_aabb );
	}

	void recomputeBondOrders( chemfiles::Frame & p_frame )
	{
		BondGuessing::BondOrderGuessing::recomputeBondOrders( p_frame );
	}

	bool recomputeBondOrdersFromFile( chemfiles::Frame & p_frame )
	{
		return BondGuessing::BondOrderGuessing::recomputeBondOrdersFromFile( p_frame );
	}

} // namespace VTX::Util::Chemfiles
