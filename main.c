#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#define BLOCK 1024
#define ROTATE_X_DEG 15
#define ROTATE_Y_DEG 15
#define ZOOM_MAX_DEG 5.0f
#define ZOOM_MIN_DEG 0.005f
#define ZOOM_DEG 1.2f
#define MOVE 0.1f
#define MOVE_MAX 1.0f
struct Triangle {
	float x1, y1, z1, x2, y2, z2, x3, y3, z3;
	float r, g, b, a;
};
size_t triangle_len = 0;
struct Triangle *triangles = NULL;
bool lighting = 1;
float zoom;
float rotateX;
float rotateY;
float moveX;
float moveY;
int w, h, s, ow, oh;
void display() {
	w  = glutGet(GLUT_WINDOW_WIDTH);
	h  = glutGet(GLUT_WINDOW_HEIGHT);
	s  = w > h ? h : w;
	ow = w > h ? (w - s) / 2 : 0;
	oh = w < h ? (h - s) / 2 : 0;
	glViewport(ow, oh, s, s);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glPointSize(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glPushMatrix();
	printf("\x1b[2KmX %f, mY %f, rX %f, rY %f, z %f\r\n\x1b[A", moveX, moveY, rotateX, rotateY, zoom);
	glTranslatef(moveX, moveY, 0);
	glScalef(zoom, zoom, zoom);
	glRotatef(rotateY, 1.0f, 0.0f, 0.0f);
	glRotatef(rotateX, 0.0f, 1.0f, 0.0f);
	glLightfv(GL_LIGHT0, GL_POSITION, (float[]){0.0f,0.0f,0.0f,1.0f});
	for (size_t i = 0; i <= triangle_len; ++i) {
		glColor4f(triangles[i].r, triangles[i].g, triangles[i].b, triangles[i].a);
		glBegin(GL_POLYGON);
		glVertex3f(triangles[i].x1, triangles[i].y1, triangles[i].z1);
		glVertex3f(triangles[i].x2, triangles[i].y2, triangles[i].z2);
		glVertex3f(triangles[i].x3, triangles[i].y3, triangles[i].z3);
		glEnd();
	}
	glPopMatrix();
	glutSwapBuffers();
}
void updateLighting(bool l) {
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  (float[]){0.0f,0.0f,0.0f,1.0f});
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  (float[]){1.0f,1.0f,1.0f,1.0f});
	glLightfv(GL_LIGHT0, GL_SPECULAR, (float[]){1.0f,1.0f,1.0f,1.0f});
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	if (l) {
		glEnable (GL_LIGHTING);
	} else {
		glDisable(GL_LIGHTING);
	}
}
void updateRotate() {
	while (rotateX >= 360) rotateX -= 360;
	while (rotateX <  0)   rotateX += 360;
	while (rotateY >= 180) rotateY -= 360;
	while (rotateY < -180) rotateY += 360;
}
void updateMove() {
	if (moveX > +MOVE_MAX) moveX = +MOVE_MAX;
	if (moveX < -MOVE_MAX) moveX = -MOVE_MAX;
	if (moveY > +MOVE_MAX) moveY = +MOVE_MAX;
	if (moveY < -MOVE_MAX) moveY = -MOVE_MAX;
}
void zoomIn() {
	if (zoom < ZOOM_MAX_DEG) zoom *= ZOOM_DEG;
}
void zoomOut() {
	if (zoom > ZOOM_MIN_DEG) zoom /= ZOOM_DEG;
}
void resetView() {
	zoom = 0.1f;
	rotateX = 0.0f;
	rotateY = 0.0f;
	moveX = 0.0f;
	moveY = 0.0f;
}
void special(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_LEFT:  rotateX += ROTATE_X_DEG; break;
		case GLUT_KEY_RIGHT: rotateX -= ROTATE_X_DEG; break;
		case GLUT_KEY_UP:    rotateY += ROTATE_Y_DEG; break;
		case GLUT_KEY_DOWN:  rotateY -= ROTATE_Y_DEG; break;
		default: return;
	}
	updateRotate();
	glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 'i': case 'I': zoomIn();  break;
		case 'o': case 'O': zoomOut(); break;
		case 'A': case 'a': moveX -= MOVE; break;
		case 'D': case 'd': moveX += MOVE; break;
		case 'W': case 'w': moveY += MOVE; break;
		case 'S': case 's': moveY -= MOVE; break;
		case 'Q': case 'q':
			exit(0);
			printf("\x1b[2K");
			return;
		case 'R': case 'r': break;
		case 'T': case 't': resetView(); break;
		case 'L': case 'l': updateLighting(lighting = !lighting); break;
		default: return;
	}
	updateMove();
	glutPostRedisplay();
}
int ix, iy;
bool down0;
bool down1;
bool down2;
void mouse(int button, int up, int x, int y) {
	ix = x; iy = y;
	down0 = down1 = down2 = 0;
	if (button == 0) down0 = !up;
	if (button == 1) down1 = !up;
	if (button == 2) down2 = !up;
	if (button == 3) { zoomIn();  glutPostRedisplay(); }
	if (button == 4) { zoomOut(); glutPostRedisplay(); }
}
void motion(int x, int y) {
	int dx = x-ix,
	    dy = y-iy;
	if (down2) {
		moveX += ((float)dx)/s;
		moveY -= ((float)dy)/s;
		updateMove();
		glutPostRedisplay();
	} else if (down0) {
		rotateX -= ((float)dx)/s*180;
		rotateY -= ((float)dy)/s*180;
		updateRotate();
		glutPostRedisplay();
	}
	ix = x; iy = y;
}
int usage(char *argv0) {
	fprintf(stderr, "\
Usage: %s <file>\n\n\
Controls:\n\
 Arrow keys or move left click to rotate\n\
 W,A,S,D or move right click to move\n\
 I,O or scroll wheel to zoom in/out\n\
 Q to quit\n\
 R to re-render\n\
 T to reset view\n\
 L to toggle lighting\n\
", argv0);
	return 2;
}
int main(int argc, char* argv[]) {
#define INVALID { return usage(argv[0]); }
	char *file_name = NULL;
	bool flag_done = 0;
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-' && argv[i][1] != '\0' && !flag_done) {
			if (argv[i][1] == '-' && argv[i][2] == '\0') flag_done = 1; // -- denotes end of flags (and -o)
			else INVALID
		} else {
			if (file_name) INVALID
			file_name = argv[i];
		}
	}
	if (!file_name) INVALID
	FILE *file;
	if (file_name[0] == '-' && file_name[1] == '\0') {
		file = stdin;
	} else {
		file = fopen(file_name, "r");
	}
	if (!file) {
		fprintf(stderr, "%s: %s\n", file_name, strerror(errno));
		return errno;
	}
	size_t size = 0;
	while (1) {
		struct Triangle tri;
		unsigned long col;
		if (ferror(file)) { fprintf(stderr, "Error reading\n"); return 4; }
		if (feof(file)) break;
		if (fscanf(file, "%e %e %e  %e %e %e  %e %e %e  %lx\n",
			&tri.x1, &tri.y1, &tri.z1, &tri.x2, &tri.y2, &tri.z2, &tri.x3, &tri.y3, &tri.z3, &col)
			!= 10) { fprintf(stderr, "Invalid line %li\n", triangle_len+1); return 3; }
		col &= 0xffffffff;
		if (col <= 0xffffff) {
			col <<= 8;
			col |= 0xff;
		}
		tri.r = (float)((col & 0xff000000) >> 030) / 0xff;
		tri.g = (float)((col & 0x00ff0000) >> 020) / 0xff;
		tri.b = (float)((col & 0x0000ff00) >> 010) / 0xff;
		tri.a = (float)((col & 0x000000ff) >> 000) / 0xff;
		if (triangle_len >= size) {
			size += BLOCK;
			triangles = realloc(triangles, size * sizeof(struct Triangle));
		}
		triangles[triangle_len] = tri;
		if (triangle_len > 0xffffff) {
			fprintf(stderr, "Limit was hit, triangles after this will not be used\n");
			break;
		}
		++triangle_len;
	}
	printf("\n");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	char *title = malloc(strlen(file_name) + 16);
	sprintf(title, "%s%s", "Render: ", file_name);
	glutCreateWindow(title);
	updateLighting(lighting);
	resetView();
	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMainLoop();
	return 0;
}
