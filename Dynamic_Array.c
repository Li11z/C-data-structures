#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


/*
 *what is a dynamic array:
simply a normal array that grows or shrinks size automatically as we add or remove data from it
what we can do:

###Index functions:

->retrieve data:
    -> using function `get()` we should be able to retireve the value at specific index without altering the array
        e.g [a,b,c], x=get(1) -> x=b

    -> use `size()` to return how many slots we have
        e.g [a,b,c] size(array)=3
    -> use `capacity()` to return how much memeory is allocated right now
        e.g [a,b,c] , capacity(arr)= 6, since 6bytes are allocated and kept for this array

    -> isfull() check is all slots are being used
    e.g[a,b,c] has 3 slots so a isfull() check return true , howver [a,b,] will return false

    -> iscapacityfull() similr but this time we check ifall the memory that has been allocated is being used


-> insert some element in any location [i]: insert(content,index) ; add(content,index)
    -> assuming the memory is allocated that far
    -> if there is existing value there
        -> overide it and replace with `insert()` this wont change size of array
            e.g [a,b,c], insert(d,1) -> [a,d,c]
        -> all elements from i forward jump forward to make space at location i for new item using 'add()' this increases size of array by 1
            e.g: [a,b,c], we add(d,1) -> [a,d,b,c], notice b, and c move upward
    -> if nothing is index so index contains null or \0 then we simply put in our value without change in content whther we use insert or add

    -> if not enough memory, we need to reallocate more memory, traditonal increase is
    current memory size * 2 or *1.5


-> remove any elment in any location [i]: delete(index), remove(index)
    -> assuming we call an existing index otherwise throw error
        -> we can delete the location completley using `delete()`, the actual space is rmeoved this is O(n) complexity and size chnages
            e.g [a,b,c] -> delete(1) -> [a,c] the second case is gone and downshift everythign that was after it
        -> we can remove the content from array using `remove()` replacing content with null or empty space, this is O(1) compelxity and size doesnt change
            e.g [a,b,c] -> remove(1) -> [a,null,c] or [a,,c]


-> free space, if user is done updating the array or wants to have more space control: freespace()
we can use `freespace()` to remove any unsued space after size
e.g array size 5, but has enough memory space for 8 slots
[a,b,c,d,e] since we started at 2 and had to increase size twice, 2->4->8
the 3 slots worth of unused space that is allocated in memory is to be freed
** this should help combat memory ineficiency of dynamic arrays

more functions for bulk operations and utility and value functions will be added and looked at later


note:
this array needs to manage memeory fully, so that when user calls it they needn't worry about memory managemnt and overflow
if they make an error as in calling an index that isnt allocated or doesnt exist, or somehting similar we need ot give error for overflows or outofbounds
everything else needs to be controled from freeing up space of buffers and more...


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


struct dynamic_array {
    dtype *items;
    uint8_t *occupied;

    // is size empty is useless as it suffice to check num_item , if =0 empty, otherwise not empty
    bool is_freed; //checks that we freed the space if its true essentially no more space allocated

    bool is_size_full;

    size_t num_items;
    size_t size;
    size_t largest_idx;
};

void print_status(struct dynamic_array *arr) {
    if (arr->is_freed) {
        printf("\n\nWarning: Array no longer exists in memory!!!\n\n");
        return;
    }

    printf("\n\n~~~Content Of Dynamic Array~~~\n"
        "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    if (arr->num_items > 0) {
        printf("\ncontent of items:\n");
        for (int i = 0; i <= arr->largest_idx; i++) {
            PRINT(arr->items[i]);
            printf("; ");
        }

        printf("\ncontent of occupied:\n");
        for (int i = 0; i <= arr->largest_idx; i++) {
            PRINT(arr->occupied[i]);
            printf("; ");
        }
    } else { printf("\nArray is empty no items to be read!\n"); }

    printf("\nLargest Index: %zu\n", arr->largest_idx);
    printf("Has it been Freed: %d\n", arr->is_freed);
    printf("Is size full: %d\n", arr->is_size_full);
    printf("How much space/size for items: %zu\n", arr->size);
    printf("How many items: %zu\n", arr->num_items);

    printf("\n"
        "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
}

void da_free(struct dynamic_array *arr) {
    if (arr->is_freed) {
        printf("\n\nError: Array no longer exists in memory!!!\n\n");
        exit(-1);
    }

    free(arr->items);
    free(arr->occupied);

    arr->items = NULL;
    arr->occupied = NULL;

    arr->size = 0;
    arr->is_size_full = false;
    arr->is_freed = true;
    arr->num_items = 0;
    arr->largest_idx = 0;
}

void da_free_to_fit(struct dynamic_array *arr) {
    if (arr->is_freed) {
        printf("\n\nError: Array no longer exists in memory!!!\n\n");
        exit(-1);
    }

    //size_t capacity = arr->size * sizeof(dtype);
    //size_t occupied_capacity = arr->size * sizeof(uint8_t);

    //logic: arr->size-(arr->size - (arr->largest_idx + 1)) avoided since its longer and can lead to sign issues potentially
    size_t optimal_size = arr->largest_idx + 1;

    size_t optimal_capacity = (optimal_size * sizeof(dtype));
    size_t optimal_occupied_capacity = (optimal_size * sizeof(uint8_t));

    dtype *item_buffer = realloc(arr->items, optimal_capacity);
    uint8_t *occupied_buffer = realloc(arr->occupied, optimal_occupied_capacity);


    if (item_buffer == NULL || occupied_buffer == NULL) {
        fprintf(stderr, "Error: realloc failed. Original data preserved.\n");

        // Decide whether to free and exit, or continue with the original size
        free(item_buffer);
        free(occupied_buffer);
        exit(EXIT_FAILURE);
    }

    arr->items = item_buffer;
    arr->occupied = occupied_buffer;
    arr->size = optimal_size;

    if (arr->num_items == arr->size) {
        arr->is_size_full = true;
    }
}

void da_realloc(struct dynamic_array *arr) {
    if (arr->is_freed) {
        printf("\n\nError: Array no longer exists in memory!!!\n\n");
        exit(-1);
    }

    if (arr->size == 0) {
        arr->size++;
        //arr->occupied[0]=0;
        //memset(arr->occupied, 0, occupied_capacity);
    }

    size_t capacity = arr->size * sizeof(dtype);
    size_t occupied_capacity = arr->size * sizeof(uint8_t);

    dtype *item_buffer = realloc(arr->items, capacity * 2);
    uint8_t *occupied_buffer = realloc(arr->occupied, occupied_capacity * 2);

    if (item_buffer == NULL || occupied_buffer == NULL) {
        fprintf(stderr, "Error: realloc failed. Original data preserved.\n");

        // Decide whether to free and exit, or continue with the original size
        free(item_buffer);
        free(occupied_buffer);
        exit(EXIT_FAILURE);
    }

    arr->items = item_buffer;
    arr->occupied = occupied_buffer;
    memset(&arr->occupied[arr->size], 0, occupied_capacity);

    arr->size *= 2;
    arr->is_size_full = false;
}

void setup(struct dynamic_array *arr, size_t size) {
    if (size == 0) { size++; } //in case user tries to make size=0 and break the malloc
    arr->is_freed = false;
    arr->is_size_full = false;

    arr->num_items = 0;
    arr->size = size;
    arr->largest_idx = 0;

    size_t capacity = arr->size * sizeof(dtype);
    size_t occupied_capacity = arr->size * sizeof(uint8_t);

    (*arr).items = malloc(capacity);
    arr->occupied = malloc(occupied_capacity);

    if (arr->items == NULL || arr->occupied == NULL) {
        free(arr->items);
        free(arr->occupied);

        printf("\nError malloc failed\n");
        exit(EXIT_FAILURE);
    }

    memset(arr->occupied, 0, occupied_capacity);
    //note (*arr). is same as arr-> operator
}

void add(struct dynamic_array *arr, int idx,dtype item) {
    //error catching index
    if (arr->is_freed) {
        printf("\n\nError: Array no longer exists in memory!!!\n\n");
        exit(-1);
    }
    if (idx < 0) {
        printf("\n\nError: index is negative!!!\n\n");
        exit(-1);
    }

    //overflow check
    if (arr->is_size_full) {
        da_realloc(arr);
    }

    if (idx + 1 > arr->size) {
        int k = 0;
        size_t current_size = arr->size;
        do {
            da_realloc(arr);
            k++;
            if (k >= 4) {
                //if we run it more than 3 times break, we essentially stop at the 4th loop, only 3 loops are allowed, this is to maintain proper coding principles, yes 3 is random but it just feels oddly apropaite, can be repalce dor removed at later time
                printf("Error: index way out of bound: idx goal: %d, OG array size: %zu !!!\n", idx, current_size);
                exit(-1);
            }
        } while (idx + 1 > arr->size);
        //since with every iteration size grows *2 this hsould end eventually, but i will temrinate after 3 passaged and throw error that index if way outofbound
        // i set up as *3 even tho in some instance a 3times growth is accpetable mainly to discourage bad behaviour of putting an index so far beyond, if really need be, user should call da realloc thmeselves in advance
    }


    //adding
    if (arr->occupied[idx] != 0) {
        if (arr->largest_idx + 1 >= arr->size) { da_realloc(arr); }

        for (int i = arr->largest_idx + 1; i > idx; i--) {
            arr->items[i] = arr->items[i - 1];
            arr->occupied[i] = arr->occupied[i - 1];
        }
        arr->largest_idx++;

        arr->items[idx] = item;
        arr->occupied[idx] = 1;
        arr->num_items++;

        /*if (arr->largest_idx<idx) {
            arr->largest_idx=idx;
        }*/ //no longer possible

        if (arr->num_items == arr->size) {
            arr->is_size_full = true;
        }
    } else {
        arr->occupied[idx] = 1;
        arr->items[idx] = item;
        arr->num_items++;

        if (arr->largest_idx < idx) {
            arr->largest_idx = idx;
        }
        if (arr->num_items == arr->size) {
            arr->is_size_full = true;
        }
    }
}

