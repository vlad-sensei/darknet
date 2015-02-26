#include "glob.h"
#include "cryptopp/sha.h"

hash_t hash512(const string& value){
    hash_t hashed_val(new unsigned char[CryptoPP::SHA512::DIGESTSIZE]);
    CryptoPP::SHA512().CalculateDigest(hashed_val.get(), (byte*) value.data(), value.size());
    return hashed_val;
}
