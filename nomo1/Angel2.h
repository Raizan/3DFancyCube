#ifndef ANGEL2_H_INCLUDED
#define ANGEL2_H_INCLUDED

//----------------------------------------------------------------------------
//
// --- Include system headers ---
//

#include <cmath>
#include <iostream>
#include <windows.h>

//  Define M_PI in the case it's not defined in the math header file
#ifndef M_PI
#  define M_PI  3.14159265358979323846
#endif

//----------------------------------------------------------------------------
//
// --- Include OpenGL header files and helpers ---
//
//   The location of these files vary by operating system.  We've included
//     copies of open-soruce project headers in the "GL" directory local
//     this this "include" directory.
//

// Rowan: Alas, OS X GLUT won't work with GLSL 1.50 anyway.
//        Recent versions of XQuartz include freeglut, which should work.
#ifdef __APPLE__  // include Mac OS X verions of headers
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else // non-Mac OS X operating systems

// Comment out the following line to use glext instead of glew
#  include <GL/glew.h>
//#    include <GL/gl.h>
//#    include <GL/glext.h>
//#    include <GL/freeglut.h>
//#    include <GL/freeglut_ext.h>

#  ifdef __glew_h__

// glewExperimental may be needed with core profiles
#    define glewInit(x)       glewExperimental = GL_TRUE; glewInit(x);
#    include <GL/freeglut.h>
#    include <GL/freeglut_ext.h>

#  else

// without glew, define glewinit to do nothing
#    define glewInit(x)
#    define GL_GLEXT_PROTOTYPES
#    include <GL/gl.h>
//#    include <GL/glcorearb.h>
#    include <GL/glext.h>
#    include <GL/freeglut.h>
#    include <GL/freeglut_ext.h>

#  endif  // __glew_h__

#endif  // __APPLE__

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

//----------------------------------------------------------------------------
//
//  --- Include our class libraries and constants ---
//

namespace Angel2 {

//  Helper function to load vertex and fragment shader files
GLuint InitShader( const char* vertexShaderFile,
		   const char* fragmentShaderFile );

//  Defined constant for when numbers are too small to be used in the
//    denominator of a division operation.  This is only used if the
//    DEBUG macro is defined.
const GLfloat  DivideByZeroTolerance = GLfloat(1.0e-07);

//  Degrees-to-radians constant
const GLfloat  DegreesToRadians = M_PI / 180.0;

}  // namespace Angel

#include "vec.h"
#include "mat.h"
#include "CheckError.h"

#define Print(x)  do { std::cerr << #x " = " << (x) << std::endl; } while(0)

//  Globally use our namespace in our example programs.
using namespace Angel2;


#endif // ANGEL2_H_INCLUDED
