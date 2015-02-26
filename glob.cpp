#include "glob.h"
#include "cryptopp/sha.h"

void hash512(const string& value){
    //const hash_t res( new unsigned char[CryptoPP::SHA512::DIGESTSIZE]);
    //byte a[CryptoPP::SHA512::DIGESTSIZE];
    //CryptoPP::SHA512().CalculateDigest(a, (byte*) value.data(), value.size());
    byte const* pbData = (byte*) value.data();
        unsigned int nDataLen = value.size();
        byte abDigest[CryptoPP::SHA256::DIGESTSIZE];

        CryptoPP::SHA256().CalculateDigest(abDigest, pbData, nDataLen);

}
