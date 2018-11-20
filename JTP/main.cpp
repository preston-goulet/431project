/**
 * SER 431
 * https://speakerdeck.com/javiergs/ser431-lecture-04
 **/

#include <stdlib.h>
#include <GL/glut.h>
#include <glut.h>
#include <fstream>
#include "mesh.h"
#include "texture.h"
#include "render.h"
#include "controls.h"
#include "particles.h"
#include "timer.h"


// global
Mesh *mesh1, *mesh2, *mesh3, *mesh4, *mesh5, *mesh6, *mesh7, *mesh8, *mesh9;
GLuint display1, display2, display3, display4, display5, display6, display7;
GLuint textures[5];
GLuint jetMesh, runway;
GLuint boundingBox;

vector<Points> box_spawn;

const int boundaryMeshSize = 60000;
const int skyBoxMeshSize = 80000;

bool doOnce = true;
float randY = 0;
float randX = 0;
float randZ = 0;

int countDown = 10000;


//Moving Flat
//float camera_x = 0.0f, camera_y = 20.0f, camera_z = 5.0f;
float lookx = 0.0f, looky = 0.0f, lookz = -1.0f;
float px, py;//for arrow
int moveSpeed = 500;
float angle = 0;
float playerLook = 0;


// draw particles
void drawParticles() {
	Particle* curr = ps.particle_head;
	// glPointSize(2);
	// glBegin(GL_POINTS);
	// while (curr) {
	//   glVertex3fv(curr->position);
	//	 curr = curr->next;
	// }
	// glEnd();
	while (curr) {
		glPushMatrix();
		glScalef(100.0, 100.0, 100.0);
		glTranslatef(curr->position[0], curr->position[1], curr->position[2]);
		glScalef(0.2, 0.2, 0.2);
		glColor4f(curr->color[0], curr->color[1], curr->color[2], 0.3);
		//front
		glBegin(GL_POLYGON); // fill connected polygon
		glVertex3f(0, 0, 0); // vertices of the triangle 1
		glVertex3f(0, 1, 0);
		glVertex3f(1, 1, 0);
		glVertex3f(1, 1, 0);
		glEnd();
		//back
		glBegin(GL_POLYGON); // fill connected polygon
		glVertex3f(0, 0, -1); // vertices of the triangle 1
		glVertex3f(0, 1, -1);
		glVertex3f(1, 1, -1);
		glVertex3f(1, 0, -1);
		glEnd();
		//left
		glBegin(GL_POLYGON); // fill connected polygon
		glVertex3f(0, 0, -1); // vertices of the triangle 1
		glVertex3f(0, 1, -1);
		glVertex3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glEnd();
		//right
		glBegin(GL_POLYGON); // fill connected polygon
		glVertex3f(1, 0, -1); // vertices of the triangle 1
		glVertex3f(1, 0, 0);
		glVertex3f(1, 1, 0);
		glVertex3f(1, 1, -1);
		glEnd();
		//top
		glBegin(GL_POLYGON); // fill connected polygon
		glVertex3f(1, 1, -1); // vertices of the triangle 1
		glVertex3f(1, 1, 0);
		glVertex3f(0, 1, 0);
		glVertex3f(0, 1, -1);
		glEnd();
		//bottom
		glBegin(GL_POLYGON); // fill connected polygon
		glVertex3f(1, 0, -1); // vertices of the triangle 1
		glVertex3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, -1);
		glEnd();

		glPopMatrix();
		curr = curr->next;
	}
}

