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

#include <log4cxx/defaultconfigurator.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/spi/loggerrepository.h>
#include <log4cxx/file.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/helpers/optionconverter.h>


using namespace log4cxx;
using namespace log4cxx::spi;
using namespace log4cxx::helpers;

void DefaultConfigurator::configure(LoggerRepository* repository)
{
   repository->setConfigured(true);
        const LogString configuratorClassName(getConfiguratorClass());

        LogString configurationOptionStr(getConfigurationFileName());
        File configuration(configurationOptionStr);

        if (configurationOptionStr.empty())
        {
                Pool pool;
                configuration = LOG4CXX_FILE("log4cxx.properties");
                if (!configuration.exists(pool)) {
                    File tmp = LOG4CXX_FILE("log4cxx.xml");
                    if (tmp.exists(pool)) {
                        configuration = tmp;
                    } else {
                        tmp = LOG4CXX_FILE("log4j.properties");
                        if (tmp.exists(pool)) {
                        configuration = tmp;
                        } else {
                        tmp = LOG4CXX_FILE("log4j.xml");
                        if (tmp.exists(pool)) {
                            configuration = tmp;
                        }
                        }
                    }
                    }
        }

        Pool pool;
        if (configuration.exists(pool))
        {
                LogString msg(LOG4CXX_STR("Using configuration file ["));
                msg += configuration.getName();
                msg += LOG4CXX_STR("] for automatic log4cxx configuration");
                LogLog::debug(msg);

            LoggerRepositoryPtr repo(repository);
                OptionConverter::selectAndConfigure(
                        configuration,
                        configuratorClassName,
                        repo);
        }
        else
        {
                LogString msg(LOG4CXX_STR("Could not find configuration file: ["));
                msg += configuration.getName();
                msg += LOG4CXX_STR("].");
        }

}


const LogString DefaultConfigurator::getConfiguratorClass() {

   // Use automatic configration to configure the default hierarchy
   const LogString log4jConfiguratorClassName(
        OptionConverter::getSystemProperty(LOG4CXX_STR("log4j.configuratorClass"),LOG4CXX_STR("")));
   const LogString configuratorClassName(
        OptionConverter::getSystemProperty(LOG4CXX_STR("LOG4CXX_CONFIGURATOR_CLASS"),
            log4jConfiguratorClassName));
   return configuratorClassName;
}


const LogString DefaultConfigurator::getConfigurationFileName() {
  static const LogString LOG4CXX_DEFAULT_CONFIGURATION_KEY(LOG4CXX_STR("LOG4CXX_CONFIGURATION"));
  static const LogString LOG4J_DEFAULT_CONFIGURATION_KEY(LOG4CXX_STR("log4j.configuration"));
  const LogString log4jConfigurationOptionStr(
          OptionConverter::getSystemProperty(LOG4J_DEFAULT_CONFIGURATION_KEY, LOG4CXX_STR("")));
  const LogString configurationOptionStr(
          OptionConverter::getSystemProperty(LOG4CXX_DEFAULT_CONFIGURATION_KEY,
              log4jConfigurationOptionStr));
  return configurationOptionStr;
}



