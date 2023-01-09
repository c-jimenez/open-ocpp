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

#include "Base64.h"
#include "Certificate.h"
#include "CertificateRequest.h"
#include "PrivateKey.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest_wrapper.h"

#include <openssl/bio.h>
#include <openssl/pem.h>

#include <fstream>
#include <iostream>
using namespace std;
using namespace ocpp::x509;

/** @brief Test CA certificate in PEM encoded file format */
#define CA_PEM_FILE CERT_DIR "/ca.pem"

/** @brief Test certificate in PEM encoded file format */
#define CERT_PEM_FILE CERT_DIR "/cert.pem"

/** @brief Test certificate in PEM encoded format */
static std::string CERT_PEM_DATA = std::string(R"(-----BEGIN CERTIFICATE-----
MIICdTCCAhqgAwIBAgIUE0SqeLRrvEnH5WWw36XvPEisumgwCgYIKoZIzj0EAwIw
gaMxCzAJBgNVBAYTAkZSMQ8wDQYDVQQIDAZTYXZvaWUxETAPBgNVBAcMCENoYW1i
ZXJ5MRIwEAYDVQQKDAlPcGVuIE9DUFAxETAPBgNVBAsMCEV4YW1wbGVzMSgwJgYD
VQQDDB9PcGVuIE9DUFAgQ2VydGlmaWNhdGUgQXV0aG9yaXR5MR8wHQYJKoZIhvcN
AQkBFhBjYUBvcGVuLW9jcHAub3JnMB4XDTIyMDEyODIyMTAwOVoXDTMyMDEyNjIy
MTAwOVowgaQxCzAJBgNVBAYTAkZSMQ8wDQYDVQQIDAZTYXZvaWUxETAPBgNVBAcM
CENoYW1iZXJ5MRIwEAYDVQQKDAlPcGVuIE9DUFAxETAPBgNVBAsMCEV4YW1wbGVz
MR8wHQYDVQQDDBZPcGVuIE9DUFAgQ2hhcmdlIFBvaW50MSkwJwYJKoZIhvcNAQkB
FhpjaGFyZ2UucG9pbnRAb3Blbi1vY3BwLm9yZzBZMBMGByqGSM49AgEGCCqGSM49
AwEHA0IABLVagM6gtbSRzktgZ2pd9o8fFxxrhiYKWlYUmZjny/Bl46EK4IwOUxjE
8yg+TJW2uqi4BLXa3c5bnA/7XWEu8u6jKTAnMAkGA1UdEwQCMAAwGgYDVR0RBBMw
EYIJbG9jYWxob3N0hwR/AAABMAoGCCqGSM49BAMCA0kAMEYCIQC6Q7xu0mqy6Gci
A0xBJizDdeWUD563hEAUmEEi17wZWQIhAOpfl5V3UnuouNBJvnnH7K+Izxig+CBI
3T6mb5f+o0yw
-----END CERTIFICATE-----
)");

/** @brief Test certificate bundle in PEM encoded file format */
#define BUNDLE_CERT_PEM_FILE CERT_DIR "/bundle.pem"

/** @brief Test certificate request in PEM encoded file format */
#define CERT_REQUEST_PEM_FILE CERT_DIR "/cert_request.pem"

/** @brief Test certificate request in PEM encoded format */
static std::string CERT_REQUEST_PEM_DATA = std::string(R"(-----BEGIN CERTIFICATE REQUEST-----
MIIBYDCCAQcCAQAwgaQxCzAJBgNVBAYTAkZSMQ8wDQYDVQQIDAZTYXZvaWUxETAP
BgNVBAcMCENoYW1iZXJ5MRIwEAYDVQQKDAlPcGVuIE9DUFAxETAPBgNVBAsMCEV4
YW1wbGVzMR8wHQYDVQQDDBZPcGVuIE9DUFAgQ2hhcmdlIFBvaW50MSkwJwYJKoZI
hvcNAQkBFhpjaGFyZ2UucG9pbnRAb3Blbi1vY3BwLm9yZzBZMBMGByqGSM49AgEG
CCqGSM49AwEHA0IABLVagM6gtbSRzktgZ2pd9o8fFxxrhiYKWlYUmZjny/Bl46EK
4IwOUxjE8yg+TJW2uqi4BLXa3c5bnA/7XWEu8u6gADAKBggqhkjOPQQDAgNHADBE
AiA9/utT5xsZenKNLyBLSeppamtmmmvvQZi/ADiLY1npzQIgTrevr648pYT/vdoH
p0lygw5wtygHTZ3+Ve+BFFQVdZQ=
-----END CERTIFICATE REQUEST-----
)");

/** @brief Test RSA key file in PEM encoded file format */
#define RSA_PEM_FILE CERT_DIR "/rsakey_no_encrypt.pem"

/** @brief Test RSA encrypted key file in PEM encoded file format */
#define RSA_ENCRYPT_PEM_FILE CERT_DIR "/rsakey_encrypt.pem"