// init
void init() {
	// particles
	init_frame_timer();

	// menu
	addMenu();

	// configuration
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	addMenu();

	// mesh
	mesh1 = createPerlinPlane(80000, 80000, 400);
	//heightMap = getHeightMap(mesh1);
	mesh2 = createCube();
	mesh3 = createCube();
	mesh4 = createCube();
	mesh5 = createSkyBox(skyBoxMeshSize);
	mesh6 = createPlane(boundaryMeshSize, boundaryMeshSize, boundaryMeshSize/10);
	mesh7 = createCube();
	mesh8 = loadFile("OBJfiles/f-16.obj");
	mesh9 = createPlane(2000, 2000, 2000);

	
	// normals
	calculateNormalPerFace(mesh1);
	calculateNormalPerFace(mesh2);
	calculateNormalPerFace(mesh3);
	calculateNormalPerFace(mesh4);
	calculateNormalPerFace(mesh5);
	calculateNormalPerFace(mesh6);
	calculateNormalPerFace(mesh7);
	calculateNormalPerFace(mesh8);
	calculateNormalPerFace(mesh9);
	calculateNormalPerVertex(mesh1);
	calculateNormalPerVertex(mesh2);
	calculateNormalPerVertex(mesh3);
	calculateNormalPerVertex(mesh4);
	calculateNormalPerVertex(mesh5);
	calculateNormalPerVertex(mesh6);
	calculateNormalPerVertex(mesh7);
	calculateNormalPerVertex(mesh8);
	calculateNormalPerVertex(mesh9);

	calculateBoundingPoints(mesh8);
	
	// textures
	loadBMP_custom(textures, "_BMP_files/grass.bmp", 0);
	loadBMP_custom(textures, "_BMP_files/oldbox.bmp", 1);
	codedTexture(textures, 2, 0); //Sky texture - noise multiscale. Type=0
	codedTexture(textures, 3, 1); //Marble texture - noise marble. Type=1
	loadBMP_custom(textures, "_BMP_files/cubesky.bmp", 4);
	codedTexture(textures, 5, 2); //Fire texture - noise fire. Type=2
	codedTexture(textures, 6, 0); //Fire texture - noise fire. Type=2
	loadBMP_custom(textures, "_BMP_files/runway.bmp", 7);
	
	// display lists
	display1 = meshToDisplayList(mesh1, 1, textures[0]);
	display2 = meshToDisplayList(mesh2, 2, textures[1]);
	display3 = meshToDisplayList(mesh3, 3, textures[2]);
	display4 = meshToDisplayList(mesh4, 4, textures[3]);
	display5 = meshToDisplayList(mesh5, 5, textures[4]);
	display6 = meshToDisplayList(mesh6, 6, textures[5]);//Lava
	display7 = meshToDisplayList(mesh7, 7, textures[6]);
	jetMesh = meshToDisplayListObjects(mesh8, 8);
	runway = meshToDisplayList(mesh9, 10, textures[7]);

	boundingBox = boundingBoxToDisplayList(mesh8, 9);

	// Materials
	GLfloat mat_diffuse[] = { 0.714,	0.4284,	0.18144, 1 };
	GLfloat mat_specular[] = { 0.714,	0.4284,	0.18144, 1 };
	GLfloat mat_ambient[] = { 0.2125,	0.1275,	0.054, 1 };
	GLfloat mat_shininess[] = { 0.2 }; //100
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	
	// light
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	GLfloat light_ambient[]  = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
	//===================================
	// Fog
	//===================================
	if (isFogOn) {
		glEnable(GL_FOG);
	}
	
	glFogi(GL_FOG_MODE, GL_LINEAR);
	GLfloat fogColor[4] = { 0.5, 0.5, 0.9, 1.0 };
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.75);
	glFogf(GL_FOG_START, 10000.0);
	glFogf(GL_FOG_END, 70000);

	glClearStencil(0);

	// floor vertex
	dot_vertex_floor.push_back(Vec3<GLfloat>(-2000.0, 0.0, 2000.0));
	dot_vertex_floor.push_back(Vec3<GLfloat>(2000.0, 0.0, 2000.0));
	dot_vertex_floor.push_back(Vec3<GLfloat>(2000.0, 0.0, -2000.0));
	dot_vertex_floor.push_back(Vec3<GLfloat>(-2000.0, 0.0, -2000.0));
	calculate_floor_normal(&floor_normal, dot_vertex_floor);

}

// reshape
void reshape(int w, int h) {
	window_width = w;
	window_height = h;
	if (h == 0) h = 1;
	window_ratio = 1.0f * w / h;
}

// text
void renderBitmapString(float x, float y, float z, const char *string) {
	const char *c;
	glRasterPos3f(x, y, z);   // fonts position
	for (c = string; *c != '\0'; c++)
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
}

//GLUnurbsObj *theNurb;

