
/* Derived from scene.c in the The OpenGL Programming Guide */
/* Keyboard and mouse rotation taken from Swiftless Tutorials #23 Part 2 */
/* http://www.swiftless.com/tutorials/opengl/camera2.html */

/* Frames per second code taken from : */
/* http://www.lighthouse3d.com/opengl/glut/index.php?fps */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "graphics.h"

extern GLubyte  world[WORLDX][WORLDY][WORLDZ];

	/* mouse function called by GLUT when a button is pressed or released */
void mouse(int, int, int, int);

	/* initialize graphics library */
extern void graphicsInit(int *, char **);

	/* lighting control */
extern void setLightPosition(GLfloat, GLfloat, GLfloat);
extern GLfloat* getLightPosition();

	/* viewpoint control */
extern void setViewPosition(float, float, float);
extern void getViewPosition(float *, float *, float *);
extern void setOldViewPosition(float, float, float);
extern void getOldViewPosition(float *, float *, float *);
extern void setViewOrientation(float, float, float);
extern void getViewOrientation(float *, float *, float *);

	/* add cube to display list so it will be drawn */
extern void addDisplayList(int, int, int);

	/* mob controls */
extern void createMob(int, float, float, float, float);
extern void setMobPosition(int, float, float, float, float);
extern void hideMob(int);
extern void showMob(int);

	/* player controls */
extern void createPlayer(int, float, float, float, float);
extern void setPlayerPosition(int, float, float, float, float);
extern void hidePlayer(int);
extern void showPlayer(int);

	/* 2D drawing functions */
extern void  draw2Dline(int, int, int, int, int);
extern void  draw2Dbox(int, int, int, int);
extern void  draw2Dtriangle(int, int, int, int, int, int);
extern void  set2Dcolour(float []);

	/* flag which is set to 1 when flying behaviour is desired */
extern int flycontrol;
	/* flag used to indicate that the test world should be used */
extern int testWorld;
	/* flag to print out frames per second */
extern int fps;
	/* flag to indicate the space bar has been pressed */
extern int space;
	/* flag indicates the program is a client when set = 1 */
extern int netClient;
	/* flag indicates the program is a server when set = 1 */
extern int netServer; 
	/* size of the window in pixels */
extern int screenWidth, screenHeight;
	/* flag indicates if map is to be printed */
extern int displayMap;
	/* flag indicates use of a fixed viewpoint */
extern int fixedVP;

	/* frustum corner coordinates, used for visibility determination  */
extern float corners[4][3];

	/* determine which cubes are visible e.g. in view frustum */
extern void ExtractFrustum();
extern void tree(float, float, float, float, float, float, int);

	/* allows users to define colours */
extern int setUserColour(int, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat,
    GLfloat, GLfloat, GLfloat);
void unsetUserColour(int);
extern void getUserColour(int, GLfloat *, GLfloat *, GLfloat *, GLfloat *,
    GLfloat *, GLfloat *, GLfloat *, GLfloat *);

extern void initTubeArray();
extern void createTube(int, float, float, float, float, float, float, int);
extern void hideTube(int);
extern void showTube(int);

/********* end of extern variable declarations **************/

clock_t newTimeGrav, oldTimeGrav;
clock_t newTimeVel, oldTimeVel;
int TUBE_COUNT = 10;

	/*** collisionResponse() ***/
	/* -performs collision detection and response */
	/*  sets new xyz  to position of the viewpoint after collision */
	/* -can also be used to implement gravity by updating y position of vp*/
	/* note that the world coordinates returned from getViewPosition()
	   will be the negative value of the array indices */
void collisionResponse() {
    
	/* your code for collisions goes here */
    
    float x, y, z;
    int xflr, yflr, zflr;
    getViewPosition(&x, &y, &z);
    x *= -1; xflr = floor(x);
    y *= -1; yflr = floor(y);
    z *= -1; zflr = floor(z);
    int y1,x1,x2,z1,z2;
    y1 = floor(y+0.1);
    x1 = floor(x+0.1);
    x2 = floor(x-0.1);
    z1 = floor(z+0.1);
    z2 = floor(z-0.1);
    if(
       world[xflr][y1][zflr] != 0
       || world[x1][yflr][zflr] != 0 || world[x2][yflr][zflr] != 0
       || world[xflr][yflr][z1] != 0 || world[xflr][yflr][z2] != 0
       || x <= 0 || y <= 0 || z <= 0
       || x > WORLDX || y > WORLDY || z > WORLDZ ){
        getOldViewPosition(&x, &y, &z);
        setViewPosition(x,y,z);
        //puts("Collided");
    }
    
}


	/******* draw2D() *******/
	/* draws 2D shapes on screen */
	/* use the following functions: 			*/
	/*	draw2Dline(int, int, int, int, int);		*/
	/*	draw2Dbox(int, int, int, int);			*/
	/*	draw2Dtriangle(int, int, int, int, int, int);	*/
	/*	set2Dcolour(float []); 				*/
	/* colour must be set before other functions are called	*/
