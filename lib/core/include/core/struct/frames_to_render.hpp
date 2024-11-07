#ifndef __VTX_CORE_STRUCT_FRAMES_TO_RENDER__
#define __VTX_CORE_STRUCT_FRAMES_TO_RENDER__

#include <queue>
#include <mutex>
#include <util/types.hpp>
#include <util/logger.hpp>

namespace VTX::Core::Struct
{
	using Frame = std::vector<Vec3f>;

	class FramesToRender final
	{
	  public:
		FramesToRender() {}
		// adding copy ctor because of mutex and integration in base player, not sure where to go from now FIXME
		FramesToRender(const FramesToRender& p_source) { _framesToRender = p_source._framesToRender; }
		~FramesToRender() {}
		
		void AddElement( const Frame & elem )
		{
			if ( 0 == elem.size() )
				return;
			std::unique_lock<std::mutex> unique_lock( access_frames_mtx );
			VTX_INFO( "ADD _framesToRender.size() {}", _framesToRender.size() );
			_framesToRender.push(elem);
		}
		void RemoveElement( Frame & elem )
		{
			if ( _framesToRender.empty() )
				return;
			std::unique_lock<std::mutex> unique_lock( access_frames_mtx );
			assert(_framesToRender.front() == elem);
			_framesToRender.pop();
		}
		bool ReadElement(Frame & elem)
		{
			if ( _framesToRender.empty() )
				return false;
			std::unique_lock<std::mutex> unique_lock( access_frames_mtx );
			elem = _framesToRender.front();
			return true;
		}
		bool GetCopyFrame(Frame &frame) // returning a copy intentionnaly
		{
			std::unique_lock<std::mutex> unique_lock( access_frames_mtx );
			if ( !ReadCopyElement( frame ) )
				return false;
			RemoveCopyElement(frame);
			return true;
		}
		bool Flush(void)
		{
			std::unique_lock<std::mutex> unique_lock( access_frames_mtx );
			// FIXME improve the way to empty this queue, can fail
			return _framesToRender.empty();
		}

	  private:
		std::queue<Frame> _framesToRender;
		std::mutex access_frames_mtx;
		
		void RemoveCopyElement( Frame & elem )
		{
			if ( _framesToRender.empty() )
				return;
			assert(_framesToRender.front() == elem);
			VTX_INFO( "REMOVE _framesToRender.size() {}", _framesToRender.size() );
			_framesToRender.pop();
		}
		//Frame ReadCopyElement( void ) // returning a copy intentionnaly
		bool ReadCopyElement(Frame & elem)
		{
			if ( _framesToRender.empty() )
				return false;
			elem = _framesToRender.front();
			VTX_INFO( "READ _framesToRender.size() {}", _framesToRender.size() );
			return true;
		}
	};
} // namespace VTX::Core::Struct

#endif
