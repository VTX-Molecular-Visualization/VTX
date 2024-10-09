#ifndef __VTX_IO_WRITER_CHEMFILES__
#define __VTX_IO_WRITER_CHEMFILES__

#include "io/util/chemfiles.hpp"
#include <forward_list>
#include <util/types.hpp>
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )

namespace VTX::IO::Writer
{
	// I'm hesitant whether I base the second level of hierarchy after the Frame on the atom directly or if it makes
	// more sense to put the residue first and thereafter the atom ... I'll take it overnight and see tomorrow

	struct Property
	{
		std::string key;
		std::string value;
	};

	/**
	 * @brief Allows to configure a frame to be written by Chemfiles. Be carefull : any instance is invalidated if the
	 * parent Chemmfiles object is moved away.
	 */
	class Frame
	{
	  public:
		/**
		 * @brief Set or replace a property for the frame.
		 */
		void set( Property );
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
		 * @brief Create a new frame that will be written eventually
		 * @return The new frame to configure
		 */
		Frame newFrame() noexcept;

	  private:
		struct _Chain
		{
		};
		using ChainCollection = std::forward_list<_Chain>; // Used for constant time insertion
		struct _Frame
		{
		};
		using FrameCollection = std::forward_list<_Frame>; // Used for constant time insertion

		FilePath						_dest;
		Util::Chemfiles::E_FILE_FORMATS _fileFormat = Util::Chemfiles::E_FILE_FORMATS::none;
		FrameCollection					_frames;
	};
} // namespace VTX::IO::Writer
#endif
