#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef unsigned char u8;
typedef unsigned short u16;

static char const hex_chars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

enum FSMState {
    Begin, Length, Address, Type, Data, CRC, LF, Error
};

enum HexType {
    DataType = 0x00,
    EOFType,
    SegmentType,
    StartSegmentRecordType,
    ExtendedAddressType,
    StartLinearAddressRecordType
};

void write(u16 address, enum HexType type, u8 *data, u8 data_len) {
    char *text_type;
    switch (type) {
        default:
            text_type = "unknown";
            break;
        case DataType:
            text_type = "DataType";
            break;
        case EOFType:
            text_type = "EOFType";
            break;
        case SegmentType:
            text_type = "SegmentType";
            break;
        case StartSegmentRecordType:
            text_type = "StartSegmentRecordType";
            break;
        case ExtendedAddressType:
            text_type = "ExtendedAddressType";
            break;
        case StartLinearAddressRecordType:
            text_type = "StartLinearAddressRecordType";
            break;
    }
    fprintf(stdout, "Write(0x%04X, %s, %d)\n", address, text_type, data_len);
}

void write_hex(u8 d, u8 buf[2]) {
    buf[0] = hex_chars[(d & 0xF0) >> 4];
    buf[1] = hex_chars[(d & 0x0F) >> 0];
}

static inline u8 half_byte(u8 ch) {
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
    /**Default is BigBox3D firmware */
    char *filename = "marlin_direct_dual.hex";
    FILE *fd;
    enum FSMState state = Begin;
    bool running = true;
    char *error = "unknown";
    u8 length = 0;
    u16 address = 0;
    u8 buf[256];
    u8 num[4];
    u8 sum = 0;
    int lines = 0;
    enum HexType type = DataType;
    int ch;

    if (argc > 1) {
        filename = argv[1];
    }
    fd = fopen(filename, "rt");
    if (0 == fd)
        return 1;
    while (!feof(fd) && running) {
        switch (state) {
            case Begin:
                length = 0;
                address = 0;
                sum = 0;
                type = DataType;
                ch = fgetc(fd);
                if (':' == ch) {
                    ++lines;
                    state = Length;
                } else if (EOF == ch) {
                    running = false;
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
                sum += read_u8_hex(num);
                sum += read_u8_hex(num + 2);
                state = Type;
                break;
            case Type:
                num[0] = fgetc(fd);
                num[1] = fgetc(fd);
                type = (enum HexType) read_u8_hex(num);
                sum += (u8) type;
                state = Data;
                break;
            case Data: {
                int it = 0;
                if (length > 0) {
                    while (it < length && !feof(fd)) {
                        num[0] = fgetc(fd);
                        num[1] = fgetc(fd);
                        buf[it] = read_u8_hex(num);
                        sum += buf[it];
                        ++it;
                    }
                }

                if (it != length) {
                    state = Error;
                    error = "Illegal data length";
                } else {
                    state = CRC;
                }

                break;
            }
            case CRC: {
                u8 crc;
                num[0] = fgetc(fd);
                num[1] = fgetc(fd);
                crc = read_u8_hex(num);
                sum = ~sum;
                sum += 0x01;
                if (crc != sum) {
                    error = "Illegal CRC";
                    fprintf(stderr, "Line %d has illegal CRC\n", lines);
                    state = LF;
                } else {
                    write(address, type, buf, length);
                    state = LF;
                }
                break;
            }
            case LF:
                ch = fgetc(fd);
                if ('\n' == ch) {
                    state = Begin;
                } else if (EOF == ch) {
                    running = false;
                } else {
                    error = "Unknown EOL";
                    state = Error;
                }
                break;
            default:
                error = "unknown state";
            case Error:
                fprintf(stderr, "Error: %s", error);
                running = false;
                break;
        }
    }
    fclose(fd);
    return state == Error ? EXIT_FAILURE : EXIT_SUCCESS;
}