/** @brief Test EC key file in PEM encoded file format */
#define EC_PEM_FILE CERT_DIR "/eckey_no_encrypt.pem"

/** @brief Test RSA encrypted key file in PEM encoded file format */
#define EC_ENCRYPT_PEM_FILE CERT_DIR "/eckey_encrypt.pem"

/** @brief Passphrase for PEM encrypted files */
static constexpr const char* PEM_PASSPHRASE = "test passphrase";

TEST_SUITE("Certificate")
{
    /** @brief Check fields from test certificate */
    void checkTestCertificateFields(const Certificate& cert)
    {
        CHECK_EQ(cert.pemChain().size(), 1u);
        CHECK_EQ(cert.certificateChain().size(), 1u);

        CHECK_EQ(cert.serialNumber().size(), 20u);
        CHECK_EQ(cert.serialNumberString(), "13:44:aa:78:b4:6b:bc:49:c7:e5:65:b0:df:a5:ef:3c:48:ac:ba:68");

        CHECK_EQ(cert.validityFrom(), 1643407809); // Jan 28 22:10:09 2022 GMT
        CHECK_EQ(cert.validityTo(), 1958767809);   // Jan 26 22:10:09 2032 GMT

        CHECK_EQ(cert.issuerString(),
                 "C = FR, ST = Savoie, L = Chambery, O = Open OCPP, OU = Examples, CN = Open OCPP Certificate Authority, emailAddress = "
                 "ca@open-ocpp.org");
        const Certificate::Subject& issuer = cert.issuer();
        CHECK_EQ(issuer.country, "FR");
        CHECK_EQ(issuer.state, "Savoie");
        CHECK_EQ(issuer.location, "Chambery");
        CHECK_EQ(issuer.organization, "Open OCPP");
        CHECK_EQ(issuer.organization_unit, "Examples");
        CHECK_EQ(issuer.common_name, "Open OCPP Certificate Authority");
        CHECK_EQ(issuer.email_address, "ca@open-ocpp.org");
        const std::vector<std::string>& issuer_alt_names = cert.issuerAltNames();
        CHECK_EQ(issuer_alt_names.size(), 0u);

        CHECK_EQ(cert.subjectString(),
                 "C = FR, ST = Savoie, L = Chambery, O = Open OCPP, OU = Examples, CN = Open OCPP Charge Point, emailAddress = "
                 "charge.point@open-ocpp.org");
        const Certificate::Subject& subject = cert.subject();
        CHECK_EQ(subject.country, "FR");
        CHECK_EQ(subject.state, "Savoie");
        CHECK_EQ(subject.location, "Chambery");
        CHECK_EQ(subject.organization, "Open OCPP");
        CHECK_EQ(subject.organization_unit, "Examples");
        CHECK_EQ(subject.common_name, "Open OCPP Charge Point");
        CHECK_EQ(subject.email_address, "charge.point@open-ocpp.org");
        const std::vector<std::string>& subject_alt_names = cert.subjectAltNames();
        CHECK_EQ(subject_alt_names.size(), 2u);
        CHECK_EQ(subject_alt_names[0u], "localhost");
        CHECK_EQ(subject_alt_names[1u], "127.0.0.1");

        CHECK_FALSE(cert.isSelfSigned());

        CHECK_EQ(cert.signatureAlgo(), "ecdsa-with-SHA256");
        CHECK_EQ(cert.signatureHash(), "SHA256");

        CHECK_EQ(cert.publicKey().size(), 65u);
        CHECK_EQ(cert.publicKeyString(),
                 "04:b5:5a:80:ce:a0:b5:b4:91:ce:4b:60:67:6a:5d:f6:8f:1f:17:1c:6b:86:26:0a:5a:56:14:99:98:e7:cb:f0:65:e3:a1:0a:e0:8c:0e:53:"
                 "18:c4:f3:28:3e:4c:95:b6:ba:a8:b8:04:b5:da:dd:ce:5b:9c:0f:fb:5d:61:2e:f2:ee");
        CHECK_EQ(cert.publicKeyAlgo(), "id-ecPublicKey");
        CHECK_EQ(cert.publicKeyAlgoParam(), "prime256v1");

        const std::vector<std::string>& extensions_names = cert.x509v3ExtensionsNames();
        CHECK_EQ(extensions_names.size(), 2u);
        CHECK_EQ(extensions_names[0u], "X509v3 Basic Constraints");
        CHECK_EQ(extensions_names[1u], "X509v3 Subject Alternative Name");

        const Certificate::Extensions& extensions = cert.x509v3Extensions();
        CHECK(extensions.basic_constraints.present);
        CHECK_FALSE(extensions.basic_constraints.is_ca);
        CHECK_EQ(extensions.basic_constraints.path_length, 0);
    }

    /** @brief Check fields from bundle CA certificate */
    void checkBundleCaCertificateFields(const Certificate& cert)
    {
        CHECK_EQ(cert.pemChain().size(), 1u);
        CHECK_EQ(cert.certificateChain().size(), 1u);

        CHECK_EQ(cert.validityFrom(), 1643407809); // Jan 28 22:10:09 2022 GMT
        CHECK_EQ(cert.validityTo(), 1958767809);   // Jan 26 22:10:09 2032 GMT

        CHECK_EQ(cert.serialNumber().size(), 20u);
        CHECK_EQ(cert.serialNumberString(), "53:4d:28:a4:55:19:00:56:67:59:84:3e:92:06:08:63:fc:51:b6:b9");

        CHECK_EQ(cert.issuerString(),
                 "C = FR, ST = Savoie, L = Chambery, O = Open OCPP, OU = Examples, CN = Open OCPP Certificate Authority, emailAddress = "
                 "ca@open-ocpp.org");
        const Certificate::Subject& issuer = cert.issuer();
        CHECK_EQ(issuer.country, "FR");
        CHECK_EQ(issuer.state, "Savoie");
        CHECK_EQ(issuer.location, "Chambery");
        CHECK_EQ(issuer.organization, "Open OCPP");
        CHECK_EQ(issuer.organization_unit, "Examples");
        CHECK_EQ(issuer.common_name, "Open OCPP Certificate Authority");
        CHECK_EQ(issuer.email_address, "ca@open-ocpp.org");
        const std::vector<std::string>& issuer_alt_names = cert.issuerAltNames();
        CHECK_EQ(issuer_alt_names.size(), 0u);

        CHECK_EQ(cert.subjectString(), cert.issuerString());
        const Certificate::Subject& subject = cert.subject();
        CHECK_EQ(subject.country, issuer.country);
        CHECK_EQ(subject.state, issuer.state);
        CHECK_EQ(subject.location, issuer.location);
        CHECK_EQ(subject.organization, issuer.organization);
        CHECK_EQ(subject.organization_unit, issuer.organization_unit);
        CHECK_EQ(subject.common_name, issuer.common_name);
        CHECK_EQ(subject.email_address, issuer.email_address);
        const std::vector<std::string>& subject_alt_names = cert.subjectAltNames();
        CHECK_EQ(subject_alt_names.size(), 2u);
        CHECK_EQ(subject_alt_names[0u], "localhost");
        CHECK_EQ(subject_alt_names[1u], "127.0.0.1");

        CHECK(cert.isSelfSigned());

        CHECK_EQ(cert.signatureAlgo(), "ecdsa-with-SHA256");
        CHECK_EQ(cert.signatureHash(), "SHA256");

        CHECK_EQ(cert.publicKey().size(), 65u);
        CHECK_EQ(cert.publicKeyString(),
                 "04:2d:13:9c:69:94:9b:35:00:56:df:1b:b8:a7:a3:37:f2:41:65:2f:e9:d9:bd:7f:82:0b:16:65:66:6f:00:62:f4:fc:09:27:1b:88:42:21:"
                 "b3:86:31:a7:03:dc:ee:bc:34:46:0f:a4:7b:ee:e6:a6:2c:95:3b:a7:9c:91:6f:00:03");
        CHECK_EQ(cert.publicKeyAlgo(), "id-ecPublicKey");
        CHECK_EQ(cert.publicKeyAlgoParam(), "prime256v1");

        const std::vector<std::string>& extensions_names = cert.x509v3ExtensionsNames();
        CHECK_EQ(extensions_names.size(), 2u);
        CHECK_EQ(extensions_names[0u], "X509v3 Basic Constraints");
        CHECK_EQ(extensions_names[1u], "X509v3 Subject Alternative Name");

        const Certificate::Extensions& extensions = cert.x509v3Extensions();
        CHECK(extensions.basic_constraints.present);
        CHECK(extensions.basic_constraints.is_ca);
        CHECK_EQ(extensions.basic_constraints.path_length, 0);
    }

    /** @brief Check fields from bundle certificate */
    void checkBundleCertificateFields(const Certificate& cert, size_t expected_chain_size = 2u)
    {
        CHECK_EQ(cert.pemChain().size(), expected_chain_size);
        CHECK_EQ(cert.certificateChain().size(), expected_chain_size);

        CHECK_EQ(cert.serialNumber().size(), 20u);
        CHECK_EQ(cert.serialNumberString(), "13:44:aa:78:b4:6b:bc:49:c7:e5:65:b0:df:a5:ef:3c:48:ac:ba:67");

        CHECK_EQ(cert.validityFrom(), 1643407809); // Jan 28 22:10:09 2022 GMT
        CHECK_EQ(cert.validityTo(), 1958767809);   // Jan 26 22:10:09 2032 GMT

        CHECK_EQ(cert.issuerString(),
                 "C = FR, ST = Savoie, L = Chambery, O = Open OCPP, OU = Examples, CN = Open OCPP Certificate Authority, emailAddress = "
                 "ca@open-ocpp.org");
        const Certificate::Subject& issuer = cert.issuer();
        CHECK_EQ(issuer.country, "FR");
        CHECK_EQ(issuer.state, "Savoie");
        CHECK_EQ(issuer.location, "Chambery");
        CHECK_EQ(issuer.organization, "Open OCPP");
        CHECK_EQ(issuer.organization_unit, "Examples");
        CHECK_EQ(issuer.common_name, "Open OCPP Certificate Authority");
        CHECK_EQ(issuer.email_address, "ca@open-ocpp.org");
        const std::vector<std::string>& issuer_alt_names = cert.issuerAltNames();
        CHECK_EQ(issuer_alt_names.size(), 0u);

        CHECK_EQ(cert.subjectString(),
                 "C = FR, ST = Savoie, L = Chambery, O = Open OCPP, OU = Examples, CN = Open OCPP Central System, emailAddress = "
                 "central.system@open-ocpp.org");
        const Certificate::Subject& subject = cert.subject();
        CHECK_EQ(subject.country, "FR");
        CHECK_EQ(subject.state, "Savoie");
        CHECK_EQ(subject.location, "Chambery");
        CHECK_EQ(subject.organization, "Open OCPP");
        CHECK_EQ(subject.organization_unit, "Examples");
        CHECK_EQ(subject.common_name, "Open OCPP Central System");
        CHECK_EQ(subject.email_address, "central.system@open-ocpp.org");
        const std::vector<std::string>& subject_alt_names = cert.subjectAltNames();
        CHECK_EQ(subject_alt_names.size(), 2u);
        CHECK_EQ(subject_alt_names[0u], "localhost");
        CHECK_EQ(subject_alt_names[1u], "127.0.0.1");

        CHECK_FALSE(cert.isSelfSigned());

        CHECK_EQ(cert.signatureAlgo(), "ecdsa-with-SHA256");
        CHECK_EQ(cert.signatureHash(), "SHA256");

        CHECK_EQ(cert.publicKey().size(), 65u);
        CHECK_EQ(cert.publicKeyString(),
                 "04:90:87:ae:71:fc:fb:e1:60:92:67:78:9e:3e:4a:f4:6c:f1:41:54:f3:a2:ef:65:ab:42:b2:ab:86:35:b2:85:38:fb:a7:f1:c4:4f:54:ef:"
                 "f8:90:54:6f:43:97:8f:04:20:23:cc:aa:0f:40:3e:e7:f9:4e:51:39:90:96:a0:34:ee");
        CHECK_EQ(cert.publicKeyAlgo(), "id-ecPublicKey");
        CHECK_EQ(cert.publicKeyAlgoParam(), "prime256v1");

        const std::vector<std::string>& extensions_names = cert.x509v3ExtensionsNames();
        CHECK_EQ(extensions_names.size(), 2u);
        CHECK_EQ(extensions_names[0u], "X509v3 Basic Constraints");
        CHECK_EQ(extensions_names[1u], "X509v3 Subject Alternative Name");

        const Certificate::Extensions& extensions = cert.x509v3Extensions();
        CHECK(extensions.basic_constraints.present);
        CHECK_FALSE(extensions.basic_constraints.is_ca);
        CHECK_EQ(extensions.basic_constraints.path_length, 0);

        if (cert.certificateChain().size() > 1u)
        {
            checkBundleCertificateFields(cert.certificateChain()[0u], 1u);
            checkBundleCaCertificateFields(cert.certificateChain()[1u]);
        }
    }

    TEST_CASE("From PEM certificate file")
    {
        // Load cert
        Certificate cert(std::filesystem::path(CERT_PEM_FILE));
        CHECK(cert.isValid());

        // Check fields
        checkTestCertificateFields(cert);
    }

    TEST_CASE("From PEM encoded data")
    {
        // Load cert
        Certificate cert(CERT_PEM_DATA);
        CHECK(cert.isValid());

        // Check fields
        checkTestCertificateFields(cert);
    }

    TEST_CASE("From PEM bundle file")
    {
        // Load cert
        Certificate cert(std::filesystem::path(BUNDLE_CERT_PEM_FILE));
        CHECK(cert.isValid());

        // Check fields
        checkBundleCertificateFields(cert);

        // Check certificate chain validity
        CHECK(cert.verify());
    }

    TEST_CASE("Verifiy against certificate chain")
    {
        // Load cert
        Certificate cert(std::filesystem::path(CERT_PEM_FILE));
        CHECK(cert.isValid());

        // Load CA
        Certificate ca(std::filesystem::path(CA_PEM_FILE));
        CHECK(ca.isValid());
        CHECK(ca.isSelfSigned());

        // Check certificate
        CHECK(cert.verify(ca.certificateChain()));
        CHECK_FALSE(ca.verify(cert.certificateChain()));
    }
}

