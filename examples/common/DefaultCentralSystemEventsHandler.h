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

#ifndef DEFAULTCENTRALSYSTEMEVENTSHANDLER_H
#define DEFAULTCENTRALSYSTEMEVENTSHANDLER_H

#include "ICentralSystemEventsHandler.h"

#include <map>

/** @brief Default central system event handlers implementation for the examples */
class DefaultCentralSystemEventsHandler : public ocpp::centralsystem::ICentralSystemEventsHandler
{
  public:
    /** @brief Constructor */
    DefaultCentralSystemEventsHandler();

    /** @brief Destructor */
    virtual ~DefaultCentralSystemEventsHandler();

    // ICentralSystemEventsHandler interface

    /** @copydoc bool ICentralSystemEventsHandler::checkCredentials(const std::string&, const std::string&) */
    bool checkCredentials(const std::string& chargepoint_id, const std::string& password) override;

    /** @copydoc bool ICentralSystemEventsHandler::chargePointConnected(std::shared_ptr<ICentralSystem::IChargePoint>) */
    void chargePointConnected(std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint> chargepoint) override;

  private:
    /** @brief Connected charge points */
    std::map<std::string, std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint>> m_chargepoints;
};

#endif // DEFAULTCENTRALSYSTEMEVENTSHANDLER_H
