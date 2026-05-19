#include "rudra512.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Constants */
static const uint64_t INIT_STATE[8] = {
    0xcbbb9d5dc1059ed8ULL, 0x629a292a367cd507ULL,
    0x9159015a3070dd17ULL, 0x152fecd8f70e5939ULL,
    0x67332667ffc00b31ULL, 0x8eb44a8768581511ULL,
    0xdb0c2e0d64f98fa7ULL, 0x47b5481dbefa4fa4ULL
};

static const int ROT[8] = { 7, 11, 13, 19, 29, 37, 41, 47 };

static const int ROUND_ROT[63] = {
    58, 17, 44,  3, 61, 28, 52,  9,
    36, 20, 47,  6, 39, 15, 62, 25,
    54, 11, 42,  1, 33, 57, 22, 49,
    13, 37,  4, 59, 18, 45,  8, 31,
    55, 24, 51, 10, 43,  2, 60, 27,
    50, 16, 38,  5, 29, 56, 21, 48,
    12, 35, 63, 23, 46,  7, 40, 26,
    53, 19, 30, 41, 14, 34, 32
};

static const uint64_t DOM_STRING = 0x243f6a8885a308d3ULL;
static const uint64_t DOM_FILE   = 0x517cc1b727220a95ULL;
static const uint64_t DOM_SALT   = 0xb5470917a2388f00ULL;
static const uint64_t DOM_ROUNDS = 0x428a2f98d728ae22ULL;
static const uint64_t DOM_FINAL  = 0xbe5466cf34e90c6cULL;
static const uint64_t DOM_FEIST  = 0x6c62272e07bb0142ULL;
static const uint64_t DOM_TOKEN  = 0xf39cc0605cedc834ULL;
static const uint64_t GOLDEN     = 0x9e3779b97f4a7c15ULL;

static const uint64_t SIP_C0 = 0x736f6d6570736575ULL;
static const uint64_t SIP_C1 = 0x646f72616e646f6dULL;
static const uint64_t SIP_C2 = 0x6c7967656e657261ULL;
static const uint64_t SIP_C3 = 0x7465646279746573ULL;

static const size_t MAX_SALT_BYTES = 4096;

static const uint64_t WK[8] = {
    0xa54ff53a5f1d36f1ULL, 0x510e527fade682d1ULL,
    0x9b05688c2b3e6c1fULL, 0x1f83d9abfb41bd6bULL,
    0x5be0cd19137e2179ULL, 0x7137449123ef65cdULL,
    0x367cd5071059ed8bULL, 0xcbbb9d5dc105a7d0ULL
};

/* Internal types */
typedef struct {
    uint64_t s[8];
} State8;

/* Primitives */
static inline uint64_t rotl64(uint64_t x, int r) {
    return (x << r) | (x >> (64 - r));
}

static inline uint64_t load_le64(const uint8_t *p) {
    uint64_t w = 0;
    memcpy(&w, p, 8);
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    w = ((w & 0x00000000000000FFULL) << 56) |
        ((w & 0x000000000000FF00ULL) << 40) |
        ((w & 0x0000000000FF0000ULL) << 24) |
        ((w & 0x00000000FF000000ULL) <<  8) |
        ((w & 0x000000FF00000000ULL) >>  8) |
        ((w & 0x0000FF0000000000ULL) >> 24) |
        ((w & 0x00FF000000000000ULL) >> 40) |
        ((w & 0xFF00000000000000ULL) >> 56);
#endif
    return w;
}

static inline void sip4_compress(uint64_t *v0, uint64_t *v1, uint64_t *v2, uint64_t *v3) {
    *v0 += *v1; *v1 = rotl64(*v1, 13); *v1 ^= *v0; *v0 = rotl64(*v0, 32);
    *v2 += *v3; *v3 = rotl64(*v3, 16); *v3 ^= *v2;
    *v0 += *v3; *v3 = rotl64(*v3, 21); *v3 ^= *v0;
    *v2 += *v1; *v1 = rotl64(*v1, 17); *v1 ^= *v2; *v2 = rotl64(*v2, 32);
}

