#ifndef __VTX_IO_WRITER_CHEMFILES__
#define __VTX_IO_WRITER_CHEMFILES__

#include <util/types.hpp>
#include <variant>

namespace VTX::IO::Writer
{

	enum class E_BOND_ORDER
	{
		unknown,
		single,
		doubl,
		triple,
		quadruple,
		quintuple,
		down,
		up,
		dative_L,
		dative_R,
		amide,
		aromatic,
	};

	using PropertyValue = std::variant<std::string, double, bool>;
	/**
	 * @brief Property that can be attached to various components of the system
	 */
	struct Property
	{
		std::string	  key;
		PropertyValue value;
	};

	/**
	 * @brief Id of an atom. This structure exists so we have explicit method taking this struct as input.
	 */
	struct AtomId
	{
		uint64_t id = 0xffffffffffffffff;
	};

	/**
	 * @brief Coordinates of an atom
	 */
	struct AtomCoordinates
	{
		double x = 0.;
		double y = 0.;
		double z = 0.;
	};

	struct _Atom;
	struct _AtomInfo;
	/**
	 * @brief Atoms are provided by the System.
	 */
	class Atom
	{
	  public:
		Atom() = default;
		Atom( _Atom & );

		/**
		 * @brief You need to be the chosen one to wield this method to its full power.
		 * @param
		 */
		void get( _AtomInfo & ) noexcept;
	};

	/**
	 * @brief Allows to configure a residue to be written. Residues can be added atoms and properties.
	 */
	class Residue
	{
	  public:
		/**
		 * @brief Inform the chain that the input atom is a part of it.
		 */
		void add( Atom & ) noexcept;

		/**
		 * @brief Set or replace a property for the chain.
		 */
		void set( Property ) noexcept;
	};

	struct _Chain;
	/**
	 * @brief Allows to configure a chain to be written.
	 */
	class Chain
	{
	  public:
		Chain() = default;
		Chain( _Chain & );
		/**
		 * @brief Inform the chain that the input residue is a part of it.
		 */
		void add( Residue & ) noexcept;

	  private:
		_Chain * _data = nullptr;
	};

	/**
	 * @brief Allow to configure one frame of the system. A frame is defined by a set of coordinates for all atoms. If a
	 * simple structure file is written, only one frame will suffice.
	 */
	class Frame
	{
	  public:
		void set( AtomId, AtomCoordinates ) noexcept;
	};

	struct _System;
	/**
	 * @brief Responsible for creating objects to configure the chemfiles system.
	 */
	class System
	{
	  public:
		System() = default;
		System( _System & );

		/**
		 * @brief Set or replace a property for the frame.
		 */
		void set( Property ) noexcept;

		/**
		 * @brief  Create a new atom in this frame
		 * @param  Id of the atom. Used as an external key to indentify the atom afterward.
		 */
		Atom newAtom( AtomId ) noexcept;

		/**
		 * @brief  Bind two atoms together. Atoms are identified by the external key provided during their creation.
		 */
		void bind( const AtomId &, const AtomId &, E_BOND_ORDER );

		/**
		 * @brief Create a new chain in this frame.
		 */
		Chain newChain() noexcept;

		/**
		 * @brief Create a new residue in this frame.
		 */
		Residue newResidue() noexcept;

		/**
		 * @brief Create a new frame. A healthy system should get at least one frame.
		 */
		Frame newFrame() noexcept;

	  public:
		_System * _data = nullptr;
	};

	/**
	 * @brief Allows to write a file containing molecule's structural information using the Chemfiles library.
	 */
	class Chemfiles
	{
	  public:
		enum class E_FILE_FORMATS
		{
			none,
			pdb,
			mmcif,
			mmtf,
			molden,
			cif,
			gro,
			mol2,
			sdf,
			smi,
			xyz,
			cml,
			cssr,
			nc,
			dcd,
			lammpstrj,
			arc,
			trr,
			xtc,
			tng,
			trj,
			COUNT
		};

		Chemfiles();

		/**
		 * @brief Mandatory to write something. Can be set empty so no files end up being saved at all.
		 */
		void setWriteDestination( FilePath ) noexcept;

		/**
		 * @brief Tells the writer in which format to write structural data. If left empty or set to none, the format
		 * will be guessed from the file's extension.
		 */
		void setWriteFormat( E_FILE_FORMATS ) noexcept;

		/**
		 * @brief Create/retrieve the system that will be written eventually.
		 * @return The system to configure
		 */
		System system() noexcept;

	  private:
		class _Impl;
		struct Del
		{
			void operator()( _Impl * ) noexcept;
		};
		std::unique_ptr<_Impl, Del> _ = nullptr;
	};
} // namespace VTX::IO::Writer
#endif
