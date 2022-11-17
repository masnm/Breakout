#include <bits/stdc++.h>
#include <GL/freeglut.h>
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

struct Brick {
	Brick () = default;
	Brick ( pair<float,float> tl, pair<float,float> br, int _l )
		: top_left(tl), bottom_right(br), life(_l) {}
	pair<float,float> top_left;
	pair<float,float> bottom_right;
	int life;
};

static size_t width = 1000, height = 1000;
static size_t mouse_x, mouse_y;
static float time_ms, ball_velocity, bat_velocity, bat_length,
			 bat_direction;
static double ball_radious;
static pair<float,float> ball_pos, ball_direction,
	bat_top_left, bat_bottom_right, brick_size;
static bool game_over, game_paused;
static int score;
static vector<Brick> bricks;

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

static void draw_block ( pair<float,float> top_left, pair<float,float> bottom_right, const float* color )
{
	glColor4fv ( color );
	glBegin ( GL_QUADS );
		glVertex2f ( top_left.first,     top_left.second     );
		glVertex2f ( bottom_right.first, top_left.second     );
		glVertex2f ( bottom_right.first, bottom_right.second );
		glVertex2f ( top_left.first,     bottom_right.second );
	glEnd ();
}

static bool ball_inside_bat ( void )
{
	float x = ball_pos.first, y = ball_pos.second + ball_radious;
	if ( x >= bat_top_left.first && x <= bat_bottom_right.first &&
			y >= bat_top_left.second && y <= bat_bottom_right.second )
		return true;
	x = ball_pos.first, y = ball_pos.second - ball_radious;
	if ( x >= bat_top_left.first && x <= bat_bottom_right.first &&
			y >= bat_top_left.second && y <= bat_bottom_right.second )
		return true;
	x = ball_pos.first + ball_radious, y = ball_pos.second;
	if ( x >= bat_top_left.first && x <= bat_bottom_right.first &&
			y >= bat_top_left.second && y <= bat_bottom_right.second )
		return true;
	x = ball_pos.first - ball_radious, y = ball_pos.second;
	if ( x >= bat_top_left.first && x <= bat_bottom_right.first &&
			y >= bat_top_left.second && y <= bat_bottom_right.second )
		return true;
	else
		return false;
}

static bool ball_inside_brick ( Brick& brick )
{
	float x = ball_pos.first, y = ball_pos.second + ball_radious;
	if ( x >= brick.top_left.first && x <= brick.bottom_right.first &&
			y >= brick.top_left.second && y <= brick.bottom_right.second )
		return 1;
	x = ball_pos.first, y = ball_pos.second - ball_radious;
	if ( x >= brick.top_left.first && x <= brick.bottom_right.first &&
			y >= brick.top_left.second && y <= brick.bottom_right.second )
		return 2;
	x = ball_pos.first + ball_radious, y = ball_pos.second;
	if ( x >= brick.top_left.first && x <= brick.bottom_right.first &&
			y >= brick.top_left.second && y <= brick.bottom_right.second )
		return 3;
	x = ball_pos.first - ball_radious, y = ball_pos.second;
	if ( x >= brick.top_left.first && x <= brick.bottom_right.first &&
			y >= brick.top_left.second && y <= brick.bottom_right.second )
		return 4;
	else
		return 0;
}

static void next_game ( void )
{
	glColor4fv ( c_white );
	glRasterPos2f ( -100, 0 );
	const unsigned char ov[] = "GAME OVER";
	glutBitmapString ( GLUT_BITMAP_HELVETICA_18, ov );
	const unsigned char ag[] = "Press Any Key to Play Again";
	glRasterPos2f ( -150, 30 );
	glutBitmapString ( GLUT_BITMAP_HELVETICA_18, ag );
}

