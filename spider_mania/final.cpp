#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include<stdio.h>
#include<stdlib.h>
#include <string>
using namespace std;
#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)


// Function Declarations
void drawScene();
void gameover_scene();
void update(int value);
//void update_spider(int value);
void drawBox(float len,float width);
void initRendering();
void handleResize(int w, int h);
void handleKeypress1(unsigned char key, int x, int y);
void handleKeypress2(int key, int x, int y);
void drawlaser();
//void handleKeypress2(unsigned char key, int x, int y);
void drawspider(float len);
void drawcanon(float canon_len,float canon_width);
void drawbasket(float basket_len,float basket_width);
void drawline();
void function();
void motion(int x, int y);
void handleMouseclick(int button, int state, int x, int y);

struct Spider {
    float pos[3]; //Position
    float color[3];
    float vel;
    float came_down;
};

struct Laser {
    float pos[3];
    float color[3];
    float angle;
};

struct laser_list
{
    Laser *laser;
    struct laser_list *next;
};

struct spider_list
{
        Spider* spider;
        struct spider_list *next;
};
spider_list *head;
laser_list *laser_head;

void insert_end(Spider *x)
{
    struct spider_list *ptr;
    ptr=head;
    if(ptr==NULL)
    {
        ptr=(spider_list *)malloc(sizeof(struct spider_list));
        ptr->spider=x;
        ptr->next=NULL;
        head=ptr;
    }
    else
    {
        while(ptr->next!=NULL)
            ptr=ptr->next;
        ptr->next=(spider_list *)malloc(sizeof(struct spider_list));
        ptr->next->spider=x;
        ptr->next->next=NULL;
    }
}

//laser insert in list
void laser_insert(Laser *x)
{
    struct laser_list *ptr;
    ptr=laser_head;
    if(ptr==NULL)
    {
        ptr=(laser_list *)malloc(sizeof(struct laser_list));
        ptr->laser=x;
        ptr->next=NULL;
        laser_head=ptr;
    }
    else
    {
        while(ptr->next!=NULL)
            ptr=ptr->next;
        ptr->next=(laser_list *)malloc(sizeof(struct laser_list));
        ptr->next->laser=x;
        ptr->next->next=NULL;
    }
}

//Returns a random float from 0 to < 1
float randomFloat() {
        return (float)rand() / ((float)RAND_MAX + 1);
}



//global variables
float box_len = 7.0f;
float box_width = 4.0f;
float spider_len = 0.25f;
int last_spider = 0;
float previous = 0.0f;
int counter=0;
float canon_len = 0.2f;
float canon_width = 0.3f;
float canon_x = 0.0f;
float basket_len = 0.8f;
float basket_width = 0.7f;
float basket1_x = box_len/2 - basket_len/2 - 0.1f;
float basket2_x = -box_len/2 + basket_len/2 + 0.1f;
float basket_y =-box_width/2 + 0.45f;
int laser_flag=0;
int last_laser = 0;
int option = 0;
float spider_min_vel = 0.03f;
float spider_max_vel = 0.055f;
int laser_counter = 0;
int laser_start = 0;
float theta=0.0f;
int gameover=0;
int pause = 0;
int gamescore = 0;
int difficulty = 1;
int action = 0;
int windowWidth,windowHeight;
int num_spiders;

int main(int argc, char **argv) {

    srand((unsigned int)time(0)); //Seed the random number generator
//    printf("Enter the difficulty level\n");
    scanf("%f%f%d%f%f%f", &box_len,&box_width,&difficulty,&canon_x,&basket1_x,&basket2_x);    
   // scanf("%d", &difficulty);
 
            
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    int w = glutGet(GLUT_SCREEN_WIDTH);
    int h = glutGet(GLUT_SCREEN_HEIGHT);
    windowWidth = w * 2 / 3;
    windowHeight = h * 2 / 3;

    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition((w - windowWidth) / 2, (h - windowHeight) / 2);

    glutCreateWindow("CSE251_sampleCode");  // Setup the window
    initRendering();
    head=NULL;
 

    // Register callbacks
    if(gameover==0)
    {
        glutDisplayFunc(drawScene);
        glutIdleFunc(drawScene);
    }
    if(gameover==1)
    {
        glutDisplayFunc(gameover_scene);
        glutIdleFunc(gameover_scene);
    }

    glutMouseFunc(handleMouseclick);
    glutMotionFunc(motion);
    glutKeyboardFunc(handleKeypress1);
    glutSpecialFunc(handleKeypress2);
    glutReshapeFunc(handleResize);
    glutTimerFunc(10, update, 0);

    glutMainLoop();
    return 0;
}

