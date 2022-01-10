/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GENERICMESSAGESCONVERTER_H
#define GENERICMESSAGESCONVERTER_H

#include <map>
#include <string>
namespace ocpp
{
namespace messages
{

template <typename DataType>
class IMessageConverter;

/** @brief Stores all the needed message converters */
class GenericMessagesConverter
{
  public:
    /** @brief Destructor */
    virtual ~GenericMessagesConverter() { }

    /**
     * @brief Get the converter for a request
     * @param action Ocpp call action corresponding to the request
     * @return Pointer to the message converter for the request or nullptr if the converter doesn't exists
     */
    template <typename RequestType>
    IMessageConverter<RequestType>* getRequestConverter(const std::string& action) const
    {
        IMessageConverter<RequestType>* ret = nullptr;
        auto                            it  = m_req_converters.find(action);
        if (it != m_req_converters.end())
        {
            ret = reinterpret_cast<IMessageConverter<RequestType>*>(it->second);
        }
        return ret;
    }

    /**
     * @brief Get the converter for a response
     * @param action Ocpp call action corresponding to the response
     * @return Pointer to the message converter for the response or nullptr if the converter doesn't exists
     */
    template <typename ResponseType>
    IMessageConverter<ResponseType>* getResponseConverter(const std::string& action) const
    {
        IMessageConverter<ResponseType>* ret = nullptr;
        auto                             it  = m_resp_converters.find(action);
        if (it != m_resp_converters.end())
        {
            ret = reinterpret_cast<IMessageConverter<ResponseType>*>(it->second);
        }
        return ret;
    }

  protected:
    /**
     * @brief Register a converter for a request
     * @param action Ocpp call action corresponding to the request
     * @param converter Message converter for the request
     */
    template <typename RequestType>
    void registerRequestConverter(const std::string& action, IMessageConverter<RequestType>& converter)
    {
        m_req_converters[action] = &converter;
    }

    /**
     * @brief Register a converter for a response
     * @param action Ocpp call action corresponding to the response
     * @param converter Message converter for the response
     */
    template <typename ResponseType>
    void registerResponseConverter(const std::string& action, IMessageConverter<ResponseType>& converter)
    {
        m_resp_converters[action] = &converter;
    }

  private:
    /** @brief Request converters */
    std::map<std::string, void*> m_req_converters;
    /** @brief Response converters */
    std::map<std::string, void*> m_resp_converters;
};

} // namespace messages
} // namespace ocpp

#endif // GENERICMESSAGESCONVERTER_H
