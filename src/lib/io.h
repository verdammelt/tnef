
#include <stdio.h>
#include <stdint.h>

extern uint32_t GETINT32(unsigned char *p);
extern uint16_t GETINT16 (unsigned char* p);
extern uint8_t GETINT8 (unsigned char *p);
extern uint8_t _read_uint8 (FILE* fp);
extern uint16_t _read_uint16 (FILE* fp);
extern uint32_t _read_uint32 (FILE* fp);
extern unsigned char* _read_buf(FILE* fp, size_t n);

