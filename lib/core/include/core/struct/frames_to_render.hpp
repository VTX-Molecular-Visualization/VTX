#ifndef __VTX_CORE_STRUCT_FRAMES_TO_RENDER__
#define __VTX_CORE_STRUCT_FRAMES_TO_RENDER__

#include <mutex>
#include <queue>
#include <util/types.hpp>

namespace VTX::Core::Struct
{
	using Frame = std::vector<Vec3f>;

	/**
	 * @brief Queue holder to provide a simple lifo structure.
	 * Frames stored to be displayed to the user.
	 */
	class FramesToRender final
	{
	  public:
		FramesToRender() {}
		/**
		 * @brief Copy ctor explicit because of a mutex in the member section.
		 */
		FramesToRender( const FramesToRender & p_source ) { _framesToRender = p_source._framesToRender; }
		~FramesToRender() {}

		/**
		 * @brief Adds the Frame element to the queue
		 */
		void addElement( const Frame & elem )
		{
			if ( 0 == elem.size() )
				return;
			std::unique_lock<std::mutex> unique_lock( access_frames_mtx );
			// VTX_INFO( "ADD _framesToRender.size() {}", _framesToRender.size() );
			_framesToRender.push( elem );
		}
		/**
		 * @brief Get the next Frame element in the queue
		 */
		bool getCopyFrame( Frame & frame ) // returning a copy intentionnaly
		{
			std::unique_lock<std::mutex> unique_lock( access_frames_mtx );
			if ( !readCopyElement( frame ) )
				return false;
			removeCopyElement( frame );
			_testindex = ( _testindex + 1 ) % _testMaxIndex;
			return true;
		}
		/**
		 * @brief Empty the queue
		 */
		void flush( void )
		{
			std::unique_lock<std::mutex> unique_lock( access_frames_mtx );
			while ( !_framesToRender.empty() )
				_framesToRender.pop();
			_testindex = 0;
		}

		void		 setMaxIndex( size_t p_maxIndex ) { _testMaxIndex = p_maxIndex; }
		const size_t getindex( void ) const { return _testindex; }

	  private:
		std::queue<Frame> _framesToRender;
		std::mutex		  access_frames_mtx;
		size_t			  _testMaxIndex = 0;
		size_t			  _testindex	= 0;

		/**
		 * @brief Removes the input Frame ensuring it is the current front element.
		 */
		void removeCopyElement( Frame & elem )
		{
			if ( _framesToRender.empty() )
				return;
			assert( _framesToRender.front() == elem );
			// VTX_INFO( "REMOVE _framesToRender.size() {}", _framesToRender.size() );
			_framesToRender.pop();
		}
		/**
		 * @brief Get the current front element ensuring the queue is not empty.
		 */
		bool readCopyElement( Frame & elem )
		{
			if ( _framesToRender.empty() )
				return false;
			elem = _framesToRender.front();
			// VTX_INFO( "READ _framesToRender.size() {}", _framesToRender.size() );
			return true;
		}
	};
} // namespace VTX::Core::Struct

#endif