static void game_play ( void )
{
	// move ball
	ball_pos.first += ball_direction.first * ball_velocity;
	ball_pos.second += ball_direction.second * ball_velocity;

	// move bat
	bat_top_left.first += bat_velocity*bat_direction;
	bat_bottom_right.first += bat_velocity*bat_direction;
	// clamp bat
	if ( bat_top_left.first <= -1.0*(width/2.0) ) {
		bat_top_left.first = -1.0*(width/2.0);
		bat_bottom_right.first = bat_top_left.first + 2.0*bat_length;
	}
	if ( bat_bottom_right.first >= width/2.0 ) {
		bat_bottom_right.first = width/2.0;
		bat_top_left.first = bat_bottom_right.first - 2.0*bat_length;
	}

	// ball vs wall collision
	if ( ball_pos.first >= width/2.0 - ball_radious/2.0 ) ball_direction.first *= -1;
	if ( ball_pos.first <= -1.0*(width/2.0) + ball_radious/2.0 ) ball_direction.first *= -1;
	if ( ball_pos.second >= height/2.0 - ball_radious/2.0 ) {
		ball_direction.second *= -1;
		game_over = true;
	}
	if ( ball_pos.second <= -1.0*(height/2.0) + ball_radious/2.0 ) ball_direction.second *= -1;

	// ball vs bat collision
	if ( ball_inside_bat () ) {
		ball_direction.second *= -1;
	}

	// ball vs brick collision
	for ( auto& brick : bricks ) {
		int code = ball_inside_brick ( brick );
		switch ( code ) {
			case 1:
			case 2:
				ball_direction.second *= -1;
				brick.life -= 1;
				score += 1;
				break;
			case 3:
			case 4:
				ball_direction.first *= -1;
				brick.life -= 1;
				score += 1;
				break;
			case 0:
				break;
		}
	}
	// remove life less bricks
	std::erase_if ( bricks, [] ( auto& brick ) {
			return brick.life < 1;
			});

	// draw stuff
	// draw ball
	draw_curcle ( ball_pos, ball_radious, c_red );
	// draw bat
	draw_block ( bat_top_left, bat_bottom_right, c_green );
	// draw bricks
	for ( auto& brick : bricks ) {
		const float *tc = NULL;
		if ( brick.life == 1 ) {
			tc = c_turquoise;
		} else if ( brick.life == 2 ) {
			tc = c_magenta;
		} else {
			tc = c_blue;
		}
		draw_block ( brick.top_left, brick.bottom_right, tc );
	}
	// draw score
	glColor4fv ( c_white );
	glRasterPos2f ( -495, -480 );
	string scr = to_string ( score );
	auto ch = reinterpret_cast<unsigned char*>(const_cast<char*>(scr.c_str()));
	glutBitmapString ( GLUT_BITMAP_HELVETICA_18, ch );
}

void paused_game ( void )
{
	// draw ball
	draw_curcle ( ball_pos, ball_radious, c_red );
	// draw bat
	draw_block ( bat_top_left, bat_bottom_right, c_green );
	// draw bricks
	for ( auto& brick : bricks ) {
		const float *tc = NULL;
		if ( brick.life == 1 ) {
			tc = c_turquoise;
		} else if ( brick.life == 2 ) {
			tc = c_magenta;
		} else {
			tc = c_blue;
		}
		draw_block ( brick.top_left, brick.bottom_right, tc );
	}
	// draw score
	glColor4fv ( c_white );
	glRasterPos2f ( -495, -480 );
	string scr = to_string ( score );
	auto ch = reinterpret_cast<unsigned char*>(const_cast<char*>(scr.c_str()));
	glutBitmapString ( GLUT_BITMAP_HELVETICA_18, ch );
	glRasterPos2f ( -10, 0 );
	string ss = "PAUSED";
	ch = reinterpret_cast<unsigned char*>(const_cast<char*>(ss.c_str()));
	glutBitmapString ( GLUT_BITMAP_HELVETICA_18, ch );
}

static void display_func ( void )
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	time_ms = ((float)glutGet ( GLUT_ELAPSED_TIME ) / 1000.0f );
	gluOrtho2D ( -1*(width/2.0), width/2.0, height/2.0, -1*(height/2.0) );

	if ( game_over ) {
		next_game ();
	} else if ( game_paused ) {
		paused_game ();
	} else {
		game_play ();
	}

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

void initialize_all ( void )
{
	ball_pos = make_pair ( 0.0f, 0.0f );
	ball_direction = make_pair ( 1.0f, -1.0f );
	ball_velocity = 5.0f;
	ball_radious = 13.0;

	bat_length = 70.0f;
	bat_velocity = 8.0f;
	bat_top_left = make_pair ( -bat_length, (height/2.0)-50.0 );
	bat_bottom_right = make_pair ( bat_length, (height/2.0)-30.0 );
	bat_direction = 0.0;

	score = 0;
	game_over = false;

	brick_size = make_pair ( 50.0, 20.0 );
	for ( float row = -500.0 + 40.0 ; row < -300.0 ; row += 30.0 ) {
		for ( float col = -455.0 ; col < 430.0 ; col += 60 ) {
			bricks.emplace_back (
					make_pair ( col + 5.0, row ),
					make_pair ( col + 5.0 + brick_size.first, row + brick_size.second ),
					1 + rand() % 3
					);
		}
	}
	game_paused = false;
}

static void keyboard_input ( unsigned char key, int x, int y )
{
	if ( game_over == true ) {
		initialize_all ();
		game_over = false;
	}
	if ( key == (unsigned char)27 )
	{
		game_paused ^= true;
	}
}

void cleanup_all ( void )
{
}

static void arrow_keys_down ( int key, int x, int y )
{
	(void) x; (void) y;
	switch ( key ) {
		case GLUT_KEY_LEFT:
			bat_direction = -1.0f;
			break;
		case GLUT_KEY_RIGHT:
			bat_direction = 1.0f;
			break;
	}
}

static void arrow_keys_up ( int key, int x, int y )
{
	(void) x; (void) y;
	switch ( key ) {
		case GLUT_KEY_LEFT:
		case GLUT_KEY_RIGHT:
			bat_direction = 0.0;
			break;
	}
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
	glutSpecialFunc ( arrow_keys_down );
	glutSpecialUpFunc ( arrow_keys_up );

	initialize_all ();
	atexit ( cleanup_all );
	glutMainLoop();

	return 0;
}
