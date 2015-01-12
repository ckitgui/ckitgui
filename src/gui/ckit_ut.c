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

-----------------------------------------------------
Modifications:
2012-02-01 extracted.  RF.


-----------------------------------------------------
*//*!
\brief utility-functions
\file ../gui/gui_gtk_ut.c
\brief utility-functions for Gtk-Toolbox only
\code

Utility-functions for Gtk-Toolbox only; extracted from
../ut/ut_os_aix.c
../ut/ut_txt.c
../xa/ut_ui_TX.c
../xa/xa_msg.c

=====================================================
List_functions_start:

OS_Wait
OS_FilSiz              returns filesize in bytes
OS_checkFilExist
OS_file_sig_wait       wait for signalfile & get its content
OS_file_delete
OS_get_dialog
OS_get_tmp_dir
OS_get_ico_dir

UTX_CleanCR
UTX_endDelChar
UTX_cnr_chr
UTX_fgetLine           read line nr. <lNr> out of file <filNam>
UTX_wrf_str            write string -> file
UTX_tmp_file           get file into temp. memspc
UTX_str_file           read file -> string
UTX_ck_num_i           test if word is numeric (int)

UTI_round_i2b          round integer to byte (back: UTI_round_b2i)
UTI_round_b2i          make integer from byte (back from UTI_round_i2b)

CSV_read__             read/decode next line in csv-file

TX_Print
TX_Error
TX_Write

MSG_get_1              DUMMY
MSG_err_1              DUMMY
MSG_pri_1              DUMMY
MSG_get_str            DUMMY
gCad_fini              DUMMY

List_functions_end:
=====================================================


cc -c gui_gtk_ut.c


\endcode *//*----------------------------------------


_______________________________________________________________________________
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <stdarg.h>          // ...)
#include <unistd.h>          /* f. access, R_OK .. */
#include <sys/stat.h>        // stat


#include "../gui/ckit_ut.h"        // UTX_tmp_file
#include "../ut/ut_cast.h"             // INT_PTR
#include "../ut/ut_umem.h"        // UME_alloc_tmp




#define ON                 0
#define OFF                1


/*  alle Ausgaben EIN */
int   TX_mode = ON;


/*
typedef struct {
  int  nrVal;      // nr of values per line
  char **pTab;     // table of <nrVal> pointers; strings; for out and in.
  char delVal;     // value-delimiter-char; comma or semicolon
  } CSV_struct;
*/




//======================================================================
  void UTX_str_file (char* txbuf, long *fSiz, char *fnam) {
//======================================================================
/// UTX_str_file           read file -> string
/// Used by UTX_tmp_file()
/// fSiz must have filesize; see OS_FilSiz.

  FILE  *fpi;


  if ((fpi = fopen (fnam, "r")) == NULL) {
    TX_Error ("UTX_str_file: Error open %s",fnam);
    *fSiz = -2;
    return;
  }

  fread (txbuf, *fSiz, 1, fpi);

  txbuf[*fSiz] = '\0';
  if(txbuf[*fSiz - 1] == 10) {    // LF
    *fSiz -= 1;
    txbuf[*fSiz] = '\0';
  }

  fclose(fpi);

  return;

}


//================================================================
  int UTX_ck_num_i (char *s1) {
//================================================================
/// \code
/// UTX_ck_num_i          test if word is numeric (int)
/// RetCod:  -1   s1 does NOT start with an integer
///          else nr of characters (including sign)
/// \endcode

  int     iNr=0;

  while (*s1  == ' ') { ++s1; ++iNr; }


  if((*s1 == '-')||(*s1 == '+')) {
    ++s1;
    ++iNr;
  }

  while(isdigit(*s1)) {
    ++s1;
    ++iNr;
  }

  if(iNr < 1) return -1;

  return iNr;

}