static inline void sip_absorb(uint64_t *v0, uint64_t *v1, uint64_t *v2, uint64_t *v3, uint64_t m) {
    *v3 ^= m;
    sip4_compress(v0, v1, v2, v3);
    sip4_compress(v0, v1, v2, v3);
    *v0 ^= m;
}

static inline uint64_t sip_finalize(uint64_t v0, uint64_t v1, uint64_t v2, uint64_t v3) {
    v2 ^= 0xffULL;
    sip4_compress(&v0, &v1, &v2, &v3);
    sip4_compress(&v0, &v1, &v2, &v3);
    sip4_compress(&v0, &v1, &v2, &v3);
    sip4_compress(&v0, &v1, &v2, &v3);
    return v0 ^ v1 ^ v2 ^ v3;
}

static inline void make_sip_state(uint64_t k0, uint64_t k1, uint64_t k2, uint64_t k3, uint64_t *v0, uint64_t *v1, uint64_t *v2, uint64_t *v3) {
    *v0 = k0 ^ SIP_C0;
    *v1 = k1 ^ SIP_C1;
    *v2 = k2 ^ SIP_C2;
    *v3 = k3 ^ SIP_C3;
}

static inline void arx_round_func(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t *d) {
    *a += *b; *d ^= *a; *d = rotl64(*d, 32);
    *c += *d; *b ^= *c; *b = rotl64(*b, 24);
    *a += *b; *d ^= *a; *d = rotl64(*d, 16);
    *c += *d; *b ^= *c; *b = rotl64(*b, 63);
}

static uint64_t strong_hash(const uint8_t *data, size_t len, uint64_t seed) {
    uint64_t v0 = seed ^ 0x243f6a8885a308d3ULL;
    uint64_t v1 = seed ^ 0x13198a2e03707344ULL;
    uint64_t v2 = seed ^ 0xa4093822299f31d0ULL;
    uint64_t v3 = seed ^ 0x082efa98ec4e6c89ULL;

    size_t offset = 0;
    while (offset + 8 <= len) {
        uint64_t word = load_le64(data + offset);
        v3 ^= word;
        arx_round_func(&v0, &v1, &v2, &v3);
        v0 ^= word;
        offset += 8;
    }

    uint64_t tail = (uint64_t)len << 56;
    for (size_t i = 0; i < len - offset; ++i)
        tail |= (uint64_t)data[offset + i] << (i * 8);
    v3 ^= tail;
    arx_round_func(&v0, &v1, &v2, &v3);
    v0 ^= tail;

    for (int i = 0; i < 10; ++i)
        arx_round_func(&v0, &v1, &v2, &v3);

    return v0 ^ v1 ^ v2 ^ v3;
}

static void expand_salt(const char *salt, uint64_t out_key[8]) {
    if (!salt || salt[0] == '\0') {
        for (int i = 0; i < 8; ++i) {
            out_key[i] = strong_hash((const uint8_t *)&DOM_SALT, 8,
                DOM_SALT ^ (GOLDEN * (uint64_t)(i + 1)) ^ rotl64(DOM_TOKEN, (i * 11 + 3) % 63 + 1));
        }
        return;
    }

    size_t salt_len = strlen(salt);
    if (salt_len > MAX_SALT_BYTES) salt_len = MAX_SALT_BYTES;

    uint8_t buf[MAX_SALT_BYTES + 4];
    memcpy(buf, salt, salt_len);

    for (int i = 0; i < 8; ++i) {
        buf[salt_len + 0] = (uint8_t)(i & 0xFF);
        buf[salt_len + 1] = (uint8_t)((i >> 8) & 0xFF);
        buf[salt_len + 2] = (uint8_t)((i >> 16) & 0xFF);
        buf[salt_len + 3] = (uint8_t)((i >> 24) & 0xFF);

        uint64_t seed = DOM_SALT ^ rotl64(GOLDEN, (i * 19 + 7) % 63 + 1) ^ (DOM_TOKEN * (uint64_t)(i * i + 1));
        out_key[i] = strong_hash(buf, salt_len + 4, seed);
    }
}

static inline int round_rot_func(int word_index, int round_index) {
    int table_idx = (word_index * 17 + round_index * 31 + (round_index >> 3) * 7) % 63;
    return ROUND_ROT[table_idx];
}