// display
void display(void) {
	glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// light source position
	light_position[0] = 500 * cos(lightAngle) + 1000;
	light_position[1] = lightHeight;
	light_position[2] = 500 * sin(lightAngle) -1000;
	light_position[3] = 0.0; // directional light
	lightAngle += 0.0005;

	// Calculate Shadow matrix
	shadowMatrix(shadow_matrix, floor_normal, light_position);

	// projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glViewport(0, 0, window_width, window_height);
	gluPerspective(45, window_ratio, 10, 100000);


	// view
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//===========================================================================
	//  Camera
	//===========================================================================
	gluLookAt(camera_x, camera_y, camera_z,
		camera_x + lookx, camera_y + looky, camera_z + lookz,
		0.0f, 1.0f, 0.0f);

	// camera
	glPushMatrix();
		glScalef(scale, scale, scale);
		glTranslatef(0.0f, 0.0f, 0.0f);
	glPopMatrix();


	//=======================================
	// Box Targets
	//=======================================
	//box_spawn.push_back(Points());
	//if (doOnce == true) {
	//	for (int i = 0; i < 3000; i++) {
	//		box_spawn.push_back(Points());
	//		randX = rand() % 5000 + (-5000);
	//		randY = rand() % 1000 + 200;
	//		randZ = rand() % 5000 + (-5000);

	//		box_spawn[i].x = randX;
	//		box_spawn[i].y = randY;
	//		box_spawn[i].z = randZ;
	//	}
	//}

	// box 1
	//glPushMatrix();
	//	for (int i = 0; i < 1000; i++) {
	//		glTranslatef( box_spawn[i].x, box_spawn[i].y, box_spawn[i].z );
	//		glCallList(display2);
	//	}
	//glPopMatrix();

	////Box 2
	//glPushMatrix();
	//	for (int i = 100; i < 2000; i++) {
	//		glTranslatef(box_spawn[i].x, box_spawn[i].y, box_spawn[i].z);
	//		glCallList(display3);
	//	}
	//glPopMatrix();

	//// Box 3
	//glPushMatrix();
	//	for (int i = 200; i < 3000; i++) {
	//		glTranslatef(box_spawn[i].x, box_spawn[i].y, box_spawn[i].z);
	//		glCallList(display4);
	//	}
	//glPopMatrix();
	//doOnce = false;

	// skybox
	glPushMatrix();
	glTranslatef(-skyBoxMeshSize / 2, -skyBoxMeshSize / 2, -skyBoxMeshSize / 2);
	glCallList(display5);
	glPopMatrix();

	//========================================
	//	Shadows
	//========================================
	glPushMatrix();
	// Tell GL new light source position
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	// Shadows
	if (areShadowsOn) {
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	}
	// Draw floor using blending to blend in reflection
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0, 1.0, 1.0, 0.5);
	glPushMatrix();
	glDisable(GL_LIGHTING);

	//runway
	for (int x = 0; x < 10; x++) {
		glPushMatrix();
		glTranslatef(runway_x - 900, runway_y + 100, -(1800 * x) + runway_z);
		glCallList(runway);
		glPopMatrix();
	}

	glEnable(GL_LIGHTING);
	glPopMatrix();
	glDisable(GL_BLEND);

	// Shadows
	if (areShadowsOn) {
		glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
		glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
		//glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		//  To eliminate depth buffer artifacts, use glEnable(GL_POLYGON_OFFSET_FILL);
		// Render 50% black shadow color on top of whatever the floor appareance is
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_LIGHTING);  /* Force the 50% black. */
		glColor4f(0.0, 0.0, 0.0, 0.5);
		glPushMatrix();
		// Project the shadow
		glMultMatrixf((GLfloat *)shadow_matrix);
		// boxes
		glDisable(GL_DEPTH_TEST);

		glTranslatef(camera_x, 20, camera_z - 500);
		glRotatef(180 + jet_rotate, 0.0, 1.0, 0.0);
		glTranslatef(lookx, looky, lookz);
		glScalef(10, 10, 10);
		glCallList(jetMesh);

		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
	}

	// ===== STENCIL DRAW ============================================================================

	glEnable(GL_STENCIL_TEST); //Start using the stencil
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); //Disable writing colors in frame buffer
	glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF); //Place a 1 where rendered
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE); 	//Replace where rendered
	// PLAIN for the stencil
	if (isReflectionOn) { //Add Toggle to reflection
		glPushMatrix();
		glTranslatef(-boundaryMeshSize / 2, -50, -boundaryMeshSize / 2);
		glCallList(display6); //Water
		glPopMatrix();
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); //Reenable color
	glEnable(GL_DEPTH_TEST);
	glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //Keep the pixel

	//================================
	//	Jet Reflection
	//================================
	//glPushMatrix();	
	//	glScalef(1.0, -1.0, 1.0);
	//	glTranslatef(camera_x, 14, camera_z - 500);
	//	glRotatef(180 + jet_rotate, 0.0, 1.0, 0.0);
	//	glTranslatef(lookx, looky, lookz);
	//	glScalef(10, 10, 10);
	//	glCallList(jetMesh); //display7
	//glPopMatrix();

	//============================================
	//	Lava part 1 of 2
	//============================================
	if (areParticlesOn) {
		//Particles with box 1
		for (int i = 0; i < 50; i++) {
			ps.add();
		}
		ps2.add();
	}
	float time = calculate_frame_time();
	ps.update(time);
	ps2.update(time);
	ps2.remove();
	ps.remove();
	glPushMatrix();
	glScalef(1.0, -1.0, 1.0);
	glTranslatef(camera_x + 10, 5, camera_z - 400);
	glRotatef(90, 1, 0, 0);
	glRotatef(jet_rotate, 0.0, 1.0, 0.0);
	glScalef(.1, .1, .1);
	drawParticles();//flames
	glPopMatrix();

	// STENCIL-STEP 4. disable it
	glDisable(GL_STENCIL_TEST);

	//========Regular 3d environment=====================================
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.7, 0.0, 0.0, 0.9);
	glColor4f(1.0, 1.0, 1.0, 0.9);

	//Water
	glPushMatrix();
	glTranslatef(-boundaryMeshSize / 2, -50, -boundaryMeshSize / 2);
	glCallList(display6);
	glPopMatrix();


	//============================================
	//	Lava part 2 of 2
	//============================================
	ps.remove();
	glPushMatrix();
	glScalef(5, 5, 5);
	glTranslatef(5000, 300, -400);
	drawParticles();//flames
	glPopMatrix();
	
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHT0);

	//if (areBoundingBoxesOn) {
	//	//bounding box
	//	glPushMatrix();
	//	glDisable(GL_LIGHTING);
	//	glColor3f(1.0, 1.0, 0.0);
	//	glTranslatef(camera_x, camera_y - 100, camera_z - 500);
	//	glRotatef(180 + jet_rotate, 0.0, 1.0, 0.0);
	//	glTranslatef(lookx, looky, lookz);
	//	glScalef(10, 10, 10);
	//	glCallList(boundingBox);

	//	glEnable(GL_LIGHTING);
	//	glPopMatrix();
	//}

	//==========================
	// jet
	//==========================
	//glPushMatrix();
	//	glTranslatef( camera_x, camera_y - 100, camera_z - 500 );
	//	glRotatef(180 + jet_rotate, 0.0, 1.0, 0.0);
	//	glTranslatef(lookx, looky, lookz);
	//	glScalef(10, 10, 10);
	//	glCallList(jetMesh); 
	//glPopMatrix();

	//plane
	glPushMatrix();
		glTranslatef(-perlinMeshSize / 2, 0, -perlinMeshSize / 2);
		glCallList(display1);
	glPopMatrix();

	//runway
	for (int x = 0; x < 10; x++) {
		glPushMatrix();
		glTranslatef(runway_x, runway_y, -(1800 * x) + runway_z);
		glCallList(runway);
		glPopMatrix();
	}
	
	if (isLightArrowOn) {
		drawLightArrow();
	}



	////==========================================
	//// NURB / Volcano
	////==========================================

	//// projection and view
	//glMatrixMode(GL_PROJECTION);
	////glLoadIdentity();
	//glMatrixMode(GL_MODELVIEW);

	//// NURBS
	//// V_size curves with U_size control points each
	//// V_size + ORDER knots per curve and U_size + ORDER knots per inter-curve conection (controlpnt + 4)
	//// V_size*3 and 3 offsets
	//// cubic equations (4)

	//const int V_size = 4;
	//const int U_size = 4;
	//const int ORDER = 4;
	//GLfloat ctlpoints[U_size][V_size][3] = {
	//	{ { -10, 0, -10 } ,{ 0, 0,  -10 },{ 0, 0,  -10 },{ 10, 0,  -10 } },
	//	{ { -10, 0,   0 } ,{ 0, 10, 0 },{ 0, 10,  0 },{ 10, 0,  0 }},
	//	{ { -10, 0,   0 } ,{ 0, 10, 0 },{ 0, 10,  0 },{ 10, 0,  0 }},
	//	{ { -10, 0,  10 } ,{ 0, 0,  10 },{ 0, 0,  10 },{ 10, 0,  10 }}
	//};
	//GLfloat vknots[V_size + ORDER] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0 };
	//GLfloat uknots[U_size + ORDER] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0 };

	//GLUnurbsObj *theNurb;
	//theNurb = gluNewNurbsRenderer();
	//gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 25.0);
	//gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);

	//glPushMatrix();
	//glScalef( 200, 200, 200 );
	//glTranslatef(100, -1, 0);
	//gluBeginSurface(theNurb);
	//gluNurbsSurface(theNurb,
	//	U_size + ORDER, uknots,
	//	V_size + ORDER, vknots,
	//	V_size * 3,
	//	3,
	//	&ctlpoints[0][0][0],
	//	ORDER, ORDER,
	//	GL_MAP2_VERTEX_3);
	//gluEndSurface(theNurb);
	//glPopMatrix();


	//end
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glDisable(GL_CULL_FACE);

	//=======================================================================
	// Lighting disabled from here on to allow other colors on the screen
	//=======================================================================
	countDown = countDown - 1;
	string countDownString = to_string(countDown);
	glDisable(GL_LIGHTING);

	// texto
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, window_width, 0, window_height);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(0.0, 1.0, 0.0);
	renderBitmapString(0.0, window_height - 13.0f, 0.0f, "Use [Arrows] to move in plain");
	renderBitmapString(0.0, window_height - 26.0f, 0.0f, "Use [W and S] to look up and down");
	renderBitmapString( (window_width / 2) - 50, (window_height / 10) * 9, 0.0f, "Time Left: " );	
	renderBitmapString( (window_width / 2) + 40, (window_height / 10) * 9, 0.0f, countDownString.c_str() );
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	

	//=====================================
	// Cockpit View
	//=====================================
	glViewport( 0, 0, window_width, window_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Draw box for hud
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	// bottom left, bottom right, top left, top right
	gluOrtho2D(0.0, window_width, 0.0, window_height);
	glShadeModel(GL_FLAT);



	// Center screen
	glColor3f(.200, .200, .200);
	glBegin(GL_QUADS);
		glVertex2f( (window_width / 8) * 3, 0 );//bottom left
		glVertex2f( (window_width / 8) * 5, 0 );//bottom right
		glVertex2f( (window_width / 8) * 5, window_height / 5 ); //top right
		glVertex2f( (window_width / 8) * 3, window_height / 5 );//top left
	glEnd();

	//Whole console
	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);
		glVertex2f( 0, 0);
		glVertex2f( window_width, 0);
		//glVertex2f( (window_width / 4) * 3,  (window_height / 7));
		glVertex2f( (window_width / 3) * 2,  (window_height / 4));
		glVertex2f( (window_width / 3),      (window_height / 4));
		glVertex2f( (window_width / 4),      (window_height / 7));
		glVertex2f( 0, 0);
	glEnd();

	
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glutSwapBuffers();
}

