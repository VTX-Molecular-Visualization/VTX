#ifndef __VTX_SAVER__
#define __VTX_SAVER__

#include "base_thread.hpp"
#include <util/types.hpp>

namespace VTX::Worker
{
	class Saver : public Worker::BaseThread
	{
		// Q_OBJECT

		enum class MODE : int
		{
			MOLECULE,
			VTX,
			UNKNOWN,
		};

	  public:
		explicit Saver( const FilePath & p_path ) : _path( p_path ) {}
		~Saver() {}

	  protected:
		uint _run() override;

	  private:
		const FilePath _path;

		bool _saveMolecule();
		bool _saveSession();

		MODE _getMode( const FilePath & ) const;
	};

} // namespace VTX::Worker
#endif
