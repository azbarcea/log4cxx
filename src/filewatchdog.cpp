/*
 * Copyright 2003,2004 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <log4cxx/helpers/filewatchdog.h>
#include <log4cxx/helpers/loglog.h>
#include <sys/stat.h>
#include <errno.h>
#include <apr_time.h>
#include <apr_thread_proc.h>
#include <apr_atomic.h>
#include <log4cxx/helpers/transcoder.h>

//Define INT64_C for compilers that don't have it
#if (!defined(INT64_C))
#define INT64_C(value)  int64_t(value)
#endif


using namespace log4cxx;
using namespace log4cxx::helpers;

long FileWatchdog::DEFAULT_DELAY = 60000;

FileWatchdog::FileWatchdog(const File& file)
 : file(file), delay(DEFAULT_DELAY), lastModif(0),
warnedAlready(false), interrupted(0), thread()
{
}

FileWatchdog::~FileWatchdog() {
	thread.stop();
}

void FileWatchdog::checkAndConfigure()
{
	if (!file.exists())
	{
              if(!warnedAlready)
              {
                      LogLog::debug(((LogString) LOG4CXX_STR("["))
                         + file.getName()
                         + LOG4CXX_STR("] does not exist."));
                      warnedAlready = true;
              }
	}
	else
	{
                apr_time_t thisMod = file.lastModified();
		if (thisMod > lastModif)
		{
			lastModif = thisMod;
			doOnChange();
			warnedAlready = false;
		}
	}
}

void* APR_THREAD_FUNC FileWatchdog::run(apr_thread_t* thread, void* data) {
	FileWatchdog* pThis = (FileWatchdog*) data;

	unsigned int interrupted = apr_atomic_read32(&pThis->interrupted);
    while(!interrupted)
	{
		apr_sleep(APR_INT64_C(1000) * pThis->delay);
		pThis->checkAndConfigure();
		interrupted = apr_atomic_read32(&pThis->interrupted);
    }
	return NULL;
}

void FileWatchdog::start()
{
	checkAndConfigure();

	thread.run(pool, run, this);
}