void draw2D() {

   if (testWorld) {
		/* draw some sample 2d shapes */
      if (displayMap == 1) {
          GLfloat green[] = {0.0, 0.5, 0.0, 0.5};
          set2Dcolour(green);
          draw2Dline(0, 0, 500, 500, 15);
          draw2Dtriangle(0, 0, 200, 200, 0, 200);

          GLfloat black[] = {0.0, 0.0, 0.0, 0.5};
          set2Dcolour(black);
          draw2Dbox(500, 380, 524, 388);
      }
   } else {
       if (displayMap == 1){
           int gap = 10;
           int cellSize = 3;
           int rightBound = screenWidth - gap;
           int leftBound = rightBound - WORLDX*cellSize;
           int topBound = screenHeight - gap;
           int bottomBound = topBound - WORLDZ*cellSize;
           
           GLfloat green[] = {0.0, 0.5, 0.0, 0.5};
           GLfloat blue[] = {0.0, 0.0, 0.5, 0.5};
           GLfloat red[] = {0.5, 0.0, 0.0, 0.5};
           GLfloat black[] = {0.0, 0.0, 0.0, 0.5};
           GLfloat white[] = {0.5, 0.5, 0.5, 0.5};
           GLfloat purple[] = {0.5, 0.0, 0.5, 0.5};
           GLfloat orange[] = {0.5, 0.2, 0.0, 0.5};
           GLfloat yellow[] = {0.5, 0.4, 0.0, 0.5};
           
           float x,y,z;
           getOldViewPosition(&x,&y,&z);
           int xPos = leftBound + ((int)x * -1 * cellSize);
           int zPos = topBound - ((int)z * -1 * cellSize);
           
           set2Dcolour(black);
           draw2Dtriangle(xPos,zPos+3,xPos-3,zPos-3,xPos+3,zPos-3);
           
           draw2Dline(leftBound, topBound, rightBound, topBound,1);
           draw2Dline(leftBound, bottomBound, rightBound, bottomBound,1);
           draw2Dline(leftBound, topBound, leftBound, bottomBound,1);
           draw2Dline(rightBound, topBound, rightBound, bottomBound,1);
           
           for (int x=0; x<WORLDX; x++){
               for (int z=0; z<WORLDZ; z++){
                   for(int y=WORLDY; y>=0; y--){
                       switch(world[x][y][z]){
                           case 1:
                               set2Dcolour(green);
                               break;
                           case 2:
                               set2Dcolour(blue);
                               break;
                           case 3:
                               set2Dcolour(red);
                               break;
                           case 4:
                               set2Dcolour(black);
                               break;
                           case 5:
                               set2Dcolour(white);
                               break;
                           case 6:
                               set2Dcolour(purple);
                               break;
                           case 7:
                               set2Dcolour(orange);
                               break;
                           case 8:
                               set2Dcolour(yellow);
                               break;
                           default:
                               break;
                       }
                       if(world[x][y][z] != 0){
                           draw2Dbox(leftBound+(x*cellSize), topBound-(z*cellSize),
                                 leftBound+((x+1)*cellSize), topBound-((z+1)*cellSize));
                       }
                   }
               }
           }
           
       }else if (displayMap == 2){
           int cellSize = 7;
           int leftBound = screenWidth/2 - WORLDX/2*cellSize;
           int rightBound = screenWidth/2 + WORLDX/2*cellSize;
           int topBound = screenHeight/2 + WORLDZ/2*cellSize;
           int bottomBound = screenHeight/2 - WORLDZ/2*cellSize;
           
           GLfloat green[] = {0.0, 0.5, 0.0, 0.5};
           GLfloat blue[] = {0.0, 0.0, 0.5, 0.5};
           GLfloat red[] = {0.5, 0.0, 0.0, 0.5};
           GLfloat black[] = {0.0, 0.0, 0.0, 0.5};
           GLfloat white[] = {0.5, 0.5, 0.5, 0.5};
           GLfloat purple[] = {0.5, 0.0, 0.5, 0.5};
           GLfloat orange[] = {0.5, 0.2, 0.0, 0.5};
           GLfloat yellow[] = {0.5, 0.4, 0.0, 0.5};
           
           float x,y,z;
           getOldViewPosition(&x,&y,&z);
           int xPos = leftBound + ((int)x * -1 * cellSize);
           int zPos = topBound - ((int)z * -1 * cellSize);
           
           set2Dcolour(black);
           draw2Dtriangle(xPos,zPos+3,xPos-3,zPos-3,xPos+3,zPos-3);
           
           int lineWidth = 2;
           draw2Dline(leftBound, topBound, rightBound, topBound, lineWidth);
           draw2Dline(leftBound, bottomBound, rightBound, bottomBound, lineWidth);
           draw2Dline(leftBound, topBound, leftBound, bottomBound, lineWidth);
           draw2Dline(rightBound, topBound, rightBound, bottomBound, lineWidth);
           
           for (int x=0; x<WORLDX; x++){
               for (int z=0; z<WORLDZ; z++){
                   for(int y=WORLDY; y>=0; y--){
                       switch(world[x][y][z]){
                           case 1:
                               set2Dcolour(green);
                               break;
                           case 2:
                               set2Dcolour(blue);
                               break;
                           case 3:
                               set2Dcolour(red);
                               break;
                           case 4:
                               set2Dcolour(black);
                               break;
                           case 5:
                               set2Dcolour(white);
                               break;
                           case 6:
                               set2Dcolour(purple);
                               break;
                           case 7:
                               set2Dcolour(orange);
                               break;
                           case 8:
                               set2Dcolour(yellow);
                               break;
                           default:
                               break;
                       }
                       if(world[x][y][z] != 0){
                           draw2Dbox(leftBound+(x*cellSize), topBound-(z*cellSize),
                                     leftBound+((x+1)*cellSize), topBound-((z+1)*cellSize));
                       }
                   }
               }
           }
           
       }
   }

}


	/*** update() ***/
	/* background process, it is called when there are no other events */
	/* -used to control animations and perform calculations while the  */
	/*  system is running */
	/* -gravity must also implemented here, duplicate collisionResponse */
