/*  Test    Bézier-Flächen                              RF                        

tst_surf2_draw ()


=============================================================================
*/




#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <GL/gl.h>
#include <GL/glu.h>

// #include "tst_gl2.h"                // 



double ctrlpt[4][3][3] = {{     // V = 0
    { -4.0f, 0.0f, 4.0f},
    { -2.0f, 4.0f, 4.0f},
    {  4.0f, 0.0f, 4.0f},
 },{                            // V = 1
    { -4.0f, 0.0f, 0.0f},
    { -2.0f, 4.0f, 0.0f},
    {  4.0f, 0.0f, 0.0f},
 },{                            // V = 2
    { -4.0f, 0.0f, -4.0f},
    { -2.0f, 4.0f, -4.0f},
    {  4.0f, 0.0f, -4.0f}
 },{                            // V = 3
    { -4.0f, 0.0f, -8.0f},
    { -2.0f, 4.0f, -8.0f},
    {  4.0f, 0.0f, -8.0f}
 }};

int vNr = 4;
int uNr = 3;

double pta[100];







//=========================================================
#ifdef _MSC_VER
  void CALLBACK my_GL_errorhandler (GLenum errNo) {
#else
  void my_GL_errorhandler (GLenum errNo) {
#endif
//=========================================================

  printf("my_GL_errorhandler %d\n",errNo);

}





//=========================================================
  int draw_nurbSur () {
//=========================================================

  long   dli;
  GLuint dlInd;


  // NURBS object pointer
  GLUnurbsObj *pNurb = NULL;

  // The number of control points for this curve
  GLint nNumPoints = 4; // 4 X 4

  // Mesh extends four units -6 to +6 along x and y axis
  // Lies in Z plane
  //                 u  v  (x,y,z)
  GLfloat ctrlPoints[4][4][3]=
               {{{  -6.0f, -6.0f, 0.0f},  // u = 0, v = 0
                {   -6.0f, -2.0f, 0.0f},  //      v = 1
                {   -6.0f,  2.0f, 0.0f},  //      v = 2
                {   -6.0f,  6.0f, 0.0f}}, //      v = 3

                {{  -2.0f, -6.0f, 0.0f},  // u = 1  v = 0
                {   -2.0f, -2.0f, 8.0f},  //      v = 1
                {   -2.0f,  2.0f, 8.0f},  //      v = 2
                {   -2.0f,  6.0f, 0.0f}}, //      v = 3

                {{   2.0f, -6.0f, 0.0f }, // u =2   v = 0
                {    2.0f, -2.0f, 8.0f }, //      v = 1
                {    2.0f,  2.0f, 8.0f }, //      v = 2
                {    2.0f,  6.0f, 0.0f }},//      v = 3

                {{   6.0f, -6.0f, 0.0f},  // u = 3  v = 0
                {    6.0f, -2.0f, 0.0f},  //      v = 1
                {    6.0f,  2.0f, 0.0f},  //      v = 2
                {    6.0f,  6.0f, 0.0f}}};//      v = 3



  // Knont sequence for the NURB
  GLfloat Knots[8] = {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};


  // Outside trimming points to include entire surface
  GLfloat outsidePts[5][2] = /* counter clockwise */
    {{0.0f, 0.0f}, {0.7f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}};


  // Inside trimming points to create triangle shaped hole in surface
  GLfloat insidePts[4][2] = /* clockwise */
    {{0.25f, 0.25f}, {0.5f, 0.5f}, {0.75f, 0.25f}, { 0.25f, 0.25f}};


  GLfloat insideCCW[4][2] = /* clockwise */
    {{0.25f, 0.25f},
     {0.75f, 0.25f},
     {0.5f,  0.5f},
     {0.25f, 0.25f}};





  //-------------------------------------------------------------


  // Setup the Nurbs object
  pNurb = gluNewNurbsRenderer();

  gluNurbsCallback (pNurb, GLU_ERROR, my_GL_errorhandler);

  // tolerance for tesselation 25
  gluNurbsProperty(pNurb, GLU_SAMPLING_TOLERANCE, 25.0f);


  // gluNurbsProperty(pNurb, GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON);
  // gluNurbsProperty(pNurb, GLU_DISPLAY_MODE, GLU_OUTLINE_PATCH);
  gluNurbsProperty(pNurb, GLU_DISPLAY_MODE, (GLfloat)GLU_FILL);


  // Begin the NURB definition
  gluBeginSurface(pNurb);


  // Automatically generate normals for evaluated surfaces
  glEnable(GL_AUTO_NORMAL);


  // Evaluate the surface
  gluNurbsSurface(pNurb,  // pointer to NURBS renderer
        8, Knots,     // No. of knots and knot array u direction
        8, Knots,     // No. of knots and knot array v direction
        4 * 3,        // Distance between control points in u dir.
        3,            // Distance between control points in v dir.
        &ctrlPoints[0][0][0], // Control points
        4, 4,         // u and v order of surface
        GL_MAP2_VERTEX_3);    // Type of surface


  // Done with surface
  gluEndSurface(pNurb);


  // delete
  // gluDeletNurbsRenderer (pNurb);


  return 0;
}



//==============================================================
void  tst_surf2_draw() {
//==============================================================

  int   i1, i2;


  // printf("tst_surf2_draw\n");


  draw_nurbSur ();


  //========================================================================
  // Draw Controlpoints
  glPointSize( 5 );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glDisable( GL_BLEND );
  glDisable( GL_LIGHTING );

  glColor3f( 0.9f, 0.3f, 0.3f );    // rot

  glBegin( GL_POINTS );

  for(i1=0; i1 < vNr; ++i1) {
    for(i2=0; i2 < uNr; ++i2) {
      // printf("P%d %d %f %f %f\n",i1,i2,
        // ctrlpt[i1][i2][0],ctrlpt[i1][i2][1],ctrlpt[i1][i2][2]);
      glVertex3dv(&ctrlpt[i1][i2][0]);
    }
  }

  glEnd();

  glEnable(GL_LIGHTING);
  glEnable(GL_BLEND);




  //========================================================================
  // Draw Kontrollpolygon
  // Vierecke an den Punkten.

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  for(i1=0; i1 < (vNr-1); ++i1) {
    for(i2=0; i2 < (uNr-1); ++i2) {
      glBegin(GL_QUADS);
      glVertex3dv(&ctrlpt[i1][i2][0]);
      glVertex3dv(&ctrlpt[i1][i2+1][0]);
      glVertex3dv(&ctrlpt[i1+1][i2+1][0]);
      glVertex3dv(&ctrlpt[i1+1][i2][0]);
      glEnd();
    }
  }



  //========================================================================
  // Draw Flaeche
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  glMap2d(            // Setup Flaeche
    GL_MAP2_VERTEX_3, // Typ
    0.0,              // u1 low u (linear mapping of u-parameter, Bereichsgrenze)
    10.0,             // u2 upper u
    3,                // Distance between points (offset im mem)
    uNr,              // dim in u (order)
    0.0,              // v1 lower v
    10.0,             // v2 upper v
    3*uNr,            // distance between points (offset im mem)
    vNr,              // dim in v (order ?)
    (GLdouble*)ctrlpt);     // data


  glEnable(GL_MAP2_VERTEX_3);  // aktualisieren !?

  glEnable(GL_AUTO_NORMAL);  // nur f. glEvalMesh2(GL_FILL erforderl


  glMapGrid2d(      // establish grid
    10,             // un nr of u-subdivisions
    0.0,            // u1 lower grid-domain values in u-direction
    10.0,           // u2 upper grid-domain values in u-direction
    10,             // vn nr of v-subdivisions
    0.0,            // v1 lower grid-domain values in v-direction
    10.0);          // v2 upper grid-domain values in v-direction

  glEvalMesh2(      // compute grid of points (=Ausgabe Polygon od Flaeche)
    GL_LINE,        // GL_LINE: Vierecke, GL_FILL: Flaechen.
    0,10,           // first/last integer val's for u-domain
    0,10);          // first/last integer val's for v-domain

  glColor3f( 0.5f, 0.5f, 0.9f );    // blau

  glEvalMesh2(      // compute grid of points (=Ausgabe Polygon od Flaeche)
    GL_FILL,        // GL_POINT, GL_LINE: Vierecke, GL_FILL: Flaechen.
    0,10,           // first/last integer val's for u-domain
    0,10);          // first/last integer val's for v-domain





  //========================================================================
  // Query points (liefert nur Controlpoints!)

  glGetMapdv(
    GL_MAP2_VERTEX_3, // Typ
    GL_COEFF,
    pta);

  glColor3f( 0.5f, 0.9f, 0.5f );    // gruen

  glPointSize( 5 );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glDisable( GL_BLEND );
  glDisable( GL_LIGHTING );

  glBegin( GL_POINTS );

  i2 = 0;

  for(i1=0; i1<12; ++i1) {
      // printf(" %d %f %f %f\n",i1,pta[i2],pta[i2+1],pta[i2+2]);
    glVertex3dv(&pta[i2]);
    i2 += 3;
  }

  glEnd();

  glEnable(GL_LIGHTING);
  glEnable(GL_BLEND);


}


//========================== EOF ================================
