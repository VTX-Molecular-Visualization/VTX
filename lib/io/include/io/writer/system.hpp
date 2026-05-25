#ifndef __VTX_IO_WRITER_SYSTEM__
#define __VTX_IO_WRITER_SYSTEM__

#include <core/struct/topology.hpp>
#include <functional>
#include <io/writer/shared.hpp>
#include <util/filesystem.hpp>
#include <util/thread.hpp>

namespace VTX::IO::Writer
{
	/**
	 * @brief Used in the context of a structural data export. Return whether the atom shall be included in the result
	 * or not.
	 */
	using AtomFilter = std::function<bool( const VTX::Core::Struct::Topology &, const size_t & )>;

	/**
	 * @brief Convenient function used to create the default AtomFilter callable.
	 * @return true
	 */
	inline bool returnTrue( const VTX::Core::Struct::Topology &, const size_t & ) noexcept { return true; }

	const AtomFilter g_takeAllAtoms = AtomFilter( &returnTrue ); // default argument for the write atom filter

	/**
	 * @brief Class responsible for providing atom positions of each frame of a trajectory
	 */
	class TrajectoryFrameGetter
	{
	  public:
		TrajectoryFrameGetter() = default;

		/**
		 * @brief Return number of available frames
		 * @return
		 */
		inline uint frameCount() const { return _ptr->frameCount(); }

		/**
		 * @brief Returns the n-th frame
		 * @return
		 */
		inline std::span<const Vec3f> getAtomPositions( const uint & p_ ) const { return _ptr->getAtomPositions( p_ ); }

		inline std::span<const Vec3f> getCurrentAtomPositions() const { return _ptr->getCurrentAtomPositions(); }

	  private:
		struct _interface
		{
			virtual ~_interface()												  = default;
			virtual uint				   frameCount() const					  = 0;
			virtual std::span<const Vec3f> getAtomPositions( const uint & ) const = 0;
			virtual std::span<const Vec3f> getCurrentAtomPositions() const		  = 0;
		};

		struct _dummy
		{
		};

		template<typename T>
		struct _wrapper final : public _interface
		{
			_wrapper( T && p_obj ) : _obj( std::forward<T>( p_obj ) ) {}

			virtual uint frameCount() const override
			{
				if constexpr ( std::same_as<T, _dummy> )
				{
					return 0;
				}
				else
				{
					return _obj.frameCount();
				}
			}

			virtual std::span<const Vec3f> getAtomPositions( const uint & p_index ) const override
			{
				if constexpr ( std::same_as<T, _dummy> )
				{
					return {};
				}
				else
				{
					return _obj.getAtomPositions( p_index );
				}
			}

			virtual std::span<const Vec3f> getCurrentAtomPositions() const override
			{
				if constexpr ( std::same_as<T, _dummy> )
				{
					return {};
				}
				else
				{
					return _obj.getCurrentAtomPositions();
				}
			}

		  private:
			T _obj;
		};

		std::unique_ptr<_interface> _ptr { new _wrapper<_dummy>( _dummy() ) };

	  public:
		template<typename T>
		TrajectoryFrameGetter( T && p_ ) : _ptr( new _wrapper<T>( std::forward<T>( p_ ) ) )
		{
		}
	};

	/**
	 * @brief Class responsible for writing structural data from a system
	 */
	struct WriteArgs
	{
		struct System
		{
			const VTX::Core::Struct::Topology * topology;
			TrajectoryFrameGetter				trajectory;
			AtomFilter							atomFilter = g_takeAllAtoms;
		};

		FilePath			destination;
		E_FILE_FORMATS		format = E_FILE_FORMATS::none;
		std::vector<System> topologies;
		E_WRITE_TYPE writeType = E_WRITE_TYPE::trajectory; // Placeholder because at some point we will probably need to
														   // write docking results and stuff
		Util::StopToken stopToken;
	};

	void writeFile( WriteArgs );

} // namespace VTX::IO::Writer

#endif