static void make_round_schedule(const uint64_t sk[8], int rounds, uint64_t *sched) {
    uint8_t sk_bytes[64];
    for (int word = 0; word < 8; ++word) {
        for (int byte = 0; byte < 8; ++byte)
            sk_bytes[word * 8 + byte] = (uint8_t)((sk[word] >> (byte * 8)) & 0xFF);
    }

    uint64_t prf[4] = {
        strong_hash(sk_bytes, 32, GOLDEN ^ DOM_ROUNDS),
        strong_hash(sk_bytes + 32, 32, GOLDEN ^ DOM_SALT),
        strong_hash(sk_bytes, 64, GOLDEN ^ DOM_TOKEN),
        strong_hash(sk_bytes + 16, 32, GOLDEN ^ DOM_FEIST)
    };

    uint64_t sv0, sv1, sv2, sv3;
    make_sip_state(prf[0], prf[1], prf[2], prf[3], &sv0, &sv1, &sv2, &sv3);

    for (int r = 0; r < rounds; ++r) {
        uint64_t av0 = sv0, av1 = sv1, av2 = sv2, av3 = sv3;
        uint64_t counter = (uint64_t)r ^ (sk[r % 8] * (uint64_t)(r + 1));
        sip_absorb(&av0, &av1, &av2, &av3, counter);
        uint64_t h = sip_finalize(av0, av1, av2, av3);
        h ^= rotl64(sk[(r + 3) % 8], (r * 13 + 7) % 63 + 1);
        sched[r] = h;
    }
}

static void permute(uint64_t s[8], int rounds, const uint64_t *sched) {
    for (int r = 0; r < rounds; ++r) {
        int off_a = (r % 7) + 1;
        int off_b = ((r + 2) % 7) + 1;
        int off_c = ((r + 4) % 7) + 1;

        uint64_t rk = sched[r];
        uint64_t old[8];
        memcpy(old, s, 64);
        s[0] ^= rk;
        uint64_t keyed[8];
        memcpy(keyed, s, 64);

        for (int w = 0; w < 8; ++w) {
            int rot_amt = round_rot_func(w, r);
            uint64_t v = keyed[w] ^ rotl64(old[(w + off_a) % 8], ROT[w]);
            v += old[(w + off_b) % 8];
            v = rotl64(v, rot_amt);
            v ^= rotl64(old[(w + 5) % 8], 17);
            s[w] = v;
        }

        uint64_t mid[8];
        memcpy(mid, s, 64);
        for (int w = 0; w < 8; ++w) {
            s[w] += rotl64(mid[(w + off_c) % 8], ROT[(w + off_c) % 8]);
        }
        s[r % 8] ^= rotl64(rk, 32);
    }
}

static void whiten_state(uint64_t s[8], const uint64_t sk[8], const uint64_t *sched, int rounds) {
    for (int i = 0; i < 8; ++i) s[i] ^= WK[i] ^ sk[i];
    int wr = (4 > (4 + rounds / 16)) ? 4 : (4 + rounds / 16);
    if (wr > rounds) wr = rounds;
    if (wr < 4) wr = 4;
    permute(s, wr, sched);
}

static void absorb_block(uint64_t s[8], const uint8_t *block, uint64_t block_idx, const uint64_t sk[8], const uint64_t *sched, int rounds) {
    uint64_t obf_idx = block_idx ^ sk[0] ^ rotl64(sk[1], 32) ^ ((uint64_t)rounds * DOM_ROUNDS);
    for (int w = 0; w < 8; ++w) {
        uint64_t iw = load_le64(block + w * 8);
        uint64_t tweak = rotl64(obf_idx + (uint64_t)w + s[w] + sk[w], ROT[w % 8]);
        s[w] ^= iw ^ tweak;
    }
    uint64_t before[8];
    memcpy(before, s, 64);
    permute(s, rounds, sched);
    for (int i = 0; i < 8; ++i) s[i] ^= before[i];
}

