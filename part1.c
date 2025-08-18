#include <stdbool.h>
#include <stdio.h>

#define u8 unsigned char

const char *regname(u8 reg, u8 w) {
  /*
   * REG w=0 w=1
   * 000 AL  AX
   * 001 CL  CX
   * 010 DL  DX
   * 011 BL  BX
   * 100 AH  SP
   * 101 CH  BP
   * 110 DH  SI
   * 111 BH  DI
   */
  switch (reg) {
  case 0:
    return w ? "AX" : "AL";
  case 1:
    return w ? "CX" : "CL";
  case 2:
    return w ? "DX" : "DL";
  case 3:
    return w ? "BX" : "BL";
  case 4:
    return w ? "SP" : "AH";
  case 5:
    return w ? "BP" : "CH";
  case 6:
    return w ? "SI" : "DH";
  case 7:
    return w ? "DI" : "BH";
  default:
    return "<unknown>";
  }
}

void print_bits(u8 v) {
  for (int i = 7; i >= 0; i--)
    printf("%i", (v >> i) & 1);
  return;
}

int main(int argc, char **argv) {
  u8 instr_buf[1024];
  if (argc < 2) {
    fprintf(stderr, "Usage: cmd <filename>");
    return 1;
  }
  char *fname = argv[1];
  FILE *in_file = fopen(fname, "r");
  fread(instr_buf, 1, 1024, in_file);

  char a = instr_buf[0];
  char b = instr_buf[1];
  printf("Buffer has: 0x%hhx 0x%hhx\n", a, b);
  size_t cursor = 0;
  while (cursor < 100) {
    int h = instr_buf[cursor];
    int l = instr_buf[cursor + 1];
    int header = h & ~0x3;
    printf("Header: %hhu\n", header);
    bool d = (h >> 1) & 1;
    bool w = (h >> 0) & 1;
    u8 mod = (l >> 6);
    u8 reg = (l >> 3) & 0x7;
    u8 rm = (l) & 0x7;
    if (header == 0x88) {
      printf("; ");
      print_bits(h);
      printf(" ");
      print_bits(l);

      if (mod != 3) {
        printf("Unsupported MOV\n");
        return 1;
      }

      u8 dst = d ? reg : rm;
      u8 src = d ? rm : reg;
      printf("\nmov %s, %s\n", regname(dst, w), regname(src, w));
    }
    cursor += 2;
  }

  return 0;
}
