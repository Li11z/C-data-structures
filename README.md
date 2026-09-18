# C-data-structures: A personal Library of different Data-Structures and Algorithms implementation in C

---


---
## Dynamic Array: `Dynamic_Array.c`

A heap-allocated, growable array of `dtype` that supports **holes**. Two parallel buffers back it: `items` holds values, `occupied` holds a presence flag per slot. A slot is valid only if its flag is set, so the array can be sparse without reserving a sentinel value. Errors (bad index, use-after-free, failed allocation) print and `exit()`.

### Configuration

- **`#define dtype int`** — the element type. All signatures and `sizeof` math derive from it, so one element type per build.
- **`PRINT(x)`** — a `_Generic` macro that picks the right `printf` specifier from the argument's static type, so printing survives a change of `dtype`.

### The struct

| Field | Role |
|---|---|
| `items` | Buffer of `size` values. Unoccupied slots hold garbage. |
| `occupied` | Parallel buffer of `size` flags: `0` empty, `1` filled. |
| `size` | Capacity — how many slots exist. |
| `num_items` | Population — how many slots are filled. |
| `largest_idx` | Highest occupied index; bounds every loop in the library. |
| `is_size_full` | Cached `num_items == size`; tells writers to grow first. |
| `is_freed` | Set by `da_free`; every entry point checks it. |

- `items` and `occupied` are always allocated, shifted and freed together — any operation that moves a value moves its flag in the same step.
- `size` and `num_items` are independent; their difference is the number of holes.
- `largest_idx` is `0` both when empty and when a single item sits at index 0; `num_items` disambiguates.

### Functions

**Lifecycle**
- `setup(arr, size)` — constructor. Clamps `size` to at least 1, allocates both buffers, zeroes `occupied`. `items` is left uninitialised on purpose: nothing may read a slot whose flag is `0`.
- `da_free(arr)` — frees both buffers, nulls the pointers, resets counters, raises `is_freed`. Calling `setup` again revives the struct.

**Capacity**
- `da_realloc(arr)` — the only place growth happens. Doubles both buffers, zeroes the newly added half of `occupied`, clears `is_size_full`.
- `da_free_to_fit(arr)` — shrinks capacity to `largest_idx + 1`. Reclaims the dead tail only; holes below the high-water mark are preserved.

**Writing** — these differ only in what happens when the target slot is already occupied.
- `insert(arr, idx, item)` — **overwrite**. Writes at `idx` regardless, incrementing `num_items` only if the slot was empty. Nothing moves.
- `add(arr, idx, item)` — **insert and shift**. On an occupied slot, everything from `largest_idx` down to `idx` shifts one right (values and flags together) before the write; O(n). On an empty slot it behaves like `insert`.
- Both grow the array if `idx` is out of range, but abort after 3 doublings — reaching a far index deliberately requires calling `da_realloc` yourself first.

**Removing** — the key distinction is emptying a slot vs. destroying it.
- `da_remove(arr, idx)` — clears the content, leaves `size` alone. Result is a hole. Lowers `largest_idx` by scanning backwards if the top slot was the one cleared.
- `da_delete(arr, idx)` — destroys the slot: everything above cascades one position left and `size` drops, so indices above `idx` change. Neither function shrinks below a single slot; use `da_free` for that.

**Inspection**
- `print_status(arr)` — dumps `items` and `occupied` up to `largest_idx`, then the metadata. Only warns on a freed array rather than exiting.

### Planned functions
- **Mutation** — `da_append` (the missing common case), `da_pop`, `da_clear`, `da_swap`, `da_sort`.
- **Structure** — `da_compact` (squeeze out holes), `da_reserve`, `da_resize`, `da_insert_range` / `da_delete_range`.
- **Composition** — `da_copy`, `da_extend`, `da_slice`, `da_equals`.
- **Iteration** — `da_foreach`, `da_map`, `da_filter`, `da_to_string`.
- **Infrastructure** — status codes instead of `exit()`.

---
## Stack: `Stack.c`

---
## Linked List: `Linked_List.c`

---
## HashMap: `Hash_Map.c`

---