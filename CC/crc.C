#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <zlib.h>
#include <TFile.h>
#include <TXNetFile.h>
#include "astring.h"
#include "crclib.h"

#define AMSCRC_INDEX 0
#define CRC_INDEX    1
#define ADLER_INDEX  2
#define ANY_INDEX    3
#define BUF_SIZE     38200000

extern "C" int _select(const dirent64 *entry);
extern "C" int _selectsdir(const dirent64 *entry);
extern "C" int _sort(const dirent64 ** e1, const dirent64 ** e2);
extern "C" int _sorta(const dirent64 ** e1, const dirent64 ** e2);
static unsigned int chunk[BUF_SIZE];

// values on rows combined by OR, values in cols combined by AND
// first col is for AMSCRC value to compare, second for CRC and third for ADLER
// -1 means nothing to compare
long long crc_to_compare[3][3] = {{-1,-1,-1},{-1,-1,-1},{-1,-1,-1}};
short int verbose_mode = 0;

int crc_xrootd(AString furl);
short int calculate_crc(const AString fnam, uLong *amscrc, uLong *crc, uLong *adler);
short int calculate_need(int mode);
unsigned int get_compare_value(int mode);
short int print_result(const AString *fdir, const uLong *amscrc, const uLong *crc, const uLong *adler);

// return codes 
//       1         everything ok
//       0         wrong crc or crc was not compared
//       128+1     too few parameters
//       128+2     file is not a plain file
//       128+3     unable to open file 

