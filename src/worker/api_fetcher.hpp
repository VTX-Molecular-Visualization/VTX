#ifndef __VTX_API_FETCHER__
#define __VTX_API_FETCHER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_worker.hpp"
#include "define.hpp"
#include <QNetworkReply>

namespace VTX
{
	namespace Worker
	{
		class ApiFetcher : public Worker::BaseWorker
		{
			Q_OBJECT

		  public:
			explicit ApiFetcher( const std::string & p_url );

			inline std::string * const getBuffer() const { return _buffer; }
			void					   work() override;

		  private:
			QUrl				_url;
			std::string * const _buffer = nullptr;

			void _onReplyFinished( QNetworkReply * const );
		};
	} // namespace Worker
} // namespace VTX
#endif

