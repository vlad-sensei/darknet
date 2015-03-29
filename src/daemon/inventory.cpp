#include "inventory.h"


void Inventory::chunkFile(string fullFilePath, Metabody& metabody) {
        ifstream fileStream;
        fileStream.open(fullFilePath, ios::in | ios::binary);
        Id tmp_bid("tmp");
        creat_bid_table(tmp_bid);
        int length=getFileSize(&fileStream);


        // File open a success
        if (fileStream.is_open()) {

            int counter = 1;



            // Keep reading until end of file
            while (!fileStream.eof()&& length!=fileStream.tellg()) {
            string data(fileStream.tellg()+CHUNK_SIZE<= length ? CHUNK_SIZE:(length-fileStream.tellg()) ,'\0');

            fileStream.read((char*)data.data(),data.size());
            Chunk chunk(data);
            insert_chunk(tmp_bid,chunk);
            metabody.cids.emplace_back(chunk.cid);

            counter++;
            debug("spliting");
            }

            // Cleanup buffer


            // Close input file stream.
            fileStream.close();
            cout << "Chunking complete! " << counter - 1 << " files created." << endl;
            metabody.update_Id();
            rename_bid_table(tmp_bid,metabody.bid);
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
                string data=get_chunk_data(metabody.bid,cid);
                outputfile.write(data.data(),data.size());
                counter++;
            }

            // Close output file.
            outputfile.close();

            cout << "File assembly complete!" << endl;
        }
        else { cout << "Error: Unable to open file for output." << endl; }

    }
