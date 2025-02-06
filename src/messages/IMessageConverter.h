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

#ifndef OPENOCPP_IMESSAGECONVERTER_H
#define OPENOCPP_IMESSAGECONVERTER_H

#include "CiStringType.h"
#include "DateTime.h"
#include "IMessageDispatcher.h"
#include "Optional.h"

namespace ocpp
{
namespace messages
{

/** @brief Interface for the message converters from JSON to C++ data type and backward */
template <typename DataType>
class IMessageConverter
{
  public:
    /** @brief Destructor */
    virtual ~IMessageConverter() { }

    /**
     * Clone a message converter instance
     * @return Cloned message converter instance
     */
    virtual IMessageConverter* clone() const = 0;

    /**
     * @brief Convert a JSON object to a C++ data type
     * @param json JSON object to convert
     * @param data C++ data type to fill
     * @param error_code Error code in case of invalid input
     * @param error_message Error message in case of invalid input
     * @return true the object has been converted, false otherwise
     */
    virtual bool fromJson(const rapidjson::Value& json, DataType& data, std::string& error_code, std::string& error_message) = 0;

    /**
     * @brief Convert a C++ data type to a JSON object
     * @param data C++ data type to convert
     * @param json JSON object to fill
     * @return true the object has been converted, false otherwise
     */
    virtual bool toJson(const DataType& data, rapidjson::Document& json) = 0;

    /**
     * @brief Helper function to fill an integer value in a JSON object
     * @param json JSON object to fill
     * @param field Name of the field to fill
     * @param value Integer value to fill
     */
    void fill(rapidjson::Value& json, const char* name, const int value)
    {
        json.AddMember(rapidjson::StringRef(name), rapidjson::Value(value), *allocator);
    }

    /**
     * @brief Helper function to fill an unsigned integer value in a JSON object
     * @param json JSON object to fill
     * @param field Name of the field to fill
     * @param value Unsigned integer value to fill
     */
    void fill(rapidjson::Value& json, const char* name, const unsigned int value)
    {
        json.AddMember(rapidjson::StringRef(name), rapidjson::Value(value), *allocator);
    }

    /**
     * @brief Helper function to fill a floating point value in a JSON object
     * @param json JSON object to fill
     * @param field Name of the field to fill
     * @param value Floating point value to fill
     */
    void fill(rapidjson::Value& json, const char* name, const float value)
    {
        json.AddMember(rapidjson::StringRef(name), rapidjson::Value(value), *allocator);
    }

    /**
     * @brief Helper function to fill a floating point value in a JSON object
     * @param json JSON object to fill
     * @param field Name of the field to fill
     * @param value Floating point value to fill
     */
    void fill(rapidjson::Value& json, const char* name, const double value)
    {
        json.AddMember(rapidjson::StringRef(name), rapidjson::Value(value), *allocator);
    }

    /**
     * @brief Helper function to fill a string value in a JSON object
     * @param json JSON object to fill
     * @param field Name of the field to fill
     * @param value String value to fill
     */
    void fill(rapidjson::Value& json, const char* name, const std::string& value)
    {
        json.AddMember(rapidjson::StringRef(name), rapidjson::Value(value.c_str(), *allocator).Move(), *allocator);
    }

    /**
     * @brief Helper function to fill a date and time value in a JSON object
     * @param json JSON object to fill
     * @param field Name of the field to fill
     * @param value Date and time value to fill
     */
    void fill(rapidjson::Value& json, const char* name, const ocpp::types::DateTime& value) { fill(json, name, value.str()); }

    /**
     * @brief Helper function to fill a boolean value in a JSON object
     * @param json JSON object to fill
     * @param field Name of the field to fill
     * @param value Boolean value to fill
     */
    void fill(rapidjson::Value& json, const char* name, const bool value)
    {
        json.AddMember(rapidjson::StringRef(name), rapidjson::Value(value), *allocator);
    }

    /**
     * @brief Helper function to fill an optional value in a JSON object
     * @param json JSON object to fill
     * @param field Name of the field to fill
     * @param value Optional value to fill
     */
    template <typename T>
    void fill(rapidjson::Value& json, const char* name, const ocpp::types::Optional<T>& value)
    {
        if (value.isSet())
        {
            fill(json, name, value.value());
        }
    }

    /**
     * @brief Helper function to extract an integer value from a JSON object
     * @param json JSON object
     * @param field Name of the field to extract
     * @param value Integer value extracted
     */
    void extract(const rapidjson::Value& json, const char* name, int& value) { value = json[name].GetInt(); }

