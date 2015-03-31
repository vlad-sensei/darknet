#include "inventory.h"

Inventory::Inventory(){
  //TODO: for now remove the old arena and create a new
  remove(DEFAULT_ARENA_PATH);
  data.arena.open(DEFAULT_ARENA_PATH);
  add_new_arena_slots(DEFAULT_ARENA_SLOT_NUM);
}

bool Inventory::get_file(const Id& bid, const string& dest_path){

  Chunk chunk;
  Metabody metabody(bid);
  //TODO: get more then one chunk to metabody
  if(!get_chunk(bid,bid,chunk)) return false;
  metabody.append_from_chunk(chunk);

  const string tmp_dest_path = dest_path + ".part";

  // Create our output file
  //TODO: check whether file exists already
  ofstream out_file(tmp_dest_path);
  if(!out_file.is_open()){
    debug("could not open output file");
    return false;
  }

  for(const Id& cid:metabody.cids){
    Chunk chunk;
    if(!get_chunk(metabody.bid,cid,chunk)){
      debug("*** need to request chunk on the network");
      return false;
    }
    out_file.write(chunk.data.data(),chunk.size());
  }

  // Close output file.
  out_file.close();

  debug("file assembly complete!");
  return true;
}


bool Inventory::get_chunk(const Id &bid, const Id &cid, Chunk &chunk){
  size_t size, slot;
  if(!Database::get_chunk(bid,cid,size,slot)) return false;
  if(!read_from_arena_slot(slot,size,chunk)) return false;
  if(!chunk.verify(cid)) return false;
  return true;
}

//TODO: hadle case where the upload is canceled before finised. Enable to cancel uploads
bool Inventory::upload_file(const string& filename, Metahead& metahead){
  Metabody metabody;
  ifstream file;
  file.open(filename, ios::in | ios::binary);
  if(!file.is_open()) return false;

  file.seekg(0,ios::end);
  file_size_t file_size = file.tellg();
  file.seekg(ios::beg);

  unordered_map<Id, pair<size_t, size_t> > cids; // cids[Id] = {slot, size} to ensure to not to write chunks twice

  // Keep reading until end of file
  while ((int)file_size<file.tellg()) {
    // creat data string with aprobriate size
    const file_size_t& bytes_to_read = ((file_size_t)file.tellg() + CHUNK_SIZE <= file_size) ? CHUNK_SIZE : (file_size_t)(file_size-file.tellg());

    string data(bytes_to_read,'\0');
    file.read((char*)data.data(),data.size());
    Chunk chunk(data);
    size_t idx;
    if(cids.find(chunk.cid)==cids.end()){
      write_to_arena_slot(chunk.data, idx);
      cids[chunk.cid]=make_pair(idx, chunk.size());
    }

    metabody.cids.emplace_back(chunk.cid);
    debug("spliting [%s]..", metabody.cids.size());
  }
  // Close input file stream.
  file.close();
  debug("chunking complete");

  deque<Chunk> metabody_chunks=metabody.to_chunks();
  for(const Chunk& chunk:metabody_chunks){
    size_t idx;
    if(cids.find(chunk.cid)==cids.end()){
      write_to_arena_slot(chunk.data, idx);
      cids[chunk.cid]=make_pair(idx, chunk.size());
    }
  }


  //once added to arena, add to database.
  //TODO: use BEGIN; .. COMMIT;
  for(size_t i = 0; i<metabody_chunks.size(); i++){
    const Id& cid = metabody_chunks[i].cid;
    Database::add_chunk(metabody.bid, cid, cids[cid].first, cids[cid].second);
  }
  for(size_t i = 0; i < metabody.cids.size(); i++){
    const Id& cid = metabody.cids[i];
    Database::add_chunk(metabody.bid, cid, cids[cid].first, cids[cid].second);
  }
  metahead.bid = metabody_chunks[0].cid;
  return true;
}

//TODO: handle disk space issues..
bool Inventory::add_new_arena_slots(const size_t &num){
  w_lock l(arena_mtx);
  data.arena.seekp(data.arena.end);
  string buff(CHUNK_SIZE, '\0');
  for(size_t i = 0; i<num; i++){
    data.arena.write(buff.data(),buff.size());
    data.free_slots.emplace(++data.arena_slots_size);
  }
  return true;
}

bool Inventory::write_to_arena_slot(const string &raw_data, size_t &idx){
  w_lock l(arena_mtx);
  if(data.free_slots.empty()) {
    debug(" *** no free slots avaible in arena");
    return false;
  }
  if(raw_data.size()>=CHUNK_SIZE) {
    debug( " *** raw_data size is bigger than CHUNK_SIZE");
    return false;
  }
  idx = *data.free_slots.begin();
  data.free_slots.erase(idx);
  data.arena.seekp(idx*CHUNK_SIZE);
  data.arena.write(raw_data.data(), raw_data.size());
  return true;
}

bool Inventory::read_from_arena_slot(const size_t &idx, const size_t &chunk_size, Chunk &chunk){
  if(chunk_size>=CHUNK_SIZE){
    debug(" *** chunk_size>=CHUNK_SIZE");
    return false;
  }
  r_lock l(arena_mtx);
  if(idx>=data.arena_slots_size){
    debug(" *** idx>=data.arena_slots_size");
    return false;
  }
  ifstream arena(DEFAULT_ARENA_PATH);
  arena.seekg(idx*CHUNK_SIZE);
  string raw_data(chunk_size, '\0');
  arena.read(&raw_data[0],chunk_size);
  return chunk.set_data(raw_data);
}
