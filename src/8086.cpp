#include <cstdio>
#include <stdlib.h> 
#include <stdint.h>
#include <string.h>

void addValueToString(char *out, size_t out_sz, const char *templ, int tempValue)
{
    // If the template contains %d, expand it, otherwise just copy it.
    if (strstr(templ, "%d") != NULL) 
    {
        snprintf(out, out_sz, templ, tempValue);
    } 

    else 
    {
        snprintf(out, out_sz, "%s", templ);
    }
}

// first index is the reg field, second index is the w field
const char* REGtable[][2] = {
    {"al", "ax"}, {"cl", "cx"}, {"dl", "dx"}, {"bl", "bx"}, 
    {"ah", "sp"}, {"ch", "bp"}, {"dh", "si"}, {"bh", "di"}  
};

// first index is the R/M field, second index is what mode (MOD) it is
const char* modTable[][3] = {
    {"[bx + si]", "[bx + si + %d]", "[bx + si + %d]"},
    {"[bx + di]", "[bx + di + %d]", "[bx + di + %d]"},
    {"[bp + si]", "[bp + si + %d]", "[bp + si + %d]"},
    {"[bp + di]", "[bp + di + %d]", "[bp + di + %d]"},
    {"[si]", "[si + %d]", "[si + %d]"},
    {"[di]", "[di + %d]", "[di + %d]"},
    {"[%d]", "[bp + %d]", "[bp + %d]"},
    {"[bx]", "[bx + %d]", "[bx + %d]"}
};

int main(int argc, char* argv[])
{

    // Variables
    FILE *file;
    uint8_t* buffer;
    const char* source;
    const char* dest;

    char destBuf[128];
    char srcBuf[128];


    // uint8_t opcode;
    uint8_t d;
    uint8_t w;
    uint8_t mod;
    uint8_t reg;
    uint8_t rm;

    uint8_t highByte;
    uint8_t lowByte;

    uint16_t wordData;

    // immediate to register mask
    uint8_t ItoRMask = (1 << 4) | (1 << 5);
    

    // Open File
    file = fopen(argv[1], "rb");
    if (!file)
    {
        printf("Error Reading File");
        return 1;
    }

    // Determine File Size
    fseek(file, 0, SEEK_END); // seek to end of file
    long fileSizeBytes = ftell(file); // gives size of file in bytes, since ftell returns the current position of the file pointer as offset from bytes from the beginning of the file
    fseek(file, 0, SEEK_SET); // reset seek

    long numValues = fileSizeBytes / sizeof(uint8_t); // 2 because sizeof returns size in bytes, so 2 / 1

    // Allocate Memory
    buffer = (uint8_t*)malloc(numValues * sizeof(uint8_t));

    // Read file contents into buffer
    fread(buffer, sizeof(int8_t), numValues, file);

    for (int i = 0; i < numValues; i++)
    {
        uint8_t v = buffer[i];

        // immediate to register mov
        if (v & ItoRMask)
        {

            w = (v & 0b00001000) >> 3;
            reg = (v & 0b00000111) >> 0;

            // move to next byte
            i++;
            v = buffer[i];

            highByte = v;

            if(w == 1)
            {
                i++;
                v = buffer[i];
                lowByte = v;
                wordData = (lowByte << 8) | highByte;
                dest = REGtable[reg][w];
                printf("mov %s, %d\n", dest, wordData);
            }

            else
            {

                dest = REGtable[reg][w];
                printf("mov %s, %d\n", dest, highByte);
            }
        }

        // register/memory to/from register
        else
        {
            // printf("%b\n", v);
            // opcode = (v & 0b11111100) >> 2;
            d = (v & 0b00000010) >> 1;
            w = (v & 0b00000001) >> 0;

            // move to next byte
            i++;
            v = buffer[i];
            // printf("%b\n", v);

            mod = (v & 0b11000000) >> 6;
            reg = (v & 0b00111000) >> 3;
            rm = (v & 0b00000111) >> 0;
            
            // if D is == 1, then the reg register is destination
            if (d == 1)
            {
                dest = REGtable[reg][w];
                source = REGtable[rm][w];
            }

            // if D is == 0, reg register is the source
            else
            {
                dest = REGtable[rm][w];
                source = REGtable[reg][w];
            }

            if (mod == 3)
            {

                printf("mov %s, %s\n", dest, source);
            }

            else if (mod == 0)
            {
                if (d == 1)
                {
                    source = modTable[rm][mod];
                }

                else
                {
                    dest = modTable[rm][mod];
                }
                printf("mov %s, %s\n", dest, source);
            }

            else if (mod == 1)
            {
                if (d == 1)
                {
                    source = modTable[rm][mod];
                }

                else
                {
                    dest = modTable[rm][mod];
                }

                // change byte
                i++;
                v = buffer[i];
                lowByte = v;

                addValueToString(destBuf, sizeof destBuf, dest, lowByte);
                addValueToString(srcBuf, sizeof srcBuf, source, lowByte);
                printf("mov %s, %s\n", destBuf, srcBuf);
            }

            else if (mod == 2)
            {
                if (d == 1)
                {
                    source = modTable[rm][mod];
                }

                else
                {
                    dest = modTable[rm][mod];
                }

                // change byte (disp-lo)
                i++;
                v = buffer[i];
                lowByte = v;

                // change byte again (disp-hi)
                i++;
                v = buffer[i];
                highByte = v;

                wordData = (highByte << 8) | lowByte;

                addValueToString(destBuf, sizeof destBuf, dest, wordData);
                addValueToString(srcBuf, sizeof srcBuf, source, wordData);
                printf("mov %s, %s\n", destBuf, srcBuf);
            }
            
            
   
        }
    }

    fclose(file);
    free(buffer);
}