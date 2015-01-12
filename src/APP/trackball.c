/*
 * (c) Copyright 1993, 1994, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(TM) is a trademark of Silicon Graphics, Inc.
 */
/*
 * Trackball code:
 *
 * Implementation of a virtual trackball.
 * Implemented by Gavin Bell, lots of ideas from Thant Tessman and
 *   the August '88 issue of Siggraph's "Computer Graphics," pp. 121-129.
 *
 * Vector manip code:
 *
 * Original code from:
 * David M. Ciemiewicz, Mark Grossman, Henry Moreton, and Paul Haeberli
 *
 * Much mucking with by:
 * Gavin Bell
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>

#include "tst_geo.h"
#include "trackball.h"

/*
 * This size should really be based on the distance from the center of
 * rotation to the point on the object underneath the mouse.  That
 * point would then track the mouse as closely as possible.  This is a
 * simple example, though, so that is left as an Exercise for the
 * Programmer.
 */
#define TRACKBALLSIZE  (0.8)

/*
 * Local function prototypes (not defined in trackball.h)
 */
static float tb_project_to_sphere(float, float, float);
static void normalize_quat(float [4]);

void
vzero(float *v)
{
    v[0] = 0.0;
    v[1] = 0.0;
    v[2] = 0.0;
}

void
vset(float *v, float x, float y, float z)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

void
vsub(const float *src1, const float *src2, float *dst)
{
    dst[0] = src1[0] - src2[0];
    dst[1] = src1[1] - src2[1];
    dst[2] = src1[2] - src2[2];
}

void
vcopy(const float *v1, float *v2)
{
    register int i;
    for (i = 0 ; i < 3 ; i++)
        v2[i] = v1[i];
}

void
vcross(const float *v1, const float *v2, float *cross)
{
    float temp[3];

    temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
    temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
    temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
    vcopy(temp, cross);
}

