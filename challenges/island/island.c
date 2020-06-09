#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> 
#include <time.h>

const int island[8][10] = {//this is a matrix that represents the Island
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 2, 1, 2, 1, 2, 1, 0},
    {0, 2, 2, 2, 3, 3, 3, 1, 2, 0},
    {0, 2, 1, 2, 3, 4, 3, 2, 2, 0},
    {0, 3, 3, 3, 3, 3, 3, 2, 1, 0},
    {0, 3, 4, 3, 1, 2, 2, 2, 1, 0},
    {0, 3, 3, 3, 1, 2, 1, 2, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

int positions[8][10] = {//this are the initial possitions of every space in the Island
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};


pthread_mutex_t lockTurn; //this is a thread that will help us when we have to modify the possitions of the matrix so that we don't have a race condition when accessing the values

struct ballData {//this is a stuct that will help us store the data of the ball
    pthread_t tid;
    long id;
    int x;
    int y;
};

struct ballData *balls; //initialize a struct
struct ballData *ballInTransition; // Special for chained collission management storage
int trappedBalls, northSea, southSea, eastSea, westSea; //initiliaze variables

void printArray(int array[][10]){//small function to print the array of islands
    int i, j;
    for(i = 0; i < sizeof(island)/sizeof(island[0]);i++){
        for (j =0; j < sizeof(island[i])/sizeof(island[i][0]); j++){
            printf("%d, ", array[i][j]);
        }
        printf("\n");
    }
}

void setColor(int level){//function to set the color of every level of the island depending on the height
    switch(level) {
        case 0:
            printf("\033[0;34m"); // Set to blue
            break;
        case 1:
            printf("\033[0;36m"); // Set to cyan
            break;
        case 2:
            printf("\033[0;32m"); // Set to green 
            break;
        case 3:
            printf("\033[0;33m"); // Set to yellow
            break;
        case 4:
            printf("\033[0;31m"); // Set to red
            break;
        case 5:
             printf("\033[0;35m"); // Set to magenta
            break;
        default:
            printf("\033[0m");
    }
}

void printStatus(){//this is a function that prints the status of the map of the island
    int i, j;
    // Print Y Axis for Positions Map
    printf("\t\t\t\t\t   ");
    for(i = 0; i < sizeof(island[0])/sizeof(island[0][0]);i++){
        printf("%-2d  ", i);
    }
    printf("\n");
    for(i = 0; i < sizeof(island)/sizeof(island[0]);i++){
        for (j =0; j < sizeof(island[i])/sizeof(island[i][0]); j++){
            setColor(island[i][j]);
            printf("%d", island[i][j]);
            setColor(-1);
            printf(", ");
        }
        printf("\t\t%d  ", i);
        for (j =0; j < sizeof(island[i])/sizeof(island[i][0]); j++){
            
            setColor(island[i][j]);
            if(positions[i][j] == -1){
                printf("__");
            }else{
                printf("%-2d", positions[i][j]);
            }
            setColor(-1);
            printf(", ");
        }
        printf("\n");
    }
}

int getDirection(int x, int y){//this is a function that checks in whick direction the ball will move
    //dir :: N=0 S=1 E=2 W=3
    int dir = 0; // North is the first one
    int currH = island[x-1][y]; 
    if(island[x+1][y] < currH) { // Check if South is better
        dir = 1;
        currH = island[x+1][y];
    }
    if(island[x][y+1] < currH) { // Check if East is better
        dir = 2;
        currH = island[x][y+1];
    }
    if(island[x][y-1] < currH) { // Check if West is better
        dir = 3;
    }
    return dir;
}

void collision(struct ballData *ball1, struct ballData *ball2);//initialize the function collision

void getXY(int dir, struct ballData *ball, int *newX, int *newY){//this is a function that will help us get X and Y of every possition of the ball
    switch (dir){
        case 0: // N
            *newX = ball->x-1;
            *newY = ball->y;
            break;
        case 1: // S
            *newX = ball->x+1;
            *newY = ball->y;
            break;
        case 2: // E
            *newX = ball->x;
            *newY = ball->y+1;
            break;
        case 3: // W
            *newX = ball->x;
            *newY = ball->y-1;
            break;
    }
}

int checkIfDrowned(struct ballData *ball){// This function checks if the ball landed in a 0 which is where there is water
    if(island[ball->x][ball->y] == 0){ // Check if drowned
        positions[ball->x][ball->y] = -1;
        if(ball->x == 0){
            northSea++;
        } else if (ball->x == (sizeof(island)/sizeof(island[0]))-1){
            southSea++;
        } else if (ball->y == 0) {
            westSea++;
        } else if (ball->y == (sizeof(island[0])/sizeof(island[0][0]))-1){
            eastSea++;
        }
        printf("Ball %ld landed in water\n", ball->id);
        return 1;
    }
    return 0;
}

void tryMoving(struct ballData *ball, int newX, int newY){//this validates if you can move to another possition before changing possitions
    if (positions[newX][newY] == -1){// No collision
        positions[newX][newY] = ball->id;
        if (positions[ball->x][ball->y] == ball->id){
            positions[ball->x][ball->y] = -1;
        }
        ball->x = newX;
        ball->y = newY;
    } else{ // Crashing
        setColor(4);
        printf("Collision!: ");
        setColor(-1);
        struct ballData *ball2 = &balls[positions[newX][newY]];
        printf("Ball %ld collided with ball %ld at [%d][%d]\n", ball->id, ball2->id, ball2->x, ball2->y);
        collision(ball, ball2);
    }
}

void collision(struct ballData *ball1, struct ballData *ball2){//this checks if the balls have colided
    // Get Random Directions
    int newX, newY;
    do {
        int dir2 = rand() % 4;
        getXY(dir2, ball2, &newX, &newY);
    }while(newX == ball1->x && newY == ball1->y);
    printf("Ball %ld moving from [%d][%d] : level %d\n", ball2->id, ball2->x, ball2->y, island[ball2->x][ball2->y]);
    tryMoving(ball2, newX, newY);
    printf("Ball %ld moved to [%d][%d] : level %d\n", ball2->id, ball2->x, ball2->y, island[ball2->x][ball2->y]);
    checkIfDrowned(ball2);
    // Finish moving the other ball
    int dir1 = rand() % 4;
    getXY(dir1, ball1, &newX, &newY);
    tryMoving(ball1, newX, newY);
}

void *ballBehaviour(void *threadId) { //this checks the ball behaviour, the movement through the map
    long tid = (long)threadId;
    int finished = 0;
    int alreadyInWater = 0;
    struct ballData *ball = &balls[tid];
    // First position
    while(!finished){
        int x = rand() % sizeof(island)/sizeof(island[0]);
        int y = rand() % sizeof(island[0])/sizeof(island[0][0]);
        pthread_mutex_lock(&lockTurn);
        if(positions[x][y] == -1){
            printf("Init pos ball %ld: [%d][%d]\n", tid, x, y);
            positions[x][y] = tid;
            ball->x = x;
            ball->y = y;
            finished = 1;
            // Check if landed in water from the start
            alreadyInWater = checkIfDrowned(ball);
        }
        pthread_mutex_unlock(&lockTurn);
    }
    finished = 0;
    if(alreadyInWater){
        finished = 1;
    }
    // Movement
    int speed = 1000; // 1 second
    while(!finished){
        usleep(speed * 1000); // sleep in microseconds
        pthread_mutex_lock(&lockTurn);
        if(island[ball->x][ball->y] == 0){
            pthread_mutex_unlock(&lockTurn);
            break;
        }
        printf("Ball %ld moving from [%d][%d] : level %d\n", tid, ball->x, ball->y, island[ball->x][ball->y]);
        int currHeight = island[ball->x][ball->y];
        int direction = getDirection(ball->x, ball->y);
        int newX, newY;
        getXY(direction, ball, &newX, &newY);
        if(island[newX][newY] <= island[ball->x][ball->y]){ // Check if height is possible
            tryMoving(ball, newX, newY);
            int newHeight = island[ball->x][ball->y];
            printf("Ball %ld moved to [%d][%d] : level %d\n", balls[tid].id, balls[tid].x, balls[tid].y, newHeight);
            printf("Ball %ld:: last speed: %d, ", tid, speed);
            speed = speed-(100*(currHeight-newHeight));
            printf("current speed: %d\n", speed);
            finished = checkIfDrowned(ball);
            printStatus();
        } else { // If Stucked
            printf("Ball %ld stucked in island at [%d][%d] : level %d\n", tid, ball->x, ball->y, island[ball->x][ball->y]);
            trappedBalls++;
            finished = 1;
            positions[ball->x][ball->y] = -1;
        }
        pthread_mutex_unlock(&lockTurn);
    }
    pthread_exit(NULL);
} 

void *printFirstMap(void *threadId){//prints the status of the map at the beggining
    usleep(1000 * 500);
    printStatus();
    pthread_exit(NULL);
}

int main(int argc, char** argv){//main function where everything is controlled
    if(argc < 2) {
        printf("Error, missing args\n");
        printf("Usage: ./island [numberOfBalls]\n");
        return -1;
    }
    srand(time(0));
    int n = atoi(argv[1]);
    int size = sizeof(island)/sizeof(island[0][0]);
    if(n <= 0 || n > size){
        printf("Error: number of balls must a number, be higher than 0 and less or equal than %d \n", size);
        return -1;
    }
    trappedBalls = 0;
    northSea = 0;
    southSea = 0;
    eastSea = 0;
    westSea = 0;
    // pthread_t threads[n];
    balls = malloc(sizeof(struct ballData)*n);
    ballInTransition = malloc(sizeof(struct ballData));
    int rc;
    long t;
    for(t=0; t<n; t++){
        // printf("In main: creating thread %ld\n", t);
        rc = pthread_create(&(balls[t].tid), NULL, ballBehaviour, (void *)t);
        balls[t].id = t;
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    pthread_t firstMapPrint;
    pthread_create(&firstMapPrint, NULL, printFirstMap, NULL);
    pthread_join(firstMapPrint, NULL);
    for(t=0;t<n;t++){
        pthread_join(balls[t].tid, NULL);
    }

    printf("\n");
    printf("+--------------------+------+\n");
    printf("| Balls trapped      | %-4d |\n", trappedBalls);
    printf("+--------------------+------+\n");
    printf("| Balls in north sea | %-4d |\n", northSea);
    printf("+--------------------+------+\n");
    printf("| Balls in south sea | %-4d |\n", southSea);
    printf("+--------------------+------+\n");
    printf("| Balls in east sea  | %-4d |\n", eastSea);
    printf("+--------------------+------+\n");
    printf("| Balls in west sea  | %-4d |\n", westSea);
    printf("+--------------------+------+\n");

    return 0;
}