//==================================================================
  int CSV_read__ (CSV_struct *fTyp, char *sb, int sSiz, FILE *fpi) {
//==================================================================
// read/decode next line in csv-file
// first line: get nr of values, value-delimiter-char and column-titles.
// Input:
//   fTyp->delVal set to blank (' ') for first call.
//   fTyp->nrVal  the maximum nr of values per line (size of pTab !)
//   fTyp->pTab   no input, but must have size fTyp->nrVal !
//   sSiz         size of stringbuffer sb
//   fpi          FILE* of inputfile; must be open.
// Output:
//   fTyp->delVal the value-delimiter-char
//   fTyp->nrVal  the nr of values per line
//   fTyp->pTab   the values (pointers into sb)
//   sb           stringbuffer
// RetCodes:
//   0            OK
//   -1           EOF; pTab and sb invalid.
//   -2           cannot find value-delimiter-char in first line
//   -3           size of pTab too small


  int   ii;
  char  *p1, *p2, *p3;
  int   vMax;


  // printf("CSV_read__ |%c| %d\n",fTyp->delVal,fTyp->nrVal);


  // read next line
  if (fgets (sb, sSiz, fpi) == NULL) {
    return -1;
  }
  // remove following CR,LF
  L_clean:
  ii = strlen(sb);
  --ii;
  if((sb[ii] == '\n')||(sb[ii] == '\r')) {
    sb[ii] = '\0';
    goto L_clean;
  }



  vMax = fTyp->nrVal;
    // printf(" vMax=%d\n",vMax);


  // read first line of file
  if(fTyp->delVal != ' ') goto L_readl;


  // decode value-delimiter-char & nr-of-values
  // is 1. char '"' ? 
  if(sb[0] != '"') goto L_noString;


  //----------------------------------------------------------------
  // decode value-delimiter-char
  p1 = strchr(&sb[1], '"');
  if(!p1) return -2;
  ++p1;
  fTyp->delVal = *p1;


  //----------------------------------------------------------------
  // first title has not "; look for , or ;
  L_noString:
    p1 = strpbrk (sb, ",;");
    if(!p1) return -2;
    fTyp->delVal = *p1;
      printf(" found delVal=|%c|\n",fTyp->delVal);



  //----------------------------------------------------------------
  // decode normal line ..
  L_readl:
  p1 = sb;
  ii = 0;

  L_nxtVal:
    printf(" nxt |%s|\n",p1);
  // find next delimiter
  p2 = strchr(p1, fTyp->delVal);
  if(p2) {
    p3 = p2 + 1;
  } else {
    p2 = strchr(p1, '\0');
    p3 = '\0';
      // printf(" end:%d %d\n",p2-p1,strlen(p1));
  }

  if(ii >= vMax) return -3;


  // remove "-s
  if(*p1 == '"') {
    ++p1;
    --p2;
  }
  *p2 = '\0';
    printf(" ii=%d p1=|%s|\n",ii,p1);
  fTyp->pTab[ii] = p1;
  p1 = p3;
  ++ii;
  if(p1) goto L_nxtVal;


  L_done:
  fTyp->nrVal = ii;
    // printf(" done-%d\n",ii);


    printf("ex CSV_read__ |%c| %d\n",fTyp->delVal,fTyp->nrVal);
    for(ii=0; ii<fTyp->nrVal; ++ii) {
      printf(" %d |%s|\n",ii,fTyp->pTab[ii]);
    }


  return 0;

}


//================================================================
  int UTX_wrf_str (char *fnam, char *txt) {
//================================================================
/// \code
/// UTX_wrf_str            write string -> file
/// see UTF_wrf_line
/// \endcode

  FILE     *fp1;

  // printf("UTX_wrf_str |%s|\n",fnam);


  // write Line -> File
  if((fp1=fopen(fnam,"w")) == NULL) {
    TX_Print("UTX_wrf_str E001 |%s|",fnam);
    return -1;
  }
  // fwrite(txt, strlen(txt)+1, 1, fp1);
  fprintf(fp1, "%s",txt);                   // 2012-07-14 \n removed
  fclose(fp1);

  return 0;

}


//=============================================================
  long OS_FilSiz (char* filnam) {
//=============================================================
/// OS_FilSiz               returns filesize in bytes

  int         rc;
  struct stat fileStatus;



  rc = stat (filnam, &fileStatus);

  // printf("rc=%d siz=%d\n",rc,fileStatus.st_size);

  if(rc < 0) {  // File nicht vorh.
    printf("ex OS_FilSiz -1 |%s|\n",filnam);
    return -1;
  }

  // printf("ex OS_FilSiz %d |%s|\n", fileStatus.st_size,filnam);

  return fileStatus.st_size;

}




//================================================================
  int MSG_Tip (char *key) {
//================================================================
/// \code
/// dummyfunction for MSG_Tip
/// MSG_Tip         add tooltip to last created GUI-widget
/// msg out of files  msg_de.txt  msg_en.txt ..
/// \endcode

  // cp1 = MSG_read (MSG_buf, MSG_bSiz, key);
  // if(cp1 == NULL) return -1;
    // // printf(" cp1=|%s|\n",cp1);
  // GUI_Tip (cp1);

  GUI_Tip (key);

  return 0;

}



