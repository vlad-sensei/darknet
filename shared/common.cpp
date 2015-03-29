#include "common.h"
#include <cmath>

deque<Chunk> Metabody::to_chunks(){
    deque<Chunk> res;
    uint32_t cid_count = cids.size();
    uint32_t bid_count = 0;
    unsigned cids_per_bid = CHUNK_SIZE/sizeof(Id);

    // bid_count = ceil((cid_count +1)*S) - 1, where S = a+a^2+..a^inf, where 0 < a = 1/cids_per_bid < 1
    // hence bid_count = (cid_count+1)/(cids_per_bid-1) -1  rounded up (geometric sum)
    // TODO: check for possible precision errors
    bid_count = ceil( (double)(cid_count+1)/(double)(cids_per_bid-1));
    /*int cid_remainder = cid_count + 1 - cids_per_bid;
    while(cid_remainder > (int)cids_per_bid) {
        unsigned new_bids = cid_remainder/cids_per_bid;
        bid_count += new_bids;
        cid_remainder%=cids_per_bid;
        cid_remainder+=new_bids;
    }
    if(cid_remainder>0) bid_count++;
    */
    for(size_t i=0; i<bid_count; i++){
      size_t cids_offs = (i+1)*cids_per_bid - bid_count - 1;
      size_t cids_to_copy = (cids_offs + cids_per_bid <= cids.size()) ? cids_per_bid : cids.size()-cids_offs;
      string data(cids_to_copy*sizeof(Id),'\0');
      memcpy(&data[0], (char*)&cids[cids_offs], data.size());
      res.emplace_back(data);
      bids.emplace_back(res.back().cid);
    }
    size_t offs = 0;
    size_t chunk_size = bid_count>0 ? CHUNK_SIZE - (CHUNK_SIZE-sizeof(bid_count) - sizeof(cid_count))%sizeof(Id) : sizeof(bid_count) + sizeof(cid_count) + (bid_count+cid_count)*sizeof(Id);
    string data(chunk_size, '\0');
    memcpy(&data[offs],(char*)&bid_count, sizeof(bid_count));
    offs+=sizeof(bid_count);
    memcpy(&data[offs],(char*)&cid_count, sizeof(cid_count));
    offs+=sizeof(cid_count);
    memcpy(&data[offs], (char*)&bids[0], sizeof(Id)*bids.size());
    offs+=sizeof(Id)*bids.size();
    memcpy(&data[offs], (char*)&cids[0], chunk_size-offs);
    res.emplace_front(data);
    return res;
}


bool Metabody::append_from_chunk(const Chunk& chunk){
    size_t offs=0;
    if(bid == chunk.cid){
        uint32_t bid_count;
        uint32_t cid_count;
        memcpy((char*)&bid_count,&chunk.data[offs],sizeof(bid_count));
        offs+=sizeof(bid_count);
        memcpy((char*)&cid_count,&chunk.data[offs],sizeof(cid_count));
        offs+=sizeof(cid_count);

        if(bid_count > MAX_BIDS_PER_METABODY || cid_count > MAX_CIDS_PER_METABODY){
            debug("*** too many cids or bids");
            return false;
        }
        bids.resize(bid_count);
        cids.resize(cid_count);

        memcpy((char*)&bids,&chunk.data[offs],sizeof(Id)*bid_count);
        offs+=sizeof(Id)*bid_count;
    }
    if((chunk.data.size()-offs)%sizeof(Id)!=0 ){
        debug("*** extra bytes ?");
        return false;
    }
    size_t cids_to_copy=(chunk.data.size()-offs)/sizeof(Id);
    memcpy((char*)&cids[cids_offs],&chunk.data[offs],cids_to_copy*sizeof(Id));
    cids_offs+=cids_to_copy*sizeof(Id);
    return true;
}
