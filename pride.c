#include "rex_defines.h"
#include <stddef.h>
#include <stdio.h>

#ifndef PRIDEDEF
#define PRIDEDEF static inline
#endif

// Helper macros
#define PRIDE_SWAP(T, a, b)                                                    \
  do {                                                                         \
    T t = a;                                                                   \
    a = b;                                                                     \
    b = t;                                                                     \
  } while (0)
#define PRIDE_SIGN(T, x) ((T)((x) > 0) - (T)((x) < 0))
#define PRIDE_ABS(T, x) (PRIDE_SIGN(T, x) * (x))

// Main context for Pride to work with.
typedef struct PrideContext PrideContext;
struct PrideContext {
  u32 *pixels;
  size_t width;
  size_t height;
  size_t stride;
};

#define PRIDE_PIXEL(pc, x, y) (pc).pixels[(y) * (pc).stride + (x)]

PRIDEDEF b32 pride_in_bounds(PrideContext pc, int x, int y);
PRIDEDEF void pride_blend_color(u32 *c1, u32 c2);
// PRIDEDEF void pride_fill(PrideContext pc, u32 color);
// PRIDEDEF void pride_rect(PrideContext pc, int x, int y, int w, int h, u32 color); 
// PRIDEDEF void pride_frame(PrideContext pc, int x, int y, int w, int h, size_t thiccness, u32 color);
// PRIDEDEF void pride_circle(PrideContext pc, int cx, int cy, int r, u32 color);
// PRIDEDEF void pride_ellipse(PrideContext pc, int cx, int cy, int rx, int ry, u32 color);
PRIDEDEF void pride_dot(PrideContext pc, int x, int y, u32 color);
PRIDEDEF void pride_line(PrideContext pc, int x1, int y1, int x2, int y2,
                         u32 color);
PRIDEDEF b32 pride_normalize_triangle(size_t width, size_t height, int x1,
                                      int y1, int x2, int y2, int x3, int y3,
                                      int *lx, int *hx, int *ly, int *hy);
PRIDEDEF b32 pride_barycentric(int x1, int y1, int x2, int y2, int x3, int y3,
                               int xp, int yp, int *u1, int *u2, int *det);
PRIDEDEF void pride_triangle(PrideContext pc, int x1, int y1, int x2, int y2,
                             int x3, int y3, u32 color);
PRIDEDEF void pride_triangle3c(PrideContext pc, int x1, int y1, int x2, int y2,
                               int x3, int y3, u32 c1, u32 c2, u32 c3);
// PRIDEDEF void pride_triangle3z(PrideContext pc, int x1, int y1, int x2, int y2, int x3, int y3, f32 z1, f32 z2, f32 z3);
// PRIDEDEF void pride_triangle3uv(PrideContext pc, int x1, int y1, int x2, int y2, int x3,
// int y3, f32 tx1, f32 ty1, f32 tx2, f32 ty2, f32 tx3, f32 ty3, f32 z1, f32 z2,
// f32 z3, PrideContext texture); 
// PRIDEDEF void pride_triangle3uv_bilinear(PrideContext pc, int x1, int y1, int x2, int y2,
// int x3, int y3, f32 tx1, f32 ty1, f32 tx2, f32 ty2, f32 tx3, f32 ty3, f32 z1,
// f32 z2, f32 z3, PrideContext texture);
// PRIDEDEF void pride_sprite_blend(PrideContext pc, int x, int y, int w, int h, PrideContext sprite);
// PRIDEDEF void pride_sprite_copy(PrideContext pc, int x, int y, int w, int h, PrideContext sprite);
// PRIDEDEF void pride_sprite_copy_bilinear(PrideContext pc, int x, int y, int w, int h, PrideContext sprite); 
// PRIDEDEF u32 pride_pixel_bilinear(PrideContext sprite, int nx, int ny, int w, int h);

#define PRIDE_RED(color) (((color) & 0x000000FF) >> (8 * 0))
#define PRIDE_GREEN(color) (((color) & 0x0000FF00) >> (8 * 1))
#define PRIDE_BLUE(color) (((color) & 0x00FF0000) >> (8 * 2))
#define PRIDE_ALPHA(color) (((color) & 0xFF000000) >> (8 * 3))
#define PRIDE_RGBA(r, g, b, a)                                                 \
  ((((r) & 0xFF) << (8 * 0)) | (((g) & 0xFF) << (8 * 1)) |                     \
   (((b) & 0xFF) << (8 * 2)) | (((a) & 0xFF) << (8 * 3)))