TEST_SUITE("Certificate request")
{
    /** @brief Check fields from test certificate certificate request */
    void checkTestCertificateRequestFields(const CertificateRequest& cert, bool check_pub_key = true)
    {
        CHECK_EQ(cert.subjectString(),
                 "C = FR, ST = Savoie, L = Chambery, O = Open OCPP, OU = Examples, CN = Open OCPP Charge Point, emailAddress = "
                 "charge.point@open-ocpp.org");
        const Certificate::Subject& subject = cert.subject();
        CHECK_EQ(subject.country, "FR");
        CHECK_EQ(subject.state, "Savoie");
        CHECK_EQ(subject.location, "Chambery");
        CHECK_EQ(subject.organization, "Open OCPP");
        CHECK_EQ(subject.organization_unit, "Examples");
        CHECK_EQ(subject.common_name, "Open OCPP Charge Point");
        CHECK_EQ(subject.email_address, "charge.point@open-ocpp.org");

        CHECK_EQ(cert.signatureAlgo(), "ecdsa-with-SHA256");

        CHECK_EQ(cert.publicKey().size(), 65u);
        if (check_pub_key)
        {
            CHECK_EQ(
                cert.publicKeyString(),
                "04:b5:5a:80:ce:a0:b5:b4:91:ce:4b:60:67:6a:5d:f6:8f:1f:17:1c:6b:86:26:0a:5a:56:14:99:98:e7:cb:f0:65:e3:a1:0a:e0:8c:0e:53:"
                "18:c4:f3:28:3e:4c:95:b6:ba:a8:b8:04:b5:da:dd:ce:5b:9c:0f:fb:5d:61:2e:f2:ee");
        }
        CHECK_EQ(cert.publicKeyAlgo(), "id-ecPublicKey");
        CHECK_EQ(cert.publicKeyAlgoParam(), "prime256v1");
    }

    TEST_CASE("From PEM certificate request file")
    {
        // Load cert
        CertificateRequest cert_request(std::filesystem::path(CERT_REQUEST_PEM_FILE));
        CHECK(cert_request.isValid());

        // Check fields
        checkTestCertificateRequestFields(cert_request);
    }

    TEST_CASE("From PEM encoded data")
    {
        // Load cert
        CertificateRequest cert_request(CERT_REQUEST_PEM_DATA);
        CHECK(cert_request.isValid());

        // Check fields
        checkTestCertificateRequestFields(cert_request);
    }

    TEST_CASE("Generate request")
    {
        // Reference request
        CertificateRequest ref_request(CERT_REQUEST_PEM_DATA);
        CHECK(ref_request.isValid());

        // Private key
        PrivateKey pkey(PrivateKey::Type::EC, PrivateKey::Curve::PRIME256_V1, "Some good passphrase");
        CHECK(pkey.isValid());

        // Generate request
        CertificateRequest cert_request(ref_request.subject(), pkey);
        CHECK(cert_request.isValid());

        // Check fields
        checkTestCertificateRequestFields(cert_request, false);

        std::fstream f("req.csr", std::fstream::out);
        f << cert_request.pem();
    }
}

