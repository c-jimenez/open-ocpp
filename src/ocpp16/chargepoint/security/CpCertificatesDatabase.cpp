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

#include "CpCertificatesDatabase.h"
#include "Certificate.h"
#include "CertificateRequest.h"
#include "DateTime.h"
#include "IChargePointConfig.h"
#include "Logger.h"
#include "PrivateKey.h"

#include <limits>
#include <sstream>

using namespace ocpp::database;
using namespace ocpp::types;
using namespace ocpp::x509;

namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
CpCertificatesDatabase::CpCertificatesDatabase(const ocpp::config::IChargePointConfig& stack_config, ocpp::database::Database& database)
    : m_stack_config(stack_config),
      m_database(database),
      m_list_query(),
      m_find_csr_query(),
      m_delete_csr_query(),
      m_update_csr_to_cert_query(),
      m_insert_csr_query()
{
}

/** @brief Destructor */
CpCertificatesDatabase::~CpCertificatesDatabase() { }

/** @brief Initialize the database table */
void CpCertificatesDatabase::initDatabaseTable()
{
    if (m_stack_config.internalCertificateManagementEnabled())
    {
        // Create database
        auto query = m_database.query("CREATE TABLE IF NOT EXISTS CpCertificates ("
                                      "[id]	INTEGER,"
                                      "[validity_from] BIGINT,"
                                      "[validity_to] BIGINT,"
                                      "[certificate] VARCHAR(5000),"
                                      "[private_key] VARCHAR(1000),"
                                      "[csr] BOOLEAN,"
                                      "PRIMARY KEY([id] AUTOINCREMENT));");
        if (query.get())
        {
            if (!query->exec())
            {
                LOG_ERROR << "Could not create charge point certificates table  : " << query->lastError();
            }
        }

        // Create parametrized queries
        m_list_query = m_database.query(
            "SELECT * FROM CpCertificates WHERE csr=FALSE AND validity_from <= ? AND validity_to >= ? ORDER BY validity_from DESC;");
        m_find_csr_query   = m_database.query("SELECT * FROM CpCertificates WHERE csr = TRUE;");
        m_delete_csr_query = m_database.query("DELETE FROM CpCertificates WHERE csr = TRUE;");
        m_update_csr_to_cert_query =
            m_database.query("UPDATE CpCertificates SET validity_from=?, validity_to=?, certificate=?, csr=FALSE WHERE id=? AND csr=TRUE;");
        m_insert_csr_query = m_database.query("INSERT INTO CpCertificates VALUES (NULL, 0, 0, ?, ?, TRUE);");
    }
    else
    {
        // Disable certificates management
        m_list_query.reset();
        m_find_csr_query.reset();
        m_delete_csr_query.reset();
        m_update_csr_to_cert_query.reset();
        m_insert_csr_query.reset();
    }
}

/** @brief Save a certificate request and its private key */
void CpCertificatesDatabase::saveCertificateRequest(const ocpp::x509::CertificateRequest& certificate_request,
                                                    const ocpp::x509::PrivateKey&         private_key)
{
    if (m_delete_csr_query && m_insert_csr_query)
    {
        // Delete any existing request
        m_delete_csr_query->exec();
        m_delete_csr_query->reset();

        // Insert new request
        m_insert_csr_query->bind(0, certificate_request.pem());
        m_insert_csr_query->bind(1, private_key.privatePem());
        if (!m_insert_csr_query->exec())
        {
            LOG_ERROR << "Unable to save certificate request into database : " << m_insert_csr_query->lastError();
        }

        // Reset all queries
        m_insert_csr_query->reset();
    }
}

/** @brief Get the saved certificate request */
std::string CpCertificatesDatabase::getCertificateRequest(unsigned int& id)
{
    std::string pem;

    if (m_find_csr_query)
    {
        // Look for the certificate request
        if (m_find_csr_query->exec())
        {
            // Read data
            id  = m_find_csr_query->getUInt32(0);
            pem = m_find_csr_query->getString(3);
        }

        // Reset all queries
        m_find_csr_query->reset();
    }

    return pem;
}

/** @brief Install a certificate */
bool CpCertificatesDatabase::installCertificate(unsigned int request_id, const ocpp::x509::Certificate& certificate)
{
    bool ret = false;

    if (m_update_csr_to_cert_query)
    {
        // Install certificate
        m_update_csr_to_cert_query->bind(0, static_cast<int64_t>(certificate.validityFrom()));
        m_update_csr_to_cert_query->bind(1, static_cast<int64_t>(certificate.validityTo()));
        m_update_csr_to_cert_query->bind(2, certificate.pem());
        m_update_csr_to_cert_query->bind(3, request_id);
        if (m_update_csr_to_cert_query->exec())
        {
            ret = true;
        }
        else
        {
            LOG_ERROR << "Unable to install certificate : " << m_update_csr_to_cert_query->lastError();
        }

        // Reset all queries
        m_update_csr_to_cert_query->reset();
    }

    return ret;
}

/** @brief Check if a valid certificate has been installed */
bool CpCertificatesDatabase::isValidCertificateInstalled()
{
    bool ret = false;

    if (m_list_query)
    {
        // Find valid certificates
        m_list_query->bind(0, static_cast<int64_t>(DateTime::now().timestamp()));
        m_list_query->bind(1, static_cast<int64_t>(DateTime::now().timestamp()));
        ret = (m_list_query->exec() && m_list_query->hasRows());

        // Reset all queries
        m_list_query->reset();
    }

    return ret;
}

/** @brief Get the installed Charge Point certificate as PEM encoded data */
std::string CpCertificatesDatabase::getChargePointCertificate(std::string& private_key)
{
    std::string certificate_pem;

    if (m_list_query)
    {
        // Find valid certificates
        m_list_query->bind(0, static_cast<int64_t>(DateTime::now().timestamp()));
        m_list_query->bind(1, static_cast<int64_t>(DateTime::now().timestamp()));
        if (m_list_query->exec() && m_list_query->hasRows())
        {
            // Get only the first = most recent
            certificate_pem = m_list_query->getString(3);
            private_key     = m_list_query->getString(4);
        }
        else
        {
            LOG_ERROR << "No valid charge point certificate found";
        }

        // Reset all queries
        m_list_query->reset();
    }

    return certificate_pem;
}

} // namespace chargepoint
} // namespace ocpp