PRIDEDEF u32 mix_colors3(u32 c1, u32 c2, u32 c3, int u1, int u2, int det) {
  i64 r1 = PRIDE_RED(c1);
  i64 g1 = PRIDE_GREEN(c1);
  i64 b1 = PRIDE_BLUE(c1);
  i64 a1 = PRIDE_ALPHA(c1);

  i64 r2 = PRIDE_RED(c2);
  i64 g2 = PRIDE_GREEN(c2);
  i64 b2 = PRIDE_BLUE(c2);
  i64 a2 = PRIDE_ALPHA(c2);

  i64 r3 = PRIDE_RED(c3);
  i64 g3 = PRIDE_GREEN(c3);
  i64 b3 = PRIDE_BLUE(c3);
  i64 a3 = PRIDE_ALPHA(c3);

  if (det != 0) {
    int u3 = det - u1 - u2;
    i64 r4 = (r1 * u1 + r2 * u2 + r3 * u3) / det;
    i64 g4 = (g1 * u1 + g2 * u2 + g3 * u3) / det;
    i64 b4 = (b1 * u1 + b2 * u2 + b3 * u3) / det;
    i64 a4 = (a1 * u1 + a2 * u2 + a3 * u3) / det;

    return PRIDE_RGBA(r4, g4, b4, a4);
  }

  return 0;
}

PRIDEDEF void pride_blend_color(u32 *c1, u32 c2) {
  u32 r1 = PRIDE_RED(*c1);
  u32 g1 = PRIDE_GREEN(*c1);
  u32 b1 = PRIDE_BLUE(*c1);
  u32 a1 = PRIDE_ALPHA(*c1);

  u32 r2 = PRIDE_RED(c2);
  u32 g2 = PRIDE_GREEN(c2);
  u32 b2 = PRIDE_BLUE(c2);
  u32 a2 = PRIDE_ALPHA(c2);

  r1 = (r1 * (255 - a2) + r2 * a2) / 255;
  if (r1 > 255)
    r1 = 255;

  g1 = (g1 * (255 - a2) + g2 * a2) / 255;
  if (g1 > 255)
    g1 = 255;

  b1 = (b1 * (255 - a2) + b2 * a2) / 255;
  if (b1 > 255)
    b1 = 255;

  *c1 = PRIDE_RGBA(r1, g1, b1, a1);
}

// NOTE: Implying u3 = det - u1 - u2
PRIDEDEF b32 pride_barycentric(int x1, int y1, int x2, int y2, int x3, int y3,
                               int xp, int yp, int *u1, int *u2, int *det) {
  *det = ((x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3));
  *u1 = ((y2 - y3) * (xp - x3) + (x3 - x2) * (yp - y3));
  *u2 = ((y3 - y1) * (xp - x3) + (x1 - x3) * (yp - y3));
  int u3 = *det - *u1 - *u2;

  return ((PRIDE_SIGN(int, *u1) == PRIDE_SIGN(int, *det) || *u1 == 0) &&
          (PRIDE_SIGN(int, *u2) == PRIDE_SIGN(int, *det) || *u2 == 0) &&
          (PRIDE_SIGN(int, u3) == PRIDE_SIGN(int, *det) || u3 == 0))
             ? 1
             : 0;
}

PRIDEDEF b32 pride_in_bounds(PrideContext pc, int x, int y) {
  return 0 <= x && x < (int)pc.width && 0 <= y && y < (int)pc.height;
}

PRIDEDEF void pride_dot(PrideContext pc, int x, int y, u32 color) {
  if (pride_in_bounds(pc, x, y)) {
    pride_blend_color(&PRIDE_PIXEL(pc, x, y), color);
  }
  return;
}

