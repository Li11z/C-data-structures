#include <stdio.h>
#include <stdlib.h>

/*
 struct Node -> data; next; prev;
 if head: prev=null;
 if tail: next=null;

 linked list manager: keep track of:
 -> size
 -> address of: head, tail
 -> finger node caching: (Node, idx) of last used node -> this is set to null immidialty when validity becomes questionable it is not a variable to be maintianed otherwise itll go back to costing O(n)


functions to add:

setup(list name):

push(list):
add a new node at the head of the list

offer(list):
add a new node at the tail of the list

add(list, idx, data):
add a new node at the specified index of the list

pop(list):
remove last node from list , also free that specific node

pull(list)
remove first node from list , also free that specific node

remove(list, idx, data)
remove a specified node , also free that specific node

ll_free(list)
frees list and remove all nodes

ll_erase(list)
delete all nodes and freeing them but we dont free the actual list

more functions to come
*/

#define PRINT(x) _Generic((x),              \
    int:      printf("%d\n",  (x)),         \
    long:     printf("%ld\n", (x)),         \
    unsigned: printf("%u\n",  (x)),         \
    double:   printf("%f\n",  (x)),         \
    float:    printf("%f\n",  (x)),         \
    char:     printf("%c\n",  (x)),         \
    char*:    printf("%s\n",  (x)),         \
    uint8_t:   printf("%u\n",  (x)),        \
    size_t:     printf("%zu\n",  (x)),     \
    default:  printf("?\n")                 \
)

#define dtype int

struct Node {
    dtype data;

    struct Node *next;
    struct Node *prev;
};

struct LinkedList {
    struct Node *head;
    struct Node *tail;

    size_t size;

    struct Node *finger;
    size_t finger_idx;

    bool is_freed;
};

void setup(struct LinkedList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->finger = NULL;
    list->finger_idx = 0;

    list->is_freed = false;
}

//adding node at head
void push(struct LinkedList *list, dtype x) {
    if (list->is_freed == true) {
        printf("\n\nError list already freed, no longer exists!!!\n\n");
        exit(-1);
    }
    if (list->size == 0) {
        struct Node node;
        node.data = x;
        node.next = NULL;
        node.prev = NULL;

        list->head = &node;
        list->tail = &node;
        list->size++;
    }
    else {
        struct Node node;
        node.data = x;
        node.prev = NULL; //can be changed to tail if we ever want to make a circular doubly linked list
        node.next = list->head;

        list->head->prev = &node;
        list->head = &node;
        list->size++;
    }
}

int main() {

    //setting up the list:
    struct LinkedList list;
    setup(&list);



}
