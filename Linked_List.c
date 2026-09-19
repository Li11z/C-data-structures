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


struct Node *get_node_private(struct LinkedList *list, int idx) {
    //private is essentially normal get_node without guard rails, since this will be a function called by other function who already have guard rails
    // this way we dont repeat the same functions over and over again useleslly

    size_t starting_point = 0;
    size_t closer_distance = 0;
    struct Node *node_buffer;

    if (list->size - idx >= idx + 1) {
        // logic is (list->size - 1) - idx +1
        starting_point = 0;
        closer_distance = idx + 1;
    }

    else if (list->size - idx < idx + 1) {
        starting_point = 1;
        closer_distance = list->size - idx;
    }

    if (abs(list->finger_idx - idx) + 1 < closer_distance && list->finger != NULL) {
        starting_point = 2;
        closer_distance = abs(list->finger_idx - idx) + 1;
    }


    if (starting_point == 0) {
        node_buffer = list->head;
        for (int i=0; i<idx; i++) {
            node_buffer = node_buffer->next;
        }
    }
    else if (starting_point == 1) {
        node_buffer = list->tail;
        for (int i=(int)(list->size)-1; i>idx; i--) {
            node_buffer = node_buffer->prev;
        }
    }
    else{
        if (list->finger_idx > idx) {
            node_buffer = list->finger;
            for (int i=(int)(list->finger_idx); i>idx; i--) {
                node_buffer = node_buffer->prev;
            }
        }
        else {
            node_buffer = list->finger;
            for (int i=(int)(list->finger_idx); i<idx; i++) {
                node_buffer = node_buffer->next;
            }
        }
    }

    return node_buffer;
}


struct Node *get_node(struct LinkedList *list, int idx) {
    if (list->is_freed == true) {
        printf("\n\nError list already freed, no longer exists!!!\n\n");
        exit(-1);
    }

    if (idx < 0) {
        printf("\n\nError: Index Out Of Bounds!!!\n\n");
        exit(-1);
    }
    if (idx > list->size) {
        printf("\n\nError: Index Out Of Bounds!!!\n\n");
        exit(-1);
    }

    if (list->size == 0 && idx != 0) {
        printf("\n\nError: Index Out Of Bounds!!!\n\n");
        exit(-1);
    }
    else if(list->size == 0 && idx == 0) {
        printf("Warning: There is nothing");
    }

    if (idx == 0) {
        return list->head;
    }
    else if (idx == list->size) {
        return list->tail;
    }
    else if (list->finger_idx == idx && list->finger != NULL) {
        return list->finger;
    }


    size_t starting_point = 0;
    size_t closer_distance = 0;
    struct Node *node_buffer;

    if (list->size - idx >= idx + 1) {
        // logic is (list->size - 1) - idx +1
        starting_point = 0;
        closer_distance = idx + 1;
    }

    else if (list->size - idx < idx + 1) {
        starting_point = 1;
        closer_distance = list->size - idx;
    }

    if (abs(list->finger_idx - idx) + 1 < closer_distance && list->finger != NULL) {
        starting_point = 2;
        closer_distance = abs(list->finger_idx - idx) + 1;
    }


    if (starting_point == 0) {
        node_buffer = list->head;
        for (int i=0; i<idx; i++) {
            node_buffer = node_buffer->next;
        }
    }
    else if (starting_point == 1) {
        node_buffer = list->tail;
        for (int i=(int)(list->size)-1; i>idx; i--) {
            node_buffer = node_buffer->prev;
        }
    }
    else{
        if (list->finger_idx > idx) {
            node_buffer = list->finger;
            for (int i=(int)(list->finger_idx); i>idx; i--) {
                node_buffer = node_buffer->prev;
            }
        }
        else {
            node_buffer = list->finger;
            for (int i=(int)(list->finger_idx); i<idx; i++) {
                node_buffer = node_buffer->next;
            }
        }
    }

    return node_buffer;
}



//adding node at head
void push(struct LinkedList *list, dtype x) {
    if (list->is_freed == true) {
        printf("\n\nError list already freed, no longer exists!!!\n\n");
        exit(-1);
    }
    if (list->size == 0) {
        struct Node *node = malloc(sizeof *node);
        node->data = x;
        node->next = NULL;
        node->prev = NULL;

        list->head = node;
        list->tail = node;
        list->size++;
    } else {
        struct Node *node = malloc(sizeof *node);
        node->data = x;
        node->prev = NULL; //can be changed to tail if we ever want to make a circular doubly linked list
        node->next = list->head;

        list->head->prev = node;
        list->head = node;
        list->size++;
    }

    list->finger_idx++;
}

//adding node at tail
void offer(struct LinkedList *list, dtype x) {
    if (list->is_freed == true) {
        printf("\n\nError list already freed, no longer exists!!!\n\n");
        exit(-1);
    }
    if (list->size == 0) {
        struct Node *node = malloc(sizeof *node);
        node->data = x;
        node->next = NULL;
        node->prev = NULL;

        list->head = node;
        list->tail = node;
        list->size++;
    }
    else {
        struct Node *node = malloc(sizeof *node);
        node->data = x;
        node->next = NULL; //can be changed to tail if we ever want to make a circular doubly linked list
        node->prev = list->tail;

        list->tail->next = node;
        list->tail = node;
        list->size++;
    }
}

void add(struct LinkedList *list, int idx,dtype x) {
    if (list->is_freed == true) {
        printf("\n\nError list already freed, no longer exists!!!\n\n");
        exit(-1);
    }
    if (idx < 0) {
        printf("\n\nError: Index Out Of Bounds!!!\n\n");
        exit(-1);
    }
    if (idx > list->size) {
        printf("\n\nError: Index Out Of Bounds!!!\n\n");
        exit(-1);
    }

    if (list->size == 0 && idx != 0) {
        printf("\n\nError: Index Out Of Bounds!!!\n\n");
        exit(-1);
    }
    else if(list->size == 0 && idx == 0){
        struct Node *node = malloc(sizeof *node);
        node->data = x;
        node->next = NULL;
        node->prev = NULL;

        list->head = node;
        list->tail = node;
        list->size++;

        return;
    }

    if (idx == 0) {
        push(list, x);
    }
    else if (idx == list->size) {
        offer(list, x);
    }
    else if (list->finger_idx == idx && list->finger != NULL) {
        struct Node *node = malloc(sizeof *node);
        node->data = x;
        node->next = list->finger;
        node->prev = list->finger->prev;

        list->finger->prev->next = node;
        list->finger->prev = node;

        list->size++;

        list->finger_idx = idx;
        list->finger = node;

        return;
    }

    else {
        //idx not head or tail or finger

        struct Node *node_buffer = get_node_private(list, idx);

        struct Node *node=malloc(sizeof(struct Node));

        node->data = x;
        node->next = node_buffer;
        node->prev = node_buffer->prev;
        node_buffer->prev->next = node;
        node_buffer->prev = node;
        list->size++;

        list->finger_idx = idx;
        list->finger = node;
    }
}


int main() {
    //setting up the list:
    struct LinkedList list;
    setup(&list);

    //pushing values
    push(&list, 1);
    printf("\n %d", list.head->data);
    push(&list, 2);
    printf("\n %d", list.head->data);
    push(&list, 3);
    printf("\n %d", list.head->data);
    printf("\n %d", list.tail->data);

    add(&list, 2, 4);
    printf("\n %d", list.tail->data);
    printf("\n %d", list.tail->prev->data);

}