TEST_SUITE("Private key")
{
    TEST_CASE("RSA key from PEM file")
    {
        PrivateKey pkey(std::filesystem::path(RSA_PEM_FILE), "");
        CHECK(pkey.isValid());
        CHECK_FALSE(pkey.privatePem().empty());
        CHECK_FALSE(pkey.publicPem().empty());
        CHECK_EQ(pkey.size(), 2048u);
        CHECK_EQ(pkey.algo(), "rsaEncryption");
        CHECK_EQ(pkey.algoParam(), "");
        CHECK_NE(pkey.object(), nullptr);
    }

    TEST_CASE("RSA key from PEM encrypted file")
    {
        // Valid passphrase
        PrivateKey pkey(std::filesystem::path(RSA_ENCRYPT_PEM_FILE), PEM_PASSPHRASE);
        CHECK(pkey.isValid());
        CHECK_FALSE(pkey.privatePem().empty());
        CHECK_FALSE(pkey.publicPem().empty());
        CHECK_EQ(pkey.size(), 2048u);
        CHECK_EQ(pkey.algo(), "rsaEncryption");
        CHECK_EQ(pkey.algoParam(), "");
        CHECK_NE(pkey.object(), nullptr);

        // Invalid passphrase
        PrivateKey comp_key(std::filesystem::path(RSA_ENCRYPT_PEM_FILE), "Not the good passphrase");
        CHECK_FALSE(comp_key.isValid());
        CHECK(comp_key.privatePem().empty());
        CHECK(comp_key.publicPem().empty());
        CHECK_EQ(comp_key.size(), 0);
        CHECK_EQ(comp_key.algo(), "");
        CHECK_EQ(comp_key.algoParam(), "");
        CHECK_EQ(comp_key.object(), nullptr);
    }

    TEST_CASE("EC key from PEM file")
    {
        PrivateKey pkey(std::filesystem::path(EC_PEM_FILE), "");
        CHECK(pkey.isValid());
        CHECK_FALSE(pkey.privatePem().empty());
        CHECK_FALSE(pkey.publicPem().empty());
        CHECK_EQ(pkey.size(), 256u);
        CHECK_EQ(pkey.algo(), "id-ecPublicKey");
        CHECK_EQ(pkey.algoParam(), "prime256v1");
        CHECK_NE(pkey.object(), nullptr);
    }

    TEST_CASE("EC key from PEM encrypted file")
    {
        // Valid passphrase
        PrivateKey pkey(std::filesystem::path(EC_ENCRYPT_PEM_FILE), PEM_PASSPHRASE);
        CHECK(pkey.isValid());
        CHECK_FALSE(pkey.privatePem().empty());
        CHECK_FALSE(pkey.publicPem().empty());
        CHECK_EQ(pkey.algo(), "id-ecPublicKey");
        CHECK_EQ(pkey.algoParam(), "prime256v1");
        CHECK_EQ(pkey.size(), 256u);
        CHECK_NE(pkey.object(), nullptr);

        // Invalid passphrase
        PrivateKey comp_key(std::filesystem::path(EC_ENCRYPT_PEM_FILE), "Not the good passphrase");
        CHECK_FALSE(comp_key.isValid());
        CHECK(comp_key.privatePem().empty());
        CHECK(comp_key.publicPem().empty());
        CHECK_EQ(comp_key.size(), 0);
        CHECK_EQ(comp_key.algo(), "");
        CHECK_EQ(comp_key.algoParam(), "");
        CHECK_EQ(comp_key.object(), nullptr);
    }

    TEST_CASE("RSA key generation - no encryption")
    {
        // Generate key
        PrivateKey pkey(PrivateKey::Type::RSA, 2048u, "");
        CHECK(pkey.isValid());
        CHECK_FALSE(pkey.privatePem().empty());
        CHECK_FALSE(pkey.publicPem().empty());
        CHECK_EQ(pkey.size(), 2048u);
        CHECK_EQ(pkey.algo(), "rsaEncryption");
        CHECK_EQ(pkey.algoParam(), "");
        CHECK_NE(pkey.object(), nullptr);

        // Reload key from PEM data
        PrivateKey comp_key(pkey.privatePem(), "");
        CHECK(comp_key.isValid());
        CHECK_EQ(comp_key.privatePem(), pkey.privatePem());
        CHECK_EQ(comp_key.publicPem(), pkey.publicPem());
        CHECK_EQ(comp_key.size(), pkey.size());
        CHECK_EQ(comp_key.algo(), pkey.algo());
        CHECK_EQ(comp_key.algoParam(), pkey.algoParam());
        CHECK_NE(comp_key.object(), nullptr);
    }

    TEST_CASE("RSA key generation - encryption")
    {
        // Generate key
        PrivateKey pkey(PrivateKey::Type::RSA, 4096u, "This is a passphrase");
        CHECK(pkey.isValid());
        CHECK_FALSE(pkey.privatePem().empty());
        CHECK_FALSE(pkey.publicPem().empty());
        CHECK_EQ(pkey.size(), 4096u);
        CHECK_EQ(pkey.algo(), "rsaEncryption");
        CHECK_EQ(pkey.algoParam(), "");
        CHECK_NE(pkey.object(), nullptr);

        // Reload key from PEM data - invalid passphrase
        PrivateKey comp_key(pkey.privatePem(), "Not the good passphrase");
        CHECK_FALSE(comp_key.isValid());
        CHECK(comp_key.privatePem().empty());
        CHECK(comp_key.publicPem().empty());
        CHECK_EQ(comp_key.size(), 0);
        CHECK_EQ(comp_key.algo(), "");
        CHECK_EQ(comp_key.algoParam(), "");
        CHECK_EQ(comp_key.object(), nullptr);

        // Reload key from PEM data - valid passphrase
        PrivateKey comp_key2(pkey.privatePem(), "This is a passphrase");
        CHECK(comp_key2.isValid());
        CHECK_EQ(comp_key2.privatePem(), pkey.privatePem());
        CHECK_EQ(comp_key2.publicPem(), pkey.publicPem());
        CHECK_EQ(comp_key2.size(), pkey.size());
        CHECK_EQ(comp_key2.algo(), pkey.algo());
        CHECK_EQ(comp_key2.algoParam(), pkey.algoParam());
        CHECK_NE(comp_key2.object(), nullptr);
    }

    TEST_CASE("EC key generation - no encryption")
    {
        // Generate key
        PrivateKey pkey(PrivateKey::Type::EC, PrivateKey::Curve::PRIME256_V1, "");
        CHECK(pkey.isValid());
        CHECK_FALSE(pkey.privatePem().empty());
        CHECK_FALSE(pkey.publicPem().empty());
        CHECK_EQ(pkey.size(), 256u);
        CHECK_EQ(pkey.algo(), "id-ecPublicKey");
        CHECK_EQ(pkey.algoParam(), "prime256v1");
        CHECK_NE(pkey.object(), nullptr);

        // Reload key from PEM data
        PrivateKey comp_key(pkey.privatePem(), "");
        CHECK(comp_key.isValid());
        CHECK_EQ(comp_key.privatePem(), pkey.privatePem());
        CHECK_EQ(comp_key.publicPem(), pkey.publicPem());
        CHECK_EQ(comp_key.size(), pkey.size());
        CHECK_EQ(comp_key.algo(), pkey.algo());
        CHECK_EQ(comp_key.algoParam(), pkey.algoParam());
        CHECK_NE(comp_key.object(), nullptr);
    }

    TEST_CASE("EC key generation - encryption")
    {
        // Generate key
        PrivateKey pkey(PrivateKey::Type::EC, PrivateKey::Curve::SECP384_R1, "A real good passphrase");
        CHECK(pkey.isValid());
        CHECK_FALSE(pkey.privatePem().empty());
        CHECK_FALSE(pkey.publicPem().empty());
        CHECK_EQ(pkey.size(), 384u);
        CHECK_EQ(pkey.algo(), "id-ecPublicKey");
        CHECK_EQ(pkey.algoParam(), "secp384r1");
        CHECK_NE(pkey.object(), nullptr);

        // Reload key from PEM data - invalid passphrase
        PrivateKey comp_key(pkey.privatePem(), "Not the good passphrase");
        CHECK_FALSE(comp_key.isValid());
        CHECK(comp_key.privatePem().empty());
        CHECK(comp_key.publicPem().empty());
        CHECK_EQ(comp_key.size(), 0);
        CHECK_EQ(comp_key.algo(), "");
        CHECK_EQ(comp_key.algoParam(), "");
        CHECK_EQ(comp_key.object(), nullptr);

        // Reload key from PEM data - valid passphrase
        PrivateKey comp_key2(pkey.privatePem(), "A real good passphrase");
        CHECK(comp_key2.isValid());
        CHECK_EQ(comp_key2.privatePem(), pkey.privatePem());
        CHECK_EQ(comp_key2.publicPem(), pkey.publicPem());
        CHECK_EQ(comp_key2.size(), pkey.size());
        CHECK_EQ(comp_key2.algo(), pkey.algo());
        CHECK_EQ(comp_key2.algoParam(), pkey.algoParam());
        CHECK_NE(comp_key2.object(), nullptr);
    }
}

