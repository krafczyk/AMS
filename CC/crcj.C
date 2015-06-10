#include <iostream.h>
#include <stdlib.h>
#include <fstream.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include "../include/astring.h"
#include "../include/crclib.h"

// return codes 
//       1         everything ok
//       0         wrong crc or crc was not compared
//       128+1     too few parameters
//       128+2     file is not a plain file
//       128+3     unable to open file 

int main(int argc, char * argv[]){
    if(argc<3){
        cerr<< "  Please provide File Name for checksum and File Name for output"<<endl;
        return 128+1;
    }
    AString fdir(argv[1]);
    AString fout(fdir);
    AString fjou(argv[2]);
    struct stat64 statdir_map;
    stat64((const char*)fdir,&statdir_map);
    ifstream fbin;
    fbin.open((const char*)fdir);
    if(!fbin){
        return 128+3;
    }

    unsigned int amscrc, crc;
    amscrc = crc = 0;
    long long fsize=statdir_map.st_size;
    long long fo=fsize;
    int buf_size = 32768*8;
    unsigned int chunk[buf_size];
    Long64_t pos = 0;
    for(;;) {
        if(!fsize) break;
        int myread = fsize>sizeof(chunk)?sizeof(chunk):fsize;
        fbin.read((char*)chunk, myread);
        fsize-=myread;
        if(fbin.good() && !fbin.eof()){
            amscrc = amscrc_update(amscrc, chunk, myread);
            crc = crc_update(crc, chunk, myread, buf_size, fo, &pos);
        }
        else break;
    }
    fbin.close();

    amscrc = amscrc_finalize(amscrc);
    crc = crc_finalize(crc, fo);

    ofstream fbout;
    fbout.open((const char*)fjou, ios::out|ios::app);
    fbout << " CRC=" << amscrc << endl;
    fbout << " CRC32=" << crc << endl;
    unsigned int fs=fo/1024/512;
    if(fs<2)fs=2;
    fbout << " Size=" << fs << endl;;

    return 0;
}
