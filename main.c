#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_NAME 50+1

typedef struct person {
    char name[MAX_NAME];
    char surname[MAX_NAME];
    char sex;
    int age;
    struct person *father;
    struct person *mother;
    int n_child;
    struct person **child;
} person_t;

typedef struct graph{
    int n_persons;
    struct person **person;
} graph_t;

graph_t *readFile(FILE *,graph_t *);
person_t *search_p(graph_t *,char *, char *);
void gender_path(person_t *, int *, int *, char, person_t **, person_t **, int, int);
void gender_eval(graph_t *, person_t **, person_t **, int);
void age_path(person_t *, int *, int *, person_t **, person_t **, int, int);
void age_eval(graph_t *GRAPH,person_t **, person_t **, int);
person_t **initpath(int);
void cleanpath(person_t **, int);
void print_path(person_t **);
void free_graph(graph_t *);

int main(int argc, const char * argv[])
{
    graph_t *GRAPH=NULL;
    person_t **currentpath;
    person_t **bestpath;
    int n;
    FILE *fp;

    if(argc!=2){
        printf("Wrong input. Exiting...\n");
        exit(1);}

    if((fp=fopen(argv[1], "r"))==NULL){
        printf("Error opening the input file. Exiting...\n");
        exit(1);};

    if((GRAPH=(graph_t*)malloc(sizeof(graph_t)))==NULL){
        printf("Error allocating the graph structure. Exiting...\n");
        exit(1);}

    /*Assuming input file format is correct*/
    GRAPH=readFile(fp, GRAPH);
    fclose(fp);

    n=GRAPH->n_persons;

    currentpath=initpath(n);
    bestpath=initpath(n);

    gender_eval(GRAPH, currentpath, bestpath, n);
    printf("->Gender path:\n");
    print_path(bestpath);

    age_eval(GRAPH, currentpath, bestpath, n);
    printf("\n->Age path:\n");
    print_path(bestpath);

    free(bestpath);
    free(currentpath);
    free_graph(GRAPH);
    free(GRAPH);

    return 0;
}

graph_t *readFile(FILE *fp, graph_t *GRAPH)
{
    int n_p,i,age;
    char sex,name[MAX_NAME],surname[MAX_NAME],f_name[MAX_NAME],f_surname[MAX_NAME],m_name[MAX_NAME],m_surname[MAX_NAME];
    person_t *curr=NULL, *parent;

    fscanf(fp, "%d", &n_p);
    GRAPH->n_persons=n_p;

    if((GRAPH->person=(person_t **)malloc(n_p*sizeof(person_t *)))==NULL){
       printf("Error allocating memory for the initial set of pointers from graph to persons. Exiting...\n");
        exit(1);}

    for (i=0; i<n_p; i++) {
        if((GRAPH->person[i]=(person_t *)malloc(sizeof(person_t)))==NULL) {
            printf("Error allocating memory for a person. Exiting...\n");
            exit(1);}
    }

    //FIRST CYCLE: Loading persons to memory
    for(i=0;i<n_p;i++){
        fscanf(fp, "%s %s %c %d",name,surname,&sex,&age);
        strcpy(GRAPH->person[i]->name,name);
        strcpy(GRAPH->person[i]->surname,surname);
        GRAPH->person[i]->sex=sex;
        GRAPH->person[i]->age=age;
        GRAPH->person[i]->n_child=0;
        GRAPH->person[i]->father=GRAPH->person[i]->mother=NULL;
        //The memory reserved for pointers to children is initially set to 0 when loading persons in the graph
        if((GRAPH->person[i]->child=(person_t**)malloc(0*sizeof(person_t*)))==NULL){
            printf("Error allocating memory for children pointers of person: %s %s. Exiting...\n",name,surname);
            exit(1);}
    }

    //SECOND CYCLE: Processing parents-children connections
    while (fscanf(fp, "%s %s %s %s %s %s",name,surname,f_name,f_surname,m_name,m_surname)!=EOF) {

        //Son-daughter processing
        if((curr=search_p(GRAPH, name, surname))==NULL){
            printf("Unknown person encountered scanning children-parent relations list. Exiting...\n");
            exit(1);}

        curr->mother=search_p(GRAPH, m_name, m_surname);
        curr->father=search_p(GRAPH, f_name, f_surname);

        if(curr->mother==NULL || curr->father==NULL){
            printf("Unknown person encountered scanning children-parent relations list. Exiting...\n");
            exit(1);}

        //Mother processing
        parent=curr->mother;
        parent->n_child++;
          //Reallocating memory to accomodate the new pointer to the son-daughter
        if((parent->child=(person_t**)realloc(parent->child, (parent->n_child)*sizeof(person_t*)))==NULL){
            printf("Error expanding memory for children pointers of person: %s %s. Exiting...\n", parent->name, parent->surname);
            exit(1);}
        parent->child[(parent->n_child)-1]=curr;

        //Father processing
        parent=curr->father;
        parent->n_child++;
        if((parent->child=(person_t**)realloc(parent->child, (parent->n_child)*sizeof(person_t*)))==NULL){
            printf("Error expanding memory for children pointers of person: %s %s. Exiting...\n", parent->name, parent->surname);
            exit(1);}
        parent->child[(parent->n_child-1)]=curr;
    }
    return GRAPH;
}

