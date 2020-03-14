#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

static char const hex_chars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

enum FSM_State {
    Begin, Length, Address, Type, Data, CRC, LF, Error
};

void write(u16 address, u8 type, u8 *data, u8 data_len) {

}

void write_hex(u8 d, u8 buf[2]) {
    buf[0] = hex_chars[(d & 0xF0) >> 4];
    buf[1] = hex_chars[(d & 0x0F) >> 0];
}

static inline u8 half_byte(char ch) {
    if (ch >= '0' && ch <= '9') ch = ch - '0';
    else if (ch >= 'a' && ch <= 'f') ch = ch - 'a' + 10;
    else if (ch >= 'A' && ch <= 'F') ch = ch - 'A' + 10;
    return ch;
}

u8 read_u8_hex(u8 buf[2]) {
    u8 value = 0;
    value = (value << 4) | (half_byte(buf[0]) & 0xF);
    value = (value << 4) | (half_byte(buf[1]) & 0xF);
    return value;
}

u16 read_u16_hex(u8 buf[4]) {
    u16 value = 0;
    value = (value << 4) | (half_byte(buf[0]) & 0xF);
    value = (value << 4) | (half_byte(buf[1]) & 0xF);
    value = (value << 4) | (half_byte(buf[2]) & 0xF);
    value = (value << 4) | (half_byte(buf[3]) & 0xF);
    return value;
}

int main(int argc, char **argv) {
    char *filename = "example.hex";
    FILE *fd;
    enum FSM_State state = Begin;
    bool running = true;
    char *error = "unknown";
    u8 length = 0;
    u16 address = 0;
    u8 buf[256];
    u8 num[4];
    u8 sum = 0;
    u8 type = 0;
    int ch;

    if (argc > 1) {
        filename = argv[1];
    }
    fd = fopen(filename, "rb");
    if (0 == fd)
        return 1;
    while (!feof(fd) && running) {
        switch (state) {
            case Begin:
                ch = fgetc(fd);
                if (':' == ch) {
                    state = Length;
                } else {
                    error = "Begin not found";
                    state = Error;
                }
                break;
            case Length:
                num[0] = fgetc(fd);
                num[1] = fgetc(fd);
                if (feof(fd)) {
                    error = "Unexpected end of file";
                    state = Error;
                } else {
                    length = read_u8_hex(num);
                    state = Address;
                    sum += length;
                }
                break;
            case Address:
                num[0] = fgetc(fd);
                num[1] = fgetc(fd);
                num[2] = fgetc(fd);
                num[3] = fgetc(fd);
                address = read_u16_hex(num);
                sum += address;
                state = Type;
                break;
            case Type:
                num[0] = fgetc(fd);
                num[1] = fgetc(fd);
                type = read_u8_hex(num);
                state = Data;
                break;
            case Data:
                if (length > 0) {
                    int it = 0;
                    while (it < length) {
                        num[0] = fgetc(fd);
                        num[1] = fgetc(fd);
                        buf[it] = read_u8_hex(num);
                        sum += buf[it];
                        ++it;
                    }
                }
                state = CRC;
                break;
            case CRC: {
                u8 crc;
                num[0] = fgetc(fd);
                num[1] = fgetc(fd);
                crc = read_u8_hex(num);
                if (crc + sum != 0) {
                    error = "Illegal CRC";
                    state = Error;
                } else {
                    write(address, type, buf, length);
                    state = LF;
                }
                break;
            }
            case LF:
                ch = fgetc(fd);
                state = Begin;
                break;
            case Error:
                fprintf(stderr, "Error: %s", error);
                running = false;
                break;
        }
    }
    fclose(fd);
    return EXIT_SUCCESS;
}