static void make_init_state_func(uint64_t s[8], int rounds, const uint64_t sk[8], const uint64_t *sched, uint64_t domain) {
    memcpy(s, INIT_STATE, 64);
    uint64_t rt = (uint64_t)rounds * DOM_ROUNDS;
    for (int w = 0; w < 8; ++w) {
        s[w] ^= sk[w];
        s[w] ^= rotl64(rt, ROT[w]);
        s[w] ^= rotl64(domain, (w * 13 + 7) % 63 + 1);
    }
    int ir = (4 > rounds / 8) ? 4 : rounds / 8;
    permute(s, ir, sched);
}

static void feistel_round_func(uint8_t *left, size_t left_len, const uint8_t *right, size_t right_len, uint64_t rk) {
    uint64_t v0, v1, v2, v3;
    make_sip_state(rk, rk, rk, rk, &v0, &v1, &v2, &v3);
    size_t fw = right_len / 8;
    for (size_t i = 0; i < fw; ++i) sip_absorb(&v0, &v1, &v2, &v3, load_le64(right + i * 8));
    if (right_len % 8 != 0) {
        size_t rem = right_len % 8;
        uint64_t tail = (uint64_t)(right_len & 0xFF) << 56;
        for (size_t i = 0; i < rem; ++i) tail |= (uint64_t)right[fw * 8 + i] << (i * 8);
        sip_absorb(&v0, &v1, &v2, &v3, tail);
    }
    size_t pos = 0;
    uint64_t ctr = 0;
    while (pos < left_len) {
        uint64_t sv0 = v0, sv1 = v1, sv2 = v2, sv3 = v3;
        sip_absorb(&sv0, &sv1, &sv2, &sv3, ctr++);
        uint64_t ks = sip_finalize(sv0, sv1, sv2, sv3);
        size_t take = (8 < (left_len - pos)) ? 8 : (left_len - pos);
        for (size_t i = 0; i < take; ++i, ++pos)
            left[pos] ^= (uint8_t)((ks >> (i * 8)) & 0xFF);
    }
}

static uint8_t* feistel_whiten(const uint8_t *input, size_t len, const uint64_t sk[8], int rounds, size_t *out_len) {
    *out_len = len;
    if (len == 0) return NULL;
    uint8_t *out = (uint8_t*)malloc(len);
    memcpy(out, input, len);
    if (len < 2) {
        out[0] ^= (uint8_t)(sk[0] & 0xFF);
        return out;
    }
    int frs = (16 < (rounds / 4)) ? 16 : (rounds / 4);
    if (frs < 8) frs = 8;
    size_t half = len / 2;
    for (int i = 0; i < frs; ++i) {
        uint64_t rk = sk[i % 8] ^ ((uint64_t)i * GOLDEN) ^ rotl64(sk[(i + 1) % 8], (i * 13 + 7) % 63 + 1);
        uint8_t *l = out;
        uint8_t *r = out + half;
        size_t ll = half;
        size_t rl = len - half;
        if (i % 2 == 1) {
            uint8_t *tmp_p = l; l = r; r = tmp_p;
            size_t tmp_l = ll; ll = rl; rl = tmp_l;
        }
        feistel_round_func(l, ll, r, rl, rk);
    }
    return out;
}

static uint8_t* tokens_to_bits(const uint8_t *ids, size_t n_ids, size_t *out_len) {
    size_t total_bits = n_ids * 17;
    *out_len = (total_bits + 7) / 8;
    uint8_t *buf = (uint8_t*)calloc(1, *out_len);
    uint64_t acc = 0;
    int acc_bits = 0;
    size_t byte_out = 0;
    for (size_t i = 0; i < n_ids; ++i) {
        acc = (acc << 17) | (ids[i] & 0x1FFFFU);
        acc_bits += 17;
        while (acc_bits >= 8) {
            acc_bits -= 8;
            buf[byte_out++] = (uint8_t)((acc >> acc_bits) & 0xFF);
        }
    }
    if (acc_bits > 0) buf[byte_out] = (uint8_t)((acc << (8 - acc_bits)) & 0xFF);
    return buf;
}