// Function to handle all calculations in the scene
// // updated evry 10 milliseconds
void update(int value) {


    if(gameover==0)
    {
        if(pause == 0)
        {
            if(difficulty==1)
            {
                if(gamescore > 9)
                    difficulty=2;
                num_spiders = 200;
            }

            if(difficulty == 2)
            {
                spider_min_vel = 0.05f;
                spider_max_vel = 0.075f;
                if(gamescore > 19)
                    difficulty=3;
                num_spiders = 180;
            }
            if(difficulty == 3)
            {
                spider_min_vel = 0.06f;
                spider_max_vel = 0.085f;
                num_spiders = 160;
            }



            //Handle canon collisions with box
            if(((canon_x + canon_width/2) > (box_len/2-0.155))) 
                canon_x -= 0.1f;
            if((canon_x - canon_width/2) < (-box_len/2+0.155))
                canon_x += 0.1f;

            //Handle basket collisions with box
            if((basket1_x+basket_len/2) > (box_len/2))
                basket1_x = box_len/2-basket_len/2;
            else if((basket1_x-basket_len/2) < (-box_len/2))
                basket1_x = -box_len/2 + basket_len/2;

            if((basket2_x+basket_len/2) > (box_len/2))
                basket2_x = box_len/2-basket_len/2;
            else if((basket2_x-basket_len/2) < (-box_len/2))
                basket2_x = -box_len/2 + basket_len/2;




            if((counter==num_spiders) || (counter==0))
            {
                Spider* spider  = new Spider();
                spider->pos[0] = 6*randomFloat() - 3;
                spider->pos[1] = box_width/2 + 0.2f;
                spider->pos[2] = 0.0f;
                float color_variable = randomFloat();
                if(color_variable < 0.33)
                {
                    spider->color[0]=0.9f;
                    spider->color[1]=0.0f;
                    spider->color[2]=0.0f;
                }
                else if(color_variable < 0.67)
                {
                    spider->color[0]=0.0f;
                    spider->color[1]=0.9f;
                    spider->color[2]=0.0f;
                }
                else
                {
                    spider->color[0]=0.0f;
                    spider->color[1]=0.0f;
                    spider->color[2]=0.0f;
                }
                float variable = randomFloat();
                if(variable < 0.5)
                    spider->vel = spider_min_vel;
                else
                    spider->vel = spider_max_vel;
                spider->came_down = 0;
                insert_end(spider);
                last_spider += 1;
                counter=0;
            }


            //if laser is to be released
            if(laser_flag==1)
            {
                printf("laser_flag-----%d\n", laser_flag);
                if((laser_counter >= 100) || (laser_counter == 0))
                {
                    printf("laser_count-%d\n", laser_counter);

                    Laser* temp_laser  = new Laser();
                    temp_laser->pos[0] = canon_x;
                    temp_laser->pos[1] = -box_width/2 + 0.25f;
                    temp_laser->pos[2] = 0.0f;
                    temp_laser->color[0]=randomFloat();
                    temp_laser->color[1]=randomFloat();
                    temp_laser->color[2]=randomFloat();
                    temp_laser->angle=theta;
                    laser_insert(temp_laser);
                    last_laser += 1;
                    laser_counter=0;
                }
                laser_flag=0;
            }
            if(laser_start == 1)
                laser_counter++;




            // update the position of laser
            laser_list *temp_head = laser_head;
            while(temp_head!=NULL)
            {
                temp_head->laser->pos[0] -= 0.09*(sin(DEG2RAD(temp_head->laser->angle)));
                temp_head->laser->pos[1] += 0.09*(cos(DEG2RAD(temp_head->laser->angle)));
                if(((temp_head->laser->pos[0]) > (box_len/2 - 0.5f)) || ((temp_head->laser->pos[0]) < (-box_len/2 + 0.5f)))
                {
                    temp_head->laser->angle = temp_head->laser->angle*(-1);
                }
                temp_head = temp_head->next;
            }


            //delete the laser after reaching top
            if(laser_head!=NULL)
            {
                if(((laser_head->laser->pos[1] + 0.4f)*cos(DEG2RAD(laser_head->laser->angle))) > box_width)
                {
                    printf("no\n");
                    laser_list *temp = laser_head;
                    laser_head = temp->next;
                    //           free(temp);
                }
            }


            //laser and spider collision
            temp_head = laser_head;
            int shifting=0;
            laser_list *laser_previous = NULL;
            while(temp_head!=NULL)
            {
                spider_list *temp_spider_list = head;
                spider_list *spider_previous = NULL;
                while(temp_spider_list!=NULL)
                {
                    if(temp_spider_list->spider->came_down==0)
                    {
                        /*  if((((temp_head->laser->pos[0] +  0.7*sin(DEG2RAD(temp_head->laser->angle))) > (temp_spider_list->spider->pos[0] - spider_len/2 -0.4f)) && ((temp_head->laser->pos[0] + 0.7*sin(DEG2RAD(temp_head->laser->angle))) < (temp_spider_list->spider->pos[0] + spider_len/2 + 0.4f))) && ((temp_head->laser->pos[1] + 0.7*cos(DEG2RAD(temp_head->laser->angle))) > (temp_spider_list->spider->pos[1] -spider_len/2 - 0.05f)) && ((temp_head->laser->pos[1] + 0.7*cos(DEG2RAD(temp_head->laser->angle)))< (temp_spider_list->spider->pos[1] + spider_len/2 + 0.05f)))
                            {*/
                        if((((temp_head->laser->pos[0]) > (temp_spider_list->spider->pos[0] - spider_len/2 -0.3f)) && ((temp_head->laser->pos[0]) < (temp_spider_list->spider->pos[0] + spider_len/2 + 0.3f))) && ((temp_head->laser->pos[1]) > (temp_spider_list->spider->pos[1] -spider_len/2 - 0.2f)) && ((temp_head->laser->pos[1])< (temp_spider_list->spider->pos[1] + spider_len/2 + 0.2f)))
                        { 
                            system("aplay LASER1.WAV &");
                            if((temp_spider_list->spider->color[0] < 0.1f) && (temp_spider_list->spider->color[1] < 0.1f) && (temp_spider_list->spider->color[2] < 0.1f))
                            {
                                gamescore += 1;
                            }
                            else
                                gamescore -= 1;

                            if(spider_previous == NULL)
                            {
                                head = temp_spider_list->next;
                                free(temp_spider_list);
                                temp_spider_list = (spider_list *)malloc(sizeof(spider_list));
                                temp_spider_list = temp_spider_list->next;
                            }
                            if(laser_previous == NULL)
                            {
                                laser_head = temp_head->next;
                                free(temp_head);
                                temp_head = (laser_list *)malloc(sizeof(laser_list));
                                temp_head = temp_head->next;
                            }

                            if(spider_previous!=NULL)
                            {
                                spider_previous->next=temp_spider_list->next;
                                free(temp_spider_list);
                                temp_spider_list = (spider_list *)malloc(sizeof(spider_list));
                                temp_spider_list = temp_spider_list->next;
                            }
                            if(laser_previous!=NULL)
                            {
                                laser_previous->next=temp_head->next;
                                free(temp_head);
                                temp_head = (laser_list *)malloc(sizeof(laser_list));
                                temp_head = temp_head->next;
                            }
                            shifting = 1;
                            break;
                        }
                        else
                        {
                            spider_previous = temp_spider_list;
                            temp_spider_list = temp_spider_list->next;
                        }
                    }
                    else
                    {
                        spider_previous = temp_spider_list;
                        temp_spider_list = temp_spider_list->next;
                    }
                }
                if(shifting!=1)
                {
                        temp_head=temp_head->next;
                }
                shifting=0;
            }

                //canon and spider collision

                spider_list *temp_spider = head;
                while(temp_spider!=NULL)
                {
                    if(temp_spider->spider->came_down==0)
                    {
                        if((((-box_width/2 + 0.25f + canon_width) >= (temp_spider->spider->pos[1] - spider_len/2 -0.05))) && (((temp_spider->spider->pos[0]) > (canon_x - canon_len - 0.15f )) && ((temp_spider->spider->pos[0] - spider_len/2) < (canon_x + canon_len + 0.15f))))
                        {
                           // system("aplay smw_lost_a_life.wav &");
                            system("aplay smw_game_over.wav &");
                            printf("final\n");
                            gameover=1;
                        }
                    }
                    if(temp_spider->spider->came_down==1)
                    {
                        if(((canon_x + canon_len + 0.05f) > (temp_spider->spider->pos[0] - spider_len/2 - 0.15f)) && ((temp_spider->spider->pos[0] - spider_len/2 - 0.15f) > canon_x))
                        {
                            printf("spider1---%f\n", temp_spider->spider->pos[0]);
                            canon_x -= 0.1f;
                        }
                        else if(((canon_x - canon_len - 0.05f) < (temp_spider->spider->pos[0] + spider_len/2 + 0.15f)) && ((temp_spider->spider->pos[0] + spider_len/2 + 0.15f) < (canon_x)))
                        {
                            printf("canon2222\n");
                            printf("spider2---%f\n", temp_spider->spider->pos[0]);
                            canon_x += 0.1f;
                        }
                    }
                    temp_spider = temp_spider->next;
                }

                //spider and basket collisions
                spider_list *temp_spider_list = head;
                spider_list *spider_previous = NULL;
                int no_collisions=0;
                while(temp_spider_list!=NULL)
                {
                    if(temp_spider_list->spider->came_down==0)
                    {

                        int basket1_collided = 0;
                        no_collisions=1;
                        //basket_1 collision
                        if(((temp_spider_list->spider->pos[1] + spider_len/2) <  (basket_y + basket_len/2))&&(((temp_spider_list->spider->pos[0]-spider_len/2-0.05f) < (basket1_x + basket_len/2)) && ((temp_spider_list->spider->pos[0] + spider_len/2 + 0.05f) > (basket1_x - basket_len/2))))
                        {
                            system("aplay dropball2.wav &");
                            if(((basket2_x - basket_len/2) < (basket1_x + basket_len/2)) && ((basket2_x + basket_len/2) > (basket1_x - basket_len/2)))
                            {

                                if(temp_spider_list->spider->color[0] > 0.1f)
                                {
                                    gamescore += 2;
                                }
                            }
                                if(temp_spider_list->spider->color[1] > 0.1f)
                                {
                                    gamescore += 1;
                                }
                                else
                                    gamescore -= 1;
                             //   printf("basket1\n");
                                if(spider_previous==NULL)
                                {
                                    head=temp_spider_list->next;
                                    free(temp_spider_list);
                                    temp_spider_list = (spider_list *)malloc(sizeof(spider_list));
                                    temp_spider_list=temp_spider_list->next;
                                }
                                else
                                {
                                    spider_previous->next = temp_spider_list->next;
                                    free(temp_spider_list);
                                    temp_spider_list = (spider_list *)malloc(sizeof(spider_list));
                                    temp_spider_list = spider_previous->next;
                                }
                                basket1_collided = 1;
                                no_collisions=0;
                           // printf("collsion--- %d\n", basket1_collided);
                        }
                        if(basket1_collided!=1)
                        {

                            if(((temp_spider_list->spider->pos[1] + spider_len/2) <  (basket_y + basket_len/2))&&((temp_spider_list->spider->pos[0] + spider_len/2 + 0.05f) > (basket2_x - basket_len/2)) && ((temp_spider_list->spider->pos[0]-spider_len/2 - 0.05f) < (basket2_x + basket_len/2)))
                            {
                                system("aplay dropball2.wav &");
                              //      printf("1111111111111111111111\n");
                              //  if(((basket1_x - basket_len/2) < (basket2_x + basket_len/2)) && ((basket1_x + basket_len/2) > (basket2_x - basket_len/2)))
                          //  {
                             /*   if(red_there==1)
                                {
                                    gamescore += 2;

                                }

                          //  }
                                else
                                {*/
                                    if(temp_spider_list->spider->color[0] > 0.1f)
                                    {
                                        gamescore += 1;
                                    }
                                    else
                                        gamescore -= 1;


                                    if(spider_previous==NULL)
                                    {
                                        head=temp_spider_list->next;
                                        free(temp_spider_list);
                                        temp_spider_list = (spider_list *)malloc(sizeof(spider_list));
                                        temp_spider_list=temp_spider_list->next;
                                    }
                                    else
                                    {
                                        spider_previous->next = temp_spider_list->next;
                                        // temp_spider_list=NULL;
                                        free(temp_spider_list);
                                        temp_spider_list = (spider_list *)malloc(sizeof(spider_list));
                                        temp_spider_list = spider_previous->next;
                                    }
                                    //       }
                                    no_collisions=0;
                          //    }
                            }
                        }
                        if(no_collisions==1)
                        {
                            temp_spider_list = temp_spider_list->next;
                        }
                    }
                    else  //basket and spider collision on ground
                    {
                        if(((basket1_x + basket_len/2) > (temp_spider_list->spider->pos[0] - spider_len/2 - 0.15f)) && ((temp_spider_list->spider->pos[0] - spider_len/2 - 0.15f) > basket1_x))
                        {
                            basket1_x -= 0.1f;
                        }
                        else if(((basket1_x - basket_len/2) < (temp_spider_list->spider->pos[0] + spider_len/2 + 0.15f)) && ((temp_spider_list->spider->pos[0] + spider_len/2 + 0.15f) < (basket1_x)))
                        {
                            basket1_x += 0.1f;
                        }
                        if(((basket2_x + basket_len/2) > (temp_spider_list->spider->pos[0] - spider_len/2 - 0.15f)) && ((temp_spider_list->spider->pos[0] - spider_len/2 - 0.15f) > basket2_x))
                        {
                            basket2_x -= 0.12f;
                        }
                        else if(((basket2_x - basket_len/2) < (temp_spider_list->spider->pos[0] + spider_len/2 + 0.15f)) && ((temp_spider_list->spider->pos[0] + spider_len/2 + 0.15f) < (basket2_x)))
                        {
                            basket2_x += 0.12f;
                        }
                        //   printf("wwwwwwww12344\n");
                        spider_previous = temp_spider_list;
                        // printf("12344\n");
                        temp_spider_list = temp_spider_list->next;

                    }
                }
                
//            }
        //update the position of spider
            if(counter%3 == 0)
            {

                spider_list *var = head;
                while(var!=NULL)
                {
                    Spider *temp_spider = var->spider;
                    if((var->spider->pos[1] - spider_len/2 - 0.05f) > (-box_width/2 + 0.25f))
                    {
                        temp_spider->pos[1] -= temp_spider->vel;
                    }
                    else
                    {
                        if(temp_spider->came_down == 0)
                            gamescore -= 5;
                        temp_spider->came_down = 1;
                    }
                    var=var->next;
                }
            }
            counter+=1;
    }
        glutTimerFunc(10, update, 0);
    }
}