//search_p allows to search a person in the graph. If the person is not found, the function returns NULL.
person_t *search_p(graph_t *GRAPH, char *name, char *surname)
{
    int n,i;

    n=GRAPH->n_persons;

    for (i=0; i<n; i++) {
        if(strcmp(GRAPH->person[i]->name,name)==0 && strcmp(GRAPH->person[i]->surname,surname)==0){
            return GRAPH->person[i];
        }
    }
    return NULL;
}

//initpath initiates an array of pointers (to persons_t) of size equal to the number of persons in the graph.
person_t ** initpath(int size)
{
    person_t **path;

    if((path=(person_t**)malloc(size*sizeof(person_t*)))==NULL){
        printf("Error allocating memory for a path. Exiting...\n");
        exit(1);}

    cleanpath(path, size);

    return path;
}

void cleanpath(person_t **path, int size)
{
    int i=0;

    for (i=0; i<size; i++) {
        path[i]=NULL;
    }
    return;
}

void print_path(person_t **path)
{
    int i=0;

    while (path[i]!=NULL) {
        printf("%s %s\n",path[i]->name, path[i]->surname);
        i++;
    }

    return;
}

/*gender_eval performs recursive visits (gender_path function) of the graph, starting from each person twice:
the first time considering male gender, the second time considering female gender. The operation saves in the
<bestpath> the requested path, in the form of an array of pointers to persons*/
void gender_eval (graph_t *GRAPH, person_t **currentpath, person_t **bestpath, int n_persons)
{
    int i, j, result, max;
    char gender;

    result=0;
    gender='M';

    for (j=0; j<2; j++) {
        for(i=0;i<(n_persons);i++){
            max=0;
            cleanpath(currentpath, n_persons);
            gender_path(GRAPH->person[i], &max, &result, gender, currentpath, bestpath, n_persons, 0);
        }
        gender='F';
    }

    return;
}

/*This recursive function adopts the counter <max> to count the number of gender matches encountered along each visited path.
When a new person is scanned, the pointer to the node is saved in the <currentpath> array. When a person with no children
is reached, if <max> is greater than <result> (initially set to 0), the <currentpath> is "copied" into the <bestpath> array*/
void gender_path(person_t *curr, int *max, int *result, char gender, person_t **currentpath, person_t **bestpath, int n_persons, int step)
{
    int i,j;

    //CASE OF A PERSON WITH NO CHILDREN
    if (curr->n_child==0) {
        if (curr->sex==gender) {
            (*max)++;
        }
        currentpath[step]=curr;

        if (*max>*result) {
            *result=*max;
            /*cleanpath is necessary to avoid incorrect result in the case in which the new <currentpath> to be "copied" is
            shorter than the previous <bestpath>: without setting all pointers of <bestpath> to NULL prior to the copy, the
            resulting array would maintain the last elements of the previous <bestpath> configuration*/
            cleanpath(bestpath,n_persons);
            for (j=0; j<(step+1); j++) {
                bestpath[j]=currentpath[j];
            }
        }

        if (curr->sex==gender) {
            (*max)--; //Bakctrack for <max> (to be executed only if there was a match (so a counter increase) for this person)
        }
        currentpath[step]=NULL; //Backtrack for <currentpath>

        return;
    }

    //CASE OF A PARENT
    if(curr->sex==gender){
        (*max)++;
    }
    currentpath[step]=curr;

    /*In case of a parent, the gender_path function is recursively called for each child, passing an increased <step> value
    for the proper functioning of the <currentpath> tracking array*/
    for (i=0; i<curr->n_child; i++) {
        gender_path(curr->child[i], max, result, gender, currentpath, bestpath, n_persons, step+1);
    }

    if(curr->sex==gender){
        (*max)--; //Backtrack
    }
    currentpath[step]=NULL; //Backtrack

    return;
}

/*age_eval is similar to gender_eval, but in this case the <max> counter keeps track of the sum of the ages
of the persons visited*/
void age_eval (graph_t *GRAPH, person_t **currentpath, person_t **bestpath, int n_persons)
{
    int i, result, max;

    result=0;

    for(i=0;i<(n_persons);i++){
        max=0;
        cleanpath(currentpath, n_persons);
        age_path(GRAPH->person[i], &max, &result, currentpath, bestpath, n_persons, 0);
    }

    return;
}

void age_path(person_t *curr, int *max, int *result, person_t **currentpath, person_t **bestpath, int n_persons, int step)
{
    int i,j;

    //CASE OF A PERSON WITH NO CHILDREN
    if (curr->n_child==0) {
        *max=(*max)+(curr->age);
        currentpath[step]=curr;

        if (*max>*result) {
            *result=*max;
            /*cleanpath needed for the same reason described in gender_path*/
            cleanpath(bestpath, n_persons);
            for (j=0; j<(step+1); j++) {
                bestpath[j]=currentpath[j];
            }
        }

        //Backtrack
        *max=(*max)-(curr->age);
        currentpath[step]=NULL;

        return;
    }

    //CASE OF A PARENT
    *max=(*max)+(curr->age);
    currentpath[step]=curr;

    //age_path is recursively called for each child of the parent (<step>+1)
    for (i=0; i<curr->n_child; i++) {
        age_path(curr->child[i], max, result, currentpath, bestpath, n_persons, step+1);
    }

    //Backtrack
    *max=(*max)-(curr->age);
    currentpath[step]=NULL;

    return;

}

void free_graph(graph_t *GRAPH)
{
    int i;

    for (i=0; i<(GRAPH->n_persons); i++) {
        free(GRAPH->person[i]->child);
        free(GRAPH->person[i]);
    }

    free(GRAPH->person);
    return;
}