static uint32_t* mix_tokens(const uint32_t *input_ids, size_t in_count, const uint32_t *salt_ids, size_t salt_count, const uint64_t sk[8], int rounds) {
    uint32_t *out = (uint32_t*)malloc(in_count * sizeof(uint32_t));
    for (size_t i = 0; i < in_count; ++i) {
        uint64_t pk = sk[i % 8] ^ ((uint64_t)i * GOLDEN) ^ ((uint64_t)rounds * DOM_TOKEN);
        int rot = (int)((pk ^ (pk >> 32)) % 31) + 1;
        uint32_t sv = salt_ids[i % salt_count];
        uint32_t rv = (sv << rot) | (sv >> (32 - rot));
        uint32_t mv = rv ^ (sv * 0x9e3779b9U);
        uint64_t bc = (uint64_t)(i / 16);
        uint64_t cm = sk[(i / 16) % 8] ^ (bc * DOM_TOKEN);
        mv ^= (uint32_t)(cm & 0xFFFFFFFFU);
        out[i] = (input_ids[i] ^ mv) & 0x01FFFFU;
    }
    return out;
}

/* Token sequence -> dense bit stream (17 bits per token, MSB first) */
static uint8_t* tokens_to_bits_u32(const uint32_t *ids, size_t n_ids, size_t *out_len) {
    size_t total_bits = n_ids * 17;
    *out_len = (total_bits + 7) / 8;
    uint8_t *buf = (uint8_t*)calloc(1, *out_len);
    uint64_t acc = 0;
    int acc_bits = 0;
    size_t byte_out = 0;
    for (size_t i = 0; i < n_ids; ++i) {
        acc = (acc << 17) | (ids[i] & 0x1FFFFU);
        acc_bits += 17;
        while (acc_bits >= 8) {
            acc_bits -= 8;
            buf[byte_out++] = (uint8_t)((acc >> acc_bits) & 0xFF);
        }
    }
    if (acc_bits > 0) buf[byte_out] = (uint8_t)((acc << (8 - acc_bits)) & 0xFF);
    return buf;
}

static uint8_t* tokenize_and_encode(const uint8_t *data, size_t len, const char *salt, const uint64_t sk[8], int rounds, size_t *out_len) {
    uint32_t *input_ids = (uint32_t*)malloc(len * sizeof(uint32_t));
    for (size_t i = 0; i < len; ++i) input_ids[i] = data[i];

    uint32_t *mixed_ids = NULL;
    size_t mixed_count = len;

    if (salt && salt[0] != '\0') {
        size_t slen = strlen(salt);
        uint32_t *salt_ids = (uint32_t*)malloc(slen * sizeof(uint32_t));
        for (size_t i = 0; i < slen; ++i) salt_ids[i] = (uint8_t)salt[i];
        mixed_ids = mix_tokens(input_ids, len, salt_ids, slen, sk, rounds);
        free(salt_ids);
        free(input_ids);
    } else {
        mixed_ids = input_ids;
    }

    uint8_t *bits = tokens_to_bits_u32(mixed_ids, mixed_count, out_len);
    free(mixed_ids);
    return bits;
}

static void scatter_positions(const uint8_t *in, size_t in_len, const uint8_t *salt, size_t salt_len, const uint64_t sk[8], int rounds, size_t *pos) {
    size_t total = in_len + salt_len;
    uint64_t dh = strong_hash(in, (in_len < 255 ? in_len : 255), DOM_SALT ^ (uint64_t)rounds);
    for (size_t i = 0; i < salt_len; ++i) {
        uint64_t h = dh ^ (sk[i % 8] * (uint64_t)(i + 1)) ^ (GOLDEN * (uint64_t)(rounds + i));
        h ^= (uint64_t)salt[i] * 0x517cc1b727220a95ULL;
        if (i + 1 < salt_len) h ^= (uint64_t)salt[i + 1] * DOM_SALT;
        uint64_t v0, v1, v2, v3;
        make_sip_state(h, sk[0], dh, GOLDEN, &v0, &v1, &v2, &v3);
        sip_absorb(&v0, &v1, &v2, &v3, h ^ (uint64_t)i);
        h = sip_finalize(v0, v1, v2, v3);
        int rot = (int)((i * 7 + (size_t)rounds) % 62) + 1;
        h = rotl64(h, rot);
        pos[i] = (size_t)(h % total);
    }
}

