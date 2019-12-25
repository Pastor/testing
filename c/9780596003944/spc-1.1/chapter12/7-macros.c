#define SPLIT_VAR(in, a, b, c, d) do { \
          (a) = (char)((in) >> 24);    \
          (b) = (char)((in) >> 16);    \
          (c) = (char)((in) >> 8);     \
          (d) = (char)((in) & 0xFF);   \
        } while (0)

#define REBUILD_VAR(a, b, c, d) \
        ((((a) << 24) & 0xFF000000) | (((b) << 16) & 0x00FF0000) | \
        (((c) << 8)  & 0x0000FF00) | ((d) & 0xFF))