// Function to draw objects on the screen
void drawScene() {
        if(gameover==0)
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(1.0f, 1.0f,0.6f,0.0f);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glPushMatrix();

            glColor3f(0.0f, 0.0f, 0.2f);
            // Draw Box
            glTranslatef(0.0f, 0.0f, -5.0f);
            glColor3f(0.3f, 0.3f, 0.9f);
            drawBox(box_len,box_width);

            //Draw line
            glPushMatrix();
            //    glTranslatef(0.0f,2.0f,0.0f);
            drawline();
            glPopMatrix();

            //draw canon
            glPushMatrix();
            glTranslatef(canon_x, -box_width/2 + 0.25f, 0.0f);
            if(option==0)
                glColor3f(0.2f, 0.1f, 0.8f);
            else
                glColor3f(0.2f,0.1f,0.5f);

            glRotatef(theta, 0.0f, 0.0f, 1.0f);
            drawcanon(canon_len,canon_width);
            glPopMatrix();

            //draw baskets
            glPushMatrix();
            glTranslatef(basket1_x,basket_y, 0.0f);
            if(option==1)
                glColor3f(0.0f,1.0f,0.0f);
            else
                glColor3f(0.0f,0.7f,0.0f);
            drawbasket(basket_len,basket_width);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(basket2_x,basket_y, 0.0f);
            if(option==2)
                glColor3f(1.0f,0.0f,0.0f);
            else
                glColor3f(0.7f,0.0f,0.0f);

            drawbasket(basket_len,basket_width);
            glPopMatrix();


            // Draw Spider
            spider_list *var  = head;
            while(var!=NULL)
            {
                Spider *temp_spider = var->spider;
                glPushMatrix();
                glTranslatef(temp_spider->pos[0], temp_spider->pos[1], temp_spider->pos[2]);
                glColor3f(temp_spider->color[0], temp_spider->color[1], temp_spider->color[2]);
                drawspider(spider_len);
                glPopMatrix();
                var=var->next;
            }

            // Draw Laser
            laser_list *var1  = laser_head;
            while(var1!=NULL)
            {
                Laser *temp_laser = var1->laser;
                glPushMatrix();
                glTranslatef(temp_laser->pos[0],temp_laser->pos[1], temp_laser->pos[2]);
                //   glColor3f(temp_laser->color[0], temp_laser->color[1], temp_laser->color[2]);
                glRotatef(temp_laser->angle, 0.0f, 0.0f, 1.0f);
               drawlaser();
                glPopMatrix();
                var1=var1->next;
            }
            laser_flag=0;
            char diff[15];
            glColor3f(0.0f,0.5f,1.0f);
            sprintf(diff, "LEVEL  %d",difficulty);
            glRasterPos3f(-0.3f,box_width/2-0.3f,0.0f);
             for(int l=0;diff[l]!='\0';l++)
                glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, diff[l]);

            char text2[15];
            glColor3f(0.5f,0.5f,0.5f);
            sprintf(text2, "Score : %d",gamescore);
            glRasterPos3f(box_len/2 - 1.0f,box_width/2-0.5f,0.0f);
             for(int l=0;text2[l]!='\0';l++)
                glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, text2[l]);
            glColor3f(0.3f, 0.3f, 0.9f);
            glPopMatrix();
            glutSwapBuffers();
        }
        else
        {
            gameover_scene();
        }
    }

