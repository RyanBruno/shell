#define ENSURE(ptr, msg, ...) \
    if (ptr == NULL) {\
        fprintf(stderr, msg, __VA_ARGS__); \
        exit(-1); }\

struct gbuf_t {
    size_t gb_s;
    size_t gb_c;
    char* gb_data;
};

void gbuf_setup(struct gbuf_t *gb, size_t c)
{
    gb->gb_s = 0;
    gb->gb_c = c;

    ENSURE((gb->gb_data = malloc(c)),
            "Cannot malloc (%d)\n", errno);
}

void gbuf_push(struct gbuf_t* gb, const char* item, size_t s)
{
    while (gb->gb_s + s > gb->gb_c)
        ENSURE((gb->gb_data = realloc(gb->gb_data, gb->gb_c *= 2)),
                "Cannot realloc (%d)\n", errno)

    memcpy(gb->gb_data + gb->gb_s, item, s);
    gb->gb_s += s;
}

char* gbuf_push_2(struct gbuf_t* gb, const char* item, size_t s)
{
    while (gb->gb_s + s > gb->gb_c)
        ENSURE((gb->gb_data = realloc(gb->gb_data, gb->gb_c *= 2)),
                "Cannot realloc (%d)\n", errno)

    memcpy(gb->gb_data + gb->gb_s, item, s);
    gb->gb_s += s;

    return (gb->gb_data + gb->gb_s) - s;
}
