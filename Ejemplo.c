
#include <stdlib.h>
#include <string.h>

#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

#include "mapa_prueba.h"

// link the pattern table into CHR ROM


//#resource "Sprites.chr"
//#link "Sprit.s"



// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"
#define TILE 0x80
#define ATTR 0x4

// define a 3x3 metasprite
#define DEF_METASPRITE_Maxio(name,code,pal)\
const unsigned char name[]={\
        0,      0,      (code)+0,   pal, \
        0,      8,     	(code)+3,   pal, \
        0,      16,      (code)+6,   pal, \
        8,      0,      (code)+1,   pal, \
  	8,      8,      (code)+4,   pal, \
        8,      16,      (code)+7,   pal, \
  	16,      0,      (code)+2,   pal, \
  	16,      8,     (code)+5,   pal, \
        16,      16,      (code)+8,   pal, \
        128};

#define DEF_METASPRITE_Maxio_FLIP(name,code,pal)\
const unsigned char name[]={\
        0,      0,      (code)+2,   (pal)|OAM_FLIP_H, \
        0,      8,     	(code)+5,   (pal)|OAM_FLIP_H, \
       0,      16,      (code)+8,   (pal)|OAM_FLIP_H, \
       8,      0,      (code)+1,   (pal)|OAM_FLIP_H, \
  	8,      8,      (code)+4,   (pal)|OAM_FLIP_H, \
      8,      16,      (code)+7,   (pal)|OAM_FLIP_H, \
  	16,      0,      (code)+0,   (pal)|OAM_FLIP_H, \
  	16,      8,     (code)+3,   (pal)|OAM_FLIP_H, \
       16,      16,     (code)+6,   (pal)|OAM_FLIP_H, \
        128};

DEF_METASPRITE_Maxio(playerRStand, 0x80, 0);
DEF_METASPRITE_Maxio(playerRRun1, 0x89, 0);
DEF_METASPRITE_Maxio(playerRRun2, 0x92, 0);
DEF_METASPRITE_Maxio(playerRRun3, 0x9b, 0);
DEF_METASPRITE_Maxio(playerRJump, 0xa4, 0);
DEF_METASPRITE_Maxio(playerRClimb, 0xa4, 0);
DEF_METASPRITE_Maxio(playerRSad, 0xa4, 0);

DEF_METASPRITE_Maxio_FLIP(playerLStand, 0x80, 0);
DEF_METASPRITE_Maxio_FLIP(playerLRun1, 0x89, 0);
DEF_METASPRITE_Maxio_FLIP(playerLRun2, 0x92, 0);
DEF_METASPRITE_Maxio_FLIP(playerLRun3, 0x9b, 0);
DEF_METASPRITE_Maxio_FLIP(playerLJump, 0xa4, 0);
DEF_METASPRITE_Maxio_FLIP(playerLClimb, 0xa4, 0);
DEF_METASPRITE_Maxio_FLIP(playerLSad, 0xa4, 0);

const unsigned char* const playerRunSeq[16] = {
  playerLRun1, playerLRun2, playerLRun3, 
  playerLRun1, playerLRun2, playerLRun3, 
  playerLRun1, playerLRun2,
  playerRRun1, playerRRun2, playerRRun3, 
  playerRRun1, playerRRun2, playerRRun3, 
  playerRRun1, playerRRun2,
};

const unsigned char palTitle[16]={ 0x30,0x0f,0x0f,0x0f,0x0f,0x1c,0x2c,0x3c,0x0f,0x12,0x22,0x32,0x0f,0x14,0x24,0x34 };
/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x2C,			// screen color

  0x16,0x3C,0x30,0x30,	// background palette 0
  0x1C,0x2A,0x0F,0x00,	// background palette 1
  0x00,0x10,0x2A,0x00,	// background palette 2
  0x06,0x16,0x26,0x00,   // background palette 3

  0x0F,0x36,0x2A,0xFF,	// sprite palette 0
  0x30,0x30,0x30,0x00,	// sprite palette 1
  0x30,0x30,0x30,0x00,	// sprite palette 2
  0x0D,0x27,0x2A	// sprite palette 3
};

// setup PPU and tables
void setup_graphics() {
  // clear sprites
  oam_clear();
  // set palette colors
  pal_all(PALETTE);
  
  ppu_on_all();
}
#define NUM_ACTORS 2		// 64 sprites (maximum)

// actor x/y positions
byte actor_x[NUM_ACTORS];
byte actor_y[NUM_ACTORS];
// actor x/y deltas per frame (signed)
sbyte actor_dx[NUM_ACTORS];
sbyte actor_dy[NUM_ACTORS];



void fade_in() {
  byte vb;
  for (vb=0; vb<=4; vb++) {
    // set virtual bright value
    pal_bright(vb);
    // wait for 4/60 sec
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
    
  }
}
/*
void titulo() {
  // disable rendering
  ppu_off();
  // set palette, virtual bright to 0 (total black)
  pal_bg(palTitle);
  pal_bright(0);
  // unpack nametable into the VRAM
  vram_adr(0x2000);
  vram_unrle(mapa_prueba);
  // enable rendering
  ppu_on_all();
  // fade in from black
  fade_in();
}*/



void main() {
  char i;     //char i;	
  char oam_id;	// sprite ID
  char pad;
  
  // initialize actors with random values
  
  vram_adr(NAMETABLE_A);
  vram_unrle(mapa_prueba);
  
  
  
   for (i=0; i<NUM_ACTORS; i++) {
    actor_x[0] = 0*32+128;
    actor_y[0] = 0*8+64;
    actor_dx[i] = 0;
    actor_dy[i] = 0;
  }
  
  // initialize PPU
  setup_graphics();
  // loop forever
  while (1) {
    // start with OAMid/sprite 0
    oam_id = 0;
    // set player 0/1 velocity based on controller
    for (i=0; i<NUM_ACTORS; i++) {
      // poll controller i (0-1)
      pad = pad_poll(i);
      // move actor[i] left/right
      if (pad&PAD_LEFT && actor_x[i]>0) actor_dx[i]=-2;
      else if (pad&PAD_RIGHT && actor_x[i]<240) actor_dx[i]=2;
      else actor_dx[i]=0;
      // move actor[i] up/down
      if (pad&PAD_UP && actor_y[i]>0) actor_dy[i]=-2;
      else if (pad&PAD_DOWN && actor_y[i]<212) actor_dy[i]=2;
      else actor_dy[i]=0;
    }
    // draw and move all actors
    for (i=0; i<NUM_ACTORS; i++) {
      byte runseq = actor_x[i] & 7;
      if (actor_dx[i] >= 0)
        runseq += 8;
      
      oam_id = oam_meta_spr(actor_x[i], actor_y[i], oam_id, playerRunSeq[runseq]);
      actor_x[i] += actor_dx[i];
      actor_y[i] += actor_dy[i];
    }
    // hide rest of sprites
    // if we haven't wrapped oam_id around to 0
    if (oam_id!=0) oam_hide_rest(oam_id);
    // wait for next frame    
    ppu_wait_frame();
  }
}