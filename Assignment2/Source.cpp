#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include "imageloader.h"
#include "glm.h"
#define PI 3.14

double eye[] = { 0.0, 0.0, 7.0 };
double center[] = { 0.0, 0.0, 0.0 };
double up[] = { 0.0, 1.0, 0.0 };



static int shoulderUp = 0, shoulderRot=0, elbow = 0, fingerBase = 0, fingerUp = 0, lLegUp=0,lLegSide=0,lLowerLeg=0, rLegUp = 0, rLegSide = 0, rLowerLeg = 0;
int moving, startx, starty;
const char* floorTexture = "images/floor1.bmp";
const char* modelName1 = "data/tavolino.obj";

const char* modelName2 = "data/soccerball.obj";

GLfloat angle = 0.0;   /* in degrees */
GLfloat angle2 = 0.0;   /* in degrees */
GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
GLfloat light_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
// x , y, z, w
GLfloat light_position[] = { 0.5, 5.0, 0.0, 1.0 };
GLfloat lightPos1[] = { -0.5, -5.0, -2.0, 1.0 };

GLuint loadTexture(Image* image)
{
    GLuint textureId;
    glGenTextures(1, &textureId);			 //Make room for our texture
    glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
    //Map the image to the texture
    glTexImage2D(GL_TEXTURE_2D,				  //Always GL_TEXTURE_2D
        0,							  //0 for now
        GL_RGB,					  //Format OpenGL uses for image
        image->width, image->height, //Width and height
        0,							  //The border of the image
        GL_RGB,					  //GL_RGB, because pixels are stored in RGB format
        GL_UNSIGNED_BYTE,			  //GL_UNSIGNED_BYTE, because pixels are stored
                                     //as unsigned numbers
        image->pixels);			  //The actual pixel data
    return textureId;						  //Returns the id of the texture
}

