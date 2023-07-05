#ifndef __VTX_APP_COMPONENT_CHEMISTRY_RESIDUE__
#define __VTX_APP_COMPONENT_CHEMISTRY_RESIDUE__

#include "_fwd.hpp"
#include <core/chemdb/residue.hpp>
#include <core/chemdb/secondary_structure.hpp>
#include <core/struct/residue.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::Core::ChemDB;

	class Residue : public Core::Struct::Residue
	{
	  public:
		Residue() = default;
		Residue( const size_t p_index );
		Residue( Chain * const p_chain, const size_t p_index );

		const Util::Color::Rgba & getColor() const { return _color; };
		void					  setColor( const Util::Color::Rgba p_color ) { _color = p_color; };

		ChemDB::Residue::TYPE getType() { return _type; };
		void				  setType( const ChemDB::Residue::TYPE p_type ) { _type = p_type; };

		int	 getIndexInOriginalChain() { return _indexInOriginalChain; };
		void setIndexInOriginalChain( const int p_indexInOriginalChain )
		{
			_indexInOriginalChain = p_indexInOriginalChain;
		};

		const char getInsertionCode() const { return _insertionCode; }
		void	   setInsertionCode( const char p_insertionCode ) { _insertionCode = p_insertionCode; }

		ChemDB::SecondaryStructure::TYPE getSecondaryStructure() const { return _ssType; }
		void setSecondaryStructure( const ChemDB::SecondaryStructure::TYPE p_ssType ) { _ssType = p_ssType; }

	  private:
		Util::Color::Rgba	  _color				= COLOR_WHITE;
		int					  _indexInOriginalChain = 0;
		ChemDB::Residue::TYPE _type					= ChemDB::Residue::TYPE::STANDARD;
		char				  _insertionCode		= ' ';

		ChemDB::SecondaryStructure::TYPE _ssType;
	};
} // namespace VTX::App::Component::Chemistry
#endif
