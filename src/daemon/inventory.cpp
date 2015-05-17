#include "inventory.h"

Inventory::Inventory(){}
void Inventory::init_inventory(){
  //TODO: for now remove the old arena and create a new

  /* Now you can overwrite existing files, even if they aren't a database
   * Should give error if it exists (a previous arena may not exist, and
   * if a user gives a path to another existing file, it will be overwritten).
   * As stated by TODO, this behaviour should be changed.
   */
  debug("initializing arena [%s]..", arena_path);
  remove(arena_path.c_str());
  data.arena.open(arena_path, ios::out | ios::binary);
  add_new_arena_slots(DEFAULT_ARENA_SLOT_NUM);
}

bool Inventory::get_file(const Id& bid, const string& dest_path){

  Chunk chunk;
  Metabody metabody(bid);

  //TODO: get more then one chunk to metabody
  if(!get_chunk(bid,bid,chunk)){
    debug("couldnt find metabody chunk");
    return false;
  }

  metabody.append_from_chunk(chunk);

  const string tmp_dest_path = dest_path + ".part";

  // Create our output file
  //TODO: check whether file exists already
  ofstream out_file(tmp_dest_path, ios::out| ios::binary);
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
  debug("get_chunk");
  if(!read_from_arena_slot(slot,size,chunk)) return false;
  debug("read_from");
  if(!chunk.verify(cid)) return false;
  debug("get_chunck successful");
  return true;
}
bool Inventory::add_chunk(const Id &bid, const Chunk &chunk){
  size_t idx;
  if (!write_to_arena_slot(chunk.data,idx)) return false;
  //TODO: check if in database
  Database::add_chunk(bid,chunk.cid,chunk.size(),idx);
  return true;
}

//TODO: hadle case where the upload is canceled before finised. Enable to cancel uploads
bool Inventory::upload_file(const string& filename, Metahead& metahead, const string tags){
  //open file and calculating its size
  Metabody metabody;
  ifstream file;
  file.open(filename, ios::in | ios::binary);
  if(!file.is_open()) return false;

  file.seekg(0,ios::end);
  file_size_t file_size = file.tellg();
  file.seekg(0,ios::beg);

  debug("file_size: %s", file_size);

  unordered_map<Id, pair<size_t, size_t> > cids; // cids[Id] = {slot, size} to ensure to not to write chunks twice

  // iterate through file, while adding chunks to arena
  while (file.tellg()<(int)file_size) {
    // creating buffer string with appropriate size
    const file_size_t& bytes_to_read = ((file_size_t)file.tellg() + CHUNK_SIZE <= file_size) ? CHUNK_SIZE : (file_size_t)(file_size-file.tellg());

    string data(bytes_to_read,'\0');
    file.read((char*)data.data(),data.size());
    Chunk chunk(data);
    size_t idx;
    if(cids.find(chunk.cid)==cids.end()){
      write_to_arena_slot(chunk.data, idx);//
      cids[chunk.cid]=make_pair(idx, chunk.size());
    }

    metabody.cids.emplace_back(chunk.cid);
  }
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
    Database::add_chunk(metabody.bid, cid, cids[cid].second, cids[cid].first);
  }
  for(size_t i = 0; i < metabody.cids.size(); i++){    
    const Id& cid = metabody.cids[i];
    Database::add_chunk(metabody.bid, cid, cids[cid].second, cids[cid].first);
  }
  metahead.bid = metabody_chunks[0].cid;
  metahead.tags = tags;
  metahead.mid=Id(string((char*)&metahead.bid, sizeof(metahead.bid)) + metahead.tags );
  return true;
}

//TODO: handle disk space issues..
bool Inventory::add_new_arena_slots(const size_t &num){
  w_lock l(arena_mtx);
  data.arena.seekp(data.arena.end);
  string buff(CHUNK_SIZE, '\0');
  for(size_t i = 0; i<num; i++){
    data.arena.write(buff.data(),buff.size());
    data.free_slots.emplace(data.arena_slots_size++);
  }
  data.arena.flush();
  return true;
}

bool Inventory::write_to_arena_slot(const string &raw_data, size_t &idx){
  w_lock l(arena_mtx);
  if(data.free_slots.empty()) {
    debug(" *** no free slots avaible in arena");
    return false;
  }
  if(raw_data.size()>CHUNK_SIZE) {
    debug( " *** raw_data size is bigger than CHUNK_SIZE");
    return false;
  }

  idx = *data.free_slots.begin();
  data.free_slots.erase(idx);
  //TODO: overflow on 32bit systems for large files!!!
  data.arena.seekp(idx*CHUNK_SIZE);

  data.arena.write(raw_data.data(), raw_data.size());
  data.arena.flush();
  return true;
}

bool Inventory::read_from_arena_slot(const size_t &idx, const size_t &chunk_size, Chunk &chunk){
  if(chunk_size>CHUNK_SIZE){
    debug(" *** chunk_size>=CHUNK_SIZE");
    return false;
  }
  r_lock l(arena_mtx);
  if(idx>=data.arena_slots_size){
    debug(" *** idx>=data.arena_slots_size");
    return false;
  }
  ifstream arena(arena_path, ios::in | ios::binary);
  //TODO: overflow on 32bit systems for large files!!!
  arena.seekg(idx*CHUNK_SIZE);

  string raw_data(chunk_size, '\0');
  arena.read(&raw_data[0],chunk_size);
  //debug("read from arena:\n[arena %s]\n[size %s]\n[substring %s]",idx,chunk_size,raw_data);
  return chunk.set_data(raw_data);
}
