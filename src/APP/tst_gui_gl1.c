/*!
\file ../APP/tst_gui_gl1.c
\brief handle OpenGL-callbacks for tst_gui1.c
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



#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#endif


#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>

#include "../ut/ut_umem.h"
#include "../gui/gui__.h"             // GUI_*




//================================================================
  int tst_gl_move (MemObj *mo, void **data) {
//================================================================

  printf("tst_gl_move event=%d %d %d\n",
         GUI_DATA_EVENT, GUI_DATA_I1, GUI_DATA_I2);

  return 0;

}


//================================================================
  int tst_gl_button (MemObj *mo, void **data) {
//================================================================

  printf("tst_gl_button event=%d %d\n",
         GUI_DATA_EVENT, GUI_DATA_I1);

  return 0;

}


//================================================================
  int tst_gl_key (MemObj *mo, void **data) {
//================================================================

  static int iCur = 0;

  printf("tst_gl_key event=%d %d\n",
         GUI_DATA_EVENT, GUI_DATA_I1);

  if(GUI_DATA_EVENT == TYP_EventPress) {

    if(GUI_DATA_I2 == 'c') {
      if(iCur < 5) ++iCur;
      else iCur = 0;
      GUI_cursor__ (mo, iCur);
    }

    if(GUI_DATA_I2 == 'b') {
      GUI_gl_block (mo, 1);
    }

  }

  return 0;

}



//================================================================
  int tst_gl_draw (MemObj *mo, void **data) {
//================================================================


  printf("tst_gl_draw event=%d %d %d\n",
         GUI_DATA_EVENT, GUI_DATA_I1, GUI_DATA_I2);

  GUI_gl_set_active (1, mo);


  // config:
  if(GUI_DATA_EVENT == TYP_EventConfig) {
    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);
    glEnable (GL_CULL_FACE);
    glCullFace (GL_BACK);
    glDisable (GL_DITHER);
    glShadeModel (GL_SMOOTH);
    glViewport (0, 0, GUI_DATA_I1, GUI_DATA_I2);
  }


  // draw:
    glClear (GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,100, 100,0, -1,1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1,1,1);
    glBegin(GL_TRIANGLES);
    glVertex2f(10,10);
    glVertex2f(10,90);
    glVertex2f(90,90);
    glEnd();


  GUI_gl_set_active (0, mo);

  return 0;     // TRUE=1   FALSE=0

}


// EOF
