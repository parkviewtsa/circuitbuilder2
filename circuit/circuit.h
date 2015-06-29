#ifndef CIRCUIT_H
#define CIRCUIT_H

// Component IDs (tags)
#define CB_BATT     0x03
#define CB_CELL     0x06
#define CB_PV_CELL  0x07
#define CB_RESISTOR 0x08
#define CB_POTENT   0x09
#define CB_THERM_R  0x0a
#define CB_PHOTO_R  0x0b
#define CB_CAPAC_E  0x0c
#define CB_CAPAC_CD 0x0d
#define CB_SWITCH   0x0e
#define CB_BUTTON   0x0f
#define CB_DIODE    0x10
#define CB_ZDIODE   0x11
#define CB_PDIODE   0x12
#define CB_RECT     0x13
#define CB_SC_RECT  0x14
#define CB_SIG_DIODE 0x15
#define CB_LED      0x16
#define CB_INDUCTOR 0x18
#define CB_TRANSFORMER 0x19
#define CB_DC_MOTOR 0x1a
#define CB_AC_MOTOR 0x1b
#define CB_DC_GEN   0x1c
#define CB_AC_GEN   0x1d
#define CB_MIC      0x1e
#define CB_SPEAKER  0x1f
#define CB_SOLENOID 0x20
#define CB_BEAD     0x22    // aka ferrite impedance
#define CB_NPN      0x80
#define CB_PNP      0x81
#define CB_IC_BASE  0x90
#define CB_IC_MAX   0x9f

// Special, branch, and meta tags
#define CB_META     0x00
#define CB_META_END 0x01
#define CB_OPEN     0x100
#define CB_CLOSE    0x101
#define CB_BRANCH   0x102
#define CB_CBRANCH  0x103
#define CB_OJUNCT   0x110
#define CB_CJUNCT   0x111

#define TABLE_CAP_BASE 20
#define TABLE_EXP_SIZE 10

#define SYMBOL_DIR "symbols/"

typedef struct cbComponent {
  int type;
  cbComponent* from;
  cbComponent* to;
  crItem* draw_item;
} cbComponent;

typedef struct cbHeader {
  int comp_count;
  int table_cap;
  // more properties?
  cbComponent* table;
} cbHeader;

void circuit_init (cbHeader*, char*);
cbComponent* cb_CreateComponent (int);
void cb_AddComponent (cbComponent*, cbHeader*);
void cb_DeleteComponent (cbComponent*, cbHeader*);
void cb_FindComponent (cbComponent*, const cbHeader*);
void cb_ConnectComponents (cbComponent*, cbComponent*);
cbComponent* cb_GetComponentByDrawItem (crItem*, const cbHeader*);

extern int crash (int, int);

#endif // CIRCUIT_H
