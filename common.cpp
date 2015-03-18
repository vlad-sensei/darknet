#include "common.h"
#include "cryptopp/sha.h"
#include "cryptopp/hex.h"

const unsigned int HASH_SIZE = CryptoPP::SHA512::DIGESTSIZE;

hash_t hash512(const string& value){
  hash_t res(HASH_SIZE,'\0');
  CryptoPP::SHA512().CalculateDigest((byte*)res.data(), (byte*) value.data(), value.size());
  return res;
}

/*
 * Uses crypto++' hexencoder.
 */
void debug_hash512(const hash_t& hash){
  CryptoPP::HexEncoder encoder;
  std::string output;
  encoder.Attach( new CryptoPP::StringSink( output ) );
  encoder.Put((byte*)hash.data(), HASH_SIZE );
  encoder.MessageEnd();

  debug("Hash value is: %s", output.c_str());
}
