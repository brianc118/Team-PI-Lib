#ifndef _pin_magic_
#define _pin_magic_

#define DELAY7        \
  asm volatile(       \
    "rjmp .+0" "\n\t" \
    "rjmp .+0" "\n\t" \
    "rjmp .+0" "\n\t" \
    "nop"      "\n"   \
    ::);
   

// teensy3 stuff is here:

// fast way
// GPIOD_PDOR = (ret); \
// slow way

#define write8inline(wr) { \
    digitalWriteFast(10, (wr & 0x01)); \
    digitalWriteFast(26, (wr & 0x02)); \
    digitalWriteFast(6 , (wr & 0x04)); \
    digitalWriteFast(5 , (wr & 0x08)); \
    digitalWriteFast(7 , (wr & 0x10)); \
    digitalWriteFast(8 , (wr & 0x20)); \
    digitalWriteFast(32, (wr & 0x40)); \
    digitalWriteFast(16, (wr & 0x80)); \
    WR_STROBE; }

// #define write8inline(cl) { \
//     ((cl & 0x01)) ? digitalWriteFast(0, HIGH) : digitalWriteFast(0, LOW); \
//     ((cl & 0x02)) ? digitalWriteFast(1, HIGH) : digitalWriteFast(1, LOW); \
//     ((cl & 0x04)) ? digitalWriteFast(2, HIGH) : digitalWriteFast(2, LOW); \
//     ((cl & 0x08)) ? digitalWriteFast(3, HIGH) : digitalWriteFast(3, LOW); \
//     ((cl & 0x10)) ? digitalWriteFast(4, HIGH) : digitalWriteFast(4, LOW); \
//     ((cl & 0x20)) ? digitalWriteFast(5, HIGH) : digitalWriteFast(5, LOW); \
//     ((cl & 0x40)) ? digitalWriteFast(6, HIGH) : digitalWriteFast(6, LOW); \
//     ((cl & 0x80)) ? digitalWriteFast(7, HIGH) : digitalWriteFast(7, LOW); \
//     WR_STROBE; }

// fast way
// ret = GPIOD_PDIR & 0xFF; \
// slow way
#define read8inline(ret) { \
    RD_ACTIVE;   \
    delayMicroseconds(1);\
    ret  =  digitalReadFast(10)  \
          | digitalReadFast(26) << 1  \
          | digitalReadFast(6 ) << 2 \
          | digitalReadFast(5 ) << 3 \
          | digitalReadFast(7 ) << 4 \
          | digitalReadFast(8 ) << 5 \
          | digitalReadFast(32) << 6 \
          | digitalReadFast(16) << 7; \
    RD_IDLE; }

#define setWriteDirInline() { \
  pinMode(10, OUTPUT); \
  pinMode(26, OUTPUT); \
  pinMode(6 , OUTPUT); \
  pinMode(5 , OUTPUT); \
  pinMode(7 , OUTPUT); \
  pinMode(8 , OUTPUT); \
  pinMode(32, OUTPUT); \
  pinMode(16, OUTPUT); }     //data7 of tft to pin5 of teensy3
#define setReadDirInline() { \
  pinMode(10, INPUT); \
  pinMode(26, INPUT); \
  pinMode(6 , INPUT); \
  pinMode(5 , INPUT); \
  pinMode(7 , INPUT); \
  pinMode(8 , INPUT); \
  pinMode(32, INPUT); \
  pinMode(16, INPUT); }

#define write8            write8inline
#define read8             read8inline
#define setWriteDir       setWriteDirInline
#define setReadDir        setReadDirInline
#define writeRegister8    writeRegister8inline
#define writeRegister16   writeRegister16inline
#define writeRegisterPair writeRegisterPairInline     

// Control signals are ACTIVE LOW (idle is HIGH)
// Command/Data: LOW = command, HIGH = data
// These are single-instruction operations and always inline

// #define RD_ACTIVE  *rdPort &=  rdPinUnset
// #define RD_IDLE    *rdPort |=  rdPinSet
// #define WR_ACTIVE  *wrPort &=  wrPinUnset
// #define WR_IDLE    *wrPort |=  wrPinSet
// #define CD_COMMAND *cdPort &=  cdPinUnset
// #define CD_DATA    *cdPort |=  cdPinSet
// #define CS_ACTIVE  *csPort &=  csPinUnset
// #define CS_IDLE    *csPort |=  csPinSet

#define RD_ACTIVE  *rdPort &=  rdPinUnset
#define RD_IDLE    *rdPort |=  rdPinSet
#define WR_ACTIVE  *wrPort &=  wrPinUnset
#define WR_IDLE    *wrPort |=  wrPinSet
#define CD_COMMAND *cdPort &=  cdPinUnset
#define CD_DATA    *cdPort |=  cdPinSet
#define CS_ACTIVE  *csPort &=  csPinUnset
#define CS_IDLE    *csPort |=  csPinSet

// #define RD_ACTIVE  digitalWriteFast(*rdPort, LOW);
// #define RD_IDLE    digitalWriteFast(*rdPort, HIGH);
// #define WR_ACTIVE  digitalWriteFast(*wrPort, LOW);
// #define WR_IDLE    digitalWriteFast(*wrPort, HIGH);
// #define CD_COMMAND digitalWriteFast(*cdPort, LOW);
// #define CD_DATA    digitalWriteFast(*cdPort, HIGH);
// #define CS_ACTIVE  digitalWriteFast(*csPort, LOW);
// #define CS_IDLE    digitalWriteFast(*csPort, LOW);

// Data write strobe, ~2 instructions and always inline
#define WR_STROBE { WR_ACTIVE; WR_IDLE; }

// These higher-level operations are usually functionalized,
// except on Mega where's there's gobs and gobs of program space.

// Set value of TFT register: 8-bit address, 8-bit value
#define writeRegister8inline(a, d) { \
  CD_COMMAND; write8(a); CD_DATA; write8(d); }

// Set value of TFT register: 16-bit address, 16-bit value
// See notes at top about macro expansion, hence hi & lo temp vars
#define writeRegister16inline(a, d) { \
  uint8_t hi, lo; \
  hi = (a) >> 8; lo = (a); CD_COMMAND; write8(hi); write8(lo); \
  hi = (d) >> 8; lo = (d); CD_DATA   ; write8(hi); write8(lo); }

// Set value of 2 TFT registers: Two 8-bit addresses (hi & lo), 16-bit value
#define writeRegisterPairInline(aH, aL, d) { \
  CD_COMMAND; write8(aH); CD_DATA; write8(d >> 8); \
  CD_COMMAND; write8(aL); CD_DATA; write8(d); }

#define writeRegister32Inline(r, d) { \
  CS_ACTIVE; \
  CD_COMMAND; \
  write8inline(r); \
  CD_DATA; \
  write8inline(d >> 24); \
  write8inline(d >> 16); \
  write8inline(d >> 8); \
  write8inline(d); \
  CS_IDLE; }
// void Adafruit_TFTLCD::writeRegister32(uint8_t r, uint32_t d) {
//   CS_ACTIVE;
//   CD_COMMAND;
//   write8inline(r);
//   CD_DATA;
//   write8inline(d >> 24);
//   write8inline(d >> 16);
//   write8inline(d >> 8);
//   write8inline(d);
//   CS_IDLE;
// }




#endif // _pin_magic_
