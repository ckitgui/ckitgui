/*!
\file ../APP/tst_gui2.c
\brief Test with OpenGL-window
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
2012-04-24 neu erstellt. RF.

-----------------------------------------------------

make run -f tst_gui2.mak "OS=Linux32"

export LD_LIBRARY_PATH=~/binLinux32 && ~/binLinux32/a.out


# if menu is unvisible: (ubuntu 11):
if [ -n "$UBUNTU_MENUPROXY" ]; then
  echo "disactivate ubuntu-global-menu"
  export UBUNTU_MENUPROXY=
fi



*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>                    // strcpy

#include "../ut/ut_cast.h"             // INT_PTR
#include "../ut/ut_umem.h"

#include "../gui/gui__.h"

#include "tst_gui_gl2.h"


static MemObj     tb1;                   // toolbox
static MemObj     rb1;                   // radioButton 1
static MemObj     wmsg;                  // Messagewindow
static MemObj     went1, went2;          // entry
static MemObj     wom1, wom2;            // optMenus
static MemObj     btList1, btList2;      // buttons List1 & 2
static MemObj     wBoxl, wList1, wgl;


// protos:
 int cb_exit     (MemObj *mo, void **data);

 int tst_CB_b0   (MemObj *mo, void **data);
 int tst_CB_b1   (MemObj *mo, void **data);
 int tst_CB_b2   (MemObj *mo, void **data);
 int tst_CB_w2   (MemObj *mo, void **data);
 int tst_CB_b3   (MemObj *mo, void **data);
 int tst_CB_b4   (MemObj *mo, void **data);
 int tst_CB_wsl  (MemObj *mo, void **data);
 int tst_CB_cb1  (MemObj *mo, void **data);


  int UI_main    (MemObj *mo, void **data);
  int cb_pop1    (MemObj *mo, void **data);
  int cb_rb1     (MemObj *mo, void **data);
  int cb_men1    (MemObj *mo, void **data);
  int cb_men2    (MemObj *mo, void **data);
  int cb_went1   (MemObj *mo, void **data);
  int cb_went2   (MemObj *mo, void **data);
  int cb_ntb     (MemObj *mo, void **data);
  int cb_keys    (MemObj *mo, void **data);
  int cb_reset   (MemObj *mo, void **data);
  int c_fileO    (MemObj *mo, void **data);
  int c_fileS    (MemObj *mo, void **data);
  int cb_list0cb (MemObj *mo, void **data);
  int cb_list1__ (MemObj *mo, void **data);
  int cb_list1cb (MemObj *mo, void **data);
  int cb_list2__ (MemObj *mo, void **data);
  int cb_list2cb (MemObj *mo, void **data);




//================================================================
int main (int argc, char *argv[]) {
//================================================================

  MemObj window;
  MemObj boxv0, boxh0, boxh1, boxh2, boxh3, boxh4, boxh5, boxh6;
  MemObj wsl, wf1, menu;
  MemObj wt1, wt2, wt3;
  char *optLst1[]={"combo1","Jaenner-Monat","Februar","Maerz",NULL};
  char *tipLst1[]={"monate ..","this is the january","engl: february","mmm"};
  char *optLst2[]={"combo2","Maerz","April","Mai","Juni",NULL};
  char *titLst0[]={"Filenames"};




  // if(GUI_Init ()) return -1;
  GUI_Init__ ("");

  // Create Mainwindow
  window = GUI_Win__ ("Gtk-Toolbox test", cb_exit, "");

  // vertical box
  boxv0 = GUI_box_v (&window, "e,e");
  boxh0 = GUI_box_h (&boxv0, "");


  //----------------------------------------------------------------
  // menu
  wt1 = GUI_menu__ (&boxh0, NULL, 1);    // the toolbar
  wt2 = GUI_menu__ (&wt1, "File ", 0);

  GUI_menu_entry  (&wt2, "New",     UI_main,      (void*)"new");
    GUI_Tip ("tip new");
  GUI_menu_entry  (&wt2, "Open",    UI_main,      (void*)"open");
    GUI_Tip ("tip open");
  GUI_menu_entry  (&wt2, "Save",    UI_main,      (void*)"save");

  wt3 = GUI_menu__ (&wt2, "Save as ", 0);
  GUI_menu_entry   (&wt3, "dxf", UI_main,      (void*)"saveDxf");
  GUI_menu_entry   (&wt3, "svg", UI_main,      (void*)"saveSvg");
  GUI_menu_entry   (&wt3, "igs", UI_main,      (void*)"saveIgs");

  GUI_menu_entry   (&wt2, "---",     NULL,         NULL);  // separator-line
  GUI_menu_entry   (&wt2, "Exit",    cb_exit,      NULL);

  wt2 = GUI_menu__ (&wt1, "Options ", 0);
  GUI_menu_entry   (&wt2, "opt1",    UI_main,      (void*)"opt1");
  GUI_menu_entry   (&wt2, "opt2",    UI_main,      (void*)"opt2");
  GUI_menu_entry   (&wt2, "---",     NULL,         NULL);  // separator-line
  GUI_menu_checkbox__  (&wt2, "Toolbar", 1, UI_main,      (void*)"tb");

  wt2 = GUI_menu__ (&wt1, "Lang ", 0);
        GUI_menu_radiobutt__ (&wt2, "de", 0,  UI_main, "de");
  wt3 = GUI_menu_radiobutt__ (&wt2, "en", 1,  UI_main, "en");
        GUI_menu_radiobutt__ (&wt2, "es", 1,  UI_main, "es");
        GUI_menu_radiobutt_set (&wt3);               // preset en

  wt2 = GUI_menu__ (&wt1, "Help ", 'r');
  GUI_menu_entry   (&wt2, "Help",    UI_main,      (void*)"help");


  //----------------------------------------------------------------
  // gtk_label.h
  boxh1 = GUI_box_h (&boxv0, "");

  // backgroundtext
  GUI_label__ (&boxh1, "Label1", "");

  // fontsize, background, foreground (textcolor), bold ..
  GUI_label_htm__ (&boxh1, 
  "<span size=\"x-large\" bgcolor=\"#ff0000\" weight=\"bold\"> Html </span>",
  "");


  //----------------------------------------------------------------
  // gtk_entry.h
  // GUI_entry__ (&boxh1, " Name ", "", NULL, -5);
  went1 = GUI_entry__ (&boxh1, NULL, "ab", cb_went1, NULL, "5");
  went2 = GUI_entry__ (&boxh1, NULL, "de", cb_went2, NULL, "10");


  //----------------------------------------------------------------
  // gtk_image.h
  GUI_img__ (&boxh1, "../../icons/xa_logo.xpm", "");


  //----------------------------------------------------------------
  // hor. box
  // boxh2 = GUI_box_h (&boxv0, 10);

  tb1 = GUI_toolbox__ (&boxv0);
  wt1 = GUI_box_h (&tb1, "");

  // button with image
  GUI_button_img (&wt1, "../../icons/link.png", tst_CB_b0, NULL, "");

  // gtk_button.h
  GUI_button__ (&wt1, "GUI_MsgBox",       tst_CB_b1, "test1", "");
  GUI_button__ (&wt1, "GUI_DialogYN",     tst_CB_b2, NULL, "");
  GUI_button__ (&wt1, "GUI_DialogEntry",  tst_CB_b3, NULL, "");
  GUI_button__ (&wt1, "GUI_Color_select", tst_CB_b4, NULL, "");


  //----------------------------------------------------------------
  boxh2 = GUI_box_h (&boxv0, "");


  // radio-buttons
  rb1 = 
  GUI_radiobutt__ (&boxh2, "rb1 ", 0, cb_rb1, "rb1", "");
  GUI_radiobutt__ (&boxh2, "rb2 ", 1, cb_rb1, "rb2", "");
  GUI_radiobutt__ (&boxh2, "rb3 ", 1, cb_rb1, "rb3", "");

  // GUI_spc__ (&boxh2, 0, 12);   // hor. space between widgets
  GUI_spc_h (&boxh2, 12);   // hor. space between widgets

  wsl = GUI_Slider__ (&boxh2, 0, tst_CB_wsl, 0., 100., 50., "10");
  GUI_Slider_set (&wsl, 25.);

  // GUI_spc__ (&boxh2, 0, 12);  // hor. space between widgets
  GUI_spc_h (&boxh2, 12);  // hor. space between widgets

  // checkbox (in frame)
  wf1 = GUI_frame__ (&boxh2, "Frame", 0);
  boxh2 = GUI_box_h (&wf1, "");
  GUI_ckbutt__ (&boxh2, "ckb 1", 1, tst_CB_cb1, NULL, "");


  //----------------------------------------------------------------
  boxh3 = GUI_box_h (&boxv0, "");
  GUI_button__ (&boxh3, "GUI_Color_select", tst_CB_b4, NULL, "");

  wom1 = GUI_optmen__ (&boxh3, "menu1", optLst1, tipLst1, cb_men1, "12");
  wom2 = GUI_optmen__ (&boxh3, "menu2", optLst2, NULL,    cb_men2, "12");


  //----------------------------------------------------------------
  boxh4 = GUI_box_h (&boxv0, "");
  btList1 = GUI_button__ (&boxh4, "List1",      cb_list1__, NULL, "");

  btList2 = GUI_button__ (&boxh4, "List2",      cb_list2__, NULL, "");

            GUI_button__ (&boxh4, "test cursor",cb_keys,    NULL, "");
            GUI_button__ (&boxh4, "reset GL",   cb_reset,   NULL, "");

  GUI_Tip ("reset opengl-blocking.\n"
           "Block OpenGL-window with key\"b\".\n"
           "Rotate OpenGL: press left mousebutton,\n"
           "  keep pressed and move mouse ..");


  GUI_button__ (&boxh4, "File open",    c_fileO,   NULL, "");
  GUI_button__ (&boxh4, "File save",    c_fileS,   NULL, "");

  //----------------------------------------------------------------
  GUI_box_paned__ (&wBoxl, &wt2, &boxv0, 0, 0, 150);

  // simple list
  wList1 = GUI_list1__ (&wBoxl, "list1.dat", "1", NULL, cb_list0cb, "e");


  // connect the OpenGL-window (config-event & draw-event)
  wgl = GUI_gl__ (&wt2, gl_draw, "e,-200e");

  // connect the mouse-move events
  GUI_gl_ev_move (&wgl, gl_move);

  // connect the mouse-button events
  GUI_gl_ev_button (&wgl, gl_button);

  // connect the keyboard-events
  GUI_gl_ev_key (&wgl, gl_key);


  //----------------------------------------------------------------
  //messagewindow on bottom
  boxh6 = GUI_box_h (&boxv0, "e,4");
  wmsg = GUI_msgwin__ (&boxh6, "e,e");


  //----------------------------------------------------------------
  // windowSetup finished; display it ..
  GUI_Win_go (&window);

  return 0;
}


//================================================================
  int cb_exit (MemObj *mo, void **data) {
//================================================================

  printf(" exiting main: save & cleanup.\n");

  // do cleanup ..

  // exit ..
  GUI_Win_kill (mo);

  return 0;

}


//================================================================
  int tst_CB_b0 (MemObj *mo, void **data) {
//================================================================

  char  *optLst[]={"func 1","func 2","func 3","\0"};
  char  *tipLst[]={"tip for func 1","tip for func 2","tip for func 3"};

  // popup-menu
  GUI_popup__ (optLst, tipLst, 0, cb_pop1, NULL);

  // get state of radioButtons
  printf(" state of radioButton 1 = %d\n",GUI_radiobutt_get(&rb1));

  printf(" entry |%s|\n",GUI_entry_get (&went1));


  // activate radioButton 1
  GUI_radiobutt_set (&rb1);


  return 0;

}


//================================================================
  int cb_pop1 (MemObj *mo, void **data) {
//================================================================
// callback popup-window

  int iEv, lNr;

  // printf("cb_pop1 event=%d lNr=%d\n",
           // GUI_DATA_EVENT,    // event:    TYP_EventEnter|TYP_EventRelease
           // GUI_DATA_I1,       // lineNr
           // GUI_DATA_S2);      // text

  iEv = GUI_DATA_EVENT;
  lNr = GUI_DATA_I1;

  if(iEv == TYP_EventEnter)
    printf(" enter popup %d\n",lNr);
  if(iEv == TYP_EventLeave)
    printf(" leave popup %d\n",lNr);
  if(iEv == TYP_EventPress)                 // = select
    printf(" select popup %d |%s|\n",lNr,GUI_DATA_S2);
  if(iEv == TYP_EventUnmap)
    printf(" unmap popup\n");

  return 0;
}


//================================================================
  int tst_CB_b1 (MemObj *mo, void **data) {
//================================================================
//

  printf("tst_CB_b1 |%s|\n",GUI_DATA_S1);

  GUI_MsgBox ("GUI_MsgBox");

  // modify buttonbackgroundColor
  GUI_button_styl (mo, 2);

  return 0;
}

//================================================================
  int tst_CB_b2 (MemObj *mo, void **data) {
//================================================================
// GUI_DialogYN

  printf("tst_CB_b2 \n");
  GUI_DialogYN ("GUI_DialogYN", tst_CB_w2);
  return 0;
}

//================================================================
  int tst_CB_w2 (MemObj *mo, void **data) {
//================================================================
// GUI_DialogYN returns UI_FuncOK | UI_FuncCancel

  if(GUI_DATA_EVENT == UI_FuncOK) printf("tst_CB_w2: OK ..\n");
  return 0;
}

//================================================================
  int tst_CB_b3 (MemObj *mo, void **data) {
//================================================================

  int    irc;
  char   s1[256];
  char   *buttons[]={"Cancel","NO","YES",NULL};

  printf("tst_CB_b3\n");

  strcpy (s1, "myName");
  irc = GUI_DialogEntry ("GUI_DialogEntry", s1, 250, buttons, 0);
    printf(" GUI_DialogEntry returns %d |%s|\n",irc,s1);

  return 0;
}

//================================================================
  int tst_CB_b4 (MemObj *mo, void **data) {
//================================================================

  int   i1, ir, ig, ib;


  i1 = GUI_Color_select (&ir, &ig, &ib);

  printf("tst_CB_b4 %d col=%d %d %d\n",i1,ir,ig,ib);

  return 0;
}


//================================================================
  int tst_CB_wsl (MemObj *mo, void **data) {
//================================================================


  printf("tst_CB_wsl slider ev = %d\n",GUI_DATA_EVENT);

  if(GUI_DATA_EVENT == TYP_EventMove) {
     printf(" val = %lf\n",GUI_DATA_D1);  // slider-value as double
  }

  return 0;
}


//================================================================
  int tst_CB_cb1 (MemObj *mo, void **data) {
//================================================================

  double   d1;

  printf(" =======================================\n");


  if(GUI_ckbutt_get(mo)) {
    // checkbox is on
    GUI_show__ (&tb1);

  } else {
    // checkbox is off
    GUI_hide__ (&tb1);
  }

  return 0;
}


//================================================================
  int cb_rb1 (MemObj *mo, void **data) {
//================================================================
// callback radio-button;

  MemObj   ntb;

  printf("cb_rb1 |%s|\n",GUI_DATA_S1);

  if(GUI_DATA_EVENT == TYP_EventRelease) return 0; // skip disactivation

  printf(" activate ..\n");

  if(!strcmp (GUI_DATA_S1, "rb2")) {

    // if(wList1) {GUI_obj_del (wList1); wList1 = NULL;}
    // else return 0;

    // ntb = GUI_notebook__   (&wBoxl, cb_ntb);
    // GUI_notebook_add (&ntb, "Page 1");
    // GUI_notebook_add (&ntb, "Page 2");
    // GUI_notebook_set (&ntb, 1);
  }


  return 0;

}


//================================================================
  int cb_ntb (MemObj *mo, void **data) {
//================================================================
// callback Notebook

  printf("cb_ntb activate notebook page pageNr %d\n",GUI_DATA_I1);

  return 0;

}


//================================================================
  int cb_went1 (MemObj *mo, void **data) {
//================================================================
// event in text-entry: returns pointers to 4 ints

  int   i1, imod, iTyp;

  iTyp = GUI_OBJ_TYP (mo);
  printf("cb_went1 ev=%d typ=%d\n",GUI_DATA_EVENT,iTyp);

  if(GUI_DATA_EVENT != TYP_EventRelease) return 0;  // skip enter-event

  // printf("cb_went %d %d\n",ikey,imod);
  printf("cb_went1 event=%d key=%d  modif=%d\n",
           GUI_DATA_EVENT,    // event:    TYP_EventEnter|TYP_EventRelease
           GUI_DATA_I2,       // key:      ascii-value of key
           GUI_DATA_I3);      // modifier: &1=shift; &4=ctrl; &8=alt.
                              //   &256=MB1; &512=MB2; &1024=MB3.

  i1 = GUI_DATA_I2;
  if((i1 >= ' ') && (i1 <= '~')) printf(" key |%c| pressed\n",i1);
  else                           printf(" key %x pressed\n",i1);

  imod = GUI_DATA_I3;
  if(imod & 1)    printf(" shift is on\n");
  if(imod & 4)    printf(" ctrl is on\n");
  if(imod & 8)    printf(" alt is on\n");
  if(imod & 256)  printf(" mouseButt. 1 is on\n");
  if(imod & 512)  printf(" mouseButt. 2 is on\n");
  if(imod & 1024) printf(" mouseButt. 3 is on\n");

  printf(" entry |%s|\n",GUI_entry_get (mo));

  return 0;

}


//================================================================
  int cb_went2 (MemObj *mo, void **data) {
//================================================================
// event in text-entry: returns pointers to 4 ints
  
  int   i1, imod;
  
  if(GUI_DATA_EVENT != TYP_EventRelease) return 0;  // skip enter-event

  // printf("cb_went %d %d\n",ikey,imod);
  printf("cb_went2 event=%d key=%d  modif=%d\n",
           GUI_DATA_EVENT,    // event:    TYP_EventEnter|TYP_EventRelease
           GUI_DATA_I2,       // key:      ascii-value of key
           GUI_DATA_I3);      // modifier: &1=shift; &4=ctrl; &8=alt.
                              //   &256=MB1; &512=MB2; &1024=MB3.

  if(GUI_DATA_EVENT != TYP_EventRelease) return 0;  // skip enter-event

  printf(" entry |%s|\n",GUI_entry_get (mo));

  return 0;

}



//================================================================
  int cb_keys (MemObj *mo, void **data) {
//================================================================

  int    i1;
  static int  ic=0;


  // get state of modifier-keys;
  i1 = GUI_get_keys_mod ();

  if(i1 & 1) printf(" shift is on\n");
  if(i1 & 4) printf(" ctrl is on\n");
  if(i1 & 8) printf(" alt is on\n");

  if(i1 & 256)  printf(" mouseButt. 1 is on\n");
  if(i1 & 512)  printf(" mouseButt. 2 is on\n");
  if(i1 & 1024) printf(" mouseButt. 3 is on\n");


  L_1:
    ++ic;
    i1 = GUI_cursor__ (NULL, ic);
    if(i1 < 0) { ic = -1; goto L_1;}

  return 0;

}


//================================================================
  int cb_reset (MemObj *mo, void **data) {
//================================================================

  GUI_gl_block (&wgl, 0);          // unblock gl
  GUI_cursor__ (&wgl, 0);       // cursor normal

  return 0;

}


//================================================================
  int c_fileO (MemObj *mo, void **data) {
//================================================================
// open file

  int    irc;
  char   filNam[256], dirNam[256];

  filNam[0]='\0';
  strcpy(dirNam, ".");
  // irc = GUI_File_open (filNam, 256, dirNam, 256, NULL, "open file", "*");
  irc = GUI_file_open__ (filNam, 256, dirNam, 256, NULL,
                       "list2.dat", "open file", "*");

  printf("ex c_fileO %d |%s|%s|\n",irc,dirNam,filNam);

  return 0;

}


//================================================================
  int c_fileS (MemObj *mo, void **data) {
//================================================================
// save file

  int    irc;
  char   filNam[256], dirNam[256];

  strcpy(filNam, "tst_gui_core.c");
  strcpy(dirNam, ".");
  // irc = GUI_File_save (filNam, 256, dirNam, 256, NULL, "save file", "*.c");
  irc = GUI_file_save__ (filNam, 256, dirNam, 256, "list2.dat", "save file", "*.c");

  printf(" returns %d |%s|%s|\n",irc,dirNam,filNam);

  return 0;

}


//================================================================
  int cb_men1 (MemObj *mo, void **data) {
//================================================================

  int   ind;
  char  *txt;

  ind = GUI_DATA_I1;
  txt = GUI_DATA_S2;
  printf("cb_men1 %d |%s|\n",ind,txt);
  GUI_msgwin_prt (&wmsg, txt);       // -> messagewindow

  GUI_optmen_set  (&wom1, 1, txt);       // parent = &wom1
  GUI_optmen_set  (&wom2, 0, NULL);

  return 0;

}

//================================================================
  int cb_men2 (MemObj *mo, void **data) {
//================================================================

  int   ind;
  char  *txt;

  ind = GUI_DATA_I1;
  txt = GUI_DATA_S2;
  printf("cb_men2 %d |%s|\n",ind,txt);
  GUI_msgwin_prt (&wmsg, txt);       // -> messagewindow

  GUI_optmen_set  (&wom1, 0, NULL);
  GUI_optmen_set  (&wom2, 1, txt);       // parent = &wom2

  return 0;

}


//================================================================
  int UI_main (MemObj *mo, void **data) {
//================================================================

  printf("UI_main %d\n",GUI_DATA_EVENT);


  if(!strcmp(GUI_DATA_S1, "tb")) {
    printf(" tb = %d\n",GUI_menu_checkbox_get (mo));
  }

  return 0;

}


//================================================================
  int cb_list0cb (MemObj *mo, void **data) {
//================================================================
  
  printf("cb_list0cb |%s|\n",GUI_DATA_S3);

  GUI_msgwin_prt (&wmsg, GUI_DATA_S3);       // -> messagewindow

  return 0;
} 


//================================================================
  int cb_list1__ (MemObj *mo, void **data) {
//================================================================

  char *titLst[]={"Filenames"};

  GUI_list1_dlg__ (NULL, "List 1 column", "list1.dat", "1",
                   titLst, cb_list1cb, "20,20");
  GUI_set_enable (mo, 0);                       // lock button
  return 0;
}

//================================================================
  int cb_list1cb (MemObj *mo, void **data) {
//================================================================

  printf("cb_list1cb \n");

  if(GUI_DATA_EVENT == TYP_EventExit) {
    printf(" list cancelled\n");
  }


  if(GUI_DATA_EVENT == TYP_EventPress) {
    // normal selection
    printf("cb_list1cb |%s|\n",GUI_DATA_S3);
    GUI_list1_dlg_del ();                      // delete ListWindow
  }

  GUI_set_enable (&btList1, 1);                    // unlock button
  return 0;
}


//================================================================
  int cb_list2__ (MemObj *mo, void **data) {
//================================================================

  char *titLst[]={"Directory","Filename"};

  GUI_list1_dlg__ (NULL, "List 2 columns", "list2.dat", "2",
                  titLst, cb_list2cb, "60,20");

  GUI_set_enable (mo, 0);                       // lock button

  return 0;
}


//================================================================
  int cb_list2cb (MemObj *mo, void **data) {
//================================================================
  

  if(GUI_DATA_EVENT == TYP_EventExit) {
    printf(" list cancelled\n");
  }

  if(GUI_DATA_EVENT == TYP_EventPress) {
    // normal selection
    printf("cb_list2cb |%s|%s|\n",GUI_DATA_S3,GUI_DATA_S4);
    GUI_list1_dlg_del ();                      // delete ListWindow
    GUI_msgwin_prt (&wmsg, GUI_DATA_S3);       // -> messagewindow
  }

  GUI_set_enable (&btList2, 1);                // unlock button

  return 0;
} 
  


// EOF



