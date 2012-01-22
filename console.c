#include "common.h"

enum { ROWS = 24, COLS = 80 };
enum { ATTRIBUTE = 7 };

static u8 cursor_row, cursor_col;
static volatile u8 * const vram = (unsigned char *) 0xB8000;

void cls()
{
  u16 i;
  for (i = 0; i < ROWS * COLS * 2; ++i)
    vram[i] = 0;
  cursor_row = cursor_col = 0;
}

static inline volatile u8 * video_chr(u8 r, u8 c)
{
  return vram + (COLS * r + c) * 2;
}

static inline volatile u8 * video_att(u8 r, u8 c)
{
  return vram + (COLS * r + c) * 2 + 1;
}

static void push_up()
{
  u8 r, c;
  for (r = 1; r < ROWS; ++r)
    for (c = 0; c < COLS; ++c)
    {
      *video_chr(r - 1, c) = *video_chr(r, c);
      *video_att(r - 1, c) = *video_att(r, c);
    }
  for (c = 0; c < COLS; ++c)
  {
    *video_chr(ROWS - 1, c) = 0;
    *video_att(ROWS - 1, c) = 0;
  }
}

void newline()
{
  if (cursor_row + 1 >= ROWS)
    push_up();
  else
    ++cursor_row;
  cursor_col = 0;
}

void kputchar(int c)
{
  if (c == '\r')
    cursor_col = 0;
  else if (c == '\n')
    newline();
  else
  {
    *video_chr(cursor_row, cursor_col) = c & 0xFF;
    *video_att(cursor_row, cursor_col) = ATTRIBUTE;
    if (++cursor_col >= COLS)
      newline();
  }
}

void kputs(const char *s)
{
  kprintf("%s\n", s);
}

void kprintf(const char *format, ...)
{
  char **arg = (char **) &format;
  int c;
  char buf[20]; // TODO: appropriate buf size?
  ++arg;

  while ((c = *format++) != 0)
  {
    if (c != '%')
      kputchar(c);
    else
    {
      char *s;

      c = *format++;
      switch (c)
      {
        case 'd':
        case 'u':
        case 'x':
          itoa(buf, c, *((int *) arg++));
          s = buf;
          goto string;

        case 's':
          s = *arg++;
          if (!s)
            s = "(null)";

        string:
          while (*s)
            kputchar(*s++);
          break;

        default:
          panic("bad format specifier");
      }
    }
  }
}
