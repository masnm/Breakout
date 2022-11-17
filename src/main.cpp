#include <bits/stdc++.h>
#include <GL/glut.h>
using namespace std;

#define PI 3.1415926535
#define COLOUR_COMPS 4
const float c_red        [COLOUR_COMPS] = { 1.0, 0.0, 0.0, 1.0 };
const float c_green      [COLOUR_COMPS] = { 0.0, 1.0, 0.0, 1.0 };
const float c_blue       [COLOUR_COMPS] = { 0.0, 0.0, 1.0, 1.0 };
const float c_yellow     [COLOUR_COMPS] = { 1.0, 1.0, 0.0, 1.0 };
const float c_white      [COLOUR_COMPS] = { 1.0, 1.0, 1.0, 1.0 };
const float c_turquoise  [COLOUR_COMPS] = { 0.0, 1.0, 1.0, 1.0 };
const float c_magenta    [COLOUR_COMPS] = { 1.0, 0.0, 1.0, 1.0 };
const float c_black      [COLOUR_COMPS] = { 0.0, 0.0, 0.0, 1.0 };

static size_t width = 1000, height = 1000;
static size_t mouse_x, mouse_y;
static float time_ms, ball_velocity;
static double ball_radious;
static pair<float,float> ball_pos, ball_direction;

static void draw_curcle ( pair<float,float> pos, double radius, const float* color )
{
	double twicePi = 2.0 * 3.142;
	float x = 0, y = 0;
	glColor4fv ( color );
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f ( pos.first, pos.second );
	for (int i = 0; i <= 30; i++)   {
		glVertex2f (
			pos.first + (x + (radius * cos(i * twicePi / 20))),
			pos.second + (y + (radius * sin(i * twicePi / 20)))
			);
	}
	glEnd(); //END
}

static void display_func ( void )
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	time_ms = ((float)glutGet ( GLUT_ELAPSED_TIME ) / 1000.0f );
	gluOrtho2D ( -1*(width/2.0), width/2.0, height/2.0, -1*(height/2.0) );

	// move ball
	ball_pos.first += ball_direction.first * ball_velocity;
	ball_pos.second += ball_direction.second * ball_velocity;

	// ball vs wall collision
	if ( ball_pos.first >= width/2.0 - ball_radious/2.0 ) ball_direction.first *= -1;
	if ( ball_pos.first <= -1.0*(width/2.0) + ball_radious/2.0 ) ball_direction.first *= -1;
	if ( ball_pos.second >= height/2.0 - ball_radious/2.0 ) ball_direction.second *= -1;
	if ( ball_pos.second <= -1.0*(height/2.0) + ball_radious/2.0 ) ball_direction.second *= -1;

	// draw stuff
	draw_curcle ( ball_pos, ball_radious, c_red );

	glutSwapBuffers();
}

static void idle ( void ) { glutPostRedisplay (); }

static void mouse_input ( int button, int state, int x, int y )
{
	if ( button == GLUT_LEFT_BUTTON ) {
		if ( state == GLUT_DOWN ) { }
		if ( state == GLUT_UP ) { }
	}
}

static void mouse_motion_input ( int x, int y )
{
	mouse_x = x; mouse_y = y;
}

static void keyboard_input ( unsigned char key, int x, int y )
{
	switch ( key ) {
		case 8: break;
		case 9: break;
		case 'A': break;
		default: break;
	}
}

void shape_menu ( int id )
{
	glutPostRedisplay();
}
void top_menu ( int id )
{
	switch ( id ) {
		case 1: exit ( 1 ); break;
	}
}

void initialize_all ( void )
{
	ball_pos = make_pair ( 0.0f, 0.0f );
	ball_direction = make_pair ( 1.0f, -1.0f );
	ball_velocity = 1.0f;
	ball_radious = 15.0;
}

void cleanup_all ( void )
{
}

int main ( int argc, char* argv[] )
{
	glutInit ( &argc, argv );
	glutInitWindowSize ( width, height );
	glutInitWindowPosition ( 10, 10 );
	glutInitDisplayMode ( GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);

	glutCreateWindow ( "GLUT Template" );

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );
	glClearColor(0.0,0.0,0.0,0.0);

	glutDisplayFunc ( display_func );
	glutIdleFunc ( idle );
	glutMouseFunc ( mouse_input );
	glutKeyboardFunc ( keyboard_input );
	glutPassiveMotionFunc ( mouse_motion_input );

	initialize_all ();
	atexit ( cleanup_all );
	glutMainLoop();

	return 0;
}