GLuint _textureId; //The id of the texture
void initRendering()
{
    Image* image = loadBMP(floorTexture);
    _textureId = loadTexture(image);
    delete image;
    // Turn on the power
    glEnable(GL_LIGHTING);
    // Flip light switch
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    // assign light parameters
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
    // Material Properties
    GLfloat lightColor1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor1);
    glEnable(GL_NORMALIZE);
    //Enable smooth shading
    glShadeModel(GL_SMOOTH);
    // Enable Depth buffer
    glEnable(GL_DEPTH_TEST);
}
void drawmodel(char* filename)
{
    GLMmodel* model = glmReadOBJ(filename);
    glmUnitize(model);
    glmFacetNormals(model);
    glmVertexNormals(model, 90.0);
    glmScale(model, .15);
    glmDraw(model, GLM_SMOOTH | GLM_MATERIAL);
}
void init(void)
{
    
    glMatrixMode(GL_PROJECTION);
    gluPerspective(60.0,
        1.0,
        1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);

}
void rotatePoint(double a[], double theta, double p[])
{

    double temp[3];
    temp[0] = p[0];
    temp[1] = p[1];
    temp[2] = p[2];

    temp[0] = -a[2] * p[1] + a[1] * p[2];
    temp[1] = a[2] * p[0] - a[0] * p[2];
    temp[2] = -a[1] * p[0] + a[0] * p[1];

    temp[0] *= sin(theta);
    temp[1] *= sin(theta);
    temp[2] *= sin(theta);

    temp[0] += (1 - cos(theta)) * (a[0] * a[0] * p[0] + a[0] * a[1] * p[1] + a[0] * a[2] * p[2]);
    temp[1] += (1 - cos(theta)) * (a[0] * a[1] * p[0] + a[1] * a[1] * p[1] + a[1] * a[2] * p[2]);
    temp[2] += (1 - cos(theta)) * (a[0] * a[2] * p[0] + a[1] * a[2] * p[1] + a[2] * a[2] * p[2]);

    temp[0] += cos(theta) * p[0];
    temp[1] += cos(theta) * p[1];
    temp[2] += cos(theta) * p[2];

    p[0] = temp[0];
    p[1] = temp[1];
    p[2] = temp[2];
}
void crossProduct(double a[], double b[], double c[])
{
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
}
void normalize(double a[])
{
    double norm;
    norm = a[0] * a[0] + a[1] * a[1] + a[2] * a[2];
    norm = sqrt(norm);
    a[0] /= norm;
    a[1] /= norm;
    a[2] /= norm;
}
void leg(int dir , float side, float up, float lower ) {
    glPushMatrix();

    glTranslatef(dir*0.4, -1.5, 0.0);
    glRotatef((GLfloat)side, 0.0, 0.0, 1.0);
    glRotatef((GLfloat)up, 1.0, 0.0, 0.0);
    glTranslatef(0.0, -0.75, 0.0);
    glPushMatrix();
    glScalef(0.4, 1.5, 0.6);
    glutWireCube(1);
    glPopMatrix();


    //lower leg
    glTranslatef(0.0, -0.75, 0.0);
    glRotatef((GLfloat)lower, 1.0, 0.0, 0.0);
    glTranslatef(0.0, -0.75, 0.0);
    glPushMatrix();

    glScalef(0.4, 1.5, 0.6);
    glutWireCube(1);
    
    glPopMatrix();

    //foot
    glTranslatef(0.0, -0.95, 0.0);
    glScalef(0.4, 0.4, 1.8);
    glutSolidCube(1);

    glPopMatrix();
}
void fingers(int dir ,int th ,float z){
    glPushMatrix();

    glTranslatef(dir*th*0.17, -0.45, z);
    glRotatef(dir * th * (GLfloat)fingerBase, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.075, 0.0);
    //Draw upper part
    glPushMatrix();
    glScalef(0.06, 0.15, 0.06);
    glutWireCube(1);
    glPopMatrix();
    //Draw lower part

    glTranslatef(0.0, -0.075, 0.0);
    glRotatef(dir * th * (GLfloat)fingerUp, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -0.075, 0.0);
    glScalef(0.06, 0.15, 0.06);
    glutWireCube(1);
    glPopMatrix();

    //end lower part
}
void arm(int dir) {
    glPushMatrix();

    glTranslatef(0.9*dir, 1.25, 0.0);
    glRotatef(dir*(GLfloat)shoulderUp, 0.0, 0.0, 1.0);
    glRotatef(dir*(GLfloat)shoulderRot, 0.0, 1.0, 0.0);
    glTranslatef(0.0, -0.45, 0.0);
    //shoulder
    glPushMatrix();
    glScalef(0.4, 0.9, 0.4);
    glutWireCube(1);
    glPopMatrix();

    //elbow
    glTranslatef(0.0, -0.45, 0.0);
    glRotatef((GLfloat)elbow, 1.0, 0.0, 0.0);
    glTranslatef(0.0, -0.45, 0.0);
    glPushMatrix();
    glScalef(0.4, 0.9, 0.4);
    glutWireCube(1);
    glPopMatrix();
    //fingers
    fingers(dir, 1, 0.17);
    fingers(dir, 1, 0.0);
    fingers(dir, 1, -0.17);
    fingers(dir, -1, 0.0);
    
    glPopMatrix();
}
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2],
        center[0], center[1], center[2],
        up[0], up[1], up[2]);
   
    glPushMatrix();
    glRotatef(angle2, 1.0, 0.0, 0.0);
    glRotatef(angle, 0.0, 1.0, 0.0);
    glPushMatrix();
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glPopMatrix();

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS);
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-8, -3.9, 8);
    glTexCoord2f(3.0f, 0.0f); glVertex3f(8, -3.9, 8);
    glTexCoord2f(3.0f, 3.0f); glVertex3f(8, -3.9, -8);
    glTexCoord2f(0.0f, 3.0f); glVertex3f(-8, -3.9, -8);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glTranslatef(0.0, 1.0, 0.0);

    
//right leg
    leg(-1, (GLfloat)rLegSide, (GLfloat)rLegUp, (GLfloat)rLowerLeg);
   
//left leg
    leg(1, (GLfloat)lLegSide, (GLfloat)lLegUp, (GLfloat)lLowerLeg);
   

//let arm
    arm(1);
//right arm
    arm(-1);

//head
    glPushMatrix();
    glTranslatef(0.0, 2.0, 0.0);
    glutWireSphere(0.375, 20, 8);
    glPopMatrix();
//body
    glPushMatrix();
    glScalef(1.2, 3.0, 0.6);
    glutWireCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0, -4.2, 6.0);
    glScalef(5, 5, 5);
    drawmodel((char*)modelName2);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0, -3.7, -6.0);
    glScalef(200, 200, 200);
    drawmodel((char *)modelName1);
    glPopMatrix();
    glPopMatrix();
    glutSwapBuffers();
}
void reset()
{
    double e[] = { 0.0, 0.0, 5.0 };
    double c[] = { 0.0, 0.0, 0.0 };
    double u[] = { 0.0, 1.0, 0.0 };
    for (int i = 0; i < 3; i++)
    {
        eye[i] = e[i];
        center[i] = c[i];
        up[i] = u[i];
    }
}
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(85.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -5.0);
}
void sitLimit(int *angle,int dir, int limite) {
     if (*angle != limite) {
            *angle = (*angle + (5*dir)) % 360;
            glutPostRedisplay();
        }
}
void horizontalLook(int dir)
{
    //1 for right -1 for left
    rotatePoint(up, dir * PI / 16, eye);
}
void screen_menu(int value)
{

    switch (value)
    {
    case '1':
        floorTexture = "images/floor1.bmp";
        break;
    case '2':
        floorTexture = "images/floor2.bmp";
        break;
    case '3':
        floorTexture = "images/floor3.bmp";
        break;
    case '4':
        floorTexture = "images/floor4.bmp";
        break;
    case '5':
        floorTexture = "images/floor5.bmp";
        break;
    case '6':
        floorTexture = "images/floor6.bmp";
        break;
    }
    reset();
    initRendering();
    glutPostRedisplay();
}
void attachMenu()
{
    glutCreateMenu(screen_menu);
    glutAddMenuEntry("Models", 0);
    glutAddMenuEntry("", 0);
    glutAddMenuEntry("floor1", '1');
    glutAddMenuEntry("floor2", '2');
    glutAddMenuEntry("floor3", '3');
    glutAddMenuEntry("floor4", '4');
    glutAddMenuEntry("floor5", '5');
    glutAddMenuEntry("floor6", '6');
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}



