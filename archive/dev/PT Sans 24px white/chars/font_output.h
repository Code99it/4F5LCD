#ifndef FONT_OUTPUT_H
#define FONT_OUTPUT_H

#pragma once
#include <stdint.h>

typedef struct {
    char symbol;
    const uint16_t *bitmap;
    uint8_t width;
    uint8_t height;
} CharDef;

extern const CharDef font[];
extern const size_t font_len;

extern const uint16_t char_PT_Sans_24_white_A[336];
extern const uint16_t char_PT_Sans_24_white_Auml[336];
extern const uint16_t char_PT_Sans_24_white_B[288];
extern const uint16_t char_PT_Sans_24_white_C[288];
extern const uint16_t char_PT_Sans_24_white_D[312];
extern const uint16_t char_PT_Sans_24_white_E[240];
extern const uint16_t char_PT_Sans_24_white_F[240];
extern const uint16_t char_PT_Sans_24_white_G[288];
extern const uint16_t char_PT_Sans_24_white_H[312];
extern const uint16_t char_PT_Sans_24_white_I[72];
extern const uint16_t char_PT_Sans_24_white_J[144];
extern const uint16_t char_PT_Sans_24_white_K[312];
extern const uint16_t char_PT_Sans_24_white_L[264];
extern const uint16_t char_PT_Sans_24_white_M[384];
extern const uint16_t char_PT_Sans_24_white_N[312];
extern const uint16_t char_PT_Sans_24_white_O[336];
extern const uint16_t char_PT_Sans_24_white_Ouml[336];
extern const uint16_t char_PT_Sans_24_white_P[264];
extern const uint16_t char_PT_Sans_24_white_Q[384];
extern const uint16_t char_PT_Sans_24_white_R[288];
extern const uint16_t char_PT_Sans_24_white_S[264];
extern const uint16_t char_PT_Sans_24_white_T[312];
extern const uint16_t char_PT_Sans_24_white_U[312];
extern const uint16_t char_PT_Sans_24_white_Uuml[312];
extern const uint16_t char_PT_Sans_24_white_V[312];
extern const uint16_t char_PT_Sans_24_white_W[456];
extern const uint16_t char_PT_Sans_24_white_X[336];
extern const uint16_t char_PT_Sans_24_white_Y[312];
extern const uint16_t char_PT_Sans_24_white_Z[288];
extern const uint16_t char_PT_Sans_24_white_bracket_close[144];
extern const uint16_t char_PT_Sans_24_white_bracket_open[144];
extern const uint16_t char_PT_Sans_24_white_comma[72];
extern const uint16_t char_PT_Sans_24_white_dash[144];
extern const uint16_t char_PT_Sans_24_white_degree[168];
extern const uint16_t char_PT_Sans_24_white_dot[72];
extern const uint16_t char_PT_Sans_24_white_doubledot[72];
extern const uint16_t char_PT_Sans_24_white_lower_a[240];
extern const uint16_t char_PT_Sans_24_white_lower_auml[240];
extern const uint16_t char_PT_Sans_24_white_lower_b[264];
extern const uint16_t char_PT_Sans_24_white_lower_c[216];
extern const uint16_t char_PT_Sans_24_white_lower_d[240];
extern const uint16_t char_PT_Sans_24_white_lower_e[240];
extern const uint16_t char_PT_Sans_24_white_lower_f[192];
extern const uint16_t char_PT_Sans_24_white_lower_g[240];
extern const uint16_t char_PT_Sans_24_white_lower_h[240];
extern const uint16_t char_PT_Sans_24_white_lower_i[96];
extern const uint16_t char_PT_Sans_24_white_lower_j[120];
extern const uint16_t char_PT_Sans_24_white_lower_k[240];
extern const uint16_t char_PT_Sans_24_white_lower_l[144];
extern const uint16_t char_PT_Sans_24_white_lower_m[408];
extern const uint16_t char_PT_Sans_24_white_lower_n[240];
extern const uint16_t char_PT_Sans_24_white_lower_o[264];
extern const uint16_t char_PT_Sans_24_white_lower_ouml[264];
extern const uint16_t char_PT_Sans_24_white_lower_p[264];
extern const uint16_t char_PT_Sans_24_white_lower_q[240];
extern const uint16_t char_PT_Sans_24_white_lower_r[168];
extern const uint16_t char_PT_Sans_24_white_lower_s[216];
extern const uint16_t char_PT_Sans_24_white_lower_szlig[264];
extern const uint16_t char_PT_Sans_24_white_lower_t[192];
extern const uint16_t char_PT_Sans_24_white_lower_u[240];
extern const uint16_t char_PT_Sans_24_white_lower_uuml[240];
extern const uint16_t char_PT_Sans_24_white_lower_v[264];
extern const uint16_t char_PT_Sans_24_white_lower_w[408];
extern const uint16_t char_PT_Sans_24_white_lower_x[288];
extern const uint16_t char_PT_Sans_24_white_lower_y[264];
extern const uint16_t char_PT_Sans_24_white_lower_z[240];
extern const uint16_t char_PT_Sans_24_white_number_0[264];
extern const uint16_t char_PT_Sans_24_white_number_1[240];
extern const uint16_t char_PT_Sans_24_white_number_2[240];
extern const uint16_t char_PT_Sans_24_white_number_3[216];
extern const uint16_t char_PT_Sans_24_white_number_4[312];
extern const uint16_t char_PT_Sans_24_white_number_5[240];
extern const uint16_t char_PT_Sans_24_white_number_6[264];
extern const uint16_t char_PT_Sans_24_white_number_7[264];
extern const uint16_t char_PT_Sans_24_white_number_8[240];
extern const uint16_t char_PT_Sans_24_white_number_9[264];
extern const uint16_t char_PT_Sans_24_white_percent[384];
extern const uint16_t char_PT_Sans_24_white_plus[264];
extern const uint16_t char_PT_Sans_24_white_quote[120];
extern const uint16_t char_PT_Sans_24_white_semicolon[96];
extern const uint16_t char_PT_Sans_24_white_slash[240];
extern const uint16_t char_PT_Sans_24_white_star[192];
extern const uint16_t char_PT_Sans_24_white_underscore[240];

#endif // FONT_OUTPUT_H