static void resolve_collisions(size_t *pos, const uint8_t *salt, size_t salt_len, const uint64_t sk[8], uint64_t dh, size_t total) {
    bool *occ = (bool*)calloc(1, total);
    for (size_t i = 0; i < salt_len; ++i) {
        uint64_t sh = strong_hash(salt, (salt_len < 255 ? salt_len : 255), dh ^ sk[i % 8] ^ (uint64_t)i);
        size_t stride = (size_t)(sh % total);
        if (stride == 0) stride = 1;
        if (stride % 2 == 0) stride ^= 1;
        size_t p = pos[i];
        size_t probe = 0;
        while (occ[p] && probe < total) {
            ++probe;
            p = (pos[i] + probe * stride) % total;
        }
        if (occ[p]) {
            p = (pos[i] + 1) % total;
            while (occ[p]) p = (p + 1) % total;
        }
        occ[p] = true;
        pos[i] = p;
    }
    free(occ);
}

static uint8_t* interleave_salt(const uint8_t *in, size_t in_len, const uint8_t *salt, size_t salt_len, const size_t *pos, size_t *out_len) {
    size_t total = in_len + salt_len;
    *out_len = total;
    uint8_t *res = (uint8_t*)calloc(1, total);
    bool *is_salt = (bool*)calloc(1, total);
    for (size_t i = 0; i < salt_len; ++i) {
        res[pos[i]] = salt[i];
        is_salt[pos[i]] = true;
    }
    size_t in_idx = 0;
    for (size_t j = 0; j < total && in_idx < in_len; ++j) {
        if (!is_salt[j]) res[j] = in[in_idx++];
    }
    free(is_salt);
    return res;
}

static uint8_t* build_scattered_message(const uint8_t *in, size_t in_len, const char *salt, const uint64_t sk[8], int rounds, size_t *out_len) {
    if (!salt || salt[0] == '\0') {
        *out_len = in_len;
        uint8_t *res = (uint8_t*)malloc(in_len);
        memcpy(res, in, in_len);
        return res;
    }
    size_t slen = strlen(salt);
    size_t *pos = (size_t*)malloc(slen * sizeof(size_t));
    scatter_positions(in, in_len, (const uint8_t*)salt, slen, sk, rounds, pos);
    uint64_t dh = strong_hash(in, (in_len < 255 ? in_len : 255), DOM_SALT ^ (uint64_t)rounds);
    resolve_collisions(pos, (const uint8_t*)salt, slen, sk, dh, in_len + slen);
    uint8_t *res = interleave_salt(in, in_len, (const uint8_t*)salt, slen, pos, out_len);
    free(pos);
    return res;
}

