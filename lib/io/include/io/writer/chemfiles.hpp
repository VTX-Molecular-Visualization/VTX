#ifndef __VTX_IO_WRITER_CHEMFILES__
#define __VTX_IO_WRITER_CHEMFILES__

#include "io/util/chemfiles.hpp"
#include <forward_list>
#include <util/types.hpp>
#include <variant>

namespace VTX::IO::Writer
{
	/**
	 * @brief Property that can be attached to various components of the system
	 */
	struct Property
	{
		std::string								key;
		std::variant<std::string, double, bool> value;
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

	/**
	 * @brief Basic information for creating a chain
	 */
	struct ChainInfo
	{
		uint64_t	id = 0xffffffffffffffff;
		std::string symbol;
	};

	/**
	 * @brief Allows to configure an atom to be written. Atoms are provided by the System.
	 */
	class Atom
	{
	  public:
		/**
		 * @brief Set the coordinates of the atom.
		 */
		void set( AtomCoordinates ) noexcept;
	};

	/**
	 * @brief Allows to configure a chain to be written. The user is supposed to provide atoms to form the chain. Chains
	 * are provided by the System.
	 */
	class Chain
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

	/**
	 * @brief Allow to configure one frame of the system. A frame is defined by a set of all atoms coordinates. If a
	 * simple structure file is written, only one frame will suffice.
	 */
	class Frame
	{
	  public:
		void add( AtomId, AtomCoordinates ) noexcept;
	};

	/**
	 * @brief Allows to configure a frame to be written by Chemfiles. Be carefull : any instance is invalidated if the
	 * parent Chemmfiles object is moved away.
	 */
	class System
	{
	  public:
		/**
		 * @brief Set or replace a property for the frame.
		 */
		void set( Property ) noexcept;

		/**
		 * @brief Create a new atom in this frame
		 * @return the new Atom
		 */
		Atom newAtom( AtomId ) noexcept;

		/**
		 * @brief Create a new chain in this frame
		 * @param
		 * @return
		 */
		Chain newChain( ChainInfo ) noexcept;
	};

	/**
	 * @brief Allows to write a file containing molecule's structural information using the Chemfiles library
	 */
	class Chemfiles
	{
	  public:
		Chemfiles() = default;

		/**
		 * @brief Actual writing happens at object destruction.
		 */
		~Chemfiles();
		Chemfiles( Chemfiles && );
		Chemfiles & operator=( Chemfiles && );
		Chemfiles( const Chemfiles & )			   = delete;
		Chemfiles & operator=( const Chemfiles & ) = delete;

		/**
		 * @brief Mandatory to write something. Can be set empty so no files end up being saved at all.
		 */
		void setWriteDestination( FilePath ) noexcept;

		/**
		 * @brief Tells the writer in which format to write structural data. If left empty or set to none, the format
		 * will be guessed from the file's extension.
		 */
		void setWriteFormat( Util::Chemfiles::E_FILE_FORMATS ) noexcept;

		/**
		 * @brief Create the system that will be written eventually
		 * @return The system to configure
		 */
		System createSystem() noexcept;

	  private:
		struct _Chain
		{
		};
		using ChainCollection = std::forward_list<_Chain>; // Used for constant time insertion and memory efficiency
		struct _Frame
		{
		};
		using FrameCollection = std::forward_list<_Frame>; // Used for constant time insertion and memory efficiency
		struct _Data // Here so move ctor/assignement are easier to write and maintain
		{
			FilePath						dest;
			Util::Chemfiles::E_FILE_FORMATS fileFormat = Util::Chemfiles::E_FILE_FORMATS::none;
			FrameCollection					frames;
		} _;
	};
} // namespace VTX::IO::Writer
#endif
