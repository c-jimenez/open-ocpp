/*
MIT License

Copyright (c) 2020 Cedric Jimenez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef LOCALCONTROLLERDEMOCONFIG_H
#define LOCALCONTROLLERDEMOCONFIG_H

#include "IniFile.h"
#include "LocalControllerConfig.h"

/** @brief Configuration of the Local Controller demo */
class LocalControllerDemoConfig
{
  public:
    /** @brief Constructor */
    LocalControllerDemoConfig(const std::string& config_file) : m_config(config_file), m_stack_config(m_config) { }

    /** @brief Stack internal configuration */
    LocalControllerConfig& stackConfig() { return m_stack_config; }

  private:
    /** @brief Configuration file */
    ocpp::helpers::IniFile m_config;

    /** @brief Stack internal configuration */
    LocalControllerConfig m_stack_config;
};

#endif // LOCALCONTROLLERDEMOCONFIG_H
