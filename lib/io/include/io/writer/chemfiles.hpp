#ifndef VTX_IO_WRITER_CHEMFILES_HPP
#define VTX_IO_WRITER_CHEMFILES_HPP

#include <io/writer/shared.hpp>
#include <string>
#include <util/filesystem.hpp>
#include <util/types.hpp>
#include <variant>

namespace VTX::IO::Writer
{

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
		uint64_t value										 = 0xffffffffffffffff;
		bool	 operator==( const AtomId & ) const noexcept = default;
	};

	inline bool operator<( const AtomId & lhs, const AtomId & rhs ) noexcept { return lhs.value < rhs.value; }

	/**
	 * @brief Coordinates of an atom
	 */
	struct AtomCoordinates
	{
		double x = 0.;
		double y = 0.;
		double z = 0.;
	};

	struct AtomImpl;
	struct AtomInfo;

	/**
	 * @brief Atoms are provided by the System.
	 */
	class Atom
	{
	  public:
		Atom() = default;
		Atom( AtomImpl & );

		/**
		 * @brief You need to be the chosen one to wield this method to its full power.
		 * @param
		 */
		void get( AtomInfo & ) const noexcept;

		void setName( std::string ) noexcept;
		void setSymbol( std::string ) noexcept;

	  private:
		AtomImpl * _data;
	};

	struct ResidueImpl;

	/**
	 * @brief Allows to configure a residue to be written. Residues can be added atoms and properties.
	 */
	class Residue
	{
	  public:
		Residue() = default;
		Residue( ResidueImpl & );
		/**
		 * @brief Inform the chain that the input atom is a part of it.
		 */
		void add( Atom & );

		/**
		 * @brief Set or replace a property for the chain.
		 */
		void set( const Property & );
		void setResId( int ) noexcept;
		void setSymbol( std::string ) noexcept;

	  private:
		ResidueImpl * _data = nullptr;
	};

	struct ChainImpl;

	/**
	 * @brief Allows to configure a chain to be written.
	 */
	class Chain
	{
	  public:
		Chain() = default;
		Chain( ChainImpl & );

		void setName( std::string ) noexcept;
		void setId( std::string ) noexcept;

		/**
		 * @brief Inform the chain that the input residue is a part of it. Does nothing however, if the chain info are
		 * not provided in the first place
		 */
		void add( Residue & );

	  private:
		ChainImpl * _data = nullptr;
	};

	struct FrameImpl;

	/**
	 * @brief Allow to configure one frame of the system. A frame is defined by a set of coordinates for all atoms. If a
	 * simple structure file is written, only one frame will suffice.
	 */
	class Frame
	{
	  public:
		Frame() = default;
		Frame( FrameImpl & );

		/**
		 * @brief Give the coordinate of the atom for this frame.
		 */
		void set( const Atom &, AtomCoordinates );

	  private:
		FrameImpl * _data;
	};

	struct SystemImpl;

	/**
	 * @brief Responsible for creating objects to configure the chemfiles system.
	 */
	class System
	{
	  public:
		System() = default;
		System( SystemImpl & );

		/**
		 * @brief Set or replace a property for the frame.
		 */
		void set( Property ) const;

		/**
		 * @brief  Create a new atom
		 * @param  Id of the atom. Used as an external key to indentify the atom afterward.
		 */
		Atom newAtom( AtomId ) const;

		/**
		 * @brief  Bind two atoms together. Atoms are identified by the external key provided during their creation.
		 * Throws if the provided IDs aren't valid.
		 */
		void bind( const AtomId &, const AtomId &, E_BOND_ORDER ) const;

		/**
		 * @brief Create a new chain.
		 */
		Chain newChain() const;

		/**
		 * @brief Create a new residue.
		 */
		Residue newResidue() const;

		/**
		 * @brief Create a new frame. A healthy system should get at least one frame.
		 */
		Frame newFrame() const;

		/**
		 * @brief Set the input Atom to the one corresponding to the input AtomId
		 * @param  Output
		 * @param  ID corresponding to the atom
		 * @return  Wether the fetch was succesful or not
		 */
		bool fetch( Atom &, const AtomId & ) const noexcept;

		/**
		 * @brief Return the frame with the right index
		 * @param Output
		 * @param frame number
		 * @return Wether the fetch was succesful or not
		 */
		bool fetch( Frame &, const size_t & ) const noexcept;

		SystemImpl * _data = nullptr;
	};

	/**
	 * @brief Allows to write a file containing system's structural information using the ChemfilesTrajectory library.
	 * This class is intended to cover needs in trajectory writing. It expect all the frames ( 1 or more) to have the
	 * same chemical configuration : no change in atoms, bond, residues, chains between frames.
	 */
	class ChemfilesTrajectory
	{
	  public:
		ChemfilesTrajectory();

		void close();

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
		class Impl;

		struct Del
		{
			void operator()( Impl * ) noexcept;
		};

		std::unique_ptr<Impl, Del> _ = nullptr;
	};
} // namespace VTX::IO::Writer
#endif
