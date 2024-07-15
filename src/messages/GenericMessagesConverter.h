/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPENOCPP_GENERICMESSAGESCONVERTER_H
#define OPENOCPP_GENERICMESSAGESCONVERTER_H

#include <memory>
#include <string>
#include <unordered_map>

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
    std::unique_ptr<IMessageConverter<RequestType>> getRequestConverter(const std::string& action) const
    {
        std::unique_ptr<IMessageConverter<RequestType>> ret;
        auto                                            it = m_req_converters.find(action);
        if (it != m_req_converters.end())
        {
            ret.reset(reinterpret_cast<IMessageConverter<RequestType>*>(it->second)->clone());
        }
        return ret;
    }

    /**
     * @brief Get the converter for a response
     * @param action Ocpp call action corresponding to the response
     * @return Pointer to the message converter for the response or nullptr if the converter doesn't exists
     */
    template <typename ResponseType>
    std::unique_ptr<IMessageConverter<ResponseType>> getResponseConverter(const std::string& action) const
    {
        std::unique_ptr<IMessageConverter<ResponseType>> ret;
        auto                                             it = m_resp_converters.find(action);
        if (it != m_resp_converters.end())
        {
            ret.reset(reinterpret_cast<IMessageConverter<ResponseType>*>(it->second)->clone());
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

    /**
     * @brief Delete a converter for a request
     * @param action Ocpp call action corresponding to the request
     */
    template <typename RequestType>
    void deleteRequestConverter(const std::string& action)
    {
        auto it = m_req_converters.find(action);
        if (it != m_req_converters.end())
        {
            delete reinterpret_cast<IMessageConverter<RequestType>*>(it->second);
        }
    }

    /**
     * @brief Delete a converter for a response
     * @param action Ocpp call action corresponding to the response
     */
    template <typename ResponseType>
    void deleteResponseConverter(const std::string& action)
    {
        auto it = m_resp_converters.find(action);
        if (it != m_resp_converters.end())
        {
            delete reinterpret_cast<IMessageConverter<ResponseType>*>(it->second);
        }
    }

  private:
    /** @brief Request converters */
    std::unordered_map<std::string, void*> m_req_converters;
    /** @brief Response converters */
    std::unordered_map<std::string, void*> m_resp_converters;
};

} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_GENERICMESSAGESCONVERTER_H
