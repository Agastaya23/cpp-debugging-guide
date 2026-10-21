# Lesson 03: Call stack and recursion

**File:** [`search.cpp`](search.cpp) · **Answer:** [`search_fixed.cpp`](search_fixed.cpp)

**You'll learn:** what the call stack really is, reading a huge stack after a stack overflow, moving between frames, and stepping through recursion.

## The program

`search` is a recursive binary search. It looks for `target` in a sorted list, between positions `lo` and `hi`:

```cpp
 8  int search(const std::vector<int>& v, int target, int lo, int hi) {
 9      if (lo > hi) {
10          return -1;
11      }
12      int mid = (lo + hi) / 2;
13      if (v[mid] == target) {
14          return mid;
15      }
16      if (v[mid] < target) {
17          return search(v, target, mid, hi);
18      }
19      return search(v, target, lo, mid - 1);
20  }
```

Run it:
```
16 is at position 4
2 is at position 0
(then it crashes: exit code 139)
```
It works for 16 and 2, then crashes on 91.

## What the call stack is

Every time a function is called, the program reserves a block of memory for that call, called a **stack frame**. The frame holds the function's parameters, its local variables, and where to return to when it finishes. When the function returns, its frame is removed.

The **Call Stack** panel shows these frames, newest on top. For recursion, each call to `search` gets its *own* frame with its *own* `lo`, `hi` and `mid`. They don't overwrite each other.

The stack has a fixed size (8 MB by default on macOS). If calls keep stacking up and never return, the stack runs out. That's a **stack overflow**, and the program crashes.

---

## Part 1: Read the crash

Open `search.cpp`, click inside it, and press **F5** with no breakpoints.

It stops with **EXC_BAD_ACCESS** at line 8, the first line of `search`. Look at the **Call Stack** panel:
```
search(v=size=10, target=<unavailable>, lo=<unavailable>, hi=<unavailable>)   search.cpp:8
search(v=size=10, target=91, lo=8, hi=9)   search.cpp:17
search(v=size=10, target=91, lo=8, hi=9)   search.cpp:17
search(v=size=10, target=91, lo=8, hi=9)   search.cpp:17
search(v=size=10, target=91, lo=8, hi=9)   search.cpp:17
... Load More Stack Frames
```
Notice three things:

1. **The top frame shows `<unavailable>`.** The crash happened *while entering* that call, because there was no stack space left to store its parameters. That frame is useless, so **click the second frame** instead.
2. **Every frame below it is identical:** `target=91, lo=8, hi=9`, called from line 17. The function is calling itself with the same arguments over and over.
3. **The stack is enormous.** When this guide was tested, the crash happened at about **174,000** frames deep. That's why there's a *Load More Stack Frames* link. Somewhere far at the bottom are `find` and `main`.

A wall of identical frames is the signature of **infinite recursion**. Something that should shrink each time isn't shrinking.

Press **Shift+F5** to stop.

---

## Part 2: Step through the recursion

The crash showed us the stuck state, `lo=8, hi=9`. Now let's watch it happen.

1. Add a breakpoint on **line 9** (`if (lo > hi) {`).
2. Right-click it → **Edit Breakpoint…** → **Expression** → `target == 91`. This skips the calls for 16 and 2, which work fine.
3. Add these to the **Watch** panel: `lo`, `hi`, `mid`, `v[mid]`, `hi - lo`. At line 9, `mid` hasn't been set yet, so it holds garbage and `v[mid]` shows an error. Both become correct once line 12 has run. (This is the "don't trust a value until the line that sets it has run" tip from Lesson 01.)
4. Press **F5**.

You're paused in the first call: `lo = 0, hi = 9`. Now:

- Press **F10** to step through to line 16 and watch `mid` get set.
- On line 17 (`return search(...)`), press **F11 (Step Into)** to follow the recursive call. A new `search` frame appears on top of the Call Stack, with new values for `lo` and `hi`.
- Repeat, writing down what you see:

| Call | `lo` | `hi` | `mid` | `v[mid]` | Goes to | `hi - lo` |
|---|---|---|---|---|---|---|
| 1 | 0 | 9 | 4 | 16 | line 17: `search(4, 9)` | 9 |
| 2 | 4 | 9 | 6 | 38 | line 17: `search(6, 9)` | 5 |
| 3 | 6 | 9 | 7 | 56 | line 17: `search(7, 9)` | 3 |
| 4 | 7 | 9 | 8 | 72 | line 17: `search(8, 9)` | 2 |
| 5 | 8 | 9 | **8** | 72 | line 17: `search(8, 9)` ← **the same arguments it was called with!** | 1 |
| 6 | 8 | 9 | 8 | 72 | line 17: `search(8, 9)` | 1 |

At call 5, `mid = (8 + 9) / 2 = 8`, which equals `lo`. Line 17 calls `search(v, target, mid, hi)`, which is `search(8, 9)` again. The range never gets smaller, so the recursion never ends.

### Look at several frames at once
While paused a few calls deep, **click different `search` frames in the Call Stack**. Each one shows its own `lo`, `hi` and `mid` in Variables, and the editor highlights the line where *that* call is waiting. This is what "each call has its own frame" looks like in practice.

### Step Out of recursion
Press **Shift+F11 (Step Out)** while inside a recursive call. Normally that finishes the current call and returns you to the caller. Here, the current call never finishes, because it recurses forever, so Step Out keeps running until the program crashes. When Step Out "never comes back", it's a strong hint the function doesn't terminate.

---

## Part 3: Explain and fix

Binary search only works if every call looks at a **strictly smaller** range. On line 17 we already know `v[mid] < target`, so position `mid` can't be the answer. The next search should start *after* it:

```cpp
return search(v, target, mid + 1, hi);
```

Line 19 already does the mirror-image correctly with `mid - 1`.

Why did 16 and 2 work? 16 was found at the very first `mid`. 2 only ever went *left* through line 19, which is correct. Only targets that go *right* until `hi - lo == 1` hit the bug. Tests that happen to pass can hide a bug like this. The debugger shows you the path each input actually takes.

### Verify
Apply the fix, keep the `target == 91` breakpoint, and step through again with **F11**. Watch the `hi - lo` column in Watch: it should shrink every call until `v[mid] == 91` at `mid = 9`. Then press **Shift+F11** a few times and watch the frames come off the stack one by one as each call returns `9`.

Final output:
```
16 is at position 4
2 is at position 0
91 is at position 9
```

Compare with [`search_fixed.cpp`](search_fixed.cpp).

> **Lesson:** the call stack is a list of frames, each with its own variables. After a crash, skip frames showing `<unavailable>` and look for **patterns**. Identical repeated frames mean a recursion that doesn't shrink.

### Try this
- In the fixed version, search for a number that **isn't** in the list, like `50`. Step through and watch how `lo > hi` eventually becomes true on line 9.
- Write an iterative (loop-based) version of `search`. Debug it with a logpoint on the loop that prints `lo={lo} hi={hi} mid={mid}`.

**[← Back to the guide](../../README.md)**