//// rotate what the user see
//void rotate_point(float angle) {
//	float s = sin(angle);
//	float c = cos(angle);
//	// translate point back to origin:
//	camera_viewing_x -= camera_x;
//	camera_viewing_z -= camera_z;
//	// rotate point
//	float xnew = camera_viewing_x * c - camera_viewing_z * s;
//	float znew = camera_viewing_x * s + camera_viewing_z * c;
//	// translate point back:
//	camera_viewing_x = xnew + camera_x;
//	camera_viewing_z = znew + camera_z;
//}

// callback function for keyboard (alfanumeric keys)
void callbackKeyboard(unsigned char key, int x, int y) {

	switch (key) {
		case 'w': case 'W':
			looky += .1;
			break;
		case 's': case 'S':
			looky -= .1;
			break;
		case 'a': case 'A':
			camera_y -= 100;
			break;
		case 'd': case 'D':
			camera_y += 100;
			break;
	}

	if (jetPositionX > meshSize) {
		jetPositionX = meshSize;
	}
	else if (jetPositionX < -meshSize) {
		jetPositionX = -meshSize;
	}

	// box Z verification
	if (jetPositionZ > meshSize) {
		jetPositionZ = meshSize;
	}
	else if (jetPositionZ < -meshSize) {
		jetPositionZ = -meshSize;
	}
}

// callback function for arrows
void moveMeFlat(int i) {
	camera_x = camera_x + i * (lookx)*1.0;	
	camera_z = camera_z + i * (lookz)*1.0;
}

void orientMe(float ang) {
	lookx = sin(ang); 
	lookz = -cos(ang);
}

void specialkeys(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		moveMeFlat(moveSpeed);
		break;
	case GLUT_KEY_DOWN:
		moveMeFlat(-moveSpeed);
		break;
	case GLUT_KEY_LEFT:
		angle -= 0.1f;
		orientMe(angle);
		playerLook += 5;
		jet_rotate += 5;
		break;
	case GLUT_KEY_RIGHT:
		angle += 0.1f;
		orientMe(angle);
		playerLook -= 5;
		jet_rotate -= 5;
		break;
	}

	glutPostRedisplay();

}

// main
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("The Game");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	
	glutSpecialFunc(specialkeys);
	glutKeyboardFunc(callbackKeyboard);
	init();
	glutMainLoop();
	return 0;
}
