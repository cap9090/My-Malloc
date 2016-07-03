
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define int_size sizeof(int)
#define HEAP_SIZE 5000
#define TRUE 1
#define FALSE 0



struct footer {                                 
    unsigned int size:29;
    unsigned int unused :2;
    unsigned int is_allocated :1;
      
};


 struct header {
    unsigned int size:29;
    unsigned int unused :2;
    unsigned int is_allocated :1;
    
};

#define HEADER_SIZE sizeof(struct header)


        /*Function Declarations*/
void *my_malloc(size_t size);
void *my_malloc_r(struct header *, size_t size);
void my_free(void *p);
void my_free_r(struct header *, struct header *);
int initialize_memory();
void make_block(struct header *, size_t );
struct header *next_header(struct header*);
struct header *next_footer(struct header*);
struct header *previous_header(struct header*);
struct header *previous_footer(struct header*);


        /*Global Variables*/
void *heap_start = NULL;
int intialized = FALSE;


int main(int argc, char** argv) 
{   
    initialize_memory();
    int i = 0;
    int test = HEADER_SIZE;
    
    char *sentence = "Here is a sentence that I will store in  p once I have"
    "allocated memory for it. Then I will free p and allocate its memory to s. " 
    "Then s should contain this sentence since it has been allocated p's memory." 
    "Furthermore, I will allocate enough memory for p to hold this but not for " 
    "s, so when I allocate memory for t, it should overwrite the second part of"
    " this message. Also, t will contain the overflow from p and s.";
    
    printf("Sentence: %s", sentence);
//    char *o = (char*)my_malloc(10);
    char *p = (char*)my_malloc(500);
    char *q = (char*)my_malloc(500);
    
    for(i = 0; i < strlen(sentence); i++)
    {
        p[i] = sentence[i];
    }
    
    printf("\n\np is : %s\n\n", p);
    
    printf("location of heap_start %p\n", heap_start);

    printf("location of p %p\n",p );
    printf("location of q %p\n",q );
    my_free((void*)p);
    
    if((p=my_malloc(0)) == NULL)
        my_free(p);
    
    if(p==NULL){
        printf("\nhandles zero input\n");
    }
    
    if((p=my_malloc(1000000)) == NULL)
        my_free(p);
    
    if(p==NULL){
        if(errno==ENOMEM){
            printf("handles error condition\n");
        }
    }
    
    printf("location of heap_start %p\n", heap_start);
    printf("location of p %p\n",p );
    printf("location of q %p\n",q );
    
    my_free((void*)q);
    my_free((void*)p);
    
    printf("location of heap_start %p\n", heap_start);
    printf("location of p %p\n",p );
    printf("location of q %p\n",q );
    
    

    p = (char*)my_malloc(600);
    q = (char*)my_malloc(500);


    printf("location of heap_start %p\n", heap_start);
    printf("\n\nlocation of p %p\n",p );
    printf("location of q %p\n",q );

    my_free((void*)q);
    my_free((void*)p);
    
    return (EXIT_SUCCESS);
}

int initialize_memory(){
    heap_start = malloc(HEAP_SIZE);
    return 1;
}


void my_free(void * p){
    
    if (p == NULL) 
        return;

    
    struct header *temp = (struct header *)p;
    temp->is_allocated = FALSE;

    
}

void my_free_r(struct header * temp, struct header * next){
    temp->size += next->size;
    struct header * nextnext = next_header(next);
    if(nextnext >= heap_start + HEAP_SIZE)
        return;
    
    if(nextnext->is_allocated == TRUE){
        return;
    }
    else
    {
        my_free_r(temp, nextnext);
    }
}

void * my_malloc(size_t size){
    
    
    /* next two if statements handle error conditions*/
    if(size == 0){
        
        return NULL;
    }
    
    if(size > HEAP_SIZE){
        errno = ENOMEM;
        return NULL;
    }
    /* when we return null we get a pointer to 0x0*/
    
    
    struct header * current = heap_start;
    //if there is no data in here already, then it is our first call to malloc
    if (intialized == FALSE){
        make_block(current, size);
        intialized = TRUE;
        return current + 1;
    }
    
    else{
        if(size <= current->size - 2 * HEADER_SIZE && current->is_allocated == FALSE){
            make_block(current, size);
            return current + 1;
        }
        else
            return my_malloc_r(current, size);
    }
}

void * my_malloc_r(struct header * current, size_t size){
    
    current = next_header(current);
    if((void *)current >= heap_start + 2 *HEAP_SIZE){
        errno = ENOMEM;
        return NULL;
    }
    //!!put a print statemnt to see how many times the recursion gets called
    if (current->size == 0){
        make_block(current, size);
        return current + 1;
    }
    
    if(size <= current->size - 2* HEADER_SIZE && current->is_allocated == FALSE){
        make_block(current, size);
        return current;
    }
    
    else
        return my_malloc_r(current, size);
}

void make_block(struct header * current, size_t size){
    current -> size = size + 2 *HEADER_SIZE;
    current -> is_allocated = 1;
}

struct header * next_header (struct header * current){
    
    char * address = (char *)current;
    address += current-> size;
    return (struct header *)address;
    
}