void drawbasket(float len, float width) {

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_QUADS);
    glVertex2f(-len / 2, -width / 2);
    glVertex2f(len / 2, -width / 2);
    glVertex2f(len / 2, width / 2);
    glVertex2f(-len / 2, width / 2);
    glEnd();
    glPushMatrix();
    glColor3f(0.0f,0.0f,0.2f);
    glTranslatef(0.0f,width/2,0.0f);
    glBegin(GL_TRIANGLE_FAN);
    for(int i=0 ; i<360; i++)
    {
        glVertex2f((len/2) * cos(DEG2RAD(i)), (0.05) * sin(DEG2RAD(i)));
    }
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glColor3f(0.0f,0.0f,0.2f);
   // glColor3f(1.0f,1.0f,1.0f);
    glTranslatef(0.0f,-width/2,0.0f);
    glBegin(GL_TRIANGLE_FAN);
    for(int i=0 ; i<360; i++)
    {
        glVertex2f((len/2) * cos(DEG2RAD(i)), (0.05) * sin(DEG2RAD(i)));
    }
    glEnd();
    glPopMatrix();


    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void drawcanon(float len, float width) {

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_QUADS);
    glVertex2f(-len, -width/2);
    glVertex2f(len, -width/2);
    glVertex2f(len, width);
    glVertex2f(-len, width);
    glEnd();
    glPushMatrix();
    glTranslatef(0.0f,width,0.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_QUADS);
    glVertex2f(-0.05f, 0.0f);
    glVertex2f(0.05f, 0.0f);
    glVertex2f(0.05f,0.4f);
    glVertex2f(-0.05f,0.4f);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-len,0.05f,0.0f);
    glLineWidth(4);
    glBegin(GL_LINES);
    glVertex3f(0.0f,0.0f,0.0f);
    glVertex3f(-0.1f,0.0f,0.0f);
    glEnd();
    glPushMatrix();
    glTranslatef(-0.1f,0.0f,0.0f);
    glLineWidth(9);
    glBegin(GL_LINES);
    glVertex3f(0.0f,0.25f,0.0f);
    glVertex3f(0.0f,-0.2f,0.0f);
    glEnd();
    glPopMatrix();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(len,0.05f,0.0f);
    glLineWidth(4);
    glBegin(GL_LINES);
    glVertex3f(0.0f,0.0f,0.0f);
    glVertex3f(0.1f,0.0f,0.0f);
    glEnd();
    glPushMatrix();
    glTranslatef(0.1f,0.0f,0.0f);
    glLineWidth(9);
    glBegin(GL_LINES);
    glVertex3f(0.0f,0.25f,0.0f);
    glVertex3f(0.0f,-0.2f,0.0f);
    glEnd();
    glPopMatrix();
    glPopMatrix();
 }