void insert(struct dynamic_array *arr, int idx,dtype item) {
    //error catching index
    if (arr->is_freed) {
        printf("\n\nError: Array no longer exists in memory!!!\n\n");
        exit(-1);
    }
    if (idx < 0) {
        printf("\n\nError: index is negative!!!\n\n");
        exit(-1);
    }

    //overflow check
    if (arr->is_size_full) {
        da_realloc(arr);
    }

    if (idx + 1 > arr->size) {
        int k = 0;
        size_t current_size = arr->size;
        do {
            da_realloc(arr);
            k++;
            if (k >= 4) {
                printf("Error: index way out of bound: idx goal: %d, OG array size: %zu !!!\n", idx, current_size);
                exit(-1);
            }
        } while (idx + 1 > arr->size);
        //since with every iteration size grows *2 this hsould end eventually, but i will temrinate after 3 passaged and throw error that index if way outofbound
        // i set up as *3 even tho in some instance a 3times growth is accpetable mainly to discourage bad behaviour of putting an index so far beyond, if really need be, user should call da realloc thmeselves in advance
    }
    //inserting

    if (arr->occupied[idx] == 0) {
        arr->num_items++;
    }
    arr->occupied[idx] = 1;
    arr->items[idx] = item;

    if (arr->largest_idx < idx) {
        arr->largest_idx = idx;
    }
    if (arr->num_items == arr->size) {
        arr->is_size_full = true;
    }
}

