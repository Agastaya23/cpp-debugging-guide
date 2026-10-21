# Lesson 01: Crash and stepping

**File:** [`grades.cpp`](grades.cpp) · **Answer:** [`grades_fixed.cpp`](grades_fixed.cpp)

**You'll learn:** how to read a crash, the call stack, breakpoints, Step Over / Into / Out, and the Debug Console.

## The program

`grades.cpp` stores students and their test scores, then prints each student's average. Run it (or just read the expected output in `main`) and you'll see two problems:

```
Alice: 83        ← should be 83.25
Bob: 74          ← should be 74.3333
(then it crashes: exit code 139, a segmentation fault)
```

There are two bugs. We'll find both with the debugger. **Don't fix anything until you've found it.**

---

## Part 1: Debugging a crash

1. Open `grades.cpp` and click inside it.
2. Press **F5**. There are no breakpoints yet.

The program **stops by itself** at line 40 with *"Exception has occurred. EXC_BAD_ACCESS"*. A crash under the debugger acts like a free breakpoint: the program is frozen at the exact moment it went wrong.

```cpp
39    Student* student = findStudent(students, name);
40    double avg = averageOf(student->scores);   ← stopped here
```

### Read the Variables panel
Expand **Locals**:
```
name    = "Charlie"
student = nullptr          ← a null pointer
avg     = 74               ← garbage: line 40 never finished, so avg was never set
```
`student->scores` reads memory through a null pointer (address 0). That's illegal, so the operating system kills the program.

> **Tip:** a variable that hasn't been assigned yet shows whatever happened to be in that memory. Here `avg = 74` is left over from Bob's call. Don't trust a value until the line that sets it has run.

### Read the Call Stack panel
This is the "stack trace". Read it top to bottom as *"where I am, and who called me"*:
```
printReport    grades.cpp:40    ← paused here (the crash)
main           grades.cpp:52    ← printReport was called from here
```
**Click `main`.** The editor jumps to line 52, `printReport(students, "Charlie");`, and the Variables panel switches to *main's* variables. Every frame keeps its own variables, and clicking a frame lets you look at any of them.

### Find the cause
`student` comes from `findStudent` (lines 28–35). Read it: when no name matches, it reaches line 34, `return nullptr;`. There's no student called Charlie, so `findStudent` returned `nullptr`, and `printReport` used it without checking.

Press **Shift+F5** to stop.

> **Lesson:** when a program crashes, look at the variables in the crashing frame, then work *up* the call stack until you find where the bad value came from.

---

## Part 2: Breakpoints and stepping

The averages are wrong but nothing crashes, so the debugger won't stop on its own. You have to tell it where to pause.

### Set a breakpoint
Click in the gutter to the **left of line number 21**, inside `averageOf`. A red dot ● appears. A breakpoint means *"pause just **before** running this line"*.

Press **F5**. The program pauses at line 21 the first time `averageOf` is called, which is for Alice. Line 21 is highlighted but **hasn't run yet**, so `total` doesn't have its value yet.

### Step Into: go inside a function call
Line 21 is `int total = totalOf(scores);`. Press **F11 (Step Into)**. You're now inside `totalOf` at line 12, and the Call Stack has a new frame on top:
```
totalOf       grades.cpp:12
averageOf     grades.cpp:21
printReport   grades.cpp:40
main          grades.cpp:50
```

### Step Over: one line at a time
Press **F10 (Step Over)** repeatedly and watch the Variables panel. `score` takes each value in turn and `total` grows: `90 → 175 → 255 → 333`. Values that just changed are highlighted.

### Step Out: finish this function
The loop is clearly fine. Press **Shift+F11 (Step Out)**. `totalOf` finishes and you're back at line 21 in `averageOf`, and its frame disappears from the Call Stack. Press **F10** once to finish line 21: `total` is now `333`. That's correct, so **the bug is not in `totalOf`**.

> **Step Over vs Step Into:** on a line that calls a function, F10 runs the whole function and stops on the next line, while F11 goes inside it. Use F10 for code you trust and F11 for code you suspect.

### Find the bad line
Keep pressing **F10**:

| After line | Variables |
|---|---|
| 21 `int total = totalOf(scores);` | `total = 333` ✓ |
| 22 `int count = scores.size();` | `count = 4` ✓ |
| 23 `double average = total / count;` | `average = 83` ✗ (333 / 4 is 83.25) |

`total` and `count` are both right, but `average` is wrong. **Line 23 is the bug.** Why, though?

---

## Part 3: The Debug Console

Open the **Debug Console** tab at the bottom (**Cmd+Shift+Y**). While the program is paused, you can type any expression that uses the variables in scope and press Enter:

```
total / count              → 83
(double)total / count      → 83.25
total % count              → 1
```

That proves it. `total` and `count` are both `int`, so `/` does **integer division** and throws away the remainder. Storing the result in a `double` happens *after* the `.25` is already gone.

The fix is to convert one side to `double` *before* dividing. Line 23 becomes:
```cpp
double average = static_cast<double>(total) / count;
```

> **Lesson:** use breakpoints to find roughly *where* the bug is, step to find the exact *line*, and use the Debug Console to prove *why* before you change any code.

> **Gotcha:** the debugger can't always call functions. Something like `scores.size()` may fail with *"Couldn't look up symbols"*, because small library functions are inlined away when the program is compiled. Plain variables and arithmetic always work. To see a vector's size, expand it in the Variables panel instead: it shows `size=4`.

---

## Part 4: Fix and verify

1. Fix line 23 as shown above.
2. Fix the crash: after line 39, check for `nullptr` before using the pointer:
   ```cpp
   Student* student = findStudent(students, name);
   if (student == nullptr) {
       std::cout << name << ": not found" << std::endl;
       return;
   }
   ```
3. **Verify with the debugger, not just the output.** Put a breakpoint on line 24 (`return average;`), press F5, and check that `average` is `83.25` for Alice. Press F5 again and check `74.3333…` for Bob. Press F5 once more: Charlie should print `not found` and the program should exit normally.

Compare your fixes with [`grades_fixed.cpp`](grades_fixed.cpp).

### Try this
- What does `averageOf` do if a student has no scores (`{}`)? Add one in `main`, put a breakpoint on line 23, and look at `count` before stepping over it. *(Hint: what is 0 / 0?)*
- Add a breakpoint on line 40 with the **condition** `student == 0` (right-click the dot → Edit Breakpoint → Expression). Now the debugger pauses *before* the crash, and only for the bad call. Lesson 02 covers this technique.

**Next: [Lesson 02, Loops and conditions →](../02-loops-and-conditions/)**
