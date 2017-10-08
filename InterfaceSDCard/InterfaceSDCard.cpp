#include "mbed.h"
#include "SDFileSystem.h"
#include "InterfaceSDCard.h"
//#include <iostream>
//#include <fstream>

SDFileSystem    sd(PB_15, PB_14, PB_13, PA_4, "sd");                           // MOSI, MISO, SCLK, SSEL
vector<string> _directory_list;

InterfaceSDCard::InterfaceSDCard(MidiParser *midiParser) : _midiParser(midiParser)
{
    //
}

// abstract to a class
// http://stackoverflow.com/questions/3463426/in-c-how-should-i-read-a-text-file-and-print-all-strings



void InterfaceSDCard::read_directory_list(char *dir)
{
    DIR *directory_handle;
    directory_handle = opendir(dir);
    _directory_list.clear();                                                    // clear _directory_list before adding elements
    struct dirent *dirent_struct;                                               // struct dirent *dirent_struct contains pointer on char d_name[]
    while((dirent_struct = readdir(directory_handle)) != NULL) {                // read all directory and file names in current directory into filename vector
        _directory_list.push_back(string(dirent_struct->d_name));
    }
    closedir(directory_handle);
}


bool InterfaceSDCard::readPatch(uint16_t patch_number)
{
    uint16_t sysex_register = 0;
    uint16_t sysex_value = 0;
    int      value;
    uint16_t patch_buffer[256];
    uint8_t  patch_buffer_ptr_read = 0;
    uint8_t  patch_buffer_ptr_write = 0;
    char     file_name[128];
    char     line_buffer[32];
    snprintf(file_name, sizeof(file_name), "/sd/ithaca%04d.pch", patch_number);
    //printf("[d] read file_name: %s\n\r", file_name);
    FILE *fp;
    fp = fopen (file_name, "r");
    if(fp == NULL) {
        printf("[!] could not open file for read\n\r");
        return(false);                                                          // can't read
    } else {
        //printf("[d] file opened for read\n\r");
        while(fgets(line_buffer, sizeof(line_buffer), fp) != NULL) {            // until not EOF
            sscanf(line_buffer, "%d", &value);                                  // make data uint16_t
            //printf("[d] reading %d\n\r", value);
            patch_buffer[patch_buffer_ptr_read] = value;                        // write to buffer
            patch_buffer_ptr_read++;                                            // next buffer slot
        }
        while(patch_buffer_ptr_write < patch_buffer_ptr_read) {
            sysex_register = patch_buffer[patch_buffer_ptr_write];
            patch_buffer_ptr_write++;
            sysex_value = patch_buffer[patch_buffer_ptr_write];
            patch_buffer_ptr_write++;
            if(sysex_register < 128 && sysex_value < 1024) {
                //printf("[d] setting sysex register %d, value %d\n\r", sysex_register, sysex_value);
                _midiParser->set_sysex_register(sysex_register, sysex_value);
            } else {
                //printf("[!] strange value(s) or end of a patch while reading (%d, %d)\n\r", sysex_register, sysex_value);
            }
        }
        fclose(fp);
        //printf("[d] file closed for read\n\r");
        return(true);
    }
}

bool InterfaceSDCard::writePatch(uint16_t patch_number, const uint16_t *patch_data)
{
    uint16_t patch_data_ptr = 0;
    uint16_t sysex_register_nr = 0;
    char     file_name[128];
    snprintf(file_name, sizeof(file_name), "/sd/ithaca%04d.pch", patch_number);
    //printf("[d] write file_name: %s\n\r", file_name);
    FILE *fp;
    fp = fopen(file_name, "w");
    if(fp == NULL) {
        printf("[!] could not open file for write\n\r");
        return(false);                                                          // can't write
    } else {
        //printf("[d] file opened for write\n\r");
        while(patch_data[patch_data_ptr] != 0xffff) {
            // first byte identifies the register from template (ROM patch)
            sysex_register_nr = patch_data[patch_data_ptr];
            //printf("[d] writting %d, %d,\n\r", sysex_register_nr, _midiParser->get_sysex_register(sysex_register_nr));
            fprintf(fp, "%d\n", sysex_register_nr);                             // register number first
            fprintf(fp, "%d\n", _midiParser->get_sysex_register(sysex_register_nr));
            patch_data_ptr++;                                                   // second is a real ROM preset data, ignore
            patch_data_ptr++;                                                   // pointer to next key/value pair
        }
        fprintf(fp, "%d\n", 0xffff);                                            // 0xffff identifies end of data
        fprintf(fp, "%d\n", 0xffff);                                            // twice *key/value
        fclose(fp);                                                             // close io
        //printf("[d] file closed for write\n\r");
        return(true);                                                           // probably ok, return true
    }
}

void InterfaceSDCard::test(void)
{
    /*
    read_directory_list("/sd");
    if(_directory_list.empty()) {
        printf("SDCard Empty or not present...\n\r");
    } else {
        for(vector<string>::iterator it=_directory_list.begin(); it < _directory_list.end(); it++)
        {
            printf("%s\n\r",(*it).c_str());                                     // Note that you must convert to C-style strings - printf cannot do this for you.
        }
    }

    INT to HEX:
    printf("%d",bah[i]); //use %x if u want it in hex
    HEX to INT:
    const char *hexstring = "0xabcdef0";
    int number = (int)strtol(hexstring, NULL, 0);
    */
}
