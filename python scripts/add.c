#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>


#define QUEUE_SIZE 1000000
#define LETTERS 4
#define DIGITS 4
#define NAMELENGTH 30
#define NUMBERLENGTH 11
#define N 26*26*26*26 + 1
#define M 10*10*10*10
#define MAX_RESULT 1000000
#define MAX_SIZE 1000000
int ID = 0;

typedef struct hashset
{
    int table[MAX_RESULT];
}hashset;

typedef struct Node
{
    char name[NAMELENGTH + 1];
    char number[NUMBERLENGTH + 1];
    int uid;
    int fhashvalue;
    int mhashvalue;
    int lhashvalue;
    struct Node *fnext;
    struct Node *fprev;
    struct Node *mnext;
    struct Node *mprev;
    struct Node *lnext;
    struct Node *lprev;
    struct Node *nnext;
} Node;

typedef struct Queue {
    int data[MAX_SIZE];
    int front;
    int rear;
} Queue;


Node *firstNameTable[N];
Node *middleNameTable[N];
Node *lastNameTable[N];
Node *numberTable[M];
Node* UIDtable[MAX_SIZE];
hashset* SET;
Queue* QUEUE;

//functions related to SET
void insert(int uid, hashset* set){
    int index = uid % MAX_SIZE;
    set->table[index] = uid;
}
void initSET(){
    for(int i = 0; i < MAX_RESULT; i++){
        SET->table[i] = -1;
    }
}

//functions related to queue
void initQueue() {
    QUEUE->front = -1;
    QUEUE->rear = -1;
}

bool isEmpty(Queue* queue) {
    return queue->front == -1;
}

bool isFull(Queue* queue) {
    return (queue->rear + 1) % QUEUE_SIZE == queue->front;
}

void enqueue(Queue* queue, int value) {
    if (isFull(queue)) {
        printf("Queue is full. Cannot enqueue.\n");
        return;
    }

    if (isEmpty(queue)) {
        queue->front = 0;
    }

    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
    queue->data[queue->rear] = value;
}

