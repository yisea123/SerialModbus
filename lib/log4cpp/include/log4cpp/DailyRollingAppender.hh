/**
 * A Daily Rolling Appender extention for log4cpp
 * 
 * @author Chen Ju(sammy105@gmail.com)
 *
 */
#ifndef __DAILY_ROLLING_APPENDER_H__
#define __DAILY_ROLLING_APPENDER_H__
#include <log4cpp/RollingFileAppender.hh>
#include <string>
#include <log4cpp/Portability.hh>
using namespace log4cpp;
namespace log4cpp{
	class LOG4CPP_EXPORT DailyRollingFileAppender : public FileAppender{
		public:
			DailyRollingFileAppender(const std::string& name, const std::string& fileName, bool append = true, mode_t mode = 00664);
			
			virtual void rollOver();

		protected:
			virtual void _append(const LoggingEvent& event);
			virtual short getCurrTime();
			virtual std::string getFileName();
			
		private:
			short lastTime_; //by day
		
	};
}





#endif