void update() {
int i, j, k;
float *la;

	/* sample animation for the test world, don't remove this code */
	/* demo of animating mobs */
   if (testWorld) {

	/* sample of rotation and positioning of mob */
	/* coordinates for mob 0 */
      static float mob0x = 50.0, mob0y = 25.0, mob0z = 52.0;
      static float mob0ry = 0.0;
      static int increasingmob0 = 1;
	/* coordinates for mob 1 */
      static float mob1x = 50.0, mob1y = 25.0, mob1z = 52.0;
      static float mob1ry = 0.0;
      static int increasingmob1 = 1;
	/* counter for user defined colour changes */
      static int colourCount = 0;
      static GLfloat offset = 0.0;

	/* move mob 0 and rotate */
	/* set mob 0 position */
      setMobPosition(0, mob0x, mob0y, mob0z, mob0ry);

	/* move mob 0 in the x axis */
      if (increasingmob0 == 1)
         mob0x += 0.2;
      else 
         mob0x -= 0.2;
      if (mob0x > 50) increasingmob0 = 0;
      if (mob0x < 30) increasingmob0 = 1;

	/* rotate mob 0 around the y axis */
      mob0ry += 1.0;
      if (mob0ry > 360.0) mob0ry -= 360.0;

	/* move mob 1 and rotate */
      setMobPosition(1, mob1x, mob1y, mob1z, mob1ry);

	/* move mob 1 in the z axis */
	/* when mob is moving away it is visible, when moving back it */
	/* is hidden */
      if (increasingmob1 == 1) {
         mob1z += 0.2;
         showMob(1);
      } else {
         mob1z -= 0.2;
         hideMob(1);
      }
      if (mob1z > 72) increasingmob1 = 0;
      if (mob1z < 52) increasingmob1 = 1;

	/* rotate mob 1 around the y axis */
      mob1ry += 1.0;
      if (mob1ry > 360.0) mob1ry -= 360.0;

	/* change user defined colour over time */
      if (colourCount == 1) offset += 0.05;
      else offset -= 0.01;
      if (offset >= 0.5) colourCount = 0;
      if (offset <= 0.0) colourCount = 1;
      setUserColour(9, 0.7, 0.3 + offset, 0.7, 1.0, 0.3, 0.15 + offset, 0.3, 1.0);

    /* end testworld animation */


   } else {
       
	/* your code goes here */
       
       /* timing variables */
       static int start = 0;
//       double timerGrav = (double)(newTimeGrav-oldTimeGrav)/CLOCKS_PER_SEC;
       /* momentum variables */
       static float velocity = 0.0;
       float xDist, yDist, zDist;
       /* location variables */
       static float x, y, z;
       static float xOld, yOld, zOld;
       static float xCurr, yCurr, zCurr;
       
       if(start == 0){
           getViewPosition(&x, &y, &z);
           setViewPosition(x,y,z);
           setOldViewPosition(x,y,z);   //init old to current pos at start
           xCurr = x;
           yCurr = y;
           zCurr = z;
           start = 1;
       }
       
       /* GRAVITY */
       newTimeGrav = clock();
       if(flycontrol==1){
           double timerGrav = (double)(newTimeGrav-oldTimeGrav)/CLOCKS_PER_SEC;
           if(timerGrav >= 0.05){
               
               for(j=1; j<WORLDY-1; j++){
                   for(i=0; i<WORLDX; i++){
                       for(k=0; k<WORLDZ; k++){
                           if(world[i][j+1][k] != 0         //above cube is not empty
                              && world[i][j][k] == 0){      //this cube is empty
                               world[i][j][k] = world[i][j+1][k];   //bring above cube down
                               world[i][j+1][k] = 0;                //clear cube above
                           }
                       }
                   }
               }
               oldTimeGrav = newTimeGrav;
           }
       }
       
       getViewPosition(&x, &y, &z);                 //get expected location
       getOldViewPosition(&xOld, &yOld, &zOld);     //get current location
       
       static int isMoving = 1;
//       if((int)xCurr == (int)xOld && (int)yCurr == (int)yOld && (int)zCurr == (int)zOld){
//           puts("NOT moving");
//       }else{
//           puts("MOVING");
//       }
//       printf("OLD: %.2f  %.2f  %.2f\n",xOld, yOld, zOld);
//       printf("NEW: %.2f  %.2f  %.2f\n",x, y, z);
//       printf("TEMP: %.2f  %.2f  %.2f\n",xCurr, yCurr, zCurr);
       
       
       // Get distance & Multiply distance with velocity modifier
       xDist = x-xOld;
       yDist = y-yOld;
       zDist = z-zOld;
       //printf("Distance: %.2f %.2f %.2f\n",xDist,yDist,zDist);
       static float prevDist = 0;
       float dist = sqrt(pow(xDist,2)+pow(yDist,2)+pow(zDist,2));
//       if (prevDist != dist)printf("prev:\t%f\t\tdist:\t%f\n", prevDist,dist);
       
       newTimeVel = clock();
       double timerVel = (double)(newTimeVel-oldTimeVel)/CLOCKS_PER_SEC;
       if(timerVel < 0.1){
           /* if there is keyDown within x seconds, speedup else slowdown*/
           if(xCurr != xOld && yCurr != yOld && zCurr != zOld){
               isMoving = 1;
           }
           if(isMoving == 1){
               if(velocity<=1) velocity += 0.06;
           }
           if(velocity > 0) velocity -= 0.05;
           
       }else{
           oldTimeVel = newTimeVel;
       }
       
       setViewPosition(x+(xDist*velocity),y+(yDist*velocity),z+(zDist*velocity));
       setOldViewPosition(x,y,z);

       prevDist=dist;
       
       
//       setViewPosition(x+(xDist*velocity),y+(yDist*velocity),z+(zDist*velocity));
       getViewPosition(&x, &y, &z);
//       setOldViewPosition(x,y,z);
       xCurr = xOld;
       yCurr = yOld;
       zCurr = zOld;
       collisionResponse();
   }
    
    
}


	/* called by GLUT when a mouse button is pressed or released */
	/* -button indicates which button was pressed or released */
	/* -state indicates a button down or button up event */
	/* -x,y are the screen coordinates when the mouse is pressed or */
	/*  released */ 
