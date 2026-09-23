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
# Linked List: `Linked_List.c`

A heap-allocated, doubly linked list of `dtype` nodes with O(1) head/tail access and a self-invalidating finger cache for faster indexed lookup. Each `Node` carries its own `data`, `next`, and `prev` pointers; the list manager tracks head, tail, size, and the finger. Errors (bad index, use-after-free) print and `exit()`. Returns from empty-list reads warn and return `-1`.

### Configuration

- **`#define dtype char`** — the element type. All node payloads and signatures derive from it, so one element type per build.
- **`PRINT(x)`** — a `_Generic` macro that picks the right `printf` specifier from the argument's static type, so printing survives a change of `dtype`.

### The structs

**`struct Node`**

| Field | Role |
|---|---|
| `data` | The stored value. |
| `next` | Pointer to the following node, `NULL` at the tail. |
| `prev` | Pointer to the preceding node, `NULL` at the head. |

**`struct LinkedList`**

| Field | Role |
|---|---|
| `head` | First node, `NULL` when empty. |
| `tail` | Last node, `NULL` when empty. |
| `size` | Number of nodes currently in the list. |
| `finger` | Cached pointer to a recently accessed node, `NULL` when invalid. |
| `finger_idx` | Index the finger corresponds to; meaningless when `finger` is `NULL`. |
| `is_freed` | Set by `free_LL`; every entry point checks it. |

- The list is **doubly linked**: every node except head has a valid `prev`, every node except tail has a valid `next`. This is what allows `pop`, `poll`, and indexed removal without a predecessor scan.
- The **finger** is a cache, not a guarantee. Any operation that shifts indices or frees the finger's node resets `finger = NULL` (or `finger_idx = 0`). Because the struct is described as "not a variable to be maintained otherwise," the finger is opportunistic: it accelerates repeated nearby lookups but the list works correctly with `finger == NULL`.
- `size` is the sole authority on list length; `head == tail` implies `size <= 1`, but the code checks `size` explicitly to distinguish empty from singleton.

### Functions

**Lifecycle**
- `setup(list)` — constructor. Nulls `head`, `tail`, `finger`; zeroes `size` and `finger_idx`; clears `is_freed`. No allocation, so no failure path.
- `free_LL(list)` — frees every node by walking from head to tail, then frees the list struct itself. Nulls `head`/`tail`, zeroes `size`. Note: this frees the manager struct too, so the caller's pointer is dangling afterward; `ll_erase` (planned) is the variant that keeps the struct alive.

**Adding**
- `push(list, x)` — insert at head, O(1). Bumps `finger_idx` by one to keep the cache's index consistent with the shift, since the cached node now sits one slot further along.
- `offer(list, x)` — insert at tail, O(1).
- `add(list, idx, x)` — insert at an arbitrary index, dispatching to `push` for `idx == 0` and `offer` for `idx == size - 1`; otherwise finds the node and splices before it, O(n). The finger is **repointed to the new node** at that index, so the next lookup at `idx` is O(1).

**Removing** — all removal functions free the node and return its data.
- `pop(list)` — remove and return the tail's data, O(1). Warns and returns `-1` if empty. Note: it dereferences `tail->prev` before the size check, so calling it on a singleton is the edge case to watch.
- `poll(list)` — remove and return the head's data, O(1). Same empty-list behavior. (Same singleton caveat: `head->next` is dereferenced before the free.)
- `pop_idx(list, idx)` — remove and return the node at `idx`, O(n). Delegates to `poll`/`pop` at the ends, uses the finger when it hits, and splices out the node otherwise. **Always clears the finger** afterward, since the cached node may have been the one freed.

**Reading**
- `get_h(list)` — return head data, O(1). No guards; assumes non-empty.
- `get_t(list)` — return tail data, O(1). No guards; assumes non-empty.
- `get_idx(list, idx)` — return data at `idx`, O(n) worst case. Checks the ends and the finger first, then falls through to `get_node_private`. Unlike the mutators, it does not invalidate or refresh the finger.

**Lookup helpers** — the internal machinery behind indexed operations.
- `get_node_private(list, idx)` — the unguarded walker. Picks the cheapest of three starting points: head, tail, or finger, whichever is closest to `idx`, then steps node-by-node. This is what makes an isolated `add`/`pop_idx`/`get_idx` cost `min(idx, size-idx, |finger_idx-idx|)` steps instead of always `idx`.
- `get_node(list, idx)` — the guarded public version. Validates `is_freed`, rejects negative and out-of-range indices, and warns on an empty list. Currently duplicates `get_node_private`'s body rather than calling it; a later refactor could collapse the two once the guard logic stabilizes.

### Planned functions
- **Mutation** — `ll_erase` (free all nodes, keep the struct), `ll_clear`, `ll_reverse`, `ll_sort`, `ll_swap`, `ll_rotate`.
- **Structure** — `ll_concat`, `ll_splice`, `ll_insert_range` / `ll_delete_range`, `ll_dedupe`.
- **Composition** — `ll_copy`, `ll_extend`, `ll_slice`, `ll_equals`.
- **Iteration** — `ll_foreach`, `ll_map`, `ll_filter`, `ll_to_string`, `ll_find`.
- **Infrastructure** — status codes instead of `exit()` and `-1`; consistent finger invalidation across all mutators; merge `get_node` and `get_node_private`.

---
## HashMap: `Hash_Map.c`

---