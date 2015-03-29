#include "inventory.h"


void Inventory::chunkFile(char *fullFilePath, char *chunkName, unsigned long chunkSize) {
        ifstream fileStream;
        fileStream.open(fullFilePath, ios::in | ios::binary);
        Id tmp_bid("tmp");
        creat_bid_table(tmp_bid);

        // File open a success
        if (fileStream.is_open()) {
            ofstream output;
            int counter = 1;

            string fullChunkName;

            // Create a buffer to hold each chunk
            char *buffer = new char[chunkSize];

            // Keep reading until end of file
            while (!fileStream.eof()) {

                // Build the chunk file name. Usually drive:\\chunkName.ext.N
                // N represents the Nth chunk
                //fullChunkName.clear();
                //fullChunkName.append(chunkName);
                //fullChunkName.append(".");

                // Convert counter integer into string and append to name.
                //char intBuf[10];
//	            itoa(counter,intBuf,10);
                //fullChunkName.append(to_string(counter));

                // Open new chunk file name for output
                //output.open(fullChunkName.c_str(),ios::out | ios::trunc | ios::binary);

                // If chunk file opened successfully, read from input and
                // write to output chunk. Then close.
                //if (output.is_open()) {
                    fileStream.read(buffer,chunkSize);
                    // gcount() returns number of bytes read from stream.
                    //output.write(buffer,fileStream.gcount());
                    string tmp_string(buffer,fileStream.gcount());
                    //insert_chunk(tmp_bid,Chunk(tmp_string));
                    string test="test";
                    insert_chunk(tmp_bid,Chunk(test));

                    counter++;
                //}
            }

            // Cleanup buffer
            delete(buffer);

            // Close input file stream.
            fileStream.close();
            cout << "Chunking complete! " << counter - 1 << " files created." << endl;
            //rename_bid_table(tmp_bid,Id("tjena"));
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
void Inventory::joinFile(char *chunkName, char *fileOutput) {
        string fileName;

        // Create our output file
        ofstream outputfile;
        outputfile.open(fileOutput, ios::out | ios::binary);

        // If successful, loop through chunks matching chunkName
        if (outputfile.is_open()) {
            bool filefound = true;
            int counter = 1;
            int fileSize = 0;

            while (filefound) {
                filefound = false;

                // Build the filename
                fileName.clear();
                fileName.append(chunkName);
                fileName.append(".");

                char intBuf[10];
//_itoa(counter,intBuf,10);
                fileName.append(to_string(counter));

                // Open chunk to read
                ifstream fileInput;
                fileInput.open(fileName.c_str(), ios::in | ios::binary);

                // If chunk opened successfully, read it and write it to
                // output file.
                if (fileInput.is_open()) {
                    filefound = true;
                    fileSize = getFileSize(&fileInput);
                    char *inputBuffer = new char[fileSize];

                    fileInput.read(inputBuffer,fileSize);
                    outputfile.write(inputBuffer,fileSize);
                    delete(inputBuffer);

                    fileInput.close();
                }
                counter++;
            }

            // Close output file.
            outputfile.close();

            cout << "File assembly complete!" << endl;
        }
        else { cout << "Error: Unable to open file for output." << endl; }

    }