void mouse(int button, int state, int x, int y) {

    if (button == GLUT_LEFT_BUTTON){
        //printf("left button - ");
        if (state == GLUT_UP){
            float xPos,yPos,zPos;
            float xRot,yRot,zRot;
            static int fireCtr = 0;
            getViewPosition(&xPos,&yPos,&zPos);
            getViewOrientation(&xRot,&yRot,&zRot);
            float xDist = sin(yRot);
            float yDist = sin(xRot);
            float zDist = cos(yRot);
            xPos *= -1;
            yPos *= -1;
            zPos *= -1;
            
            //fire a ray
            //createTube
            puts("FIRE");
            createTube(fireCtr,xPos,yPos,zPos,xPos-xDist,yPos+yDist,zPos+zDist,3);
            printf("%.2f %.2f %.2f - %.2f %.2f %.2f\n", xPos,yPos,zPos,xPos+xDist,yPos+yDist,zPos+zDist);
        }
    }else if (button == GLUT_MIDDLE_BUTTON){
      //printf("middle button - ");
    }else{
      //printf("right button - ");
    }
    
    if (state == GLUT_UP){
      //printf("up - ");
    }else{
      //printf("down - ");
    }
//   printf("%d %d\n", x, y);
}



int main(int argc, char** argv)
{
int i, j, k;
	/* initialize the graphics system */
   graphicsInit(&argc, argv);

	/* the first part of this if statement builds a sample */
	/* world which will be used for testing */
	/* DO NOT remove this code. */
	/* Put your code in the else statment below */
	/* The testworld is only guaranteed to work with a world of
		with dimensions of 100,50,100. */
   if (testWorld == 1) {
	/* initialize world to empty */
      for(i=0; i<WORLDX; i++)
         for(j=0; j<WORLDY; j++)
            for(k=0; k<WORLDZ; k++)
               world[i][j][k] = 0;

	/* some sample objects */
	/* build a red platform */
      for(i=0; i<WORLDX; i++) {
         for(j=0; j<WORLDZ; j++) {
            world[i][24][j] = 3;
         }
      }
	/* create some green and blue cubes */
      world[50][25][50] = 1;
      world[49][25][50] = 1;
      world[49][26][50] = 1;
      world[52][25][52] = 2;
      world[52][26][52] = 2;

	/* create user defined colour and draw cube */
      setUserColour(9, 0.7, 0.3, 0.7, 1.0, 0.3, 0.15, 0.3, 1.0);
      world[54][25][50] = 9;


	/* blue box shows xy bounds of the world */
      for(i=0; i<WORLDX-1; i++) {
         world[i][25][0] = 2;
         world[i][25][WORLDZ-1] = 2;
      }
      for(i=0; i<WORLDZ-1; i++) {
         world[0][25][i] = 2;
         world[WORLDX-1][25][i] = 2;
      }

	/* create two sample mobs */
	/* these are animated in the update() function */
      createMob(0, 50.0, 25.0, 52.0, 0.0);
      createMob(1, 50.0, 25.0, 52.0, 0.0);

	/* create sample player */
      createPlayer(0, 52.0, 27.0, 52.0, 0.0);
       
   } else {

	/* Code Here -- Create World */
       
       FILE* fp;
       char c;
       int retval;
       
       fp = fopen("ground.pgm", "r");
       
    /* ignore pgm header */
       char bin[20];
       for (int i=0; i<4; i++)
           fgets(bin, 20, fp);
       
       
    /* initialize world to empty */
       for(i=0; i<WORLDX; i++)
           for(j=0; j<WORLDY; j++)
               for(k=0; k<WORLDZ; k++)
                   world[i][j][k] = 0;
       
    /* create blue cubes as bounds 
       for(i=0; i<WORLDX-1; i++) {
           world[i][25][0] = 2;
           world[i][25][WORLDZ-1] = 2;
       }
       for(i=0; i<WORLDZ-1; i++) {
           world[0][25][i] = 2;
           world[WORLDX-1][25][i] = 2;
       }*/
       
    /* create green cubes as ground */
       for(i=0; i<WORLDX; i++){
           for(k=0; k<WORLDZ; k++){
               fscanf(fp, "%d", &retval);
               retval /= 40;
               world[i][retval][k] = 2;
               /* fill holes below */
               for(j=retval; j>0; j--){
                   world[i][j][k] = 7;
               }
           }
       }
       
       fclose(fp);
   }
    j = WORLDY-1;
    for(i=0; i<WORLDX; i+=15){
        for(k=0; k<WORLDZ; k+=15){
            world[i][j][k] = 1;
            world[i][j-1][k] = 3;
            world[i][j-2][k] = 8;
        }
    }
    
    /* creates all cubes */
    for(i=0; i<9; i++) {
        world[35+i][45][50] = i;
    }
    
    oldTimeGrav = clock();
    oldTimeVel = clock();

	/* starts the graphics processing loop */
	/* code after this will not run until the program exits */
   glutMainLoop();
   return 0; 
}