//delete() the location completley
void da_delete(struct dynamic_array *arr, int idx) {
    //error catching index
    if (arr->is_freed) {
        printf("\n\nError: Array no longer exists in memory!!!\n\n");
        exit(-1);
    }
    if (idx < 0) {
        printf("\n\nError: index is negative!!!\n\n");
        exit(-1);
    }
    if (arr->size < idx + 1) {
        printf("\n\nError: Index out of bounds\n\n");
        exit(-1);
    }
    if (arr->size == 1) {
        if (arr->occupied[0] != 0) {
            arr->items[0] = 0;
            arr->occupied[0] = 0;
            arr->num_items = 0;
            arr->is_size_full = false;
        }
        printf(
            "Warning cant delete/decrease size of array if only one slot available.\nSlot is cleared but size remains 1\nIf wanting to delete use da_free()");
        return;
    }
    if (arr->occupied[idx] == 0 && idx >= arr->largest_idx) {
        //technically idx > largest index implies occupied 0
        printf("\n\nWarning: slot already empty and nothing will cascade down, simply deleting slot\n\n");
        arr->size--;

        if (arr->num_items == arr->size) {
            arr->is_size_full = true;
        }
        return;
    }

    if (arr->occupied[idx] != 0) {
        arr->num_items--;
        //in this situation is_size_full shouldnt change
    }

    if (arr->largest_idx == idx) {
        //since we know largest idx is by definition occupied no need for a sizefull checkup it remains the same
        arr->items[idx] = 0;
        arr->occupied[idx] = 0;
        arr->size--;

        if (arr->num_items >= 1) {
            for (int i = arr->largest_idx; i > 0; i--) {
                if (arr->occupied[i - 1] == 1) {
                    arr->largest_idx = i - 1;
                    break;
                }
            }
        } else {
            arr->largest_idx = 0;
        }
    } else {
        if (arr->occupied[idx] == 0 && (arr->num_items == arr->size - 1)) {
            arr->is_size_full = true;
        }

        for (int i = idx; i < arr->largest_idx; i++) {
            arr->items[i] = arr->items[i + 1];
            arr->occupied[i] = arr->occupied[i + 1];
        }


        arr->items[arr->largest_idx] = 0;
        arr->occupied[arr->largest_idx] = 0;
        arr->largest_idx--;
        arr->size--;
    }
}

