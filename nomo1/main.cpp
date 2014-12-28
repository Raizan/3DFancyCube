#include "Angel2.h"

using namespace std;

typedef Angel2::vec4  color4;
typedef Angel2::vec4  point4;

const int NumTriangles = 12;
const int NumVertices = 8;
const int NumElements = 3 * NumTriangles;

// cube ///////////////////////////////////////////////////////////////////////
//    v2----- v6
//   /|      /|
//  v3------v7|
//  | |     | |
//  | |v0---|-|v4
//  |/      |/
//  v1------v5

// Each vertex now appears only once, so we have only 8 rather than 36
vec3 points[NumVertices] = {    vec3( -0.5, -0.5, -0.5 ), vec3( -0.5, -0.5, 0.5 ),
                                vec3( -0.5, 0.5, -0.5 ), vec3( -0.5, 0.5, 0.5 ),
                                vec3( 0.5, -0.5, -0.5 ), vec3( 0.5, -0.5, 0.5 ),
                                vec3( 0.5, 0.5, -0.5 ), vec3( 0.5, 0.5, 0.5 ) };


vec3 normals[NumElements] = {   vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0),
                                vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0),
                                vec3(0.0, 0.0, -1.0),vec3(0.0, 0.0, -1.0),vec3(0.0, 0.0, -1.0),
                                vec3(0.0, 0.0, -1.0),vec3(0.0, 0.0, -1.0),vec3(0.0, 0.0, -1.0),
                                vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0),
                                vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0),
                                vec3(-1.0, 0.0, 0.0),vec3(-1.0, 0.0, 0.0),vec3(-1.0, 0.0, 0.0),
                                vec3(-1.0, 0.0, 0.0),vec3(-1.0, 0.0, 0.0),vec3(-1.0, 0.0, 0.0),
                                vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 0.0),
                                vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 0.0),
                                vec3(0.0, -1.0, 0.0),vec3(0.0, -1.0, 0.0),vec3(0.0, -1.0, 0.0),
                                vec3(0.0, -1.0, 0.0),vec3(0.0, -1.0, 0.0),vec3(0.0, -1.0, 0.0) };

// The following builds triangles from the 8 vertices above,
// using numbers 0-7 to refer to the element positions in the array
GLuint elements[NumElements] = {    1, 5, 3,        7, 3, 5,    //front
                                    0, 2, 4,        4, 2, 6,    //back
                                    4, 6, 5,        7, 5, 6,    //right
                                    0, 1, 2,        3, 2, 1,    //left
                                    2, 3, 6,        7, 6, 3,    //top
                                    0, 1, 4,        5, 4, 1 };  //bottom

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;
GLint timeParam;
GLint windowHeight=512, windowWidth=512;

// Perspective projection
mat4 projection = Frustum(-0.2, 0.2, -0.2, 0.2, 0.2, 2.0);

// Move camera backwards relative to everything else
mat4 view = Translate(0.0, 0.0, -0.35);

//----------------------------------------------------------------------------

void
init( void )
{

    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );

    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals),
		  NULL, GL_STATIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(normals), normals );

    // ADDED: load the element index data
    GLuint elementBufferId;
    glGenBuffers(1, &elementBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader_a6.glsl", "fshader_a6.glsl" );
    glUseProgram( program );

    // Initialize the vertex position attribute from the vertex shader
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(points)) );

    // Initialize shader lighting parameters
    point4 light_position( 1.0, 1.0, -1.0, 0.0 );
    color4 light_ambient( 0.5, 0.5, 0.5, 0.5 );
    //color4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
    color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
    color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

    color4 material_ambient( 0.2, 0.2, 0.2, 1.0 );
    color4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );
    color4 material_specular( 1.0, 0.8, 0.0, 1.0 );
    float  material_shininess = 100.0;

    color4 ambient_product = light_ambient * material_ambient;
    color4 diffuse_product = light_diffuse * material_diffuse;
    color4 specular_product = light_specular * material_specular;

    glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),
		  1, ambient_product );
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),
		  1, diffuse_product );
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),
		  1, specular_product );

    glUniform4fv( glGetUniformLocation(program, "LightPosition"),
		  1, light_position );

    glUniform1f( glGetUniformLocation(program, "Shininess"),
		 material_shininess );

    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );

    glEnable( GL_DEPTH_TEST );
    //glShadeModel(GL_FLAT);
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); /* white background */
}

void drawCube(mat4 model) {
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, Projection*view*model);
    glDrawElements(GL_TRIANGLES, NumElements, GL_UNSIGNED_INT, NULL);
}
//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT   );
    timeParam = glutGet(GLUT_ELAPSED_TIME);
    float angle = 0.001*timeParam;
    angle = angle * 7.0 * 180.0 / 22.0;

    //kiri atas
    mat4 mTransform1 =  Translate(-0.5 , -0.5, 0.0) * Scale(0.3) * RotateX(angle) * RotateY(angle) ;
    mat4 mTransform2 =  Translate(-0.5 , 0.5, 0.0) *Scale(0.3) * RotateX(angle) * RotateY(angle) ;
    mat4 mTransform3 =  Translate(0.5 , -0.5, 0.0) *Scale(0.3) * RotateX(angle) * RotateY(angle) ;
    mat4 mTransform4 =  Translate(0.5 , 0.5, 0.0) *Scale(0.3) * RotateX(angle) * RotateY(angle) ;
    //mat4 mTransform =  Scale(0.5) ;
    drawCube(mTransform1);
    drawCube(mTransform2);
    drawCube(mTransform3);
    drawCube(mTransform4);

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
    }
}

//----------------------------------------------------------------------------

void
idle( void )
{
    glutPostRedisplay();
}

void reshape( int width, int height ) {

    windowWidth = width;
    windowHeight = height;

    glViewport(0, 0, width, height);

    //projection = Frustum(-0.2, 0.2, -0.2, 0.2, 0.2, 2.0);
    mat4 projection = Frustum(-0.2*(float)width/(float)height, 0.2*(float)width/(float)height, -0.2, 0.2, 0.2, 2.0);

    //GLfloat aspect = GLfloat(width)/height;
    //mat4  projection = Perspective( 45.0, aspect, 0.5, 3.0 );

    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    //glutInitContextVersion( 3, 2 );
    //glutInitContextProfile( GLUT_CORE_PROFILE );
    glutCreateWindow( "Contoh Lighting Menggunakan Vertex Shader" );

    glewInit();

    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutIdleFunc( idle );
    glutReshapeFunc( reshape );

    glutMainLoop();
    return 0;
}
