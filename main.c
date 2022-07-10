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
void display() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glPointSize(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	for (size_t i = 0; i <= triangle_len; ++i) {
		glColor4f(triangles[i].r, triangles[i].g, triangles[i].b, triangles[i].a);
		glBegin(GL_POLYGON);
		glVertex3f(triangles[i].x1, triangles[i].y1, triangles[i].z1);
		glVertex3f(triangles[i].x2, triangles[i].y2, triangles[i].z2);
		glVertex3f(triangles[i].x3, triangles[i].y3, triangles[i].z3);
		glEnd();
	}
	glFlush();
}
void special() {

}
int usage(char *argv0) {
	fprintf(stderr, "\
Usage: %s <file>\n\
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
		if (ferror(file)) { fprintf(stderr, "Error reading\n"); break; }
		if (feof(file)) break;
		fscanf(file, "%e %e %e  %e %e %e  %e %e %e  %lx\n",
			&tri.x1, &tri.y1, &tri.z1, &tri.x2, &tri.y2, &tri.z2, &tri.x3, &tri.y3, &tri.z3, &col);
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
		if (triangle_len == SIZE_MAX || triangle_len == 0xffffff) {
			fprintf(stderr, "SIZE_MAX limit was hit, triangles after this will not be used\n");
			break;
		}
		++triangle_len;
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(192, 108);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("render");
	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutMainLoop();
	return 0;
}
