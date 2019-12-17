/*
 * Copyright (C) 2005 by Grigoriy A. Sitkarev
 * sitkarev@komi.tgk-9.ru
 *
 * Adapted by Ricardo Olsen from original in http://mrts-ng.googlecode.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __IEC104_TYPES_H
#define __IEC104_TYPES_H

#include <cstdint>

#pragma pack(push)
#pragma pack(1)

// 32-bit string state and change data unit
struct iec_stcd {
  union {
    unsigned short  st;
    struct {
      unsigned char st1 : 1;
      unsigned char st2 : 1;
      unsigned char st3 : 1;
      unsigned char st4 : 1;
      unsigned char st5 : 1;
      unsigned char st6 : 1;
      unsigned char st7 : 1;
      unsigned char st8 : 1;
      unsigned char st9 : 1;
      unsigned char st10 : 1;
      unsigned char st11 : 1;
      unsigned char st12 : 1;
      unsigned char st13 : 1;
      unsigned char st14 : 1;
      unsigned char st15 : 1;
      unsigned char st16 : 1;
    };
  };
  union {
    unsigned short  cd;
    struct {
      unsigned char cd1 : 1;
      unsigned char cd2 : 1;
      unsigned char cd3 : 1;
      unsigned char cd4 : 1;
      unsigned char cd5 : 1;
      unsigned char cd6 : 1;
      unsigned char cd7 : 1;
      unsigned char cd8 : 1;
      unsigned char cd9 : 1;
      unsigned char cd10 : 1;
      unsigned char cd11 : 1;
      unsigned char cd12 : 1;
      unsigned char cd13 : 1;
      unsigned char cd14 : 1;
      unsigned char cd15 : 1;
      unsigned char cd16 : 1;
    };
  };
};

// CP56Time2a timestamp
struct cp56time2a {
  unsigned short msec;
  unsigned char min : 6;
  unsigned char res1 : 1;
  unsigned char iv : 1;
  unsigned char hour : 5;
  unsigned char res2 : 2;
  unsigned char su : 1;
  unsigned char mday : 5;
  unsigned char wday : 3;
  unsigned char month : 4;
  unsigned char res3 : 4;
  unsigned char year : 7;
  unsigned char res4 : 1;
};

typedef struct cp56time2a cp56time2a;

struct iec_qualif {
  unsigned char var : 2;
  unsigned char res : 2;
  unsigned char bl : 1; // blocked/not blocked
  unsigned char sb : 1; // substituted/not substituted
  unsigned char nt : 1; // not topical/topical
  unsigned char iv : 1; // valid/invalid
};

// M_SP_NA_1 - single point information with quality description
struct iec_type1 {
  unsigned char sp : 1; // single point information
  unsigned char res : 3;
  unsigned char bl : 1; // blocked/not blocked
  unsigned char sb : 1; // substituted/not substituted
  unsigned char nt : 1; // not topical/topical
  unsigned char iv : 1; // valid/invalid
};

// M_DP_NA_1 - double point information with quality description
struct iec_type3 {
  unsigned char dp : 2; // double point information
  unsigned char res : 2;
  unsigned char bl : 1; // blocked/not blocked
  unsigned char sb : 1; // substituted/not substituted
  unsigned char nt : 1; // not topical/topical
  unsigned char iv : 1; // valid/invalid
};

// M_ST_NA_1 - step position
struct iec_type5 {
  unsigned char mv  : 7; // value
  unsigned char t : 1; // transient flag
  unsigned char ov : 1; // overflow/no overflow
  unsigned char res : 3;
  unsigned char bl : 1; // blocked/not blocked
  unsigned char sb : 1; // substituted/not substituted
  unsigned char nt : 1; // not topical/topical
  unsigned char iv : 1; // valid/invalid
};

// M_BO_NA_1 - state and change information bit string
struct iec_type7 {
  union {
    struct iec_stcd stcd;
    uint32_t bsi;
  };
  unsigned char ov : 1; // overflow/no overflow
  unsigned char res : 3;
  unsigned char bl : 1; // blocked/not blocked
  unsigned char sb : 1; // substituted/not substituted
  unsigned char nt : 1; // not topical/topical
  unsigned char iv : 1; // valid/invalid
};

// M_ME_NA_1 - normalized measured value
struct iec_type9 {
  short mv; // normalized value
  unsigned char ov : 1; // overflow/no overflow
  unsigned char res : 3;
  unsigned char bl : 1; // blocked/not blocked
  unsigned char sb : 1; // substituted/not substituted
  unsigned char nt : 1; // not topical/topical
  unsigned char iv : 1; // valid/invalid
};

// M_ME_NB_1 - scaled measured value
struct iec_type11 {
  short mv; // scaled value
  unsigned char ov : 1; // overflow/no overflow
  unsigned char res : 3;
  unsigned char bl : 1; // blocked/not blocked
  unsigned char sb : 1; // substituted/not substituted
  unsigned char nt : 1; // not topical/topical
  unsigned char iv : 1; // valid/invalid
};

// M_ME_NC_1 - short floating point measured value
struct iec_type13 {
  float mv; // float value
  unsigned char ov : 1; // overflow/no overflow
  unsigned char res : 3;
  unsigned char bl : 1; // blocked/not blocked
  unsigned char sb : 1; // substituted/not substituted
  unsigned char nt : 1; // not topical/topical
  unsigned char iv : 1; // valid/invalid
};

// M_ME_NC_1 - integrated totals
struct iec_type15 {
  uint32_t bcr;
  unsigned char sq : 5; // sequence
  unsigned char cy : 1; // carry
  unsigned char ca : 1; // counter adjusted
  unsigned char iv : 1; // valid/invalid
};

// M_SP_TB_1 - single point information with quality description and time tag
struct iec_type30 {
  unsigned char sp : 1; // single point information
  unsigned char res : 3;
  unsigned char bl : 1; // blocked/not blocked
  unsigned char sb : 1; // substituted/not substituted
  unsigned char nt : 1; // not topical/topical
  unsigned char iv : 1; // valid/invalid
  cp56time2a time;
};

// M_DP_TB_1 - double point information with quality description and time tag
struct iec_type31 {
  unsigned char dp : 2; // double point information
  unsigned char res : 2;
  unsigned char bl : 1; // blocked/not blocked
  unsigned char sb : 1; // substituted/not substituted
  unsigned char nt : 1; // not topical/topical
  unsigned char iv : 1; // valid/invalid
  cp56time2a time;
};

// M_ST_TB_1 - step position with time tag
struct iec_type32 {
  unsigned char mv  : 7; // value
  unsigned char t : 1; // transient flag
  unsigned char ov : 1; // overflow/no overflow
  unsigned char res : 3;
  unsigned char bl : 1; // blocked/not blocked
  unsigned char sb : 1; // substituted/not substituted
  unsigned char nt : 1; // not topical/topical
  unsigned char iv : 1; // valid/invalid
  cp56time2a time;
};

// M_BO_TB_1 - state and change information bit string and time tag
struct iec_type33 {
  union {
    struct iec_stcd stcd;
    uint32_t bsi;
  };
  unsigned char ov : 1; // overflow/no overflow
  unsigned char res : 3;
  unsigned char bl : 1; // blocked/not blocked
  unsigned char sb : 1; // substituted/not substituted
  unsigned char nt : 1; // not topical/topical
  unsigned char iv : 1; // valid/invalid
  cp56time2a time;
};

// M_ME_TD_1 - normalized measured value with time tag
struct iec_type34 {
  short mv; // normalized value
  unsigned char ov : 1; // overflow/no overflow
  unsigned char res : 3;
  unsigned char bl : 1; // blocked/not blocked
  unsigned char sb : 1; // substituted/not substituted
  unsigned char nt : 1; // not topical/topical
  unsigned char iv : 1; // valid/invalid
  cp56time2a time;
};

// M_ME_TE_1 - scaled measured value with time tag
struct iec_type35 {
  short mv; // scaled value
  unsigned char ov : 1; // overflow/no overflow
  unsigned char res : 3;
  unsigned char bl : 1; // blocked/not blocked
  unsigned char sb : 1; // substituted/not substituted
  unsigned char nt : 1; // not topical/topical
  unsigned char iv : 1; // valid/invalid
  cp56time2a time;
};

// M_ME_TF_1 - short floating point measurement value and time tag
struct iec_type36 {
  float  mv;
  unsigned char ov : 1; // overflow/no overflow
  unsigned char res : 3;
  unsigned char bl : 1; // blocked/not blocked
  unsigned char sb : 1; // substituted/not substituted
  unsigned char nt : 1; // not topical/topical
  unsigned char iv : 1; // valid/invalid
  cp56time2a time;
};

// M_IT_TB_1 - Integrated totals with time tag CP56Time2a
struct iec_type37 {
  uint32_t bcr;
  unsigned char sq : 5;
  unsigned char cy : 1;
  unsigned char ca : 1;
  unsigned char iv : 1;
  cp56time2a time;
};

// C_SC_NA_1 - Single command
struct iec_type45 {
  unsigned char scs : 1; // single command state
  unsigned char res : 1; // must be zero
  unsigned char qu : 5;
  unsigned char se : 1; // select=1 / execute=0
};

// C_DC_NA_1 - Double command
struct iec_type46 {
  unsigned char dcs : 2; // double command state
  unsigned char qu : 5;
  unsigned char se : 1; // select=1 / execute=0
};

// C_RC_NA_1 - Regulating step command
struct iec_type47 {
  unsigned char rcs : 2; // regulating step command
  unsigned char qu : 5;
  unsigned char se : 1; // select=1 / execute=0
};

// C_SE_NA_1 - Set-point Command, normalised value
struct iec_type48 {
  short nva; // normalised value
  unsigned char ql : 7; // command qualifier
  unsigned char se : 1; // select=1 / execute=0
};

// C_SE_TA_1 - Normalised value command with time tag CP56Time2a
struct iec_type61 {
  short nva; // normalised value
  unsigned char ql : 7; // command qualifier
  unsigned char se : 1; // select=1 / execute=0
  cp56time2a time;
};

// C_SE_NB_1 - Set-point Command, scaled value
struct iec_type49 {
  short sva; // scaled value
  unsigned char ql : 7; // command qualifier
  unsigned char se : 1; // select=1 / execute=0
};

// C_SE_TB_1 - Scaled value command with time tag CP56Time2a
struct iec_type62 {
  short sva; // scaled value
  unsigned char ql : 7; // command qualifier
  unsigned char se : 1; // select=1 / execute=0
  cp56time2a time;
};

// C_SE_NC_1 - Set-point Command, short floating point value
struct iec_type50 {
  float r32; // float value
  unsigned char ql : 7; // command qualifier
  unsigned char se : 1; // select=1 / execute=0
};

// C_SE_TC_1 - Short floating point number command with time tag CP56Time2a
struct iec_type63 {
  float r32; // float value
  unsigned char ql : 7; // command qualifier
  unsigned char se : 1; // select=1 / execute=0
  cp56time2a time;
};

// C_SC_TA_1 - Single command with time tag CP56Time2a
struct iec_type58 {
  unsigned char scs : 1; // single command state
  unsigned char res : 1; // must be zero
  unsigned char qu : 5;
  unsigned char se : 1; // select=1 / execute=0
  cp56time2a time;
};

// C_DC_TA_1 - Double command with time tag CP56Time2a
struct iec_type59 {
  unsigned char dcs : 2; // double command state
  unsigned char qu : 5;
  unsigned char se  : 1; // select=1 / execute=0
  cp56time2a time;
};

// C_RC_TA_1 - Regulating step command with time tag CP56Time2a
struct iec_type60 {
  unsigned char rcs : 2; // regulating step command
  unsigned char qu : 5;
  unsigned char se  : 1; // select=1 / execute=0
  cp56time2a time;
};

struct iec_type100 {
  unsigned char qoi; // pointer of interrogation
};

struct iec_type101 {
  unsigned char rqt : 6; // request
  unsigned char frz : 2; // freeze
};

struct iec_type103 {
  unsigned short ioa16; // object address bytes 1,2
  unsigned char ioa8; // object address byte 3
  cp56time2a time;
};

struct iec_type105 {
  unsigned char qrp; // QRP qualifier of reset process
};

struct iec_type107 {
  unsigned short ioa16; // object address bytes 1,2
  unsigned char ioa8; // object address byte 3
  unsigned short tsc; // TSC test sequence counter
  cp56time2a time;
};

// P_ME_NA_1 - Parameter of measured value, normalized value.
struct iec_type110 {
  short nva; // normalized value
  unsigned char kpa : 6; // Kind of parameter. 0=unused, 1=thresh, 2=filter, 3=lolimit, 4=hilimit
  unsigned char pop : 1; // pop?
  unsigned char lpc : 1; // Local parameter change flag: no change=0, changed=1
};

// P_ME_NB_1 - Parameter of measured value, scaled value.
struct iec_type111 {
  short sva; // scaled value
  unsigned char kpa : 6; // Kind of parameter. 0=unused, 1=thresh, 2=filter, 3=lolimit, 4=hilimit
  unsigned char pop : 1; // pop?
  unsigned char lpc : 1; // Local parameter change flag: no change=0, changed=1
};

// P_ME_NC_1 - Parameter of measured value, short floating point value.
struct iec_type112 {
  float r32; // float value
  unsigned char kpa : 6; // Kind of parameter. 0=unused, 1=thresh, 2=filter, 3=lolimit, 4=hilimit
  unsigned char pop : 1; // pop?
  unsigned char lpc : 1; // Local parameter change flag: no change=0, changed=1
};

// P_AC_NA_1 - Parameter activation.
struct iec_type113 {
  unsigned char qpa; // Qualifier of parameter activation. 0=unused, 1=general, 2=object, 3=transmission
};

// Data unit identifier block - ASDU header
struct iec_unit_id {
  unsigned char type;  // type identification
  unsigned char num : 7; // number of information objects
  unsigned char sq : 1; // sequenced/not sequenced address
  unsigned char cause : 6; // cause of transmission
  unsigned char pn : 1; // positive/negative app. confirmation
  unsigned char t : 1; // test
  unsigned char oa; // originator addres
  unsigned short ca; // common address of ASDU
};

struct iec_apdu {
  unsigned char start;
  unsigned char length;
  unsigned short NS;
  unsigned short NR;
  struct iec_unit_id asduh;
  union {
    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type1 obj[1];
    } sq1;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type1 obj;
    } nsq1[1];

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type3 obj[1];
    } sq3;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type3 obj;
    } nsq3[1];

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type5 obj[1];
    } sq5;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type5 obj;
    } nsq5[1];

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type7 obj[1];
    } sq7;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type7 obj;
    } nsq7[1];

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type9 obj[1];
    } sq9;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type9 obj;
    } nsq9[1];

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type11 obj[1];
    } sq11;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type11 obj;
    } nsq11[1];

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type13 obj[1];
    } sq13;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type13 obj;
    } nsq13[1];

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type15 obj[1];
    } sq15;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type15 obj;
    } nsq15[1];

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type30 obj[1];
    } sq30;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type30 obj;
    } nsq30[1];

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type31 obj[1];
    } sq31;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type31 obj;
    } nsq31[1];

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type32 obj[1];
    } sq32;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type32 obj;
    } nsq32[1];

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type33 obj[1];
    } sq33;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type33 obj;
    } nsq33[1];

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type34 obj[1];
    } sq34;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type34 obj;
    } nsq34[1];

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type35 obj[1];
    } sq35;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type35 obj;
    } nsq35[1];

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type36 obj[1];
    } sq36;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type36 obj;
    } nsq36[1];

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type37 obj[1];
    } sq37;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type37 obj;
    } nsq37[1];

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type45 obj;
    } nsq45;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type46 obj;
    } nsq46;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type47 obj;
    } nsq47;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type48 obj;
    } nsq48;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type49 obj;
    } nsq49;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type50 obj;
    } nsq50;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type61 obj;
    } nsq61;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type62 obj;
    } nsq62;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type63 obj;
    } nsq63;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type58 obj;
    } nsq58;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type59 obj;
    } nsq59;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type60 obj;
    } nsq60;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type110 obj;
    } nsq110;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type111 obj;
    } nsq111;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type112 obj;
    } nsq112;

    struct {
      unsigned short ioa16;
      unsigned char ioa8;
      iec_type113 obj;
    } nsq113;

    unsigned char dados[255];
    iec_type103 asdu103;
    iec_type105 asdu105;
    iec_type107 asdu107;
  };
};

#pragma pack(pop)

#endif // __IEC104_TYPES_H

