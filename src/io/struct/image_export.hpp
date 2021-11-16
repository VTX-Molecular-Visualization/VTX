#ifndef __VTX_IO_STRUCT_IMAGE_EXPORT__
#define __VTX_IO_STRUCT_IMAGE_EXPORT__

#include <string>
#include <utility>
#include <vector>

namespace VTX::IO::Struct
{
	class ImageExport
	{
	  public:
		enum class Format : int
		{
			PNG,
			JPEG,
			BMP,

			UNKNOWN
		};
		enum AdvancedSettings : int
		{
			TRANSPARENCY = 1 << 0,
			QUALITY		 = 1 << 1,

			NONE = 0,
			ALL	 = INT_MAX,
		};
		inline static const std::vector<std::string> FORMAT_STR { "png", "jpeg", "bmp" };

		inline static const std::vector<AdvancedSettings> ADVANCED_SETTINGS_BY_FORMAT {
			AdvancedSettings::ALL,	   // PNG
			AdvancedSettings::QUALITY, // JPEG
			AdvancedSettings::NONE,	   // BMP
			AdvancedSettings::NONE	   // UNKNOWN,
		};

		static const Format getFormatFromExtension( const std::string & p_extension );

		enum class RESOLUTION : int
		{
			_800x600,
			_1024x768,
			_1280x800,
			_1600x900,
			_1920x1080,
			_2048x1152,
			_2560x1440,
			_3840x2160,
			_8KUHD,
			_16K,
			Free,
		};
		inline static const std::vector<std::string>		 RESOLUTION_STR { "800x600",   "1024x768",	"1280x800",
																	  "1600x900",  "1920x1080", "2048x1152",
																	  "2560x1440", "3840x2160", "8K",
																	  "16K",	   "Free" };
		inline static const std::vector<std::pair<int, int>> RESOLUTION_SIZE {
			{ 800, 600 },	{ 1024, 768 },	{ 1280, 800 },	{ 1600, 900 },	{ 1920, 1080 },
			{ 2048, 1152 }, { 2560, 1440 }, { 3840, 2160 }, { 7680, 4320 }, { 15360, 8640 }
		};

		inline static int SNAPSHOT_MIN_WIDTH  = 16;
		inline static int SNAPSHOT_MIN_HEIGHT = 16;
		inline static int SNAPSHOT_MAX_WIDTH  = 16000;
		inline static int SNAPSHOT_MAX_HEIGHT = 16000;

		static std::pair<int, int> getSize( const RESOLUTION & p_resolution );

		ImageExport() : ImageExport( RESOLUTION::Free, 1.f, 1.f ) {};
		ImageExport( const RESOLUTION & p_resolution, const float p_backgroundOpacity, const float p_quality ) :
			_size( getSize( p_resolution ) ), _backgroundOpacity( p_backgroundOpacity ), _quality( p_quality ) {};
		ImageExport( const std::pair<int, int> & p_size, const float p_backgroundOpacity, const float p_quality ) :
			_size( p_size ), _backgroundOpacity( p_backgroundOpacity ), _quality( p_quality ) {};

		const std::pair<int, int> & getSize() const { return _size; }
		void						setSize( const std::pair<int, int> & p_size ) { _size = p_size; }
		void setSize( const RESOLUTION & p_resolution ) { _size = RESOLUTION_SIZE[ int( p_resolution ) ]; }
		void setSize( const int p_width, const int p_height ) { _size = std::pair<int, int>( p_width, p_height ); }

		float getBackgroundOpacity() const { return _backgroundOpacity; }
		void  setBackgroundOpacity( const float p_backgroundOpacity ) { _backgroundOpacity = p_backgroundOpacity; }

		float getQuality() const { return _quality; }
		int	  getIntQuality() const { return int( _quality * 100.f ); }
		void  setQuality( const float p_quality ) { _quality = p_quality; }

	  private:
		std::pair<int, int> _size;
		float				_backgroundOpacity = 1.f;
		float				_quality		   = 1.f;
	};
} // namespace VTX::IO::Struct
#endif