int main(int argc, char * argv[]) {
    int l_argc = argc;
    // get verbose mode
    for (int i=1; i<argc; i++) {
        if (strncmp(argv[i], "-v", 2) == 0) {
            verbose_mode = 1;
            l_argc--;
        }
    }

    if(l_argc<2){
        cerr<< "  Please provide Directory Or File Name"<<endl;
        return 128+1;
    } else if (l_argc == 2) {
        crc_to_compare[0][AMSCRC_INDEX] = 0;
        crc_to_compare[0][CRC_INDEX]    = 0;
    } else if (l_argc == 3) {
        unsigned int compare_value = 0;
        sscanf(argv[2], "%u", &compare_value);
        crc_to_compare[0][AMSCRC_INDEX] = compare_value;
        crc_to_compare[0][CRC_INDEX]    = compare_value;
    } else {
        // last parameter must be checksum to compare
        if (strncmp(argv[argc-1], "-", 1) == 0) {
            cerr << "  Please provide checksum to compare" << endl;
            return 128+1;
        }
        unsigned int amscrc_flag = 0;
        unsigned int crc_flag = 0;
        unsigned int adler_flag = 0;
        unsigned int and_counter = 0;
        for (int i=2; i<argc; i++) {
            if (strncmp(argv[i], "-", 1) != 0) {
                if (amscrc_flag || crc_flag || adler_flag) {
                    if (amscrc_flag) {
                        uLong compare_value = 0;
                        sscanf(argv[i], "%u", &compare_value);
                        crc_to_compare[and_counter][AMSCRC_INDEX] = compare_value;
                    }
                    if (crc_flag) {
                        uLong compare_value = 0;
                        sscanf(argv[i], "%u", &compare_value);
                        crc_to_compare[and_counter][CRC_INDEX] = compare_value;
                    }
                    if (adler_flag) {
                        char *compare_value;
                        sscanf(argv[i], "%s", &compare_value);
                        crc_to_compare[and_counter][ADLER_INDEX] = (int)strtol((char *)&compare_value, NULL, 16);
                    }
                    and_counter++;
                    amscrc_flag = crc_flag = adler_flag = 0;
                }
            } else if (strncmp(argv[i], "-crc32", 6) == 0) {
                crc_flag = 1;
            } else if (strncmp(argv[i], "-amscrc32", 9) == 0) {
                amscrc_flag = 1;
            } else if (strncmp(argv[i], "-adler32", 8) == 0) {
                adler_flag = 1;
            }
        }
    }

    /*
    for(int i=0; i<3; i++) {
        cout << "(";
        for(int j=0; j<3; j++) {
            if (crc_to_compare[i][j] != -1) {
                if (j == AMSCRC_INDEX) {
                    cout << " amscrc32=" << crc_to_compare[i][j] << " OR ";
                } else if (j == CRC_INDEX) {
                    cout << " crc32=" << crc_to_compare[i][j] << " OR ";
                } else if (j == ADLER_INDEX) {
                    cout << " adler32=" << crc_to_compare[i][j] << " OR ";
                }
            }
        }
        cout << ") AND " <<  endl;
    }
    */

    AString fdir(argv[1]);
    if (strncmp((const char*)fdir, "root://", 7) == 0) {
        return crc_xrootd(fdir);
    }
    struct stat64 statdir_map;
    stat64((const char*)fdir,&statdir_map);
    if((statdir_map.st_mode & 16384) ) {
        // reset values
        crc_to_compare[0][AMSCRC_INDEX] = -1;
        crc_to_compare[0][CRC_INDEX]    = -1;
        if(l_argc > 3) {
            return 128+2;
        } else if (l_argc == 3) {
            if (strncmp(argv[2], "-amscrc32", 9) == 0) {
                crc_to_compare[0][AMSCRC_INDEX] = 0;
            } else if (strncmp(argv[2], "-crc32", 6) == 0) {
                crc_to_compare[0][CRC_INDEX] = 0;
            } else if (strncmp(argv[2], "-adler32", 8) == 0) {
                crc_to_compare[0][ADLER_INDEX] = 0;
            }
        } else {
            crc_to_compare[0][AMSCRC_INDEX] = 0;
        }

        ofstream fbout;
        fbout.open((const char*)(fdir+".crc"));
#if defined(__LINUXSLC6__)
        dirent64 ** namelistsubdir;
        int nptrdir = scandir64((const char *)fdir, &namelistsubdir, &_selectsdir, reinterpret_cast<int(*)(const dirent64**, const dirent64**)>(&_sort));
#elif defined(__LINUXNEW__)
        dirent64 ** namelistsubdir;
        int nptrdir = scandir64((const char *)fdir, &namelistsubdir, &_selectsdir, (&_sort));
#elif defined(__LINUXGNU__)
        dirent64 ** namelistsubdir;
        int nptrdir = scandir64((const char *)fdir, &namelistsubdir, &_selectsdir, reinterpret_cast<int(*)(const void*, const void*)>(&_sort));
#else
        dirent64 ** namelistsubdir;
        int nptrdir = scandir64((const char *)fdir, &namelistsubdir, &_selectsdir, &_sort);
#endif

        for(int is=nptrdir-1;is<nptrdir;is++){
            AString fsdir(fdir);
#if defined(__LINUXSLC6__)
            dirent64 ** namelist;
            int nptr = scandir64((const char *)fsdir, &namelist, &_select, reinterpret_cast<int(*)(const dirent64**, const dirent64**)>(&_sorta));
#elif defined(__LINUXNEW__)
            dirent64 ** namelist;
            int nptr = scandir64((const char *)fsdir, &namelist, &_select, (&_sorta));
#elif defined(__LINUXGNU__)
            dirent64 ** namelist;
            int nptr = scandir64((const char *)fsdir, &namelist, &_select, reinterpret_cast<int(*)(const void*, const void*)>(&_sorta));
#else
            dirent64 ** namelist;
            int nptr = scandir64((const char *)fsdir, &namelist, &_select, &_sorta);
#endif

            for(int ii=0;ii<nptr;ii++){
                AString fnam(fsdir);
                fnam+="/";
                fnam+=namelist[ii]->d_name;    
                uLong amscrc, crc, adler = 0;
                calculate_crc(fnam, &amscrc, &crc, &adler);
                if (calculate_need(AMSCRC_INDEX)) {
                    cout  << fnam << " " << amscrc << endl;
                    fbout << fnam << " " << amscrc << endl;
                } else if(calculate_need(CRC_INDEX)) {
                    cout  << fnam << " " << crc << endl;
                    fbout << fnam << " " << crc << endl;
                } else if(calculate_need(ADLER_INDEX)) {
                    cout  << fnam << " " << std::setfill('0') << std::setw(8) << std::hex << adler << endl;
                    fbout << fnam << " " << std::setfill('0') << std::setw(8) << std::hex << adler << endl;
                }
            }
        }     
        fbout.close();
    } else {
        uLong amscrc, crc, adler = 0;
        short int result;
        result = calculate_crc(fdir, &amscrc, &crc, &adler);
        if (result != 0) {
            return result;
        }
        return print_result(&fdir, &amscrc, &crc, &adler);
    }
    return 0;
}

