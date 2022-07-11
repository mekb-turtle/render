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
struct Triangle {
	float x1, y1, z1, x2, y2, z2, x3, y3, z3;
	float r, g, b, a;
};
size_t triangle_len = 0;
struct Triangle *triangles = NULL;
float zoom = 0.1f;
float rotateX = 0.0f;
float rotateY = 0.0f;
void display() {
	int w = glutGet(GLUT_WINDOW_WIDTH),
	    h = glutGet(GLUT_WINDOW_HEIGHT),
	    s = w > h ? h : w,
	   ow = w > h ? (w - s) / 2 : 0,
	   oh = w < h ? (h - s) / 2 : 0;
	glViewport(ow, oh, s, s);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glPointSize(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glPushMatrix();
	glScalef(zoom, zoom, zoom);
	printf("\x1b[2K%f %f %f\r\n\x1b[A", rotateX, rotateY, zoom);
	glRotatef(rotateY, 1.0f, 0.0f, 0.0f);
	glRotatef(rotateX, 0.0f, 1.0f, 0.0f);
	for (size_t i = 0; i <= triangle_len; ++i) {
		glColor4f(triangles[i].r, triangles[i].g, triangles[i].b, triangles[i].a);
		glBegin(GL_POLYGON);
		glVertex3f(triangles[i].x1, triangles[i].y1, triangles[i].z1);
		glVertex3f(triangles[i].x2, triangles[i].y2, triangles[i].z2);
		glVertex3f(triangles[i].x3, triangles[i].y3, triangles[i].z3);
		glEnd();
	}
	glPopMatrix();
	glFlush();
}
void special(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_LEFT:  rotateX += 15; break;
		case GLUT_KEY_UP:    rotateY += 15; break;
		case GLUT_KEY_RIGHT: rotateX -= 15; break;
		case GLUT_KEY_DOWN:  rotateY -= 15; break;
		default: return;
	}
	while (rotateX >= 360) rotateX -= 360;
	while (rotateY >= 360) rotateY -= 360;
	while (rotateX <  0)   rotateX += 360;
	while (rotateY <  0)   rotateY += 360;
	glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 'i':
		case 'I':
			if (zoom < 5)
				zoom *= 1.2;
			break;
		case 'o':
		case 'O':
			if (zoom > 0.01)
				zoom /= 1.2;
			break;
		case 'q':
		case 'Q':
			exit(0);
			printf("\x1b[2K");
			return;
		default: return;
	}
	glutPostRedisplay();
}
int usage(char *argv0) {
	fprintf(stderr, "\
Usage: %s <file>\n\n\
Controls:\n\
 Arrow keys to rotate camera\n\
 W,A,S,D to pan camera\n\
 I,O to zoom in/out\n\
 Q to quit\n\
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
	glutInitWindowSize(192, 108);
	glutInitWindowPosition(0, 0);
	char *title = malloc(strlen(file_name) + 16);
	sprintf(title, "%s%s", "Render: ", file_name);
	glutCreateWindow(title);
	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}
