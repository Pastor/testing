#define FONT_WIDTH 8

const unsigned char font[] =
{
        0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //  
        0x02, 0x5F, 0x5F, 0x00, 0x00, 0x00, 0x00, 0x00,  // !
        0x06, 0x07, 0x07, 0x00, 0x00, 0x07, 0x07, 0x00,  // "
        0x07, 0x14, 0x7F, 0x7F, 0x14, 0x7F, 0x7F, 0x14,  // #
        0x06, 0x24, 0x2E, 0x6B, 0x6B, 0x3A, 0x12, 0x00,  // $
        0x06, 0x63, 0x73, 0x18, 0x0C, 0x67, 0x63, 0x00,  // %
        0x07, 0x32, 0x7F, 0x4D, 0x4D, 0x77, 0x72, 0x50,  // &
        0x04, 0x06, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00,  // '
        0x04, 0x1C, 0x3E, 0x63, 0x41, 0x00, 0x00, 0x00,  // (
        0x04, 0x41, 0x63, 0x3E, 0x1C, 0x00, 0x00, 0x00,  // )
        0x07, 0x08, 0x2A, 0x3E, 0x1C, 0x3E, 0x2A, 0x08,  // *
        0x06, 0x08, 0x08, 0x3E, 0x3E, 0x08, 0x08, 0x00,  // +
        0x03, 0x80, 0xE0, 0x60, 0x00, 0x00, 0x00, 0x00,  // ,
        0x06, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00,  // -
        0x02, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00,  // .
        0x07, 0x40, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x02,  // /
        0x06, 0x3E, 0x7F, 0x49, 0x45, 0x7F, 0x3E, 0x00,  // 0
        0x06, 0x40, 0x44, 0x7F, 0x7F, 0x40, 0x40, 0x00,  // 1
        0x06, 0x62, 0x73, 0x59, 0x49, 0x4F, 0x46, 0x00,  // 2
        0x06, 0x22, 0x63, 0x49, 0x49, 0x7F, 0x36, 0x00,  // 3
        0x07, 0x18, 0x1C, 0x16, 0x13, 0x7F, 0x7F, 0x10,  // 4
        0x06, 0x27, 0x67, 0x45, 0x45, 0x7D, 0x39, 0x00,  // 5
        0x06, 0x3E, 0x7F, 0x49, 0x49, 0x7B, 0x32, 0x00,  // 6
        0x06, 0x01, 0x01, 0x79, 0x7D, 0x07, 0x03, 0x00,  // 7
        0x06, 0x36, 0x7F, 0x49, 0x49, 0x7F, 0x36, 0x00,  // 8
        0x06, 0x26, 0x6F, 0x49, 0x49, 0x7F, 0x3E, 0x00,  // 9
        0x02, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00,  // :
        0x03, 0x80, 0xC4, 0x44, 0x00, 0x00, 0x00, 0x00,  // ;
        0x06, 0x10, 0x30, 0x7F, 0x7F, 0x30, 0x10, 0x00,  // < min
        0x05, 0x00, 0x7E, 0x43, 0x43, 0x7E, 0x00, 0x00,  // = batt
        0x06, 0x04, 0x06, 0x7F, 0x7F, 0x06, 0x04, 0x00,  // > max
        0x06, 0x02, 0x03, 0x51, 0x59, 0x0F, 0x06, 0x00,  // ?
        0x06, 0x3E, 0x7F, 0x41, 0x4D, 0x6F, 0x2E, 0x00,  // @
        0x06, 0x7C, 0x7E, 0x13, 0x13, 0x7E, 0x7C, 0x00,  // A
        0x06, 0x7F, 0x7F, 0x49, 0x49, 0x7F, 0x36, 0x00,  // B
        0x06, 0x3E, 0x7F, 0x41, 0x41, 0x63, 0x22, 0x00,  // C
        0x06, 0x7F, 0x7F, 0x41, 0x41, 0x7F, 0x3E, 0x00,  // D
        0x06, 0x7F, 0x7F, 0x49, 0x49, 0x41, 0x41, 0x00,  // E
        0x06, 0x7F, 0x7F, 0x09, 0x09, 0x01, 0x01, 0x00,  // F
        0x06, 0x3E, 0x7F, 0x41, 0x49, 0x7B, 0x3A, 0x00,  // G
        0x06, 0x7F, 0x7F, 0x08, 0x08, 0x7F, 0x7F, 0x00,  // H
        0x04, 0x41, 0x7F, 0x7F, 0x41, 0x00, 0x00, 0x00,  // I
        0x06, 0x20, 0x60, 0x41, 0x7F, 0x3F, 0x01, 0x00,  // J
        0x06, 0x7F, 0x7F, 0x1C, 0x36, 0x63, 0x41, 0x00,  // K
        0x06, 0x7F, 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00,  // L
        0x07, 0x7F, 0x7F, 0x06, 0x0C, 0x06, 0x7F, 0x7F,  // M
        0x06, 0x7F, 0x7F, 0x0C, 0x18, 0x7F, 0x7F, 0x00,  // N
        0x06, 0x3E, 0x7F, 0x41, 0x41, 0x7F, 0x3E, 0x00,  // O
        0x06, 0x7F, 0x7F, 0x09, 0x09, 0x0F, 0x06, 0x00,  // P
        0x06, 0x1E, 0x3F, 0x21, 0x61, 0x7F, 0x5E, 0x00,  // Q
        0x06, 0x7F, 0x7F, 0x19, 0x39, 0x6F, 0x46, 0x00,  // R
        0x06, 0x26, 0x6F, 0x49, 0x49, 0x7B, 0x32, 0x00,  // S
        0x06, 0x01, 0x01, 0x7F, 0x7F, 0x01, 0x01, 0x00,  // T
        0x06, 0x3F, 0x7F, 0x40, 0x40, 0x7F, 0x3F, 0x00,  // U
        0x06, 0x1F, 0x3F, 0x60, 0x60, 0x3F, 0x1F, 0x00,  // V
        0x07, 0x7F, 0x7F, 0x30, 0x18, 0x30, 0x7F, 0x7F,  // W
        0x06, 0x41, 0x63, 0x3E, 0x3E, 0x63, 0x41, 0x00,  // X
        0x06, 0x07, 0x0F, 0x78, 0x78, 0x0F, 0x07, 0x00,  // Y
        0x06, 0x61, 0x71, 0x59, 0x4D, 0x47, 0x43, 0x00,  // Z
        0x04, 0x7F, 0x7F, 0x41, 0x41, 0x00, 0x00, 0x00,  // [
        0x07, 0x02, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40,  // BackSlash
        0x04, 0x41, 0x41, 0x7F, 0x7F, 0x00, 0x00, 0x00,  // ]
        0x06, 0x08, 0x0C, 0xFE, 0xFE, 0x0C, 0x08, 0x00,  // ^
        0x07, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,  // _
        0x07, 0x1C, 0x3E, 0x63, 0x41, 0x63, 0x14, 0x14,  // `
        0x06, 0x20, 0x74, 0x54, 0x54, 0x7C, 0x78, 0x00,  // a
        0x06, 0x7F, 0x7F, 0x44, 0x44, 0x7C, 0x38, 0x00,  // b
        0x05, 0x38, 0x7C, 0x44, 0x44, 0x44, 0x00, 0x00,  // c
        0x06, 0x38, 0x7C, 0x44, 0x44, 0x7F, 0x7F, 0x00,  // d
        0x06, 0x38, 0x7C, 0x54, 0x54, 0x5C, 0x18, 0x00,  // e
        0x05, 0x08, 0x7E, 0x7F, 0x09, 0x09, 0x00, 0x00,  // f
        0x06, 0x98, 0xBC, 0xA4, 0xA4, 0xFC, 0x7C, 0x00,  // g
        0x06, 0x7F, 0x7F, 0x04, 0x04, 0x7C, 0x78, 0x00,  // h
        0x04, 0x44, 0x7D, 0x7D, 0x40, 0x00, 0x00, 0x00,  // i
        0x05, 0x80, 0x80, 0x80, 0xFD, 0x7D, 0x00, 0x00,  // j
        0x06, 0x7F, 0x7F, 0x10, 0x38, 0x6C, 0x44, 0x00,  // k
        0x04, 0x41, 0x7F, 0x7F, 0x40, 0x00, 0x00, 0x00,  // l
        0x07, 0x78, 0x7C, 0x0C, 0x38, 0x0C, 0x7C, 0x78,  // m
        0x06, 0x7C, 0x7C, 0x04, 0x04, 0x7C, 0x78, 0x00,  // n
        0x06, 0x38, 0x7C, 0x44, 0x44, 0x7C, 0x38, 0x00,  // o
        0x06, 0xFC, 0xFC, 0x24, 0x24, 0x3C, 0x18, 0x00,  // p
        0x06, 0x18, 0x3C, 0x24, 0x24, 0xFC, 0xFC, 0x00,  // q
        0x06, 0x7C, 0x7C, 0x04, 0x04, 0x0C, 0x08, 0x00,  // r
        0x06, 0x48, 0x5C, 0x54, 0x54, 0x74, 0x24, 0x00,  // s
        0x05, 0x04, 0x3F, 0x7F, 0x44, 0x44, 0x00, 0x00,  // t
        0x06, 0x3C, 0x7C, 0x40, 0x40, 0x7C, 0x7C, 0x00,  // u
        0x06, 0x1C, 0x3C, 0x60, 0x60, 0x3C, 0x1C, 0x00,  // v
        0x07, 0x1C, 0x7C, 0x60, 0x38, 0x60, 0x7C, 0x1C,  // w
        0x06, 0x44, 0x6C, 0x38, 0x38, 0x6C, 0x44, 0x00,  // x
        0x06, 0x9C, 0xBC, 0xA0, 0xE0, 0x7C, 0x3C, 0x00,  // y
        0x06, 0x44, 0x64, 0x74, 0x5C, 0x4C, 0x44, 0x00,  // z
        0x05, 0x08, 0x3E, 0x7F, 0x41, 0x41, 0x00, 0x00,  // {
        0x02, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,  // |
        0x05, 0x41, 0x41, 0x7F, 0x3E, 0x08, 0x00, 0x00,  // }
        0x05, 0x10, 0x18, 0x18, 0x18, 0x08, 0x00, 0x00,  // ~
        0x02, 0x7E, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00,   // 

        0x07, 0x7C, 0x7E, 0x13, 0x13, 0x7E, 0xFC, 0x80,  // Ą
        0x06, 0x3C, 0x7E, 0x46, 0x43, 0x66, 0x24, 0x00,  // !
        0x07, 0x7F, 0x7F, 0x49, 0x49, 0x41, 0xC1, 0x80,  // "
        0x06, 0x7F, 0x7F, 0x48, 0x44, 0x40, 0x40, 0x00,  // #
        0x06, 0x7E, 0x7E, 0x0C, 0x19, 0x7E, 0x7E, 0x00,  // $
        0x06, 0x3C, 0x7E, 0x46, 0x43, 0x7E, 0x3C, 0x00,  // %
        0x06, 0x24, 0x6E, 0x4A, 0x4E, 0x7B, 0x30, 0x00,  // &
        0x06, 0x62, 0x76, 0x5A, 0x4F, 0x46, 0x42, 0x00,  // '
        0x06, 0x69, 0x79, 0x59, 0x4D, 0x4F, 0x4B, 0x00,  // (

        0x07, 0x20, 0x74, 0x54, 0x54, 0x7C, 0xF8, 0x80,  // ą
        0x05, 0x38, 0x7C, 0x44, 0x46, 0x45, 0x00, 0x00,  // *
        0x06, 0x38, 0x7C, 0x54, 0x54, 0xDC, 0x98, 0x00,  // +
        0x04, 0x51, 0x7F, 0x7F, 0x44, 0x00, 0x00, 0x00,  // ,
        0x06, 0x7C, 0x7C, 0x04, 0x06, 0x7D, 0x78, 0x00,  // -
        0x06, 0x38, 0x7C, 0x44, 0x46, 0x7D, 0x38, 0x00,  // .
        0x06, 0x48, 0x5C, 0x54, 0x56, 0x75, 0x24, 0x00,  // /
        0x06, 0x44, 0x64, 0x76, 0x5D, 0x4C, 0x44, 0x00,  // 0
        0x06, 0x44, 0x64, 0x75, 0x5D, 0x4C, 0x44, 0x00,  // 1
};

