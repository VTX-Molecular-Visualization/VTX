#include "app/helper/chemistry.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/chemistry/system.hpp"
#include <app/filesystem.hpp>
#include <io/reader/residue_data_reader.hpp>

namespace VTX::App::Helper::Chemistry
{
	std::vector<Component::Chemistry::Chain *> findChainsByName(
		const Component::Chemistry::System & p_system,
		const std::string &					 p_chainName
	)
	{
		std::vector<Component::Chemistry::Chain *> res = std::vector<Component::Chemistry::Chain *>();

		for ( const std::unique_ptr<Component::Chemistry::Chain> & chain : p_system.getChains() )
		{
			if ( chain != nullptr && chain->getName() == p_chainName )
				res.emplace_back( chain.get() );
		}

		return res;
	}

	std::vector<Component::Chemistry::Residue *> findResiduesByName(
		const Component::Chemistry::Chain & p_chain,
		const std::string &					p_residueName
	)
	{
		using namespace VTX::Core;

		std::vector<Component::Chemistry::Residue *> res	  = std::vector<Component::Chemistry::Residue *>();
		Component::Chemistry::System &				 system = *( p_chain.getSystemPtr() );

		const ChemDB::Residue::SYMBOL residueSymbol = ChemDB::Residue::getSymbolFromAnyName( p_residueName );

		if ( residueSymbol != ChemDB::Residue::SYMBOL::UNKNOWN )
		{
			for ( size_t iRes = p_chain.getIndexFirstResidue(); iRes <= p_chain.getIndexLastResidue(); iRes++ )
			{
				Component::Chemistry::Residue * const residue = system.getResidue( iRes );

				if ( residue != nullptr && residue->getSymbol() == residueSymbol )
					res.emplace_back( residue );
			}
		}
		else // Unknown symbol => check names
		{
			for ( size_t iRes = p_chain.getIndexFirstResidue(); iRes <= p_chain.getIndexLastResidue(); iRes++ )
			{
				Component::Chemistry::Residue * const residue = system.getResidue( iRes );

				if ( residue == nullptr || residue->getSymbol() != ChemDB::Residue::SYMBOL::UNKNOWN )
					continue;

				if ( residue->getName() == p_residueName || residue->getShortName() == p_residueName
					 || residue->getLongName() == p_residueName )
				{
					res.emplace_back( residue );
				}
			}
		}

		return res;
	}

	void loadResidueData( const std::string & p_residueSymbol )
	{
		IO::Reader::ResidueDataReader reader = IO::Reader::ResidueDataReader();
		IO::Struct::ResidueData		  residueData;

		if ( reader.readResidueData(
				 Filesystem::getResidueDataFilePath( p_residueSymbol ), p_residueSymbol, residueData
			 ) )
			mapLoadedResidueData.emplace( p_residueSymbol, residueData );
		else
			mapLoadedResidueData.emplace( p_residueSymbol, IO::Struct::ResidueData::DEFAULT );
	}

	const std::string & getResidueFullName( const std::string & p_residueSymbol )
	{
		if ( mapLoadedResidueData.find( p_residueSymbol ) == mapLoadedResidueData.end() )
			loadResidueData( p_residueSymbol );

		return mapLoadedResidueData[ p_residueSymbol ].fullname;
	}

	const std::vector<VTX::IO::Struct::BondData> & getResidueBonds( const std::string & p_residueSymbol )
	{
		if ( mapLoadedResidueData.find( p_residueSymbol ) == mapLoadedResidueData.end() )
			loadResidueData( p_residueSymbol );

		return mapLoadedResidueData[ p_residueSymbol ].bondData;
	}

	VTX::Core::ChemDB::Category::TYPE getResidueCategory( const std::string & p_residueSymbol )
	{
		if ( mapLoadedResidueData.find( p_residueSymbol ) == mapLoadedResidueData.end() )
			loadResidueData( p_residueSymbol );

		return mapLoadedResidueData[ p_residueSymbol ].category;
	}

} // namespace VTX::App::Helper::Chemistry
