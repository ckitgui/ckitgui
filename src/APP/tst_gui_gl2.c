/*
 *
 * Copyright (C) 2015 CADCAM-Servies Franz Reiter (franz.reiter@cadcam.co.at)
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
 *
-----------------------------------------------------
TODO:
  ..

-----------------------------------------------------
Modifications:
  ..

-----------------------------------------------------
*/
/*!
\file ../APP/tst_gui_gl2.c
\brief handle OpenGL-callbacks for tst_gui2.c
\code

=====================================================
List_functions_start:

gl_init__
gl_init_trackball
gl_move
gl_button
gl_key
gl_draw
gl_config

List_functions_end:
=====================================================

\endcode *//*

*/


#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>

#include "../ut/ut_umem.h"
#include "../ut/ckit_ut.h"

#include "../gui/gui__.h"             // GUI_*

#include "tst_gui_gl2.h"
#include "tst_geo.h"                    // Pointf
#include "trackball.h"



int    gl_stat = 0;    // true if initgl not yet called
float  beginx,beginy;  // position of mouse
float  quat[4];        // orientation of object
float  zoom;           // field of view in degrees

Pointf cenPt;          // view-center in user-coords

// Modelsize; box from 2 points; all coords must be inside this box.
Pointf mdlMax = {30.0, 30.0,  30.0};
Pointf mdlMin = {-30.0, -30.0,  -30.0};

int    actBut = 0;



static int    wSizX=200, wSizY=200;




//================================================================
void gl_init_trackball (int mode) {
//================================================================
/// init trackball

  printf("gl_init_trackball %d\n",mode);

  
  // set up trackball
  beginx = 0;
  beginy = 0;

  if(mode != 0) return;

  zoom   = 1.;

  cenPt.x  = 0.;
  cenPt.y  = 0.;
  cenPt.z  = 0.;

  trackball (quat, 0.0, 0.0, 0.0, 0.0);

  return;

} 


//====================================================================
void gl_init__ (void) {
//====================================================================
/// init OpenGL

  GLfloat light0_pos[4]   = { -50.0, 50.0, 0.0, 0.0 };
  GLfloat light0_color[4] = { .6, .6, .6, 1.0 }; /* white light */
  GLfloat light1_pos[4]   = {  50.0, 50.0, 0.0, 0.0 };
  GLfloat light1_color[4] = { .4, .4, 1, 1.0 };  /* cold blue light */


  printf("gl_init__\n");

  /* remove back faces */
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  // glDepthFunc(GL_LESS);

  /* light */
  glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_color);
  glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
  glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_color);
  glEnable(GL_LIGHT0);
  // glEnable(GL_LIGHT1);
  glEnable(GL_LIGHTING);

  glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

  glViewport (0, 0, wSizX, wSizY);

}


//================================================================
  int gl_move (MemObj *mo, void **data) {
//================================================================
/// callback from mouse-move. if mousbuttons down: rotate|zoom|pan.

  int   x, y;


  // printf("gl_move %d %d %d\n",x,y,actBut);
  // printf("gl_move event=%d device=%d x=%d  y=%d\n",
           // GUI_DATA_EVENT,    // TYP_EventMove
           // GUI_DATA_DEVICE,   // TYP_DeviceMouse
           // GUI_DATA_I1,       // x-val mousepos
           // GUI_DATA_I2);      // y-val mousepos

  // printf(" modif=%d\n",GUI_keys_mod());

  x = GUI_DATA_I1;  // *(int*)data[2];
  y = GUI_DATA_I2;


  if (actBut == 1) {   // rotate
    trackball_rot (quat, beginx, beginy, x, y, wSizX, wSizY, &cenPt);
    gl_draw (mo, NULL);
  }

  if (actBut == 2) {   // zoom
    trackball_zoom (&zoom, beginx, beginy, x, y, wSizX, wSizY);
    gl_draw (mo, NULL);
  }

  if (actBut == 3) {   // pan
    trackball_pan (&cenPt, beginx, beginy, x, y, wSizX, wSizY);
    gl_draw (mo, NULL);
  }

  beginx = x;
  beginy = y;

  return 0;

}


//================================================================
  int gl_button (MemObj *mo, void **data) {
//================================================================
/// mousebuttons changed: save state.

  double        fs;
  Obj_gui2      *go;

  // printf("gl_button %d %d\n",iButt,iStat);
  printf("gl_button event=%d key=%d\n",
           GUI_DATA_EVENT,    // TYP_EventPress|TYP_EventRelease
           GUI_DATA_I2);      // 1=left MB; 2=middle MB; 3=right MB; 4=wheel


  // mousewheel/scroll: do zoom
  if(GUI_DATA_I2 == GUI_MouseScUp) {
    fs = 1.2;
    goto L_zoom;
  }
    
  if(GUI_DATA_I2 == GUI_MouseScDown) {
    fs = 1. / 1.2;
    goto L_zoom;
  }


  // button-release
  if(GUI_DATA_EVENT == TYP_EventRelease) { actBut = 0; return 0; }

  // button down
  actBut = GUI_DATA_I2;

  return 0;


  //----------------------------------------------------------------
  L_zoom:
    zoom *= fs;
      printf(">>>>>>>>>>>>>>>>>>>>>> zoom=%f\n",zoom);

    gl_draw (mo, NULL);
    return 0;

}


