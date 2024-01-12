#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#define QUEUE_SIZE 1000000
#define LETTERS 5
#define DIGITS 8
#define NAMELENGTH 40
#define NUMBERLENGTH 11
#define N 26 * 26 * 26 * 26 * 26 + 1
#define M 10 * 10 * 10 * 10 * 10 * 10 * 10 * 10
#define MAX_RESULT 1000000
#define MAX_SIZE 1000000
int ID = 0;

typedef struct hashset
{
    int table[MAX_RESULT];
} hashset;

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

typedef struct Queue
{
    int data[MAX_SIZE];
    int front;
    int rear;
} Queue;

Node *firstNameTable[N];
Node *middleNameTable[N];
Node *lastNameTable[N];
Node *numberTable[M];
Node *UIDtable[MAX_SIZE];
hashset *SET;
Queue *QUEUE;

// prototype of all the functions
void deleteByNumber(char *number, int flag);
void deleteByName(char *name, int flag);

// functions related to SET
void insert(int uid, hashset *set)
{
    int index = uid % MAX_SIZE;
    set->table[index] = uid;
}
void initSET()
{
    for (int i = 0; i < MAX_RESULT; i++)
    {
        SET->table[i] = -1;
    }
}

// functions related to queue
void initQueue()
{
    QUEUE->front = -1;
    QUEUE->rear = -1;
}

bool isEmpty(Queue *queue)
{
    return queue->front == -1;
}

bool isFull(Queue *queue)
{
    return (queue->rear + 1) % QUEUE_SIZE == queue->front;
}

void enqueue(Queue *queue, int value)
{
    if (isFull(queue))
    {
        printf("Queue is full. Cannot enqueue.\n");
        return;
    }

    if (isEmpty(queue))
    {
        queue->front = 0;
    }

    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
    queue->data[queue->rear] = value;
}

int dequeue(Queue *queue)
{
    if (isEmpty(queue))
    {
        printf("Queue is empty. Cannot dequeue.\n");
        return -1;
    }

    int value = queue->data[queue->front];

    if (queue->front == queue->rear)
    {
        queue->front = queue->rear = -1;
    }
    else
    {
        queue->front = (queue->front + 1) % QUEUE_SIZE;
    }

    return value;
}