int _select(const dirent64 *entry){
    return strstr(entry->d_name,".root")!=NULL;    
}

int _selectsdir(const dirent64 *entry){
    return (entry->d_name)[0] != '.';   
}

int _sort(const dirent64 ** e1, const dirent64 ** e2){
    return strcmp((*e1)->d_name,(*e2)->d_name);
}

int _sorta(const dirent64 ** e1, const dirent64 ** e2){
    char tmp[255];
    char buf[255];
    unsigned int run1(0),run2(0),ev1(0),ev2(0);
    strcpy(tmp,(*e1)->d_name);
    for (int i=strlen(tmp)-1;i>=0;i--){
        if(tmp[i]=='/'){
            for (int j=i+1;j<strlen(tmp);j++){
                if(tmp[j]=='.'){
                    tmp[j]='\0';
                    sscanf(tmp+i+1,"%d",&run1);
                    for (int k=j+1;k<strlen(tmp);k++){
                        if(tmp[k]=='.'){
                            tmp[k]='\0';
                            sscanf(tmp+j+1,"%d",&ev1);
                        }
                    }
                    break;
                }
            }
            break; 
        }
    }  

    strcpy(tmp,(*e2)->d_name);
    for (int i=strlen(tmp)-1;i>=0;i--){
        if(tmp[i]=='/'){
            for (int j=i+1;j<strlen(tmp);j++){
                if(tmp[j]=='.'){
                    tmp[j]='\0';
                    sscanf(tmp+i+1,"%d",&run2);
                    for (int k=j+1;k<strlen(tmp);k++){
                        if(tmp[k]=='.'){
                            tmp[k]='\0';
                            sscanf(tmp+j+1,"%d",&ev2);
                        }
                    }
                    break;
                }
            }
            break; 
        }
    }  

    if(run1<run2){
        return -1;
    }
    else if(run1==run2){
        if(ev1<ev2){
            return -1;
        }
        else if (ev1==ev2)return 0;
        else return 1;
    }
    else return 1;
}

int crc_xrootd(AString furl) {
    TXNetFile *rfile = new TXNetFile(furl+"?filetype=raw", "READ");
    if (!rfile) {
        return 128+3;
    }
    Long64_t fsize, mysize;
    fsize = mysize = rfile->GetSize();
    uLong amscrc = 0;
    uLong crc = 0;
    uLong adler = adler32(0L, Z_NULL, 0);
    Long64_t pos = 0;
    for(;;){
        if(!fsize) break;
        int myread=fsize>sizeof(chunk)?sizeof(chunk):fsize;
        fsize-=myread;
        Bool_t ret = rfile->ReadBuffer((char*)chunk,myread);
        if (!ret) {
            // ams crc calculation
            if (calculate_need(AMSCRC_INDEX)) {
                amscrc = amscrc_update(amscrc, chunk, myread);
            }
            // cksum crc calculation
            if (calculate_need(CRC_INDEX)) {
                crc = crc_update(crc, chunk, myread, BUF_SIZE, mysize, &pos);
            }
            // adler calculation
            if (calculate_need(ADLER_INDEX)) {
                adler = adler32(adler, (const Bytef*)chunk, myread);
            }
        }
        else break;
    }
    rfile->Close();

    if (calculate_need(AMSCRC_INDEX)) {
        amscrc = amscrc_finalize(amscrc);
    }

    if (calculate_need(CRC_INDEX)) {
        crc = crc_finalize(crc, mysize);
    }

    return print_result(&furl, &amscrc, &crc, &adler);
}