PRIDEDEF void pride_line(PrideContext pc, int x1, int y1, int x2, int y2,
                         u32 color) {
  int dx = x2 - x1;
  int dy = y2 - y1;

  // If both of the differences are 0 there will be a division by 0 below.
  if (dx == 0 && dy == 0) {
    if (pride_in_bounds(pc, x1, y1)) {
      pride_blend_color(&PRIDE_PIXEL(pc, x1, y1), color);
    }
    return;
  }

  if (PRIDE_ABS(int, dx) > PRIDE_ABS(int, dy)) {
    if (x1 > x2) {
      PRIDE_SWAP(int, x1, x2);
      PRIDE_SWAP(int, y1, y2);
    }

    for (int x = x1; x <= x2; x++) {
      int y = dy * (x - x1) / dx + y1;
      if (pride_in_bounds(pc, x, y)) {
        pride_blend_color(&PRIDE_PIXEL(pc, x, y), color);
      }
    }
  } else {
    if (y1 > y2) {
      PRIDE_SWAP(int, x1, x2);
      PRIDE_SWAP(int, y1, y2);
    }

    for (int y = y1; y <= y2; y++) {
      int x = dx * (y - y1) / dy + x1;
      if (pride_in_bounds(pc, x, y)) {
        pride_blend_color(&PRIDE_PIXEL(pc, x, y), color);
      }
    }
  }
}

PRIDEDEF b32 pride_normalize_triangle(size_t width, size_t height, int x1,
                                      int y1, int x2, int y2, int x3, int y3,
                                      int *lx, int *hx, int *ly, int *hy) {
  *lx = x1;
  *hx = x1;
  if (*lx > x2)
    *lx = x2;
  if (*lx > x3)
    *lx = x3;
  if (*hx < x2)
    *hx = x2;
  if (*hx < x3)
    *hx = x3;
  if (*lx < 0)
    *lx = 0;
  if ((size_t)*lx >= width)
    return 0;
  if (*hx < 0)
    return 0;
  if ((size_t)*hx >= width)
    *hx = width - 1;

  *ly = y1;
  *hy = y1;
  if (*ly > y2)
    *ly = y2;
  if (*ly > y3)
    *ly = y3;
  if (*hy < y2)
    *hy = y2;
  if (*hy < y3)
    *hy = y3;
  if (*ly < 0)
    *ly = 0;
  if ((size_t)*ly >= height)
    return 0;
  if (*hy < 0)
    return 0;
  if ((size_t)*hy >= height)
    *hy = height - 1;

  return 1;
}

PRIDEDEF void pride_triangle(PrideContext pc, int x1, int y1, int x2, int y2,
                             int x3, int y3, u32 color) {
  int lx, hx, ly, hy;
  if (pride_normalize_triangle(pc.width, pc.height, x1, y1, x2, y2, x3, y3, &lx,
                               &hx, &ly, &hy)) {
    for (int y = ly; y <= hy; y++) {
      for (int x = lx; x <= hx; x++) {
        int u1, u2, det;
        if (pride_barycentric(x1, y1, x2, y2, x3, y3, x, y, &u1, &u2, &det)) {
          pride_blend_color(&PRIDE_PIXEL(pc, x, y), color);
        }
      }
    }
  }
}

PRIDEDEF void pride_triangle3c(PrideContext pc, int x1, int y1, int x2, int y2,
                               int x3, int y3, u32 c1, u32 c2, u32 c3) {
  int lx, hx, ly, hy;
  if (pride_normalize_triangle(pc.width, pc.height, x1, y1, x2, y2, x3, y3, &lx,
                               &hx, &ly, &hy)) {
    for (int y = ly; y <= hy; y++) {
      for (int x = lx; x <= hx; x++) {
        int u1, u2, det;
        if (pride_barycentric(x1, y1, x2, y2, x3, y3, x, y, &u1, &u2, &det)) {
          pride_blend_color(&PRIDE_PIXEL(pc, x, y),
                            mix_colors3(c1, c2, c3, u1, u2, det));
        }
      }
    }
  }
}

// TEST CODE

#include <stdlib.h>

int main(void) {
  size_t display_width = 640;
  size_t display_height = 320;

  PrideContext pc = {
      .pixels = (u32 *)malloc(sizeof(u32) * display_width * display_height),
      .width = display_width,
      .height = display_height,
      .stride = display_width};

  int cx = display_width / 2;
  int cy = display_height / 2;

  pride_triangle3c(pc, cx - display_width / 4, cy + display_height / 4,
                   cx + display_width / 4, cy + display_height / 4, cx,
                   cy - display_height / 4, 0xFFFF0000, 0xFF00FF00, 0xFF0000FF);

  return 0;
}