float
vlength(const float *v)
{
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void
vscale(float *v, float div)
{
    v[0] *= div;
    v[1] *= div;
    v[2] *= div;
}

void
vnormal(float *v)
{
    vscale(v,1.0/vlength(v));
}

float
vdot(const float *v1, const float *v2)
{
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

void
vadd(const float *src1, const float *src2, float *dst)
{
    dst[0] = src1[0] + src2[0];
    dst[1] = src1[1] + src2[1];
    dst[2] = src1[2] + src2[2];
}

/*
 * Ok, simulate a track-ball.  Project the points onto the virtual
 * trackball, then figure out the axis of rotation, which is the cross
 * product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
 * Note:  This is a deformed trackball-- is a trackball in the center,
 * but is deformed into a hyperbolic sheet of rotation away from the
 * center.  This particular function was chosen after trying out
 * several variations.
 *
 * It is assumed that the arguments to this routine are in the range
 * (-1.0 ... 1.0)
 */
void
trackball(float q[4], float p1x, float p1y, float p2x, float p2y)
{
    float a[3]; /* Axis of rotation */
    float phi;  /* how much to rotate about axis */
    float p1[3], p2[3], d[3];
    float t;

  printf("trackball p1=%f,%f p2=%f,%f\n",p1x,p1y,p2x,p2y);

    if (p1x == p2x && p1y == p2y) {
        /* Zero rotation */
        vzero(q);
        q[3] = 1.0;
        return;
    }

    /*
     * First, figure out z-coordinates for projection of P1 and P2 to
     * deformed sphere
     */
    vset(p1,p1x,p1y,tb_project_to_sphere(TRACKBALLSIZE,p1x,p1y));
    vset(p2,p2x,p2y,tb_project_to_sphere(TRACKBALLSIZE,p2x,p2y));

    /*
     *  Now, we want the cross product of P1 and P2
     */
    vcross(p2,p1,a);

    /*
     *  Figure out how much to rotate around that axis.
     */
    vsub(p1,p2,d);
    t = vlength(d) / (2.0*TRACKBALLSIZE);

    /*
     * Avoid problems with out-of-control values...
     */
    if (t > 1.0) t = 1.0;
    if (t < -1.0) t = -1.0;
    phi = 2.0 * asin(t);

    axis_to_quat(a,phi,q);
}

/*
 *  Given an axis and angle, compute quaternion.
 */
void
axis_to_quat(float a[3], float phi, float q[4])
{
    vnormal(a);
    vcopy(a,q);
    vscale(q,sin(phi/2.0));
    q[3] = cos(phi/2.0);
}

/*
 * Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
 * if we are away from the center of the sphere.
 */
static float
tb_project_to_sphere(float r, float x, float y)
{
    float d, t, z;

    d = sqrt(x*x + y*y);
    if (d < r * 0.70710678118654752440) {    /* Inside sphere */
        z = sqrt(r*r - d*d);
    } else {           /* On hyperbola */
        t = r / 1.41421356237309504880;
        z = t*t / d;
    }
    return z;
}

/*
 * Given two rotations, e1 and e2, expressed as quaternion rotations,
 * figure out the equivalent single rotation and stuff it into dest.
 *
 * This routine also normalizes the result every RENORMCOUNT times it is
 * called, to keep error from creeping in.
 *
 * NOTE: This routine is written so that q1 or q2 may be the same
 * as dest (or each other).
 */

#define RENORMCOUNT 97

void
add_quats(float q1[4], float q2[4], float dest[4])
{
    static int count=0;
    float t1[4], t2[4], t3[4];
    float tf[4];

    vcopy(q1,t1);
    vscale(t1,q2[3]);

    vcopy(q2,t2);
    vscale(t2,q1[3]);

    vcross(q2,q1,t3);
    vadd(t1,t2,tf);
    vadd(t3,tf,tf);
    tf[3] = q1[3] * q2[3] - vdot(q1,q2);

    dest[0] = tf[0];
    dest[1] = tf[1];
    dest[2] = tf[2];
    dest[3] = tf[3];

    if (++count > RENORMCOUNT) {
        count = 0;
        normalize_quat(dest);
    }
}

/*
 * Quaternions always obey:  a^2 + b^2 + c^2 + d^2 = 1.0
 * If they don't add up to 1.0, dividing by their magnitued will
 * renormalize them.
 *
 * Note: See the following for more information on quaternions:
 *
 * - Shoemake, K., Animating rotation with quaternion curves, Computer
 *   Graphics 19, No 3 (Proc. SIGGRAPH'85), 245-254, 1985.
 * - Pletinckx, D., Quaternion calculus as a basic tool in computer
 *   graphics, The Visual Computer 5, 2-13, 1989.
 */
static void
normalize_quat(float q[4])
{
    int i;
    float mag;

    mag = (q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
    for (i = 0; i < 4; i++) q[i] /= mag;
}

/*
 * Build a rotation matrix, given a quaternion rotation.
 *
 */
void
build_rotmatrix(float m[4][4], float q[4])
{
    m[0][0] = 1.0 - 2.0 * (q[1] * q[1] + q[2] * q[2]);
    m[0][1] = 2.0 * (q[0] * q[1] - q[2] * q[3]);
    m[0][2] = 2.0 * (q[2] * q[0] + q[1] * q[3]);
    m[0][3] = 0.0;

    m[1][0] = 2.0 * (q[0] * q[1] + q[2] * q[3]);
    m[1][1]= 1.0 - 2.0 * (q[2] * q[2] + q[0] * q[0]);
    m[1][2] = 2.0 * (q[1] * q[2] - q[0] * q[3]);
    m[1][3] = 0.0;

    m[2][0] = 2.0 * (q[2] * q[0] - q[1] * q[3]);
    m[2][1] = 2.0 * (q[1] * q[2] + q[0] * q[3]);
    m[2][2] = 1.0 - 2.0 * (q[1] * q[1] + q[0] * q[0]);
    m[2][3] = 0.0;

    m[3][0] = 0.0;
    m[3][1] = 0.0;
    m[3][2] = 0.0;
    m[3][3] = 1.0;
}


//================================================================
  int trackball_pan (Pointf *ptc,
               float oldx, float oldy,
               float newx, float newy,
               float width, float height) {
//================================================================
// Input:
//   oldx, oldy      last position of mouse
//   newx, newy      new position of mouse
//   width, height   total size of grafic-window
// Output:
//   zoom            updated zoomfactor

// nur eine dummyfunktion; man muss den echten Verschiebeweg im Raum in 
// usercoords rechnen ..


#define maxPan   10.f
#define panFakt  0.2f


  float dx, dy, d1;

  printf("trackball_pan old %.f %.f new %.f %.f siz %.f %.f\n",
          oldx,oldy,newx,newy,width,height);

  dx = newx - oldx;
  dy = -(newy - oldy);

  d1 = fabsf (dx + dy);
  if (d1 < 1.f) return -1;
  // if (d1 > maxPan) d1 = copysignf (maxPan, d1);
  if (d1 > maxPan) return -1;

    printf("trackball_pan %f %f\n",dx,dy);

  ptc->x += (dx * panFakt);
  ptc->y += (dy * panFakt);

  return 0;

}


//================================================================
  int trackball_zoom (float *zoom,
                float oldx, float oldy,
                float newx, float newy,
                float width, float height) {
//================================================================
// Input:
//   oldx, oldy      last position of mouse
//   newx, newy      new position of mouse
//   width, height   total size of grafic-window
// Output:
//   zoom            updated zoomfactor

#define maxZoom   0.01f   // 10.f
#define minZoom  160.f
#define dZoom     1.f
#define zoomFakt  10.f   // 500.f

  float d1, d2;

  // printf("trackball_zoom old %.f %.f new %.f %.f siz %.f %.f\n",
          // oldx,oldy,newx,newy,width,height);

  d1 = (newy - oldy);
  d2 = fabsf (d1);
  if (d2 < 1.f) return -1;
  // if (d2 > dZoom) d1 = copysignf (dZoom, d1);
  if (d2 > dZoom) return -1;


  d1 *= (zoomFakt / height);


  *zoom += d1;

    printf("trackball_zoom %f %f\n",*zoom,d1);


  if (*zoom < maxZoom) *zoom = maxZoom;        // min 
  if (*zoom > minZoom) *zoom = minZoom;        // max

  return 0;

}


//================================================================
  int trackball_rot (float *quat,
               float oldx, float oldy,
               float newx, float newy,
               float width, float height, Pointf *cenPt) {
//================================================================
// Input:
//   oldx, oldy      last position of mouse
//   newx, newy      new position of mouse
//   width, height   total size of grafic-window
// Output:
//   quat            updated orientation

 
  float spin_quat[4];
  float x1, y1, x2, y2, dx, dy;

  printf("trackball_rot old %.f %.f new %.f %.f siz %.f %.f\n",
          oldx,oldy,newx,newy,width,height);

  // TODO: center soll nicht screencenter sondern cenPt
  // dx = 0.f; // distance from screenCenter to rotationCenter in screenCoords
  // dy = 0.f; // distance from screenCenter to rotationCenter in screenCoords
  // x1 = 2.0 * oldx - dx;
  // y1 = 2.0 * oldy - dy;
  // x2 = 2.0 * newx - dx;
  // y2 = 2.0 * newy - dy;


  /* drag in progress, simulate trackball */
  x1 = 2.0 * oldx;
  y1 = 2.0 * oldy;
  x2 = 2.0 * newx;
  y2 = 2.0 * newy;

  trackball(spin_quat,
      ( x1 -  width) / width,
      ( height - y1) / height,
      ( x2 - width)  / width,
      ( height - y2) / height);

  add_quats(spin_quat, quat, quat);

}


//================================================================
  int trackball_ortho (Pointf *cenPt, float *zoom, float *quat,
                  Pointf *mdlMax, Pointf *mdlMin) {
//================================================================
// ortho-view:

  // float dx, dy;
  float m[4][4];


  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  //----------------------------------------------------------------
/*
  dx = (mdlMax->x - mdlMin->x) / 2.f * *zoom;   // mdlsiz/2
  dy = (mdlMax->y - mdlMin->y) / 2.f * *zoom;   // mdlsiz/2

  // zoom & pan
  glOrtho (
      cenPt->x + dx, cenPt->x - dx,         // left right (viewport-x)
      cenPt->y + dy, cenPt->y - dy,         // bottom top (viewport-y)
      mdlMin->z, mdlMax->z  // near far  (size of obj * scale)
      // mdlMin->z * *zoom, mdlMax->z * *zoom  // near far  (size of obj * scale)
    );
*/



  //----------------------------------------------------------------
  glOrtho (
      mdlMin->x, mdlMax->x, // near far  (size of obj * scale)
      mdlMin->y, mdlMax->y, // near far  (size of obj * scale)
      mdlMin->z * *zoom, mdlMax->z * *zoom  // near far  (size of obj * scale)
    );
  glTranslatef(cenPt->x, cenPt->y, cenPt->z);
  glScalef (*zoom, *zoom, *zoom);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  build_rotmatrix(m, quat);
  glMultMatrixf(&m[0][0]);



  return 0;

}


//================================================================
  int trackball_persp (Pointf *cenPt, float *scl, float *quat,
                  Pointf *mdlMax, Pointf *mdlMin) {
//================================================================
// perspective-view:


  float m[4][4], dz;



  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();


//----------------------------------------------------------------
/*
  // perspective-view:
  // gluPerspective(zoom, VIEW_ASPECT, 1,100);
  glFrustum (
      -25, 25,   // left right (viewport-x)
      -25, 25,   // bottom top (viewport-y)
       25, 75    // near far  (size of obj * scale)
    );
  gluLookAt (0.0,25.0, 25.0,     // eyepos
             0.0, 0.0, 0.0,      // center of scene
             0.0, 0.0, 1.0);     // up-vector

*/

//----------------------------------------------------------------
//
//  view-         near   scene-  far
//  point                center
//                  |             |
//    x             |      x      |
//                  |             |
//
//    |     2dz     |  dz  |  dz  |


  dz = (mdlMax->z - mdlMin->z) / 2.f;   // mdlsiz/2
    printf(" dz=%.1f scl=%.1f\n",dz,*scl);
  dz /= *scl;


  glFrustum (
      mdlMin->x, mdlMax->x, // left right (viewport-x)
      mdlMin->y, mdlMax->y, // bottom top (viewport-y)
      dz * 2.f, dz * 4.f     // near far  (size of obj * scale)
    );

  dz *= 3.f;


//----------------------------------------------------------------

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();


//----------------------------------------------------------------
/*
    m[0][0] = 3.f;
    m[0][1] = 0.f;
    m[0][2] = 0.f;
    m[0][3] = 0.f;
    m[1][0] = 0.f;
    m[1][1] = 3.f;
    m[1][2] = 0.f;
    m[1][3] = 0.f;
    m[2][0] = 0.f;
    m[2][1] = 0.f;
    m[2][2] = 3.f;
    m[2][3] = 0.0;
    m[3][0] = 0.0;
    m[3][1] = 0.0;
    m[3][2] = 0.0;
    m[3][3] = 1.0;
  glMultMatrixf(m);    // scale !
  glTranslatef (0.f, -5.f, -25.f);

  // glMatrixMode(GL_MODELVIEW);
  // glLoadIdentity();
*/


//----------------------------------------------------------------
  // make axis from quat
  build_rotmatrix(m, quat);

  // scale
  m[0][0] /= *scl;
  m[0][1] /= *scl;
  m[0][2] /= *scl;

  m[1][0] /= *scl;
  m[1][1] /= *scl;
  m[1][2] /= *scl;

  m[2][0] /= *scl;
  m[2][1] /= *scl;
  m[2][2] /= *scl;

  glTranslatef (cenPt->x, cenPt->y, -dz);
  // glTranslatef (0.f, -0.f, -dz);
  glMultMatrixf((const GLfloat *)m);    // scale !

  return 0;

}

// EOF