//================================================================
  int UTI_round_i2b (int i1) {
//================================================================
/// \code
/// UTI_round_i2b             round integer to byte (back: UTI_round_b2i)
///   Purpose: save Integers in a single byte.
///     UTI_round_i2b(10000)  -> 14
///     UTI_round_b2i(14)     -> 16384
/// \endcode

//  ii = 10000;
//  printf(" ii=%d, i2b=%d; b2i=%d\n",ii,
//           UTI_round_i2b(ii),UTI_round_b2i(UTI_round_i2b(ii)));


  int   ii=0;

  while(i1) {i1 /= 2; ++ii;}

  return ii;

}


//================================================================
  int UTI_round_b2i (int i1) {
//================================================================
/// UTI_round_b2i         make integer from byte (back from UTI_round_i2b)

  int ii=1;

  while(i1) {--i1; ii *=2;}

  return ii;

}


//================================================================
  char* OS_get_tmp_dir () {
//================================================================
/// returns tempDir (with closing '/')  <gcad_dir_local>tmp/

  return "./";

}


//================================================================
  char* OS_get_ico_dir () {
//================================================================
/// returns directory for icons (with closing '/')  <gcad_dir_bin>icons/

  return "../icons/";

}


//=============================================================
  void OS_Wait (int millisecs) {
//=============================================================
/// wait - milli-seconds (<millisecs> * 0.001 seconds)

  unsigned int i1;


  /* sleep geht nur in sec ! */
  if (millisecs > 1000) {
    i1 = millisecs / 1000;
    sleep (i1);

  } else {
    i1 = millisecs * 1000;
    usleep (i1);     // microseconds !
  }



  /*  PC-Version: */
  /* Sleep (millisecs); */


}


//=============================================================
  int OS_checkFilExist (char* filnam, int mode) {
//=============================================================
/// \code
/// OS_checkFilExist         check if File or Directory exists
/// mode = 0: display message sofort;
/// mode = 1: just fix returncode, no message
/// mode = 2: make Errormessage (TX_Error) if File does not exist
/// 
/// rc = 0 = Fehler; datei existiert NICHT.
/// rc = 1 = OK, Datei existiert.
/// \endcode


  char    buf[256];


  // printf("OS_checkFilExist |%s| %d\n", filnam, mode);

  // remove following "/"
  strcpy (buf, filnam);
  UTX_endDelChar (buf, '/');

  /* Version PC: (braucht IO.h) */
  /* if ((access (buf, 0)) != 0) { */


  /* if ((access (buf, R_OK|W_OK)) != 0) { */

  if ((access (buf, R_OK)) != 0) {
    if (mode == 0) {
      // printf ("*** Error OS_checkFilExist: %s does not exist\n",filnam);
      MSG_get_1 (buf, 256, "NOEX_fil", "%s", filnam);
      printf ("*** Error OS_checkFilExist: %s\n", buf);
      /* UI_Dialog (NULL, buf); */

    } else if (mode == 2) {
      // TX_Error(" - File %s does not exist ..",filnam);
      MSG_err_1 ("NOEX_fil", "%s", filnam);
    }

    return 0;
  }

  // printf("ex OS_checkFilExist YES |%s| %d\n",filnam,mode);
  return 1;

}


//========================================================================
  int OS_file_delete (char *fNam) {
//========================================================================
/// delete File; NO Wildcards !
// MS u Unix gleich.


  remove (fNam);    // ACHTUNG: keine Wildcards mit remove !
  return 0;

}


//================================================================
  int OS_get_dialog () {
//================================================================
/// check if zenity is installed. 1=yes, -1=no.

// zenity --error --text 'text'
// zenity --entry --text="input" --entry-text=" alter text"

  static int installed = 0;  // 0=undefined; 1=installed; -1 = not installed.

  if(installed) goto L_exit;

  if(system("which zenity 1>/dev/null 2>/dev/null") == 0) {
    installed = 1;
  } else {
    installed = -1;
  }

  L_exit:
  return installed;
}


//================================================================
  void OS_get_curDir (char *sd, int sSiz) {
//================================================================
/// OS_get_curDir            get current directory

  getcwd (sd, sSiz);

}


//================================================================
  int UTX_cnr_chr (char *txt, char c1) {
//================================================================
/// returns nr of char c1 in string txt


  int   iNr;
  char  *p1;

  iNr = 0;

  p1 = txt;

  while (*p1) {
    if(*p1 == c1) ++iNr;
    ++p1;
  }

  return iNr;

}


