#include <cstdio>
#include <stdlib.h> 
#include <stdint.h>

const char* table[][2] = {
    {"al", "ax"}, {"cl", "cx"}, {"dl", "dx"}, {"bl", "bx"}, // w = 0
    {"ah", "sp"}, {"ch", "bp"}, {"dh", "si"}, {"bh", "di"}  // w = 1
};

const uint8_t MOV_OPCODE = 0x22;


int main(int argc, char* argv[])
{

    // Variables
    FILE *file;
    uint16_t* buffer;
    const char* source;
    const char* dest;

    // Open File
    file = fopen(argv[1], "rb");
    if (!file)
    {
        printf("Error Reading File");
        return 1;
    }

    // Determine File Size
    fseek(file, 0, SEEK_END);
    long fileSizeBytes = ftell(file);
    fseek(file, 0, SEEK_SET);

    long numValues = fileSizeBytes / sizeof(uint16_t);

    // Allocate Memory
    buffer = (uint16_t*)malloc(numValues * sizeof(uint16_t));

    // Read file contents into buffer
    fread(buffer, sizeof(int16_t), numValues, file);

    for (int i = 0; i < numValues; i++)
    {

        uint16_t v = buffer[i];
        uint16_t fixed = (v >> 8) | (v << 8);

        // high byte first
        uint8_t high = (fixed >> 8) & 0xFF; // top 8 bits
        uint8_t low = fixed & 0xFF;

        uint8_t opcode = (high & 0b11111100) >> 2;
        uint8_t d = (high & 0b00000010) >> 1;
        uint8_t w = (high & 0b00000001) >> 0;
        // uint8_t mod = (low & 0b11000000) >> 6; // 11 for our purposes
        uint8_t reg = (low & 0b00111000) >> 3;
        uint8_t rm = (low & 0b00000111) >> 0;


        // MOV opcode check
        if (opcode != MOV_OPCODE)
        {
            printf("Error reading Opcode");
            return 1;   
        }

        // if D is == 1, then the reg register is destination
        if (d == 1)
        {
            dest = table[reg][w];
            source = table[rm][w];
        }

        // if D is == 0, reg register is the source
        else
        {
            dest = table[rm][w];
            source = table[reg][w];
        }

        printf("mov %s, %s\n", dest, source);
    }

    fclose(file);
    free(buffer);

}