short int calculate_crc(const AString fnam, uLong *amscrc, uLong *crc, uLong *adler) {
    *amscrc = 0;
    *crc = 0;
    *adler = 0;

    ifstream fbin;
    fbin.open((const char*)fnam);
    if(!fbin){
        return 128+3;
    }
    struct stat64 statbuf_map;
    stat64((const char*)fnam, &statbuf_map);
    long long fsize, mysize;
    fsize = mysize = statbuf_map.st_size;
    uLong l_amscrc = 0;
    uLong l_crc = 0;
    uLong l_adler = adler32(0L, Z_NULL, 0);
    Long64_t pos = 0;
    for(;;) {
        if(!fsize) break;
        int myread = fsize>sizeof(chunk)?sizeof(chunk):fsize;
        fbin.read((char*)chunk, myread);
        fsize-=myread;
        if(fbin.good() && !fbin.eof()){
            // ams crc calculation
            if (calculate_need(AMSCRC_INDEX)) {
                l_amscrc = amscrc_update(l_amscrc, chunk, myread);
            }
            // cksum crc calculation
            if (calculate_need(CRC_INDEX)) {
                l_crc = crc_update(l_crc, chunk, myread, BUF_SIZE, mysize, &pos);
            }
            // adler calculation
            if (calculate_need(ADLER_INDEX)) {
                l_adler = adler32(l_adler, (const Bytef*)chunk, myread);
            }
        }
        else break;
    }
    fbin.close();

    if (calculate_need(AMSCRC_INDEX)) {
        *amscrc = amscrc_finalize(l_amscrc);
    }

    if (calculate_need(CRC_INDEX)) {
        *crc = crc_finalize(l_crc, mysize);
    }

    if (calculate_need(ADLER_INDEX)) {
        *adler = l_adler;
    }

    return 0;
}

short int calculate_need(int mode)
{
    short int result = 0;
    for(int i=0; i<3; i++) {
        if (mode == ANY_INDEX) {
            if (crc_to_compare[i][AMSCRC_INDEX] != -1 || crc_to_compare[i][CRC_INDEX] != -1 || crc_to_compare[i][ADLER_INDEX] != -1) {
                result = 1;
                break;
            }
        } else if (crc_to_compare[i][mode] != -1) {
            result = 1;
            break;
        }
    }
    return result;
}

unsigned int get_compare_value(int mode)
{
    unsigned int result = 0;
    for(int i=0; i<3; i++) {
        if (crc_to_compare[i][mode] != -1) {
            result = crc_to_compare[i][mode];
            break;
        }
    }
    return result;
}

short int print_result(const AString *fdir, const uLong *amscrc, const uLong *crc, const uLong *adler)
{
    int or_result[3] = {-1,-1,-1};
    for(int i=0; i<3; i++) {
        if (
            (crc_to_compare[i][AMSCRC_INDEX] != -1 && crc_to_compare[i][AMSCRC_INDEX] == *amscrc) ||
            (crc_to_compare[i][CRC_INDEX] != -1 && crc_to_compare[i][CRC_INDEX] == *crc) ||
            (crc_to_compare[i][ADLER_INDEX] != -1 && crc_to_compare[i][ADLER_INDEX] == *adler)
            ) {
            or_result[i] = 1;
        } else if(
            (crc_to_compare[i][AMSCRC_INDEX] != -1 && crc_to_compare[i][AMSCRC_INDEX] != *amscrc) ||
            (crc_to_compare[i][CRC_INDEX] != -1 && crc_to_compare[i][CRC_INDEX] != *crc) ||
            (crc_to_compare[i][ADLER_INDEX] != -1 && crc_to_compare[i][ADLER_INDEX] != *adler)
            ) {
            or_result[i] = 0;
        }
    }
    int and_result = -1;
    for(int i=0; i<3; i++) {
        //cout << i << ":" << or_result[i] << endl;
        if (and_result != -1) {
            and_result = and_result && or_result[i];
        } else if(or_result[i] != -1) {
            and_result = or_result[i];
        }
    }

    int return_value = 0;
    if (and_result == 1) {
        return_value = 1;
    }
    if (return_value == 0 || verbose_mode) {
        cout << *fdir << " ";
        if (calculate_need(AMSCRC_INDEX)) {
            cout << *amscrc << " " << get_compare_value(AMSCRC_INDEX) << " ";
        }
        if (calculate_need(CRC_INDEX)) {
            cout << *crc << " " << get_compare_value(CRC_INDEX) << " ";
        }
        if (calculate_need(ADLER_INDEX)) {
            cout << std::setfill('0') << std::setw(8) << std::hex << *adler << " " << std::hex << get_compare_value(ADLER_INDEX) << " ";
        }
        cout << endl;
    }
    return return_value;
}