void verticalLook(int dir)
{
    //1 for up -1 for down
    double horizontal[3];
    double look[] = { center[0] - eye[0], center[1] - eye[1], center[2] - eye[2] };
    crossProduct(up, look, horizontal);
    normalize(horizontal);
    rotatePoint(horizontal, dir*PI / 16, eye);
    rotatePoint(horizontal, dir*PI / 16, up);
}



// Forward and Backward
void fWardBward(int dir)
{
    //forward 1
    //backward -1
    double direction[3];
    direction[0] = center[0] - eye[0];
    direction[1] = center[1] - eye[1];
    direction[2] = center[2] - eye[2];
    float speed = 0.1;
    eye[0] += dir * direction[0] * speed;
    eye[1] += dir * direction[1] * speed;
    eye[2] += dir * direction[2] * speed;

    center[0] += dir * direction[0] * speed;
    center[1] += dir * direction[1] * speed;
    center[2] += dir * direction[2] * speed;
}


void specialKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        horizontalLook(-1);
        break;
    case GLUT_KEY_RIGHT:
        horizontalLook(1);
        break;
    case GLUT_KEY_UP:
        verticalLook(1);
        break;
    case GLUT_KEY_DOWN:
        verticalLook(-1);
        break;
    }
    glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'x':
        fWardBward(1);
        glutPostRedisplay();
        break;
    case 'c':
        fWardBward(-1);
        glutPostRedisplay();
        break;
    case 'z':
        reset();
        glutPostRedisplay();
        break;
    case 's':
        sitLimit(&shoulderUp, 1, 180);
        break;
    case 'S':
        sitLimit(&shoulderUp, -1, -45);
        break;
    case 'r':
        sitLimit(&shoulderRot, 1, 90);
        break;
    case 'R':
        sitLimit(&shoulderRot, -1, -90);
        break;
    case 'e':
        sitLimit(&elbow, 1, 0);
        break;
    case 'E':
        sitLimit(&elbow, -1, -140);
        break;
    case 'l':
        sitLimit(&lLegUp, 1, 90);
        break;
    case 'L':
        sitLimit(&lLegUp, -1, -90);
        break;
    case 'k':
        sitLimit(&lLegSide, 1, 90);
        break;
    case 'K':
        sitLimit(&lLegSide, -1, -10);
        break;
    case 'j':
        sitLimit(&lLowerLeg, 1, 120);
        break;
    case 'J':
        sitLimit(&lLowerLeg, -1, 0);
        break;
    case 'p':
        sitLimit(&rLegUp, 1, 90);
        break;
    case 'P':
        sitLimit(&rLegUp, -1, -90);
        break;
    case 'o':
        sitLimit(&rLegSide, 1, 10);
 
        break;
    case 'O':
        sitLimit(&rLegSide, -1, -90);
        break;
    case 'i':
        sitLimit(&rLowerLeg, 1, 120);
        break;
    case 'I':
        sitLimit(&rLowerLeg, -1, 0);
        break;
    case 'f':
        sitLimit(&fingerBase, 1, 20);
        break;
    case 'F':
        sitLimit(&fingerBase, -1, -40);
        break;
    case 'g':
        sitLimit(&fingerUp, 1, 0);
        break;
    case 'G':
        sitLimit(&fingerUp, -1, -90);
        break;

    case 27:
        exit(0);
        break;
    default:
        break;
    }
}

static void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            moving = 1;
            startx = x;
            starty = y;
        }
        if (state == GLUT_UP) {
            moving = 0;
        }
    }
}


static void motion(int x, int y)
{
    if (moving) {
        angle = angle + (x - startx);
        angle2 = angle2 + (y - starty);
        startx = x;
        starty = y;
        glutPostRedisplay();
    }
}



int main(int argc, char** argv)
{
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    initRendering();
    attachMenu();
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMainLoop();
    init();
    return 0;
}