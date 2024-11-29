#ifndef __VTX_CORE_STRUCT_FRAMES_DATA_VECTOR__
#define __VTX_CORE_STRUCT_FRAMES_DATA_VECTOR__

#include <util/types.hpp>
#include <vector>

namespace VTX::Core::Struct
{
	using Frame = std::vector<Vec3f>;

	class FramesDataVector final
	{
	  public:
		FramesDataVector() {}
		~FramesDataVector() {}

		void fillFrame( const size_t p_systemFrameIndex, const std::vector<Vec3f> & p_atomPositions )
		{
			_framesVector[ p_systemFrameIndex ].resize( p_atomPositions.size() );
			std::copy( p_atomPositions.begin(), p_atomPositions.end(), _framesVector[ p_systemFrameIndex ].begin() );
		}

		const Frame & getCurrentFrame() const { return _framesVector[ _currentFrameIndex ]; }
		Frame &		  getCurrentFrame() { return _framesVector[ _currentFrameIndex ]; }

		void		 setCurrentFrameIndex( size_t p_currentFrameIdx ) { _currentFrameIndex = p_currentFrameIdx; }
		const size_t getCurrentFrameIndex( void ) const { return _currentFrameIndex; }

		const Frame & getFrameFromIndex( size_t p_index ) const { return _framesVector[ p_index ]; }
		Frame &		  getFrameFromIndex( size_t p_index ) { return _framesVector[ p_index ]; }

		size_t getTotalElements() const { return _framesVector.size(); }
		void   setTotalElements( const size_t size ) { _framesVector.resize( size ); }

		void eraseEmptyFrames( void )
		{
			if ( _framesVector.back().size() == 0 )
			{
				do
				{
					_framesVector.pop_back();
				} while ( _framesVector.back().size() == 0 );

				_framesVector.shrink_to_fit();
			}
		}

	  private:
		size_t			   _currentFrameIndex = 0;
		std::vector<Frame> _framesVector;
	};
} // namespace VTX::Core::Struct

#endif