// functions related to hashing
char *removeSpaces(char *input)
{
    int input_length = strlen(input);
    char *output = (char *)malloc(input_length + 1);
    int j = 0;

    for (int i = 0; i < input_length; i++)
    {
        if (input[i] != ' ')
        {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
    return output;
}
char *removeEscape(char *input)
{
    int input_length = strlen(input);
    char *output = (char *)malloc(input_length + 1);
    int j = 0;
    for (int i = 0; i < input_length; i++)
    {
        if (input[i] != '\n')
        {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
    return output;
}
int namehash(char *input)
{
    char *word = removeSpaces(input);
    int h = 0;
    int len = strlen(word);
    if (len == 0)
    {
        return N - 1;
    }
    if (len > LETTERS)
    {
        len = LETTERS;
    }

    for (int i = 0; i < len; i++)
    {
        h = h + pow(26, LETTERS - i - 1) * (toupper(word[i]) - 'A');
    }

    return h;
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
        h = h + pow(10, DIGITS - i - 1) * (toupper(number[i]) - '0');
    }

    return h;
}
int load(void)
{
    for (int i = 0; i < N; i++)
    {
        firstNameTable[i] = NULL;
        middleNameTable[i] = NULL;
        lastNameTable[i] = NULL;
    }
    FILE *fp = fopen("contacts.csv", "r");
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
    int c = 1;
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
        // check for any reusable id in queue.
        if (isEmpty(QUEUE))
        {
            new->uid = ID;
            UIDtable[new->uid] = new;
            ID++;
        }
        else
        {
            new->uid = dequeue(QUEUE);
        }

        int names = sscanf(name_buffer, "%s %s %s", first_name, middle_name, last_name);
        if (names == 2)
        {
            strcpy(last_name, "");
        }
        else if (names == 1)
        {
            strcpy(middle_name, "");
            strcpy(last_name, "");
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
        c++;
    }
    fclose(fp);
    return 0;
}
int numberSearch(char *number)
{
    clock_t start, end;
    double cpu_time_used;
    const double CLOCKS_PER_MILLISEC = CLOCKS_PER_SEC / 1000000.0;
    int digits = strlen(number);
    char filename[20];
    sprintf(filename, "%s-result.csv", number);
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        perror("ERROR!!!");
        return 1;
    }
    start = clock();
    int hashvalue = numberhash(number);
    if (digits >= DIGITS)
    {
        Node *curr = numberTable[hashvalue];
        while (curr != NULL)
        {
            fprintf(fp, "%s, %s\n", curr->name, curr->number);
            curr = curr->nnext;
        }
    }
    else
    {
        int current_index = hashvalue;
        int limit = pow(10, DIGITS - digits);
        do
        {
            Node *curr = numberTable[current_index];
            while (curr != NULL)
            {
                fprintf(fp, "%s, %s\n", curr->name, curr->number);
                curr = curr->nnext;
            }
            current_index++;
        } while ((current_index) % limit != 0);
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_MILLISEC;
    fprintf(fp, "Time taken: %lf milliseconds", cpu_time_used);
    fclose(fp);
    return 0;
}
int nameSearch(char *name)
{
    clock_t start, end;
    double cpu_time_used;
    const double CLOCKS_PER_MILLISEC = CLOCKS_PER_SEC / 1000000.0;
    int letters = strlen(name);
    char filename[50]; // Adjust the size according to your needs
    sprintf(filename, "%s-result.csv", name);
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        perror("ERROR!!!");
        return 1;
    }
    start = clock();
    int hashvalue = namehash(name);

    if (letters >= LETTERS)
    {
        Node *curr = firstNameTable[hashvalue];
        while (curr != NULL)
        {
            insert(curr->uid, SET);
            curr = curr->fnext;
        }
    }
    else
    {
        int current_index = hashvalue;
        int limit = pow(26, LETTERS - letters);
        do
        {
            Node *curr = firstNameTable[current_index];
            while (curr != NULL)
            {
                insert(curr->uid, SET);
                curr = curr->fnext;
            }
            current_index++;
        } while ((current_index) % limit != 0);
    }

    if (letters >= LETTERS)
    {
        Node *curr = middleNameTable[hashvalue];
        while (curr != NULL)
        {
            insert(curr->uid, SET);
            curr = curr->mnext;
        }
    }
    else
    {
        int current_index = hashvalue;
        int limit = pow(26, LETTERS - letters);
        do
        {
            Node *curr = middleNameTable[current_index];
            while (curr != NULL)
            {
                insert(curr->uid, SET);
                curr = curr->mnext;
            }
            current_index++;
        } while ((current_index) % limit != 0);
    }

    if (letters >= LETTERS)
    {
        Node *curr = lastNameTable[hashvalue];
        while (curr != NULL)
        {
            insert(curr->uid, SET);
            curr = curr->lnext;
        }
    }
    else
    {
        int current_index = hashvalue;
        int limit = pow(26, LETTERS - letters);
        do
        {
            Node *curr = lastNameTable[current_index];
            while (curr != NULL)
            {
                insert(curr->uid, SET);
                curr = curr->lnext;
            }
            current_index++;
        } while ((current_index) % limit != 0);
    }
    end = clock();
    cpu_time_used = (double)(end - start) / CLOCKS_PER_MILLISEC;
    for (int i = 0; i < MAX_RESULT; i++)
    {
        if (SET->table[i] != -1)
        {
            Node *node = UIDtable[SET->table[i]];
            fprintf(fp, "%s, %s\n", node->name, node->number);
        }
    }
    initSET();
    fprintf(fp, "Time taken: %lf microseconds", cpu_time_used);
    fclose(fp);
    return 0;
}
// if the function is called directly then flag = 0 else flag = 1
void deleteByName(char *name, int flag)
{
    name = removeEscape(name);
    Node *del = NULL;
    char number[NUMBERLENGTH];
    int hashvalue = namehash(name);
    Node *curr = firstNameTable[hashvalue];
    if (curr == NULL)
    {
        printf("Contact not found\n");
        return;
    }
    else if (strcmp(curr->name, name) == 0)
    {
        strcpy(number, curr->number);

        // deleting from first name table
        if (curr->fnext != NULL)
        {
            curr->fnext->fprev = curr->fprev;
        }
        firstNameTable[hashvalue] = curr->fnext;

        // deleting from middle name table
        if (curr->mnext != NULL)
        {
            curr->mnext->mprev = curr->mprev;
        }
        if (curr->mprev == NULL)
        {
            middleNameTable[curr->mhashvalue] = curr->mnext;
        }
        else
        {
            curr->mprev->mnext = curr->mnext;
        }

        // deleting from last name table
        if (curr->lnext != NULL)
        {
            curr->lnext->lprev = curr->lprev;
        }
        if (curr->lprev == NULL)
        {
            lastNameTable[curr->lhashvalue] = curr->lnext;
        }
        else
        {
            curr->lprev->lnext = curr->lnext;
        }

        // removing the entry from UID table and adding that id to the queue
        UIDtable[curr->uid] = NULL;
        enqueue(QUEUE, curr->uid);
        del = curr;
        curr = NULL;
    }
    while (curr != NULL && curr->fnext != NULL)
    {
        if (strcmp(curr->fnext->name, name) == 0)
        {
            // since we know that the next node is to be deleted we set curr to be that node
            curr = curr->fnext;

            strcpy(number, curr->number);

            // deleting from middle name table
            if (curr->mnext != NULL)
            {
                curr->mnext->mprev = curr->mprev;
            }
            if (curr->mprev == NULL)
            {
                middleNameTable[curr->mhashvalue] = curr->mnext;
            }
            else
            {
                curr->mprev->mnext = curr->mnext;
            }

            // deleting from last name table
            if (curr->lnext != NULL)
            {
                curr->lnext->lprev = curr->lprev;
            }
            if (curr->lprev == NULL)
            {
                lastNameTable[curr->lhashvalue] = curr->lnext;
            }
            else
            {
                curr->lprev->lnext = curr->lnext;
            }

            // deleting from the first name table
            if (curr->fnext != NULL)
            {
                curr->fnext->fprev = curr->fprev;
            }
            curr->fprev->fnext = curr->fnext;

            // removing the entry from UID table and adding that id to the queue
            UIDtable[curr->uid] = NULL;
            enqueue(QUEUE, curr->uid);
            del = curr;
            break;
        }
        curr = curr->fnext;
    }

    if (flag == 0)
    {
        deleteByNumber(number, 1);
    }
    else
    {
        free(del);
    }
    return;
}

void deleteByNumber(char *number, int flag)
{
    number = removeEscape(number);
    char name[NAMELENGTH];
    int hashvalue = numberhash(number);
    Node *del = NULL;
    Node *curr = numberTable[hashvalue];
    if (curr == NULL)
    {
        printf("Contact not found\n");
        return;
    }
    else if (strcmp(curr->number, number) == 0)
    {
        strcpy(name, curr->name);
        numberTable[hashvalue] = curr->nnext;
        del = curr;
        curr = NULL;
    }
    while (curr != NULL && curr->nnext != NULL)
    {
        if (strcmp(curr->nnext->number, number) == 0)
        {
            Node *temp = curr->nnext;
            strcpy(name, curr->nnext->name);
            curr->nnext = curr->nnext->nnext;
            del = temp;
            break;
        }
        curr = curr->nnext;
    }
    if (flag == 0)
    {
        deleteByName(name, 1);
    }
    else
    {
        free(del);
    }
    return;
}
void freeTables()
{
    FILE *fp = fopen("contacts.csv", "w");
    for (int i = 0; i < M; i++)
    {
        Node *curr = numberTable[i];
        while (curr != NULL)
        {
            fprintf(fp, "%s, %s\n", curr->name, curr->number);
            curr = curr->nnext;
        }
    }
    for (int i = 0; i < M; i++)
    {
        Node *curr = numberTable[i];
        while (curr != NULL)
        {
            Node *temp = curr->nnext;
            free(curr);
            curr = temp;
        }
    }
}
void add_contact(char *name, char *number)
{
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
    // check for any reusable id in queue.
    if (isEmpty(QUEUE))
    {
        new->uid = ID;
        UIDtable[new->uid] = new;
        ID++;
    }
    else
    {
        new->uid = dequeue(QUEUE);
    }
    int names = sscanf(new->name, "%s %s %s", first_name, middle_name, last_name);
    if (names == 2)
    {
        strcpy(last_name, "");
    }
    else if (names == 1)
    {
        strcpy(middle_name, "");
        strcpy(last_name, "");
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

// user iteractive functions
void new_contacts()
{
    while (1)
    {
        int choice;
        printf("how would you like to add new contacts\n");
        printf("1.add manually\n");
        printf("2.batch\n");
        printf("3.exit\n");
        printf("choice: ");
        scanf("%d", &choice);
        if (choice == 1)
        {
            char *name = (char *)malloc(NAMELENGTH);
            char *number = (char *)malloc(NUMBERLENGTH);
            printf("enter the name:");
            scanf("%s", name);
            printf("\n");
            printf("enter the number: ");
            scanf("%s", number);
            add_contact(name, number);
            printf("your contacts has beed added\n");
        }
        else if (choice == 2)
        {
            char file[15];
            printf("enter the name of the file: ");
            scanf("%s", file);
            printf("\n");
            FILE *fp = fopen(file, "r");
            if (fp == NULL)
            {
                printf("there is no such file\n");
                return;
            }
            char line[NAMELENGTH + NUMBERLENGTH];
            char name_buffer[NAMELENGTH];
            char number_buffer[NUMBERLENGTH];
            while (fgets(line, sizeof(line), fp) != NULL)
            {
                sscanf(line, "%29[^,],%9s", name_buffer, number_buffer);
                add_contact(name_buffer, number_buffer);
            }
            printf("Contacts in file are added to contacts.csv\n");
        }
        else if (choice == 3)
        {
            return;
        }
    }
}
void delete()
{
    while (1)
    {
        int choice;
        printf("how would you like to delete\n");
        printf("1.By name\n");
        printf("2.By number\n");
        printf("3.batch\n");
        printf("4.exit");
        printf("choice: ");
        scanf("%d", &choice);
        if (choice == 1)
        {
            char name[NAMELENGTH];
            printf("Enter the name: ");
            scanf(" %[^\n]", name);
            deleteByName(name, 0);
        }
        else if (choice == 2)
        {
            char number[NUMBERLENGTH];
            printf("Enter the number: ");
            scanf(" %[^\n]", number);
            deleteByNumber(number, 0);
        }
        else if (choice == 3)
        {
            char file[15];
            printf("enter the name of the file: ");
            scanf("%s", file);
            printf("\n");
            FILE *fp = fopen(file, "r");
            if (fp == NULL)
            {
                printf("there is no such file\n");
                return;
            }
            char buffer[NAMELENGTH];
            while (fgets(buffer, sizeof(buffer), fp) != NULL)
            {
                if (isdigit(buffer[0]))
                {
                    deleteByNumber(buffer, 0);
                }
                else
                {
                    deleteByName(buffer, 0);
                }
            }
            printf("Contacts in file are deleted from contacts.csv\n");
        }
        else if (choice == 4)
        {
            return;
        }
    }
}
void edit()
{
    char name[NAMELENGTH];
    char new_name[NAMELENGTH];
    char new_number[NUMBERLENGTH];
    printf("enter the name of the contact you want to edit: ");
    scanf(" %[^\n]", name);
    deleteByName(name, 0);
    printf("Enter the new name: ");
    scanf(" %[^\n]", new_name);
    printf("\n");
    printf("Enter the new number: ");
    scanf(" %[^\n]", new_number);
    printf("\n");
    add_contact(new_name, new_number);
    printf("contact editted\n");
}
void search()
{
    int choice;
    printf("1.search by name\n");
    printf("2.search by number\n");
    printf("choice: ");
    scanf("%d", &choice);
    if (choice == 1)
    {
        char name[NAMELENGTH];
        printf("enter your name: ");
        scanf("%s", name);
        nameSearch(name);
    }
    else if (choice == 2)
    {
        char number[NUMBERLENGTH];
        printf("enter you number: ");
        scanf("%s", number);
        numberSearch(number);
    }
    printf("check result.csv\n");
    return;
}
int main()
{
    SET = (hashset *)malloc(sizeof(hashset));
    QUEUE = (Queue *)malloc(sizeof(Queue));
    initSET();
    initQueue();

    clock_t start, end;
    start = clock();
    load();
    end = clock();
    double time = (double)(end - start) / ((double)CLOCKS_PER_SEC);
    printf("time to load tables: %lf seconds\n", time);

    while (1)
    {
        int choice;
        printf("enter a number.\n");
        printf("1.Create new contacts\n");
        printf("2.delete contact\n");
        printf("3.Edit contact\n");
        printf("4.search contact\n");
        printf("5.exit\n");
        printf("choice: ");
        scanf("%d", &choice);
        printf("\n");
        switch (choice)
        {
        case 1:
            new_contacts();
            break;
        case 2:
            delete ();
            break;
        case 3:
            edit();
            break;
        case 4:
            search();
            break;
        case 5:
            start = clock();
            freeTables();
            end = clock();
            double time2 = (double)(end - start) / ((double)CLOCKS_PER_SEC);
            printf("time to load tables: %lf seconds\n", time2);
            return 0;
        default:
            printf("please enter a valid a choice\n");
            break;
        }
    }
}