int dequeue(Queue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty. Cannot dequeue.\n");
        return -1;
    }

    int value = queue->data[queue->front];

    if (queue->front == queue->rear) {
        queue->front = queue->rear = -1;
    } else {
        queue->front = (queue->front + 1) % QUEUE_SIZE;
    }

    return value;
}
//functions related to hashing
char *removeSpaces(char *input)
{
    int input_length = strlen(input);
    char *output = (char *)malloc(input_length + 1);
    int j = 0;

    for (int i = 0; i < input_length; i++)
    {
        if (input[i] != ' ' )
        {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
    return output;
}
char* removeEscape(char *input){
    int input_length = strlen(input);
    char* output = (char*) malloc(input_length + 1);
    int j = 0;
    for(int i = 0; i < input_length; i++){
        if(input[i] != '\n'){
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
    return output;
}

int numberhash(char *number)
{
    int h = 0;
    int len = strlen(number);
    if (len > DIGITS)
    {
        len = DIGITS;
    }

    for (int i = 0; i < len; i++)
    {
        h = h + pow(10, DIGITS - i - 1)*(toupper(number[i]) - '0');
    }

    return h;
}
int namehash(char *input)
{
    char *word = removeSpaces(input);
    int h = 0;
    int len = strlen(word);
    if(len == 0){
        return N-1;
    }
    if (len > LETTERS)
    {
        len = LETTERS;
    }

    for (int i = 0; i < len; i++)
    {
        h = h + pow(26, LETTERS - i - 1)*(toupper(word[i]) - 'A');
    }

    return h;
}
int load(char* file)
{
    for (int i = 0; i < N; i++)
    {
        firstNameTable[i] = NULL;
        middleNameTable[i] = NULL;
        lastNameTable[i] = NULL;
    }
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        printf("please create contacts.csv\n");
        return 1;
    }
    char first_name[NAMELENGTH];
    char middle_name[NAMELENGTH];
    char last_name[NAMELENGTH];
    char line[NAMELENGTH + NUMBERLENGTH];
    char name_buffer[NAMELENGTH];
    char number_buffer[NUMBERLENGTH];
    while (fgets(line, sizeof(line), fp) != NULL)
    {

        sscanf(line, "%29[^,],%10s", name_buffer, number_buffer);
        Node *new = (Node *)malloc(sizeof(Node));
        strcpy(new->name, name_buffer);
        strcpy(new->number, number_buffer);
        new->fnext = NULL;
        new->fprev = NULL;
        new->mnext = NULL;
        new->mprev = NULL;
        new->lnext = NULL;
        new->lprev = NULL;
        new->nnext = NULL;
        //check for any reusable id in queue.
        if(isEmpty(QUEUE)){
            new->uid = ID;
            UIDtable[new->uid] = new;
            ID++;
        }
        else{
            new->uid = dequeue(QUEUE);
        }


        int names = sscanf(name_buffer,"%s %s %s", first_name, middle_name, last_name);
        if(names == 2){
            strcpy(last_name,"");
        }
        else if(names == 1){
            strcpy(middle_name,"");
            strcpy(last_name,"");
        }
        int fhashvalue = namehash(first_name);
        new->fhashvalue = fhashvalue;
        int mhashvalue = namehash(middle_name);
        new->mhashvalue = mhashvalue;
        int lhashvalue = namehash(last_name);
        new->lhashvalue = lhashvalue;
        if (firstNameTable[fhashvalue] == NULL)
        {
            firstNameTable[fhashvalue] = new;
        }
        else
        {
            new->fnext = firstNameTable[fhashvalue];
            new->fnext->fprev = new->fnext;
            firstNameTable[fhashvalue] = new;
        }

        if (middleNameTable[mhashvalue] == NULL)
        {
            middleNameTable[mhashvalue] = new;
        }
        else
        {
            new->mnext = middleNameTable[mhashvalue];
            new->mnext->mprev = new->mnext;
            middleNameTable[mhashvalue] = new;
        }

        if (lastNameTable[lhashvalue] == NULL)
        {
            lastNameTable[lhashvalue] = new;
        }
        else
        {
            new->lnext = lastNameTable[lhashvalue];
            new->lnext->lprev = new->lnext;
            lastNameTable[lhashvalue] = new;
        }
        int nhashvalue = numberhash(new->number);
        if (numberTable[nhashvalue] == NULL)
        {
            numberTable[nhashvalue] = new;
        }
        else
        {
            new->nnext = numberTable[nhashvalue];
            numberTable[nhashvalue] = new;
        }
    }
    fclose(fp);
    return 0;
}

void add_contact(char* name, char* number){
    char first_name[NAMELENGTH];
    char middle_name[NAMELENGTH];
    char last_name[NAMELENGTH];
    Node *new = (Node *)malloc(sizeof(Node));
    strcpy(new->name, name);
    strcpy(new->number, number);
    new->fnext = NULL;
    new->fprev = NULL;
    new->mnext = NULL;
    new->mprev = NULL;
    new->lnext = NULL;
    new->lprev = NULL;
    new->nnext = NULL;
    //check for any reusable id in queue.
    if(isEmpty(QUEUE)){
        new->uid = ID;
        UIDtable[new->uid] = new;
        ID++;
    }
    else{
        new->uid = dequeue(QUEUE);
    }
    int names = sscanf(new->name,"%s %s %s", first_name, middle_name, last_name);
    if(names == 2){
        strcpy(last_name,"");
    }
    else if(names == 1){
        strcpy(middle_name,"");
        strcpy(last_name,"");
    }
    int fhashvalue = namehash(first_name);
    new->fhashvalue = fhashvalue;
    int mhashvalue = namehash(middle_name);
    new->mhashvalue = mhashvalue;
    int lhashvalue = namehash(last_name);
    new->lhashvalue = lhashvalue;
    if (firstNameTable[fhashvalue] == NULL)
    {
        firstNameTable[fhashvalue] = new;
    }
    else
    {
        new->fnext = firstNameTable[fhashvalue];
        new->fnext->fprev = new->fnext;
        firstNameTable[fhashvalue] = new;
    }

    if (middleNameTable[mhashvalue] == NULL)
    {
        middleNameTable[mhashvalue] = new;
    }
    else
    {
        new->mnext = middleNameTable[mhashvalue];
        new->mnext->mprev = new->mnext;
        middleNameTable[mhashvalue] = new;
    }

    if (lastNameTable[lhashvalue] == NULL)
    {
        lastNameTable[lhashvalue] = new;
    }
    else
    {
        new->lnext = lastNameTable[lhashvalue];
        new->lnext->lprev = new->lnext;
        lastNameTable[lhashvalue] = new;
    }
    int nhashvalue = numberhash(new->number);
    if (numberTable[nhashvalue] == NULL)
    {
        numberTable[nhashvalue] = new;
    }
    else
    {
        new->nnext = numberTable[nhashvalue];
        numberTable[nhashvalue] = new;
    }
}
void add(char* file){
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        printf("there is no such file\n");
        return ;
    }
    char line[NAMELENGTH + NUMBERLENGTH];
    char name_buffer[NAMELENGTH];
    char number_buffer[NUMBERLENGTH];
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        sscanf(line, "%29[^,],%9s", name_buffer, number_buffer);
        add_contact(name_buffer,number_buffer);
    }
}
//./name filename add_file
float main(int argc, char* argv[]){
    SET = (hashset*)malloc(sizeof(hashset));
    QUEUE = (Queue*)malloc(sizeof(Queue));
    initSET();
    initQueue();
    load(argv[1]);
    clock_t start, end;
    start = clock();
    add(argv[2]);
    end = clock();
    double time = (double)(end - start) / ((double)CLOCKS_PER_SEC);
    printf("%f",time);
    return time;
}