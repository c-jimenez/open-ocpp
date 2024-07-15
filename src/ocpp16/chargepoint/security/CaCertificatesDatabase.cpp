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

#include "CaCertificatesDatabase.h"
#include "Certificate.h"
#include "DateTime.h"
#include "IChargePointConfig.h"
#include "Logger.h"

#include <limits>
#include <sstream>

using namespace ocpp::database;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;
using namespace ocpp::x509;

namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
CaCertificatesDatabase::CaCertificatesDatabase(const ocpp::config::IChargePointConfig& stack_config, ocpp::database::Database& database)
    : m_stack_config(stack_config),
      m_database(database),
      m_list_query(),
      m_count_query(),
      m_find_query(),
      m_delete_query(),
      m_insert_query()
{
}

/** @brief Destructor */
CaCertificatesDatabase::~CaCertificatesDatabase() { }

/** @brief Initialize the database table */
void CaCertificatesDatabase::initDatabaseTable()
{
    if (m_stack_config.internalCertificateManagementEnabled())
    {
        // Create database
        auto query = m_database.query("CREATE TABLE IF NOT EXISTS CaCertificates ("
                                      "[id]	INTEGER,"
                                      "[type] INT UNSIGNED,"
                                      "[validity_from] BIGINT,"
                                      "[validity_to] BIGINT,"
                                      "[issuer_hash] VARCHAR(128),"
                                      "[pub_key_hash] VARCHAR(128),"
                                      "[serial] VARCHAR(40),"
                                      "[certificate] VARCHAR(5000),"
                                      "[in_use] BOOLEAN,"
                                      "[backup] BOOLEAN,"
                                      "PRIMARY KEY([id] AUTOINCREMENT));");
        if (query.get())
        {
            if (!query->exec())
            {
                LOG_ERROR << "Could not create CA certificates table  : " << query->lastError();
            }
        }

        // Create parametrized queries
        m_list_query = m_database.query("SELECT * FROM CaCertificates WHERE type = ? AND validity_from <= ? AND validity_to >= ?;");
        m_count_query =
            m_database.query("SELECT count(id) FROM CaCertificates WHERE type = ? AND validity_from <= ? AND validity_to >= ?;");
        m_find_query   = m_database.query("SELECT * FROM CaCertificates WHERE issuer_hash = ? AND pub_key_hash = ? AND serial = ?;");
        m_delete_query = m_database.query("DELETE FROM CaCertificates WHERE id = ?;");
        m_insert_query = m_database.query("INSERT INTO CaCertificates VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
    }
    else
    {
        // Disable certificates management
        m_list_query.reset();
        m_count_query.reset();
        m_find_query.reset();
        m_delete_query.reset();
        m_insert_query.reset();
    }
}

/** @brief Delete an installed CA certificate */
ocpp::types::ocpp16::DeleteCertificateStatusEnumType CaCertificatesDatabase::deleteCertificate(
    const ocpp::types::ocpp16::CertificateHashDataType& certificate)
{
    DeleteCertificateStatusEnumType ret = DeleteCertificateStatusEnumType::NotFound;

    if (m_delete_query)
    {
        // Look for the requested certificate
        bool         in_use = false;
        unsigned int id     = 0;
        if (findCertificate(certificate, id, in_use))
        {
            // Check if the certificate is in use
            if (!in_use)
            {
                // Delete the requested certificate
                m_delete_query->bind(0, id);
                if (m_delete_query->exec())
                {
                    ret = DeleteCertificateStatusEnumType::Accepted;
                }
                else
                {
                    LOG_ERROR << "Could not delete the requested CA certificate : " << m_delete_query->lastError();
                    ret = DeleteCertificateStatusEnumType::Failed;
                }

                // Reset all queries
                m_delete_query->reset();
            }
            else
            {
                LOG_ERROR << "Could not delete the requested CA certificate : certificate in use";
                ret = DeleteCertificateStatusEnumType::Failed;
            }
        }
    }

    return ret;
}

/** @brief Get the list of certificates */
void CaCertificatesDatabase::getCertificateList(ocpp::types::ocpp16::CertificateUseEnumType                type,
                                                std::vector<ocpp::types::ocpp16::CertificateHashDataType>& certificates)
{
    if (m_list_query)
    {
        // List certificates
        m_list_query->bind(0, static_cast<unsigned int>(type));
        m_list_query->bind(1, static_cast<int64_t>(std::numeric_limits<std::time_t>::max()));
        m_list_query->bind(2, 0);
        if (m_list_query->exec() && m_list_query->hasRows())
        {
            // Read data
            do
            {
                certificates.emplace_back();
                CertificateHashDataType& certificate = certificates.back();
                certificate.hashAlgorithm            = HashAlgorithmEnumType::SHA256;
                certificate.issuerNameHash.assign(m_list_query->getString(4));
                certificate.issuerKeyHash.assign(m_list_query->getString(5));
                certificate.serialNumber.assign(m_list_query->getString(6));
            } while (m_list_query->next());
        }

        // Reset query
        m_list_query->reset();
    }
}

/** @brief Get the list of valid certificates in PEM encoded data format */
std::string CaCertificatesDatabase::getCertificateListPem(ocpp::types::ocpp16::CertificateUseEnumType type)
{
    std::string ca_certificates_pem;

    if (m_list_query)
    {
        // List certificates
        m_list_query->bind(0, static_cast<unsigned int>(type));
        m_list_query->bind(1, static_cast<int64_t>(DateTime::now().timestamp()));
        m_list_query->bind(2, static_cast<int64_t>(DateTime::now().timestamp()));
        if (m_list_query->exec() && m_list_query->hasRows())
        {
            // Read data
            do
            {
                ca_certificates_pem += m_list_query->getString(7) + "\n";
            } while (m_list_query->next());
        }

        // Reset query
        m_list_query->reset();
    }

    return ca_certificates_pem;
}

/** @brief Get the number of installed certificates */
unsigned int CaCertificatesDatabase::getCertificateCount(ocpp::types::ocpp16::CertificateUseEnumType type, bool check_validity)
{
    unsigned int ret = 0;

    if (m_count_query)
    {
        // Count certificates
        std::time_t validity_from;
        std::time_t validity_to;
        if (check_validity)
        {
            validity_from = DateTime::now().timestamp();
            validity_to   = DateTime::now().timestamp();
        }
        else
        {
            validity_from = std::numeric_limits<std::time_t>::max();
            validity_to   = 0;
        }
        m_count_query->bind(0, static_cast<unsigned int>(type));
        m_count_query->bind(1, static_cast<int64_t>(validity_from));
        m_count_query->bind(2, static_cast<int64_t>(validity_to));
        if (m_count_query->exec() && m_count_query->hasRows())
        {
            // Read count
            ret = m_count_query->getUInt32(0);
        }

        // Reset query
        m_count_query->reset();
    }

    return ret;
}

/** @brief Add a new certificate */
bool CaCertificatesDatabase::addCertificate(ocpp::types::ocpp16::CertificateUseEnumType         type,
                                            const ocpp::x509::Certificate&                      certificate,
                                            const ocpp::types::ocpp16::CertificateHashDataType& hash_data)
{
    bool ret = false;

    if (m_insert_query)
    {
        // Look for the certificate
        bool         in_use = false;
        unsigned int id     = 0;
        if (!findCertificate(hash_data, id, in_use))
        {
            // Add certificate
            m_insert_query->bind(0, static_cast<unsigned int>(type));
            m_insert_query->bind(1, static_cast<int64_t>(certificate.validityFrom()));
            m_insert_query->bind(2, static_cast<int64_t>(certificate.validityTo()));
            m_insert_query->bind(3, hash_data.issuerNameHash);
            m_insert_query->bind(4, hash_data.issuerKeyHash);
            m_insert_query->bind(5, hash_data.serialNumber);
            m_insert_query->bind(6, certificate.pem());
            m_insert_query->bind(7, false);
            m_insert_query->bind(8, false);
            ret = m_insert_query->exec();
            if (!ret)
            {
                LOG_ERROR << "Could not add the requested CA certificate : " << m_insert_query->lastError();
            }

            // Reset query
            m_insert_query->reset();
        }
        else
        {
            LOG_WARNING << "Certificat already present in database";
            ret = true;
        }
    }

    return ret;
}

/** @brief Look for a certificate */
bool CaCertificatesDatabase::findCertificate(const ocpp::types::ocpp16::CertificateHashDataType& certificate,
                                             unsigned int&                                       id,
                                             bool&                                               in_use)
{
    bool found = false;

    if (m_find_query)
    {
        // Look for the requested certificate
        m_find_query->bind(0, certificate.issuerNameHash);
        m_find_query->bind(1, certificate.issuerKeyHash);
        m_find_query->bind(2, certificate.serialNumber);
        if (m_find_query->exec() && m_find_query->hasRows())
        {
            // Read data
            id     = m_find_query->getUInt32(0);
            in_use = m_find_query->getBool(8);
            found  = true;
        }

        // Reset all queries
        m_find_query->reset();
    }

    return found;
}

} // namespace chargepoint
} // namespace ocpp
