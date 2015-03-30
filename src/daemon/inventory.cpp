#include "inventory.h"


void Inventory::chunkFile(string fullFilePath, Metabody& metabody) {
        ifstream fileStream;
        fileStream.open(fullFilePath, ios::in | ios::binary);
        //TODO: tmp_bid how to save chunks with out bid
        Id tmp_bid("tmp");
        creat_bid_table(tmp_bid);
        int length=getFileSize(&fileStream);


        // File open a success
        if (fileStream.is_open()) {

            int counter = 1;

            // Keep reading until end of file
            while (!fileStream.eof()&& length!=fileStream.tellg()) {
            // creat data string with aprobriate size
            string data(fileStream.tellg()+CHUNK_SIZE<= length ? CHUNK_SIZE:(length-fileStream.tellg()) ,'\0');

            fileStream.read((char*)data.data(),data.size());
            Chunk chunk(data);
            add_chunk(tmp_bid,chunk);
            metabody.cids.emplace_back(chunk.cid);

            counter++;
            debug("spliting");
            }

            // Close input file stream.
            fileStream.close();
            cout << "Chunking complete! " << counter - 1 << " files created." << endl;


        }
        else { cout << "Error opening file!" << endl; }
}


// Simply gets the file size of file.
int Inventory::getFileSize(ifstream *file) {
file->seekg(0,ios::end);
int filesize = file->tellg();
file->seekg(ios::beg);
return filesize;
}




// Finds chunks by "chunkName" and creates file specified in fileOutput
void Inventory::joinFile(const Metabody& metabody, string fileOutput) {

        // Create our output file
        ofstream outputfile;
        outputfile.open(fileOutput, ios::out | ios::binary);

        // If successful, loop through chunks matching chunkName
        if (outputfile.is_open()) {

            int counter = 1;


            for(Id cid:metabody.cids){

                Chunk chunk;

                if(!get_chunk(metabody.bid,cid,chunk)){
                    debug("*** need to request chunk on the nettwork");
                }
                outputfile.write(chunk.data.data(),chunk.data.size());
                counter++;
            }

            // Close output file.
            outputfile.close();

            cout << "File assembly complete!" << endl;
        }
        else { cout << "Error: Unable to open file for output." << endl; }

    }