//===========================================================
  char* UTX_CleanCR (char* string) {
//===========================================================
/// \code
/// UTX_CleanCR              Delete Blanks, CR's and LF's at end of string
/// returns positon of stringterminator \0
///  
/// see also UTX_del_follBl UTX_CleanSC
/// \endcode


  int  ilen;
  char *tpos;
    
  ilen = strlen (string);
    
  tpos = &string[ilen];
    
  if(ilen < 1) goto L_exit;

  --tpos;


  while ((*tpos  == ' ')  ||
         (*tpos  == '\t') ||          /* tab */
         (*tpos  == '\n') ||          /* newline */
         (*tpos  == '\r'))   {        /* CR */

    *tpos    = '\0';
    if(tpos <= string) goto L_exit;
    --tpos;
  }

  ++tpos;


  L_exit:
  // printf("ex UTX_CleanCR |%s|\n", string);

  return tpos;
}


//=============================================================
  int UTX_endDelChar (char* txtbuf, char chr) {
//=============================================================
/// UTX_endDelChar           if last char of string == chr: remove it.
                                                              
  
  if (txtbuf[strlen(txtbuf)-1] == chr) {
    txtbuf[strlen(txtbuf)-1] = '\0';
  }

  /* printf("UTX_endDelChar |%s| %c\n", txtbuf,chr); */

  return 1;
  
} 
  

//================================================================
// dummyfunctions:

int MSG_get_1 (char *msg, int msgSiz, char *key, char *fmt, ...) {
  printf("MSG_get_1: %s\n",msg);
  return 0;
}

int MSG_err_1 (char *key, char *fmt, ...) {
  printf("MSG_err_1: %s\n",key);
  return 0;
}

int MSG_pri_1 (char *key, char *fmt, ...) {
  printf("MSG_pri_1: %s\n",key);
  return 0;
}

char* MSG_get_str (char *key) {
  return key;
}


int gCad_fini () {
  return 0;
}



///==========================================================
  void TX_Print (char* txt, ...) {
///==========================================================
///   formatiertes Rausschreiben; wie bei printf; mehr als 4 Parm's Problem?



  va_list va;
  char    TX_buf1[1024];


  if(TX_mode == OFF) {
    return;
  }


  va_start(va,txt);

  vsprintf(TX_buf1,txt,va);

  va_end(va);

  if(strlen(TX_buf1) > 100) {
    TX_buf1[100] ='\0';
  }

  TX_Write (TX_buf1);


}





///==========================================================
  void TX_Error (char* txt, ...) {
///==========================================================


  char   TX_buf1[1024];


  va_list va;
  // void    *p1,*p2,*p3,*p4,*p5,*p6,*p7,*p8,*p9,*p10,*p11;


  va_start(va,txt);

  // p1  = va_arg(va,void*);
  // p2  = va_arg(va,void*);
  // p3  = va_arg(va,void*);
  // p4  = va_arg(va,void*);
  // p5  = va_arg(va,void*);
  // p6  = va_arg(va,void*);
  // p7  = va_arg(va,void*);
  // p8  = va_arg(va,void*);
  // p9  = va_arg(va,void*);
  // p10 = va_arg(va,void*);
  // p11 = va_arg(va,void*);


  // sprintf(TX_buf1,txt,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11);
  strcpy(TX_buf1, "*** Fehler: ");
                // o123456789012
  vsprintf(&TX_buf1[12],txt,va);
  // UTX_CleanCR (TX_buf1);
  va_end(va);

  TX_Write (TX_buf1);

  /* os_beep (); */


}

///==========================================================
  void TX_Write (char* txt) {
///==========================================================
///  direkt rausschreiben.


  printf("%s\n",txt);

  if(TX_mode == OFF) {
    return;
  }



}


//=======================================================================
  int UTX_fgetLine (char *cbuf, int sizBuf, char *filNam, int lNr) {
//=======================================================================
/// \code
/// UTX_fgetLine           read line nr. <lNr> out of file <filNam>
///             first line has lineNr 1
/// sizBuf      size of cbuf
/// rc -1   File does not exist
/// rc -2   Line does not exist
/// rc  0   OK
/// \endcode


  int    i1;
  FILE   *fpi;

  // printf("UTX_fgetLine |%s| %d\n",filNam,lNr);


  if ((fpi = fopen (filNam, "r")) == NULL) {
    return -1;
  }

  i1=0;
  while (!feof (fpi)) {
    ++i1;
    if (fgets (cbuf, sizBuf, fpi) == NULL) goto Fertig;
    // printf(" ..--in |%s|\n",cbuf);
    if(i1 != lNr) continue;
    fclose(fpi);
    UTX_CleanCR (cbuf);
    // printf("ex UTX_fgetLine %d |%s|\n",lNr,cbuf);
    return 0;
  }
  Fertig:
  fclose(fpi);
  return -2;
}


