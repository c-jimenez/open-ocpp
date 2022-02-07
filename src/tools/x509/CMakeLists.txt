
# X509 library
include_directories(impl)
add_library(x509 STATIC
    Certificate.cpp
    CertificateRequest.cpp
    PrivateKey.cpp
    Sha2.cpp
    X509Document.cpp

    impl/sign.cpp
)
target_include_directories(x509 PUBLIC .)
target_link_libraries(x509 helpers OpenSSL::SSL OpenSSL::Crypto)