static void stream_engine(uint64_t state[8], uint64_t total_len, uint64_t original_len, int rounds, const uint64_t sk[8], const uint64_t *sched, const uint8_t *data, size_t data_len, char out[129]) {
    whiten_state(state, sk, sched, rounds);

    uint64_t ev0 = sk[0] ^ DOM_FINAL;
    uint64_t ev1 = (uint64_t)rounds * DOM_ROUNDS ^ sk[1];
    uint64_t ev2 = total_len ^ original_len ^ sk[2];
    uint64_t ev3 = DOM_FEIST ^ sk[3];

    uint64_t bidx = 0;
    size_t offset = 0;
    while (offset + 64 <= data_len) {
        absorb_block(state, data + offset, bidx, sk, sched, rounds);
        sip_absorb(&ev0, &ev1, &ev2, &ev3, bidx ^ state[0] ^ state[7]);
        offset += 64;
        bidx++;
    }

    uint8_t carry[128];
    size_t rem = data_len - offset;
    memcpy(carry, data + offset, rem);
    size_t ci = rem;
    carry[ci++] = 0x80;
    while ((offset + ci) % 64 != 48) carry[ci++] = 0x00;
    uint64_t bllo = total_len << 3;
    uint64_t blhi = total_len >> 61;
    for (int i = 7; i >= 0; --i) carry[ci++] = (uint8_t)((blhi >> (i * 8)) & 0xFF);
    for (int i = 7; i >= 0; --i) carry[ci++] = (uint8_t)((bllo >> (i * 8)) & 0xFF);

    for (size_t i = 0; i < ci; i += 64, ++bidx) {
        absorb_block(state, carry + i, bidx, sk, sched, rounds);
        sip_absorb(&ev0, &ev1, &ev2, &ev3, bidx ^ state[0] ^ state[7]);
    }

    uint64_t ef = sip_finalize(ev0, ev1, ev2, ev3);
    state[7] ^= ef ^ rotl64(ef, 31);
    permute(state, rounds, sched);

    state[0] ^= original_len ^ DOM_FINAL;
    state[1] ^= total_len ^ rotl64(DOM_FINAL, 32);
    state[2] ^= rotl64(original_len ^ total_len, 17);
    state[3] ^= (uint64_t)rounds * DOM_ROUNDS ^ DOM_FINAL;
    state[4] ^= sk[4] ^ rotl64(ef, 13);
    state[5] ^= rotl64(original_len, 41) ^ total_len ^ sk[5];
    state[6] ^= ef ^ (uint64_t)rounds ^ sk[6];
    state[7] ^= rotl64(sk[7] ^ total_len, 29);

    permute(state, rounds, sched);

    for (int i = 0; i < 8; ++i) {
        sprintf(out + (i * 16), "%016llx", (unsigned long long)state[i]);
    }
    out[128] = '\0';
}

/* Public API Implementation */

void rudra512_init_ctx(RUDRA512_CTX *ctx, int rounds, const char *salt) {
    if (!ctx) return;
    ctx->data = (uint8_t*)malloc(4096);
    ctx->len = 0;
    ctx->capacity = 4096;
    ctx->rounds = (rounds > 0) ? rounds : 32;
    ctx->salt = salt ? strdup(salt) : NULL;
}

void rudra512_update(RUDRA512_CTX *ctx, const uint8_t *data, size_t len) {
    if (!ctx || !data || len == 0) return;
    if (ctx->len + len > ctx->capacity) {
        while (ctx->len + len > ctx->capacity) ctx->capacity *= 2;
        ctx->data = (uint8_t*)realloc(ctx->data, ctx->capacity);
    }
    memcpy(ctx->data + ctx->len, data, len);
    ctx->len += len;
}

void rudra512_final(RUDRA512_CTX *ctx, char out[129]) {
    if (!ctx || !out) return;
    if (ctx->rounds <= 1) {
        fprintf(stderr, "Error: rudra::hash: rounds must be > 1\n");
        goto cleanup;
    }

    uint64_t sk[8];
    expand_salt(ctx->salt, sk);
    uint64_t *sched = (uint64_t*)malloc(ctx->rounds * sizeof(uint64_t));
    make_round_schedule(sk, ctx->rounds, sched);

    size_t tb_len;
    uint8_t *token_bits = tokenize_and_encode(ctx->data, ctx->len, ctx->salt, sk, ctx->rounds, &tb_len);

    size_t sm_len;
    uint8_t *scattered = build_scattered_message(token_bits, tb_len, ctx->salt, sk, ctx->rounds, &sm_len);
    free(token_bits);

    size_t fw_len;
    uint8_t *processed = feistel_whiten(scattered, sm_len, sk, ctx->rounds, &fw_len);
    free(scattered);

    uint64_t state[8];
    make_init_state_func(state, ctx->rounds, sk, sched, DOM_STRING);
    stream_engine(state, (uint64_t)fw_len, (uint64_t)ctx->len, ctx->rounds, sk, sched, processed, fw_len, out);
    free(processed);
    free(sched);

cleanup:
    free(ctx->data);
    if (ctx->salt) free(ctx->salt);
    ctx->data = NULL;
    ctx->salt = NULL;
}

void rudra512_hex(const uint8_t *data, size_t len, int rounds, const char *salt, char out[129]) {
    if (!out) return;
    RUDRA512_CTX ctx;
    rudra512_init_ctx(&ctx, rounds, salt);
    rudra512_update(&ctx, data, len);
    rudra512_final(&ctx, out);
}