//================================================================
  int OS_file_sig_wait (int mode, void *data) {
//================================================================
// wait for signalfile & get its content
// mode     1=string, 2=int
///   RetCod:   0  OK, data valid
///            -1  cancelled by user

  int        i1;
  char       fn[240], *p1;

  printf("OS_file_sig_wait %d\n",mode);


  // clear signal-file
  sprintf(fn, "%sGDW1.sig", OS_get_tmp_dir());
    // printf(" fn=|%s|\n",fn);
  OS_file_sig_cre (0, fn);


  // wait for answer from GUI_DialogEntryCB
  for(i1=0; i1<100; ++i1) {   // 100 sec
    GUI_update__ ();
    OS_Wait (200);    // millisecs
      // printf(" i1=%d\n",i1);
    if(OS_checkFilExist(fn, 1) == 0) continue;
    goto L_finish;
  }
  for(;;) {
    GUI_update__ ();
    OS_Wait (600);    // millisecs
    if(OS_checkFilExist(fn, 1) == 0) continue;
    goto L_finish;
  }

  // get entry back
  L_finish:
    mode *= -1;   // read

  return OS_file_sig_cre (mode, data);   // get file-content

}


//================================================================
  int OS_file_sig_cre (int mode, void *data) {
//================================================================
/// \code
/// OS_file_sig_cre          create signal-file
/// Input:
///   mode      0  clear file; data = signalfilename; must be done first
///             1  write data = string (null terminated) -> signalfile
///             2  write data = int (4 bytes) -> signalfile
///            99  cancelled by user (following read gives retCod -1)
///            -1  read data = string (null terminated) -> data (size of data !)
///            -2  read data = int (4 bytes) -> data
/// Output:
///   data     text or int (mode);
///   RetCod:   0  OK, data valid
///            -1  cancelled by user
/// \endcode

// after read must be clear befor next write !


  long   ls;
  char   *fmact, *fmw = "w", *fmr = "r";
  static char fn[256];
  static int  lastMode = -1;
  FILE        *fpo;


  printf("..OS_file_sig_cre %d %d\n",mode,lastMode);

  // skip delete window before init
  if(mode > 0) {
    if(lastMode < 0) return 0;
  }

  // if(fnam) {
  if(mode == 0) {
    // fn = fnam;
    strcpy(fn, data);
      printf(" sig_cre fn=|%s|\n",fn);

    // delete file
    OS_file_delete (fn);
    lastMode = 0;
    return 0;
  }

  //  write | read
  if(mode < 0) {
    // read
    ((char*)data)[0] ='\0';
    if(lastMode == 99) {   // user cancelled !
      return -1;
    }
    fmact = fmr;
    ls = OS_FilSiz (fn);       printf(" ls=%ld\n",ls);

  } else {
    // write
    fmact = fmw;
  }

  lastMode = mode;

  if((fpo=fopen(fn, fmact)) == NULL) {
    TX_Print("OS_file_sig_cre E001 %s",fn);
    return -1;
  }


  // if(data) {    can be int=0 !
    if     (mode ==  1) {
      fprintf(fpo, "%s", TXT_PTR(data));
        printf(" wr s |%s|\n",TXT_PTR(data));

    } else if(mode ==  2) {
      fprintf(fpo, "%d", INT_PTR(data));
        printf(" wr int %d\n",INT_PTR(data));


    } else if(mode == -1) {
      // fgets(data, 250, fpo);
      fread(data, ls, 1, fpo);
      ((char*)data)[ls] = '\0';
        printf(" rd s |%s|\n",(char*)data);

    } else if(mode == -2) {
      fscanf(fpo, "%d", (int*)data);
        printf(" rd int %d\n",*((int*)data));
    }

  // } else {
    // if(mode > 0) fprintf(fpo, "");
  // }


  fclose(fpo);

  // rename after write: necessary if > 1 processes 


  return 0;

}


//================================================================
  int UTX_tmp_file (char *fBuf, long *fSiz, char *fNam) {
//================================================================

  *fSiz = OS_FilSiz (fNam);\
  fBuf = UME_alloc_tmp (*fSiz);\
  UTX_str_file (fBuf, fSiz, fNam);\

  return 0;

}


// EOF