TEST_SUITE("Certificate generation")
{
    TEST_CASE("Certificate chain")
    {
        // Create new CA
        PrivateKey pkey_ca(PrivateKey::Type::EC, PrivateKey::Curve::PRIME256_V1, "A real good passphrase");
        CHECK(pkey_ca.isValid());

        CertificateRequest::Subject subject_ca;
        subject_ca.country           = "FR";
        subject_ca.state             = "Savoie";
        subject_ca.location          = "Chambery";
        subject_ca.organization      = "Open OCPP";
        subject_ca.organization_unit = "Unit tests";
        subject_ca.common_name       = "Temporary CA";
        subject_ca.email_address     = "ca.unit.test@open-ocpp.org";

        CertificateRequest::Extensions ca_extensions;
        ca_extensions.basic_constraints.present     = true;
        ca_extensions.basic_constraints.is_ca       = true;
        ca_extensions.basic_constraints.path_length = 3;
        ca_extensions.subject_alternate_names.push_back("ca.open-ocpp.org");
        ca_extensions.subject_alternate_names.push_back("10.11.12.13");

        CertificateRequest req_ca(subject_ca, ca_extensions, pkey_ca);
        CHECK(req_ca.isValid());

        Certificate ca(req_ca, pkey_ca, Sha2::Type::SHA256, 7300u);
        CHECK(ca.isValid());

        const CertificateRequest::Extensions& ca_extensions_gen = ca.x509v3Extensions();
        CHECK(ca_extensions_gen.basic_constraints.present);
        CHECK(ca_extensions_gen.basic_constraints.is_ca);
        CHECK_EQ(ca_extensions_gen.basic_constraints.path_length, 3);
        CHECK_EQ(ca.subjectAltNames(), ca_extensions.subject_alternate_names);

        // Create new certificate
        PrivateKey pkey_cert(PrivateKey::Type::EC, PrivateKey::Curve::PRIME256_V1, "Another real good passphrase");
        CHECK(pkey_cert.isValid());

        CertificateRequest::Subject subject_cert;
        subject_cert.country           = "FR";
        subject_cert.state             = "Savoie";
        subject_cert.location          = "Chambery";
        subject_cert.organization      = "Open OCPP";
        subject_cert.organization_unit = "Unit tests";
        subject_cert.common_name       = "UT signing and verifying";
        subject_cert.email_address     = "unit.test@open-ocpp.org";

        CertificateRequest::Extensions cert_extensions;
        cert_extensions.basic_constraints.present = true;
        cert_extensions.basic_constraints.is_ca   = false;
        cert_extensions.subject_alternate_names.push_back("localhost");
        cert_extensions.subject_alternate_names.push_back("127.0.0.1");

        CertificateRequest req_cert(subject_cert, cert_extensions, pkey_cert);
        CHECK(req_cert.isValid());

        Certificate cert(req_cert, ca, pkey_ca, Sha2::Type::SHA256, 7300u);
        CHECK(cert.isValid());

        CHECK(cert.verify(ca.certificateChain()));

        const CertificateRequest::Extensions& cert_extensions_gen = cert.x509v3Extensions();
        CHECK(cert_extensions_gen.basic_constraints.present);
        CHECK_FALSE(cert_extensions_gen.basic_constraints.is_ca);
        CHECK_EQ(cert.issuerAltNames(), ca.subjectAltNames());
        CHECK_EQ(cert.subjectAltNames(), cert_extensions.subject_alternate_names);
    }

    TEST_CASE("Signature")
    {
        // Create certificate
        PrivateKey pkey_cert(PrivateKey::Type::EC, PrivateKey::Curve::PRIME256_V1, "A real good passphrase");
        CHECK(pkey_cert.isValid());

        CertificateRequest::Subject subject_cert;
        subject_cert.country           = "FR";
        subject_cert.state             = "Savoie";
        subject_cert.location          = "Chambery";
        subject_cert.organization      = "Open OCPP";
        subject_cert.organization_unit = "Unit tests";
        subject_cert.common_name       = "UT signing and verifying";
        subject_cert.email_address     = "unit.test@open-ocpp.org";

        CertificateRequest::Extensions cert_extensions;
        cert_extensions.subject_alternate_names.push_back("localhost");
        cert_extensions.subject_alternate_names.push_back("127.0.0.1");

        CertificateRequest req_cert(subject_cert, cert_extensions, pkey_cert);
        CHECK(req_cert.isValid());

        Certificate cert(req_cert, pkey_cert, Sha2::Type::SHA256, 7300u);
        CHECK(cert.isValid());

        const CertificateRequest::Extensions& cert_extensions_gen = cert.x509v3Extensions();
        CHECK_FALSE(cert_extensions_gen.basic_constraints.present);
        CHECK_EQ(cert.issuerAltNames(), cert.subjectAltNames());
        CHECK_EQ(cert.subjectAltNames(), cert_extensions.subject_alternate_names);

        // Sign a message with the private key
        std::string data = "Some data buffer with not so interested data but which must signed to ensure both integrity and authenticity "
                           "of the test input!";

        std::vector<uint8_t> data_signature = pkey_cert.sign(data.c_str(), data.size());
        CHECK_FALSE(data_signature.empty());

        // Check signature with the certificate
        CHECK(cert.verify(data_signature, data.c_str(), data.size()));

        // Check modified signature
        data_signature[24]++;
        CHECK_FALSE(cert.verify(data_signature, data.c_str(), data.size()));

        // Check restored signature with the certificate
        data_signature[24]--;
        CHECK(cert.verify(data_signature, data.c_str(), data.size()));
    }
}