void drawlaser()
{
    glLineWidth(3.0);
 //   glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.0f,0.0f, 0.0f);
    glVertex3f(0.0f,0.4f,0.0f);
   // glVertex3f(0.0f,0.8f,0.0f);
    glEnd();
}

void drawline()
{
    glLineWidth(2.5);
 //   glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(-box_len/2, -1.4f, 0.0f);
    glVertex3f(box_len/2,-1.4f,0.0f);
    glEnd();
}

void drawBox(float len, float width) {

    glLineWidth(3.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_QUADS);
    glVertex2f(-len / 2, -width / 2);
    glVertex2f(len / 2, -width / 2);
    glVertex2f(len / 2, width / 2);
    glVertex2f(-len / 2, width / 2);
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void drawspider(float len) {

    glBegin(GL_TRIANGLE_FAN);
    for(int i=0 ; i<360; i++)
    {
        glVertex2f((len/2-0.2) * cos(DEG2RAD(i)), (len/2+0.05) * sin(DEG2RAD(i)));
    }
    glEnd();
    glPushMatrix();
        glTranslatef((len/2-0.2)*cos(DEG2RAD(30)),(len/2+0.05) * sin(DEG2RAD(30)),0.0f);
        glLineWidth(2.5);
        glBegin(GL_LINES);
            glVertex3f(0.0f,0.0f,0.0f);
            glVertex3f(0.15f,0.15f,0.0f);
        glEnd();
       glLineWidth(2);
        glBegin(GL_LINES);
            glVertex3f(0.15f,0.15f,0.0f);
            glVertex3f(0.15f,0.2f,0.0f);
        glEnd();
   glPopMatrix();
   glPushMatrix();
    glTranslatef((len/2-0.2)*cos(DEG2RAD(150)),(len/2+0.05) * sin(DEG2RAD(150)),0.0f);
    glLineWidth(2.5);
    glBegin(GL_LINES);
        glVertex3f(0.0f,0.0f,0.0f);
        glVertex3f(-0.15f,0.15f,0.0f);
    glEnd();
    glLineWidth(2);
    glBegin(GL_LINES);
        glVertex3f(-0.15f,0.15f,0.0f);
        glVertex3f(-0.15f,0.2f,0.0f);
    glEnd();
   glPopMatrix();
   glPushMatrix();
    glTranslatef((len/2-0.2)*cos(DEG2RAD(5)),(len/2+0.05) * sin(DEG2RAD(5)),0.0f);
    glLineWidth(2.5);
    glBegin(GL_LINES);
        glVertex3f(0.0f,0.0f,0.0f);
        glVertex3f(0.2f,0.0,0.0f);
    glEnd();
    glLineWidth(2);
    glBegin(GL_LINES);
        glVertex3f(0.2f,0.0f,0.0f);
        glVertex3f(0.25f,0.1f,0.0f);
    glEnd();
   glPopMatrix();
   glPushMatrix();
    glTranslatef((len/2-0.2)*cos(DEG2RAD(355)),(len/2+0.05) * sin(DEG2RAD(355)),0.0f);
    glLineWidth(2.5);
    glBegin(GL_LINES);
        glVertex3f(0.0f,0.0f,0.0f);
        glVertex3f(0.2f,0.0,0.0f);
    glEnd();
    glLineWidth(2);
    glBegin(GL_LINES);
        glVertex3f(0.2f,0.0f,0.0f);
        glVertex3f(0.25f,-0.1f,0.0f);
    glEnd();
   glPopMatrix();
   glPushMatrix();
    glTranslatef((len/2-0.2)*cos(DEG2RAD(175)),(len/2+0.05) * sin(DEG2RAD(175)),0.0f);
    glLineWidth(2.5);
    glBegin(GL_LINES);
        glVertex3f(0.0f,0.0f,0.0f);
        glVertex3f(-0.2f,0.0,0.0f);
    glEnd();
    glLineWidth(2);
    glBegin(GL_LINES);
        glVertex3f(-0.2f,0.0f,0.0f);
        glVertex3f(-0.25f,0.1f,0.0f);
    glEnd();
   glPopMatrix();
   glPushMatrix();
    glTranslatef((len/2-0.2)*cos(DEG2RAD(185)),(len/2+0.05) * sin(DEG2RAD(185)),0.0f);
    glLineWidth(2.5);
    glBegin(GL_LINES);
        glVertex3f(0.0f,0.0f,0.0f);
        glVertex3f(-0.2f,0.0,0.0f);
    glEnd();
    glLineWidth(2);
    glBegin(GL_LINES);
        glVertex3f(-0.2f,0.0f,0.0f);
        glVertex3f(-0.25f,-0.1f,0.0f);
    glEnd();
   glPopMatrix();
   glPushMatrix();
    glTranslatef((len/2-0.2)*cos(DEG2RAD(240)),(len/2+0.05) * sin(DEG2RAD(240)),0.0f);
    glLineWidth(2.5);
    glBegin(GL_LINES);
        glVertex3f(0.0f,0.0f,0.0f);
        glVertex3f(-0.15f,-0.15f,0.0f);
    glEnd();
    glLineWidth(2);
    glBegin(GL_LINES);
        glVertex3f(-0.15f,-0.15f,0.0f);
        glVertex3f(-0.15f,-0.2f,0.0f);
    glEnd();
   glPopMatrix();
   glPushMatrix();
    glTranslatef((len/2-0.2)*cos(DEG2RAD(300)),(len/2+0.05) * sin(DEG2RAD(300)),0.0f);
    glLineWidth(2.5);
    glBegin(GL_LINES);
        glVertex3f(0.0f,0.0f,0.0f);
        glVertex3f(0.15f,-0.15f,0.0f);
    glEnd();
    glLineWidth(2);
    glBegin(GL_LINES);
        glVertex3f(0.15f,-0.15f,0.0f);
        glVertex3f(0.15f,-0.2f,0.0f);
    glEnd();
   glPopMatrix();
}

void gameover_scene()
{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 1.0f,0.7f,1.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glPushMatrix();
        //    glColor3f(255,255,255);

        // Draw Box
        glTranslatef(0.0f, 0.0f, -5.0f);
        glColor3f(0.3f, 0.3f, 0.9f);
        drawBox(box_len,box_width);
        char text2[9];
        glColor3f(1.0f,0.0f,0.0f);
        sprintf(text2, "GAME OVER");
        glRasterPos3f(-0.5,0,0.0f);
    /*    if(gameover==1)
        {
            system("aplay smw_game_over.wav &");
            gameover=0;
        }*/
        for(int l=0;text2[l]!='\0';l++)
            glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, text2[l]);
        
        sprintf(text2, "Your Game score is: %d",gamescore);
        glRasterPos3f(-0.8f,-0.4f,0.0f);
        for(int l=0;text2[l]!='\0';l++)
            glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, text2[l]);

        sprintf(text2, "Press 'z' to Restart");
        glRasterPos3f(-0.6f,-0.7f,0.0f);
        for(int l=0;text2[l]!='\0';l++)
            glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, text2[l]);

        if(option==3)
        {
            gameover=0;
            gamescore=0;
            head=NULL;
            option=0;
            laser_head=NULL;
            glutTimerFunc(10, update, 0);
            drawScene();
        }


     //   for(l =0;str[l]!='\0';l++)
       //     glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, s);


        

