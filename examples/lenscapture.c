/* -*- mode: c; c-basic-offset: 2; indent-tabs-mode: nil; -*-
 * Using the C-API of this library.  */

#define _GNU_SOURCE

#include "led-matrix-c.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "bcm_host.h"

#ifndef ALIGN_TO_16
#define ALIGN_TO_16(x)  ((x + 15) & ~15)
#endif

#define DEFAULT_DISPLAY_NUMBER 0

//-----------------------------------------------------------------------

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

//-----------------------------------------------------------------------

int main(int argc, char **argv) {
  struct RGBLedMatrixOptions options;
  struct RGBLedMatrix *matrix;
  struct LedCanvas *offscreen_canvas;
  int ledWidth, ledHeight, result = 0;
  uint32_t displayNumber = DEFAULT_DISPLAY_NUMBER;
  VC_IMAGE_TYPE_T imageType = VC_IMAGE_RGBA32;
  int32_t i, j;
  uint8_t rr,gg,bb;
  uint32_t xa=0, ya=0, xb=0, yb=0;
  uint32_t Xoffset, Yoffset;
  VC_RECT_T rect;

  bcm_host_init();

  //-------------------------------------------------------------------
  // Init RGB Panel
  memset(&options, 0, sizeof(options));
  options.rows = 32;
  options.chain_length = 5;
  options.parallel = 2;

  /* This supports all the led commandline options. Try --led-help */
  matrix = led_matrix_create_from_options(&options, &argc, &argv);
  if (matrix == NULL)
    return 1;

  /* Let's do an example with double-buffering. We create one extra
   * buffer onto which we draw, which is then swapped on each refresh.
   * This is typically a good aproach for animations and such.  */
  offscreen_canvas = led_matrix_create_offscreen_canvas(matrix);

  led_canvas_get_size(offscreen_canvas, &ledWidth, &ledHeight);

  //-------------------------------------------------------------------
  DISPMANX_DISPLAY_HANDLE_T displayHandle = vc_dispmanx_display_open(displayNumber);
  if (displayHandle == 0) {
    fprintf(stderr,"unable to open display handle");
    exit(EXIT_FAILURE);
  }

  DISPMANX_MODEINFO_T modeInfo;
  result = vc_dispmanx_display_get_info(displayHandle, &modeInfo);

  if (result != 0) {
    fprintf(stderr, "unable to get display information\n");
    exit(EXIT_FAILURE);
  }

  uint32_t pngWidth = modeInfo.width;
  uint32_t pngHeight = modeInfo.height;

  fprintf(stderr, "Screen size: %d x %d \n", pngWidth, pngHeight);

  fprintf(stderr, "Leds Size: %d x %d.\n", ledWidth, ledHeight);

  //Calcul du plus gros snapshot sans deformation
  if ( (ledWidth/ledHeight)>(pngWidth/pngHeight) ) {
    // si le panneau est plus horizontal large que la TV
    fprintf(stderr, "Led Matrix plus Large !\n");
    xa = 0; ya = 0;
    xb = pngWidth;
    yb = (pngHeight*ledHeight)/ledWidth;
  }
  else 
  {
    // si le panneau est plus haut que la TV
    fprintf(stderr, "Led Matrix plus Haute !\n");
    xa = 0; ya = 0;
    xb = (pngWidth*ledWidth)/ledHeight;
    yb = pngHeight;
  }

  //---------------------------------------------------------
  int opt;
  while ((opt = getopt(argc, argv, "a:b:c:d:")) != -1) {
    switch (opt) {
    case 'a':
      xa = atoi(optarg);
      break;
    case 'b':
      ya = atoi(optarg);
      break;
    case 'c':
      xb = atoi(optarg);
      break;
    case 'd':
      yb = atoi(optarg);
      break;
    default: break;
    }
  }

  fprintf(stderr, "Sprite coins : %d-%d x %d-%d.\n", xa,ya,xb,yb);
  fprintf(stderr, "Sprite dims : %d x %d.\n", xb-xa,yb-ya);
  
  //-------------------------------------------------------------

  uint32_t dmxBytesPerPixel = 4;
  uint32_t dmxPitch = dmxBytesPerPixel * ALIGN_TO_16(pngWidth);
  uint8_t *dmxImagePtr = (uint8_t*)malloc(dmxPitch * (pngHeight));
  if (dmxImagePtr == NULL) {
    fprintf(stderr, "unable to allocated image buffer\n");
    exit(EXIT_FAILURE);
  }
  
  uint32_t vcImagePtr = 0;
  DISPMANX_RESOURCE_HANDLE_T resourceHandle;
  resourceHandle = vc_dispmanx_resource_create(imageType,
                                     pngWidth, pngHeight, &vcImagePtr);

 char k[1];
  while (1) {

  uint32_t thoX = (1+xb-xa) / ledWidth;
  uint32_t thoY= (1+yb-ya) / ledHeight;

  fprintf(stderr, "Sprite/led factor : %f x %f.\n", thoX,thoY); 

  while (!interrupt_received) {
   // It is always good to set up a signal handler to cleanly exit when we
   // receive a CTRL-C for instance. The DrawOnCanvas() routine is looking
   // for that.
   signal(SIGTERM, InterruptHandler);
   signal(SIGINT, InterruptHandler);

   result = vc_dispmanx_snapshot(displayHandle, resourceHandle, DISPMANX_NO_ROTATE);
   if (result != 0) {
      vc_dispmanx_resource_delete(resourceHandle);
      vc_dispmanx_display_close(displayHandle);
      fprintf(stderr, "vc_dispmanx_snapshot() failed\n");
      exit(EXIT_FAILURE);
   }
  
    result = vc_dispmanx_rect_set(&rect, 0, 0, pngWidth, pngHeight);
    if (result != 0) {
       vc_dispmanx_resource_delete(resourceHandle);
       vc_dispmanx_display_close(displayHandle);
       fprintf(stderr, "vc_dispmanx_rect_set() failed\n");
       exit(EXIT_FAILURE);
    }

   result = vc_dispmanx_resource_read_data(resourceHandle, &rect, dmxImagePtr,dmxPitch);
   if (result != 0) {
      vc_dispmanx_resource_delete(resourceHandle);
      vc_dispmanx_display_close(displayHandle);
      fprintf(stderr, "vc_dispmanx_resource_read_data() failed\n");
      exit(EXIT_FAILURE);
   }

    // Convert from RGBA (32 bit) to RGB (24 bit)
    for (j = 0 ; j < ledHeight-1 ; j++) {
       Yoffset = ( ya+(j * thoY)) * dmxPitch;
       for (i = 0 ; i < ledWidth ; i++) {
          Xoffset = (xa+ (i*thoX)) * dmxBytesPerPixel;
          uint8_t *dmxPixelPtr = dmxImagePtr + Xoffset + Yoffset;
          rr = *dmxPixelPtr++;
          gg = *dmxPixelPtr++;
          bb = *dmxPixelPtr++;
          led_canvas_set_pixel(offscreen_canvas, i, j, rr, gg, bb);
       }
     }
 
     /* Now, we swap the canvas. We give swap_on_vsync the buffer we
     * just have drawn into, and wait until the next vsync happens.
     * we get back the unused buffer to which draw in the next iteration.  */
     offscreen_canvas = led_matrix_swap_on_vsync(matrix, offscreen_canvas);
  }

  interrupt_received=false;
  fprintf(stderr, "Enter a key :");
  scanf("%s", &k);
  fprintf(stderr, " key = %s\n",k);

  if (k[0]=='4') {
    uint32_t dx=pngWidth/50;
    if (xa >= dx) {xa -= dx;  xb -= dx;}
    fprintf(stderr, "New png Sprite %d x %d - %d x %d\n",xa,ya,xb,yb);
  }
  if (k[0]=='6') {
    uint32_t dx=pngWidth/50;
    if (xb+dx < pngWidth) {xa += dx;  xb += dx;}
    fprintf(stderr, "New png Sprite %d x %d - %d x %d\n",xa,ya,xb,yb);
  }

  if (k[0]=='8') {
    uint32_t dy=pngHeight/40;
    if (ya > dy) {ya -= dy;  yb -= dy;}
    fprintf(stderr, "New png Sprite %d x %d - %d x %d\n",xa,ya,xb,yb);
  }
  if (k[0]=='2') {
    uint32_t dy=pngHeight/40;
    if (yb+dy < pngHeight) {ya += dy;  yb += dy;}
    fprintf(stderr, "New png Sprite %d x %d - %d x %d\n",xa,ya,xb,yb);
  }
  // ZOOM +
  if (k[0]=='+') {
    uint32_t cx = (xa+xb)/2;
    uint32_t dx = (xb-xa)*0.9;
    xa=cx-(dx/2);
    xb=cx+(dx/2);
    uint32_t cy = (ya+yb)/2;
    uint32_t dy = (yb-ya)*0.9;
    ya=cy-(dy/2);
    yb=cy+(dy/2);
    fprintf(stderr, "New png Sprite %d x %d - %d x %d\n",xa,ya,xb,yb);
  }

  // ZOOM -
  if (k[0]=='-') {
    uint32_t cx = (xa+xb)/2;
    uint32_t dx = (xb-xa)*1.1;
    if (cx > (dx/2)) xa=cx-(dx/2); else xa=0;
    xb=cx+(dx/2);
    if (xb > pngWidth) xb=pngWidth; 
    uint32_t cy = (ya+yb)/2;
    uint32_t dy = (yb-ya)*1.1;
    if (cy > (dy/2)) ya=cy-(dy/2); else ya=0;
    yb=cy+(dy/2);
    if (yb > pngHeight) yb=pngHeight;
    fprintf(stderr, "New png Sprite %d x %d - %d x %d\n",xa,ya,xb,yb);
  }

  if (k[0]=='q') break;  
 }

  vc_dispmanx_resource_delete(resourceHandle);
  vc_dispmanx_display_close(displayHandle);
  
  free(dmxImagePtr);
  dmxImagePtr = NULL;

  led_canvas_clear(offscreen_canvas);
  offscreen_canvas = led_matrix_swap_on_vsync(matrix, offscreen_canvas);
  usleep(1000);

  /* Make sure to always call led_matrix_delete() in the end to reset the
   * display. Installing signal handlers for defined exit is a good idea. */
  led_matrix_delete(matrix);

  fprintf(stderr, "Fin myscreen ...\n");
  
  return 0;
}
