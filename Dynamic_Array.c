#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#define dtype int

struct dynamic_array {
    dtype *items;

    bool is_size_empty;
    bool is_capacity_empty;

    bool is_size_full;
    bool is_capacity_full;

    size_t num_items;
    size_t size;
    size_t  capacity;
};

void setup(struct dynamic_array *arr, size_t size) {

    arr->is_size_empty=true;
    arr->is_capacity_empty=false;

    arr->is_size_full=false;
    arr->is_capacity_full=false;

    arr->num_items=0;
    arr->capacity=size*sizeof(dtype);
    arr->size=size;

    (*arr).items=malloc(arr->capacity);
    //note (*arr). is same as arr-> operator


}

void add(struct dynamic_array *arr, int idx ,dtype item) {

}


int main() {

    //first allow to create the dynamic array, with data type, and intial #of slots
    struct dynamic_array a;
    setup(&a, 2);

    //printf("%d\n", a.items[0]);

    //retrieve get() -> var.items[index]
    a.items[0];

    //add item at specific index add(arr, idx, item) or if you dont add idx, itll put it at the end add(arr,item)



    //retireve size() -> a.size
    printf("%d\n", (int)a.size); //or printf(%zu)

    //retrieve capacity -> a.capacity
    printf("%d\n", (int)a.size); //or printf(%zu)

    //retirve how many items we have in array
    printf("%d\n", (int)a.num_items);

    //retireve if size isfull() -> a.is_size_full()
    printf("%d\n", a.is_size_full);

    //retrieve iscapacityfull()
    printf("%d\n", a.is_capacity_full);

    //can do the same ot check if is empty just say is_capacity/size_empty instead of full
    printf("%d\n", a.is_capacity_empty);


}