/*
        string str;
        str="Game over";
        int len;
        len = str.length();
        glColor3f(0.3f, 0.3f, 0.9f);
        for (int i = 0; i < len; i++)
        {
            printf("%c", str[i]);
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
        }*/
        glPopMatrix();
        glutSwapBuffers();
}

void function()
{
    system("aplay smw_game_over.wav &");
}


// Initializing some openGL 3D rendering options
void initRendering() {

    glEnable(GL_DEPTH_TEST);        // Enable objects to be drawn ahead/behind one another
    glEnable(GL_COLOR_MATERIAL);    // Enable coloring
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);   // Setting a background color
}

void handleResize(int w, int h) {

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / (float)h, 0.1f, 200.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void handleKeypress1(unsigned char key, int x, int y) {
     
      if(key == 27)
        exit(0);
    if(key ==' ')
    {
        laser_start=1;
        laser_flag=1;
    }
    if(key=='g')
        option=1;
    if(key == 'r')
    {
            option = 2;
    }
    if(key == 'b')
    {
            option = 0;
    }
    if(key == 'p')
    {
        if(pause==0)
        {
            pause=1;
        }
        else if(pause==1)
        {
            pause=0;
        }
    }
    if(key == 'z')
    {
        option=3;
    }
}

void handleKeypress2(int key, int x, int y) {

    if (key == GLUT_KEY_LEFT)
    {
        if(pause==0)
        {
            if(option==0)
                canon_x -= 0.1;
            else if(option==1)
                basket1_x -= 0.2;
            else if(option==2)
                basket2_x -= 0.2;
        }
    }
    if (key == GLUT_KEY_RIGHT)
    {
        if(pause==0)
        {
            if(option==0)
                canon_x += 0.1;
            else if(option==1)
                basket1_x += 0.1;
            else if(option==2)
                basket2_x += 0.1;
        }
    }
    if (key == GLUT_KEY_UP)
    {
        if(pause==0)
        {
            theta += 2;
            if(theta > 50)
                theta = 50;
        }
    }
    if (key == GLUT_KEY_DOWN)
    {
        if(pause==0)
        {
            theta -= 2;
            if(theta < -50)
                theta = -50;
        }
    }
}

void handleMouseclick(int button, int state, int x, int y) {

    if ((state == GLUT_DOWN) && (button == GLUT_RIGHT_BUTTON))
    {
        if (button == GLUT_RIGHT_BUTTON)
            action = 'r';
        if (button == GLUT_LEFT_BUTTON)
        {
            action = 's';
        }
    }
}


void motion(int x, int y)
{
    //float mouseX = (x / windowWidth) - 0.4395f;
    //float mouseY = (y / windowHeight) - 0.5f;
    //float ratio = float(windowWidth)/float(x); 
    if(action=='r')
    {
        printf("%f %f\n", ((float(windowWidth)/(float(x)))-0.4395),canon_x);
        printf("x-- %d y-- %d w-- %d h--- %d c---- %f\n", x,y,windowWidth,windowHeight,canon_x);
        if(((float(x)/float(windowWidth))-0.4395) <= 0.0f)
        {
            printf("111111\n");
            theta += 2;
            if(theta > 50)
                theta = 50;
        }
        if(((float(x)/float(windowWidth))-0.4395) > 0.0f)
        {
            theta -= 2;
            if(theta < -50)
                theta = -50;
        }
        
    }
    if(action=='s')
    {
     /*   if(((float(x)/(float(windowWidth)) - 0.4395)) > (canon_x - canon_len) && ((float(x)/(float(windowWidhth)) - 0.4395)> (canon_x + canon_len)) && (float(y)/float(windowHeight) )
        {
        }*/

    }
}

