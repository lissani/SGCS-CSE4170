#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>
#include "Definitions.h"

Window wd;
Status st;
My_Polygon pg;

// GLUT callbacks
void timer(int value) {
	rotate_points_around_center_of_grivity(&pg);
	glutPostRedisplay();
	if (st.rotation_mode)
		glutTimerFunc(ROTATION_STEP, timer, 0);
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	if (pg.n_points > 0)
		draw_lines_by_points(&pg);
	if (st.polygon_mode) {
		update_center_of_gravity(&pg);
		glColor3f(CENTER_POINT_COLOR);
		glBegin(GL_POINTS);
		glVertex2f(pg.center_x, pg.center_y);
		glEnd();
	}
	if (st.move_mode) {
	}
	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'c':
		if (!st.rotation_mode) {
			pg.n_points = 0, st.polygon_mode = 0;
		}
		glutPostRedisplay();
		break;
	case 'p':
		if (!st.polygon_mode) {
			if (pg.n_points >= 3) {
				st.polygon_mode = 1; //다각형 완성 모드
				fprintf(stderr, "*** Polygon selection is finished!\n");
				glutPostRedisplay();
			}
			else
				fprintf(stderr, "*** Choose at least three points!\n");
		}
		break;
	case 'r':
		if (st.polygon_mode) {
			st.rotation_mode = 1 - st.rotation_mode; //rotation mode on-off
			if (st.rotation_mode)
				glutTimerFunc(ROTATION_STEP, timer, 0);
		}
		break;
	case 'f':
		glutLeaveMainLoop(); 
		break;
	}
}

void special(int key, int x, int y) {
	if (st.rotation_mode || !st.polygon_mode) return;
	switch (key) {
	case GLUT_KEY_LEFT:
		move_points(&pg, -TRANSLATION_OFFSET, 0.0f);
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		move_points(&pg, TRANSLATION_OFFSET, 0.0f);
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		move_points(&pg, 0.0f, -TRANSLATION_OFFSET);
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		move_points(&pg, 0.0f, TRANSLATION_OFFSET);
		glutPostRedisplay();
		break;
	}
}

static int prev_x, prev_y;
void mousepress(int button, int state, int x, int y) {
	if (!st.polygon_mode) {
		if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
			int key_state = glutGetModifiers();
			if (key_state & GLUT_ACTIVE_CTRL) {
				add_point(&pg, &wd, x, y);
				glutPostRedisplay();
			}
		}
	}
	else {
		// 마우스 좌표 -> opengl 좌표 변환
		float glX = 2.0f * ((float)x) / wd.width - 1.0f;
		float glY = 2.0f * ((float)wd.height - y) / wd.height - 1.0f;

		float distance = sqrt(pow(glX - pg.center_x, 2) + pow(glY - pg.center_y, 2));
		float tolerance = 0.05f;

		if (distance < tolerance && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			st.move_mode = 1;
			pg.line_color[0] = 0.0f;
			glutPostRedisplay();

			prev_x = x, prev_y = y;
		}
		else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
			st.move_mode = 0;
			pg.line_color[0] = 1.0f;
			glutPostRedisplay();
		}
	}
}

void mousemove(int x, int y) {
	if (st.move_mode && st.polygon_mode) {
		float delx, dely;
		delx = 2.0f * ((float) x - prev_x) / wd.width;
		dely = 2.0f * ((float) prev_y - y) / wd.height;
		prev_x = x, prev_y = y;
		move_points(&pg, delx, dely);
		glutPostRedisplay();
	}
}

void mouseWheel(int wheel, int direction, int x, int y) {
	//+1 위로 스크롤, -1 아래로 스크롤
	if (direction > 0) {
		scale_polygon(&pg, 0.9f);
		glutPostRedisplay();
	}
	else {
		scale_polygon(&pg, 1.1f);
		glutPostRedisplay();
	}
}
	
void reshape(int width, int height) {
	fprintf(stdout, "### The new window size is %dx%d.\n", width, height);
	wd.width = width, wd.height = height;
	glViewport(0, 0, wd.width, wd.height);
}

void close(void) {
	fprintf(stdout, "\n^^^ The control is at the close callback function now.\n\n");
}
// End of GLUT callbacks

void initialize_polygon_editor(void) {
	wd.width = 800, wd.height = 600, wd.initial_anchor_x = 500, wd.initial_anchor_y = 200;
	st.move_mode = 0, st.rotation_mode = 0, st.polygon_mode = 0;
	pg.n_points = 0; pg.center_x = 0.0f; pg.center_y = 0.0f;
	pg.line_color[0] = 1.0f; pg.line_color[1] = 0.0f; pg.line_color[2] = 0.0f;
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mousepress);
	glutMotionFunc(mousemove);
	glutReshapeFunc(reshape);
	glutCloseFunc(close);
	glutMouseWheelFunc(mouseWheel);
}

void initialize_renderer(void) {
	register_callbacks();
	glPointSize(5.0);
	glClearColor(BACKGROUND_COLOR, 1.0f);
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = TRUE;
	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 4
int main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 SimplefreeGLUTcode_Polygon_Editor";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'p', 'c', 'r', 'f'",
		"    - Special keys used: LEFT, RIGHT, UP, DOWN",
		"    - Mouse used: L-click, R-click and move",
		"    - Other operations: window reshape"
	};

	glutInit(&argc, argv);
	initialize_polygon_editor();

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE); // <-- Be sure to use this profile for this example code!
 //	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_RGBA);

	glutInitWindowSize(wd.width, wd.height);
	glutInitWindowPosition(wd.initial_anchor_x, wd.initial_anchor_y);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

   // glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_EXIT); // default
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	
	glutMainLoop();
	fprintf(stdout, "^^^ The control is at the end of main function now.\n\n");
	return 0;
}