//remove() content from slot
void da_remove(struct dynamic_array *arr, int idx) {
    //error catching index
    if (arr->is_freed) {
        printf("\n\nError: Array no longer exists in memory!!!\n\n");
        exit(-1);
    }
    if (idx < 0) {
        printf("\n\nError: index is negative!!!\n\n");
        exit(-1);
    }
    if (arr->largest_idx < idx) {
        printf("\n\nWarning: No Content to be removed\n\n");
        return;
    }

    if (arr->size == 1) {
        if (arr->occupied[0] != 0) {
            arr->items[0] = 0;
            arr->occupied[0] = 0;
            arr->num_items = 0;
            arr->is_size_full = false;
        } else {
            printf("\n\nWarning: No Content to be removed\n\n");
        }
        return;
    }

    if (arr->occupied[idx] == 0) {
        printf("\n\nWarning: No Content to be removed\n\n");
        return;
    }

    if (arr->largest_idx == idx) {
        for (int i = arr->largest_idx; i > 0; i--) {
            if (arr->occupied[i - 1] == 1) {
                arr->largest_idx = i - 1;
                break;
            } else if (i - 1 == 0) {
                arr->largest_idx = i - 1;
                break;
            }
        }


        arr->items[idx] = 0;
        arr->occupied[idx] = 0;
        arr->num_items--;
    } else if (arr->largest_idx > idx) {
        arr->items[idx] = 0;
        arr->occupied[idx] = 0;
        arr->num_items--;
    }

    if (arr->is_size_full) {
        arr->is_size_full = false;
    }
}

int main() {
    //first allow to create the dynamic array, with data type, and intial #of slots
    struct dynamic_array a;
    setup(&a, 2);

    //printf("%d\n", a.items[0]);

    //retrieve get() -> var.items[index]
    a.items[0];

    //add item at specific index add(arr, idx, item) or if you dont add idx, itll put it at the end add(arr,item)
    add(&a, 0, 11);

    printf("\nidx 0 %d\n", a.items[0]);

    add(&a, 0, 12);
    printf("\nidx 0 %d\n", a.items[0]);
    printf("\nidx 1 %d\n", a.items[1]);

    //retireve size() -> a.size
    printf("%d\n", (int) a.size); //or printf(%zu)

    //retrieve capacity -> a.capacity
    printf("%d\n", (int) a.size); //or printf(%zu)

    //retirve how many items we have in array
    printf("%d\n", (int) a.num_items);

    //retireve if size isfull() -> a.is_size_full()
    printf("%d\n", a.is_size_full);


    //can do the same ot check if is empty just say is_capacity/size_empty instead of full
    printf("%d\n", a.is_freed);
}
