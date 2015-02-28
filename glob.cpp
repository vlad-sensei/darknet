#include "glob.h"
#include "cryptopp/sha.h"
#include "cryptopp/hex.h"
#include <sstream>

const unsigned int HASH_SIZE = CryptoPP::SHA512::DIGESTSIZE;

hash_t hash512(const string& value){
    hash_t hashed_val(new unsigned char[HASH_SIZE]);
    CryptoPP::SHA512().CalculateDigest(hashed_val.get(), (byte*) value.data(), value.size());
    return hashed_val;
}

/*
 * Uses crypto++' hexencoder.
 */
void debug_hash512(const hash_t& hash){
    CryptoPP::HexEncoder encoder;
    std::string output;
    encoder.Attach( new CryptoPP::StringSink( output ) );
    encoder.Put( hash.get(), HASH_SIZE );
    encoder.MessageEnd();

    debug("Hash value is: %s\n", output.c_str());
}