TEST_SUITE("Base64")
{
    TEST_CASE("Encode/Decode nominal")
    {
        // Data to encode
        const std::string input_data =
            "This string could have been some binary data but it is way more easier to do it with human readable data instead :) !";

        // Expected result
        const std::string expected_result = "VGhpcyBzdHJpbmcgY291bGQgaGF2ZSBiZWVuIHNvbWUgYmluYXJ5IGRhdGEgYnV0IGl0IGlzIHdheSBtb3JlIGVhc2llci"
                                            "B0byBkbyBpdCB3aXRoIGh1bWFuIHJlYWRhYmxlIGRhdGEgaW5zdGVhZCA6KSAh";

        // Check encoding
        CHECK_EQ(ocpp::x509::base64::encode(input_data.c_str(), input_data.size()), expected_result);

        // Check decoding
        std::vector<uint8_t> decoded_data = ocpp::x509::base64::decode(expected_result);
        std::string          decoded_data_str(reinterpret_cast<char*>(&decoded_data[0]), decoded_data.size());
        CHECK_EQ(decoded_data_str, input_data);
    }

    TEST_CASE("Encode/Decode limits")
    {
        // Empty buffer to encode
        std::string encoded_null = ocpp::x509::base64::encode("hjkl", 0u);
        CHECK(encoded_null.empty());

        // NULL buffer to encode
        encoded_null = ocpp::x509::base64::encode(nullptr, 10u);
        CHECK(encoded_null.empty());

        // Empty buffer to decode
        std::vector<uint8_t> decoded_null = ocpp::x509::base64::decode("");
        CHECK(decoded_null.empty());

        // Invalid input data to decode
        decoded_null = ocpp::x509::base64::decode("VGh");
        CHECK(decoded_null.empty());
    }
}