//================================================================
  int gl_key (MemObj *mo, void **data) {
//================================================================
/// keyboard-event. Key 'B': block input.

  int  iKey, iMod;

  printf("gl_key event=%d key=%d  modif=%d\n",
           GUI_DATA_EVENT,    // event:    TYP_EventPress|TYP_EventRelease
           GUI_DATA_I2,       // key:      ascii-value of key
           GUI_DATA_I3);      // modifier: &1=shift; &4=ctrl; &8=alt.




  if(GUI_DATA_EVENT == TYP_EventRelease) return 0;     // skip keyRelease


  iMod = GUI_DATA_I3;
  if(iMod & 256)  printf(" mousebutton 1 = ON\n");
  if(iMod & 512)  printf(" mousebutton 2 = ON\n");
  if(iMod & 1024) printf(" mousebutton 3 = ON\n");


  iKey = GUI_DATA_I2;


  // key b: block all input
  if(iKey == 'b') {
    GUI_gl_block (mo, 1);       // block
    GUI_cursor__ (mo, 1);       // cursor wait
  }


  if(iKey == 'd') {
    gl_draw (mo, NULL);
  }


  if(iKey == 'r') {           // reset zoom
    zoom   = 1.;
    gl_draw (mo, NULL);
  }


  return 0;

}

/*
//================================================================
  int gl_idle (void *data) {
//================================================================

  // printf(".");


  // if(gl_stat == 0) {
    // gl_draw (UI_win_gl, NULL);
    // return 1;
  // }


  // if(GLB_pending()) return 1;
  // if(GUI_gl_pending()) return 1;
  // if(gtk_events_pending()) return 1;


  if(gl_stat == 1) {
    printf("gl_idle 1 -> 2\n");
    // gtk_widget_map  (UI_win_glArea);
    gl_stat = 2;
    // GUI_update__ ();
    // gl_draw (UI_win_gl, NULL);
    // g_signal_emit_by_name (UI_win_gl, "draw", 0,NULL);
    // gdk_window_end_paint (gtk_widget_get_window(UI_win_gl));
    // gl_draw (UI_win_gl, NULL);
    // gtk_widget_map  (UI_win_glArea);
    gl_draw (UI_win_gl, NULL);
    // gtk_widget_queue_resize (UI_win_gl);
  }


  return 1;  // 0=remove, 1=keep

}
*/


//================================================================
  int gl_draw (MemObj *mo, void **data) {
//================================================================
/// Redraw necessary. Redraw whole scene.
///   GUI_DATA_EVENT =*(int*)data[0]=TYP_EventConfig|TYP_EventDraw
///   GUI_DATA_DEVICE=*(int*)data[1]=TYP_DeviceScreen0
///   GUI_DATA_I1    =*(int*)data[2]=window-width (for event=TYP_EventConfig)
///   GUI_DATA_I2    =*(int*)data[3]=window-height (for event=TYP_EventConfig)


  // int        i1;
  GLfloat    m[4][4];


  printf("gl_draw stat=%d actBut=%d\n",gl_stat,actBut);


  if(data) {     // internal draws: data=NULL
    if(GUI_DATA_EVENT == TYP_EventEnter) {
      printf(" enter gl-win, shift/ctrl/alt=%d\n",GUI_DATA_I1);

    } else if(GUI_DATA_EVENT == TYP_EventConfig) {
      wSizX = GUI_DATA_I1;
      wSizY = GUI_DATA_I2;
      if(gl_stat > 2) gl_stat = 2;  // do not overwrite gl_stat=0
        printf(" new siz %d %d\n",wSizX,wSizY);
    }
  }


  // activate GL
  GUI_gl_set_active (1, mo);


  if(gl_stat == 2) {           // after config after reEnter
    gl_init_trackball (1);     // set up trackball
    gl_init__ ();
  }


    // basic initialization
    if (gl_stat == 0) {
      gl_init_trackball (0);     // set up trackball
      gl_init__ ();
    }


    // set backgrnd
    L_draw:
    glClearColor(.3,.4,.6,1);   // hellblau
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // set view
    trackball_persp (&cenPt, &zoom, quat, &mdlMax, &mdlMin);

    // draw all 
    tst_surf2_draw ();


  // execute GL
  GUI_gl_set_active (0, mo);

  gl_stat = 3;

  return 0;

}


// EOF
