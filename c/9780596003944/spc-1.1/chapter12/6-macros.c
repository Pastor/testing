/* x and y are chars, returns a short */
/* x is in position 0, y is in position 1 */
#define MERGE_CHAR(x, y)    (((y) << 8 ) | (x))

/* s is a short and c is position -- 0 or 1 */
#define GET_CHAR(s, c)      (char)(((s) >> (8 * (c))) & 0x00FF)

/* s is a short, c is a position, and val is a char value */
#define SET_CHAR(s, c, val) (((s) & (0xFF00 >> (8 * (c)))) | ((val) << (8 * (c))))

/* x and y are shorts.  returns an int */
/* x is in position 0, y is in position 1 */
#define MERGE_SHORT(x, y)   (((y) << 16 ) | (x))

/* i is an int and s is position -- 0 or 1 */
#define GET_SHORT(i, s)     (short)(((i) >> (16 * (s))) & 0x0FFFF)

/* i is an int, s is position, and val is a short value */
#define SET_SHORT(i, s, val) (((i) & (0xFFFF0000 >> (16 * (s)))) | \
                             ((val) << (16 * (s))))