    /**
     * @brief Helper function to extract an unsigned integer value from a JSON object
     * @param json JSON object
     * @param field Name of the field to extract
     * @param value Unsigned integer value extracted
     * @param error_message Error message to fill
     * @return true if the value is an unsigned integer, false otherwise
     */
    bool extract(const rapidjson::Value& json, const char* name, unsigned int& value, std::string& error_message)
    {
        bool                    ret = false;
        const rapidjson::Value& val = json[name];
        if (val.GetInt() >= 0)
        {
            value = val.GetUint();
            ret   = true;
        }
        else
        {
            error_message = name;
            error_message += " parameter is not an unsigned integer";
        }
        return ret;
    }

    /**
     * @brief Helper function to extract a floating point value from a JSON object
     * @param json JSON object
     * @param field Name of the field to extract
     * @param value Floating point value extracted
     */
    void extract(const rapidjson::Value& json, const char* name, float& value) { value = json[name].GetFloat(); }

    /**
     * @brief Helper function to extract a string value from a JSON object
     * @param json JSON object
     * @param field Name of the field to extract
     * @param value String value extracted
     */
    void extract(const rapidjson::Value& json, const char* name, std::string& value) { value = json[name].GetString(); }

    /**
     * @brief Helper function to extract a size limited string value from a JSON object
     * @param json JSON object
     * @param field Name of the field to extract
     * @param value Size limited string value extracted
     */
    void extract(const rapidjson::Value& json, const char* name, ocpp::types::ICiStringType& value)
    {
        value.assign(json[name].GetString());
    }

    /**
     * @brief Helper function to extract a date and time value from a JSON object
     * @param json JSON object
     * @param field Name of the field to extract
     * @param value Date and time value extracted
     * @param error_message Error message to fill
     * @return true if the value is a date and time, false otherwise
     */
    bool extract(const rapidjson::Value& json, const char* name, ocpp::types::DateTime& value, std::string& error_message)
    {
        bool                    ret = false;
        const rapidjson::Value& val = json[name];
        if (val.IsString())
        {
            ret = value.assign(val.GetString());
        }
        if (!ret)
        {
            error_message = name;
            error_message += " parameter is not a valid date-time object";
        }
        return ret;
    }

    /**
     * @brief Helper function to extract a boolean value from a JSON object
     * @param json JSON object
     * @param field Name of the field to extract
     * @param value Boolean value extracted
     */
    void extract(const rapidjson::Value& json, const char* name, bool& value) { value = json[name].GetBool(); }

    /**
     * @brief Helper function to extract an optional value from a JSON object
     * @param json JSON object
     * @param field Name of the field to extract
     * @param value Optional value extracted
     */
    template <typename T>
    void extract(const rapidjson::Value& json, const char* name, ocpp::types::Optional<T>& value)
    {
        if (json.HasMember(name))
        {
            extract(json, name, value.value());
        }
    }

    /**
     * @brief Helper function to extract an optional value from a JSON object
     * @param json JSON object
     * @param field Name of the field to extract
     * @param value Optional value extracted
     * @param error_message Error message to fill
     * @return true if the value is not present or is valid, false otherwise
     */
    template <typename T>
    bool extract(const rapidjson::Value& json, const char* name, ocpp::types::Optional<T>& value, std::string& error_message)
    {
        bool ret = true;
        if (json.HasMember(name))
        {
            ret = extract(json, name, value.value(), error_message);
        }
        return ret;
    }

    /** @brief Set the allocator to use */
    void setAllocator(RAPIDJSON_DEFAULT_ALLOCATOR* _allocator) { allocator = _allocator; }

    /** @brief Allocator */
    RAPIDJSON_DEFAULT_ALLOCATOR* allocator;
};

/** @brief Helper macro to declare a converter class for req and conf messages
 *  @param MessageType Message type name
 */
#define MESSAGE_CONVERTERS(MessageType)                                                                                                    \
    class MessageType##ReqConverter : public ocpp::messages::IMessageConverter<MessageType##Req>                                           \
    {                                                                                                                                      \
      public:                                                                                                                              \
        IMessageConverter<MessageType##Req>* clone() const override { return new MessageType##ReqConverter(); }                            \
        bool fromJson(const rapidjson::Value& json, MessageType##Req& data, std::string& error_code, std::string& error_message) override; \
        bool toJson(const MessageType##Req& data, rapidjson::Document& json) override;                                                     \
    };                                                                                                                                     \
    class MessageType##ConfConverter : public ocpp::messages::IMessageConverter<MessageType##Conf>                                         \
    {                                                                                                                                      \
      public:                                                                                                                              \
        IMessageConverter<MessageType##Conf>* clone() const override { return new MessageType##ConfConverter(); }                          \
        bool                                  fromJson(const rapidjson::Value& json,                                                       \
                                                       MessageType##Conf&      data,                                                       \
                                                       std::string&            error_code,                                                 \
                                                       std::string&            error_message) override;                                               \
        bool                                  toJson(const MessageType##Conf& data, rapidjson::Document& json) override;                   \
    };

} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_IMESSAGECONVERTER_H
