/***************************************************************************
                          loggingevent.h  -  description
                             -------------------
    begin                : mer avr 16 2003
    copyright            : (C) 2003 by Michael CATANZARITI
    email                : mcatan@free.fr
 ***************************************************************************/

/***************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.      *
 *                                                                         *
 * This software is published under the terms of the Apache Software       *
 * License version 1.1, a copy of which has been included with this        *
 * distribution in the LICENSE.txt file.                                   *
 ***************************************************************************/

#ifndef _LOG4CXX_SPI_LOGGING_EVENT_H
#define _LOG4CXX_SPI_LOGGING_EVENT_H

#include <log4cxx/helpers/objectptr.h>
#include <log4cxx/helpers/tchar.h>
#include <time.h>
#include <log4cxx/logger.h>
#include <log4cxx/mdc.h>
#include <set>

namespace log4cxx
{
	class Logger;
	typedef helpers::ObjectPtrT<Logger> LoggerPtr;
	
	class Level;

	namespace helpers
	{
		class SocketOutputStream;
		typedef helpers::ObjectPtrT<SocketOutputStream> SocketOutputStreamPtr;

		class SocketInputStream;
		typedef helpers::ObjectPtrT<SocketInputStream> SocketInputStreamPtr;
	};
	
	namespace spi
	{
		/**
		The internal representation of logging events. When an affirmative
		decision is made to log then a <code>LoggingEvent</code> instance
		is created. This instance is passed around to the different log4cxx
		components.

		<p>This class is of concern to those wishing to extend log4cxx.
		*/
		class LoggingEvent
		{
		public:
			LoggingEvent(const LoggingEvent& event);
			
			/** For serialization only
			*/
			LoggingEvent();
			
			/**
			Instantiate a LoggingEvent from the supplied parameters.

			<p>Except #timeStamp all the other fields of
			<code>LoggingEvent</code> are filled when actually needed.
			<p>
			@param logger The logger of this event.
			@param level The level of this event.
			@param message  The message of this event.
			@param file The file where this log statement was written.
			@param line The line where this log statement was written.
			*/
			LoggingEvent(const tstring& fqnOfLoggerClass,
				const LoggerPtr& logger,
				const Level& level,	const tstring& message,
				const char* file=0, int line=-1);

			/** Return the #level of this event. */
			inline const Level& getLevel() const
				{ return *level; }

			/**  Return the name of the #logger. */
			inline const tstring& getLoggerName() const
				{ return logger->getName(); }

			/** Return the #message for this logging event. */
			inline const tstring& getMessage() const
				{ return message; }

			/** Return the #message for this logging event. */
			inline const tstring& getRenderedMessage() const
				{ return message; }

			/**Returns the time when the application started,
			in seconds elapsed since 01.01.1970.
			*/
			static long getStartTime()
				{ return startTime; }

			/** Return the #threadId of this event. */
			inline unsigned long getThreadId() const
				{ return threadId; }

			/** Return the #timeStamp of this event. */
			inline time_t getTimeStamp() const
				{ return timeStamp; }

			/* Return the file where this log statement was written. */
			inline char * getFile() const
				{ return file; }

			/* Return the line where this log statement was written. */
			inline int getLine() const
				{ return line; }

			/**
			* This method returns the NDC for this event. It will return the
			* correct content even if the event was generated in a different
			* thread or even on a different machine. The NDC#get method
			* should <em>never</em> be called directly.  */
			const tstring& getNDC() const;

			/** Write this event to a helpers::SocketOutputStream. */
			void write(helpers::SocketOutputStreamPtr os) const;

			/** Read this event from a helpers::SocketOutputStream. */
			void read(helpers::SocketInputStreamPtr is);

			/** Obtain a copy a this event. */
			LoggingEvent * copy() const;

			/**
			* Returns the the context corresponding to the <code>key</code> parameter.
			* If there is a local MDC copy, possibly because we are in a logging
			* server or running inside AsyncAppender, then we search for the key in
			* MDC copy, if a value is found it is returned. Otherwise, if the search
			* in MDC copy returns an empty result, then the current thread's
			* <code>MDC</code> is used.
			*
			* <p>
			* Note that <em>both</em> the local MDC copy and the current thread's MDC
			* are searched.
			* </p>
			*/
			tstring getMDC(const tstring& key) const;

			/**
			* Returns the set of of the key values in the MDC for the event.
			* The returned set is unmodifiable by the caller.
			*
			* @return Set an unmodifiable set of the MDC keys.
			* @since 1.3
			*/
			std::set<tstring> getMDCKeySet() const;

			/**
			Obtain a copy of this thread's MDC prior to serialization
			or asynchronous logging.
			*/
			void getMDCCopy() const;

			/**
			* Return a previously set property. The return value can be null.
			*/
			tstring getProperty(const tstring& key) const;
			/**
			* Returns the set of of the key values in the properties
			* for the event. The returned set is unmodifiable by the caller.
			*
			* @return Set an unmodifiable set of the property keys.
			*/
			std::set<tstring> getPropertyKeySet() const;

			/**
			* Set a string property using a key and a string value.  since 1.3
			*/
			void setProperty(const tstring& key, const tstring& value);

		public:
			/**
			* Fully qualified name of the calling category class.
			*/
			tstring fqnOfCategoryClass;

		private:
			/**
			* The logger of the logging event. This field is not serialized for
			* performance reasons.
			*
			* <p>
			* It is set by the LoggingEvent constructor or set by a remote
			* entity after deserialization.
			**/
			LoggerPtr logger;

            /** level of logging event. */
			const Level * level;

			/** The nested diagnostic context (NDC) of logging event. */
			tstring ndc;

			/** The mapped diagnostic context (MDC) of logging event. */
			MDC::Map mdcCopy;

			/**
			* A map of tstring keys and tstring values.
			*/
			std::map<tstring, tstring> properties;

			/** Have we tried to do an NDC lookup? If we did, there is no need
			*  to do it again.  Note that its value is always false when
			*  serialized. Thus, a receiving SocketNode will never use it's own
			*  (incorrect) NDC. See also writeObject method.
			*/
			bool ndcLookupRequired;

			/**
			* Have we tried to do an MDC lookup? If we did, there is no need to do it
			* again.  Note that its value is always false when serialized. See also
			* the getMDC and getMDCCopy methods.
			*/
			bool mdcCopyLookupRequired;

			/** The application supplied message of logging event. */
			tstring message;

            /** The name of thread in which this logging event was generated. */
            //const LOG4CPLUS_THREAD_KEY_TYPE thread;

            /** The number of seconds elapsed from 1/1/1970 until logging event
            was created. */
            time_t timeStamp;

			/** The is the file where this log statement was written. */
			char* file;

			/** The is the line where this log statement was written. */
			int line;

			/** The identifier of thread in which this logging event
			was generated.
			*/
			unsigned long threadId;

			static time_t startTime;
  		};
	};
};

#endif //_LOG4CXX_SPI_LOGGING_EVENT_H
