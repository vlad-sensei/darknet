#include "common.h"


static size_t BID_OFFSET = MID_WIDTH;
static size_t FILE_SIZE_OFFSET = BID_OFFSET + BID_WIDTH;
static size_t TAGS_OFFSET = FILE_SIZE_OFFSET + FILE_WIDTH;

/* Serialization format of metahead:
 *
 * +----------+----------+----------+------------+
 * | MID [64] | BID [64] | SIZE [4] | TAGS [892] |
 * +----------+----------+----------+------------+
 * 0        63|64     127|128    131|132         892
 */
 metahead_ser_t serialize_metahead(const Metahead& metahead){
  metahead_ser_t buffer = {0};

  memcpy(&buffer[0]               , (byte*) metahead.mid.get_data(), MID_WIDTH);
  memcpy(&buffer[BID_OFFSET]      , (byte*) metahead.bid.get_data(), BID_WIDTH);
  memcpy(&buffer[FILE_SIZE_OFFSET], (byte*) &metahead.file_size    , FILE_WIDTH);
  memcpy(&buffer[TAGS_OFFSET]     , (byte*) metahead.tags.data()   , metahead.tags.size());

  return buffer;
}

 Metahead deserialize_metahead(const metahead_ser_t& meta_ser){
   Metahead meta;
   meta.mid = Id((byte*) &meta_ser[0]);
   meta.bid = Id((byte*) &meta_ser[BID_OFFSET]);
   memcpy(&meta.file_size, &meta_ser[FILE_SIZE_OFFSET], FILE_WIDTH);
   meta.tags = string((char*) &meta_ser[TAGS_OFFSET], TAGS_WIDTH);

   return meta;
 }
