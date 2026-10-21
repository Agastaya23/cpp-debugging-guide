# C++ Debugging in VS Code: a hands-on guide

Learn to debug C++ with breakpoints, stepping, the call stack and the Debug Console, by fixing real bugs in small programs.

Each lesson gives you a short program with a bug planted in it, and a walkthrough that finds the bug with the debugger. Every lesson also has a `_fixed.cpp` version so you can check your answer.

| Lesson | Bug | What you learn |
|---|---|---|
| [01: Crash and stepping](examples/01-crash-and-stepping/) | Wrong averages, then a crash | Reading a crash, the call stack, breakpoints, Step Over / Into / Out, the Debug Console |
| [02: Loops and conditions](examples/02-loops-and-conditions/) | A bad value hidden in 1,440 loop iterations | Conditional breakpoints, hit counts, logpoints, the Watch panel, Run to Cursor, changing variables live |
| [03: Call stack and recursion](examples/03-call-stack-recursion/) | Infinite recursion (a stack overflow) | Reading a 170,000-frame call stack, moving between frames, stepping through recursion |

Do them in order. Each lesson assumes you know the tools from the ones before it.

---

## Setup

The `.vscode` folder in this repo already contains a build task and a debug configuration, so once the tools below are installed you just press **F5**.

**You need:**
1. [VS Code](https://code.visualstudio.com/)
2. The **C/C++** extension by Microsoft (`ms-vscode.cpptools`). Install it from the Extensions sidebar.
3. A compiler and a debugger:
   - **macOS:** run `xcode-select --install` in a terminal. This installs `clang++` and `lldb`.
   - **Linux:** install `clang` and `gdb` (for example `sudo apt install clang gdb`). The launch config switches to `gdb` automatically on Linux.

**To start a debug session:**
1. Open this repo's folder in VS Code (**File → Open Folder…**). Open the folder itself, not a single file, so VS Code picks up the `.vscode` settings.
2. Open one of the `.cpp` files in the editor and click inside it. The config debugs *whichever C++ file is active*.
3. Press **F5**. VS Code compiles the file and starts it under the debugger.

> These guides were written and tested on macOS with `clang++` and `lldb`. Exact panel text can differ slightly on other systems.

### What the build task does
The task in [`.vscode/tasks.json`](.vscode/tasks.json) runs:
```
clang++ -std=c++17 -g -O0 -Wall yourfile.cpp -o yourfile.out
```
Two of those flags are what make debugging possible:

- **`-g`** stores *debug symbols*: a map from the machine code back to your source lines and variable names. Without it the debugger can only show raw memory addresses. On macOS the symbols go into a `yourfile.out.dSYM` folder next to the program.
- **`-O0`** turns optimisation off, so the compiled code follows your source line by line. With optimisation on, the compiler removes and reorders code, variables show as `<optimized out>`, and stepping jumps around unpredictably.

`.out` files and `.dSYM` folders are build output. They're rebuilt on every F5 and ignored by git.

---

## What a debugger does

Normally a program runs start to finish in milliseconds and you only see its final output. A debugger lets you **pause** it in the middle of running and then:

- **look** at the current value of every variable
- see the **call stack**, the chain of function calls that led to the current line
- **step** forward one line at a time
- **evaluate** any expression, or **change** a variable, while paused

That turns "the output is wrong somewhere" into "on this line, this variable became wrong".

## Cheat sheet

### Controls (debug toolbar)

| Key (macOS) | Action | What it does |
|---|---|---|
| **F5** | Start / Continue | Run until the next breakpoint, a crash, or the end |
| **F10** | Step Over | Run the current line, including any functions it calls, then pause on the next line |
| **F11** | Step Into | If the current line calls a function, go inside it and pause on its first line |
| **Shift+F11** | Step Out | Finish the current function and pause back in its caller |
| **Shift+F5** | Stop | End the program |
| **Cmd+Shift+F5** | Restart | Rebuild and start again from the beginning |
| **F9** | Toggle breakpoint | Add or remove a breakpoint on the current line |

On Windows/Linux, use Ctrl instead of Cmd.

### Panels (Run and Debug sidebar, Cmd+Shift+D)

| Panel | Shows |
|---|---|
| **Variables** | Every local variable in the selected stack frame. Expand structs and vectors with ▸. Values that just changed are highlighted. |
| **Watch** | Expressions you pin. They're re-evaluated every time the program pauses. |
| **Call Stack** | The chain of function calls. The top frame is where you're paused. Click any frame to see *its* variables and line. |
| **Breakpoints** | All breakpoints. Untick one to disable it without deleting it. |
| **Debug Console** (bottom panel, Cmd+Shift+Y) | Type an expression while paused and press Enter to evaluate it. Logpoint output also appears here. |

### Kinds of breakpoint

| Kind | How to add it | Use it when |
|---|---|---|
| Breakpoint ● | Click left of a line number | You want to pause on that line |
| Conditional breakpoint | Right-click the dot → Edit Breakpoint → Expression | You only care about one case, like `i == 600` |
| Hit count breakpoint | Right-click the dot → Edit Breakpoint → Hit Count | You want the Nth time the line runs |
| Logpoint ◆ | Right-click the gutter → Add Logpoint | You want to print values without pausing or editing code |

## The debugging process

1. **Reproduce:** make the bug happen reliably.
2. **Locate:** use a crash, or breakpoints, to narrow down *where* it goes wrong.
3. **Inspect:** step line by line and find the *first* line where a value is wrong.
4. **Explain:** use the Debug Console to prove *why* before changing anything.
5. **Fix and verify:** change one thing, then re-run under the debugger to confirm.

## Repo layout

```
.vscode/
  tasks.json      build the active file with debug symbols
  launch.json     debug the active file (lldb on macOS, gdb on Linux)
examples/
  01-crash-and-stepping/      grades.cpp, grades_fixed.cpp, README.md
  02-loops-and-conditions/    sensor.cpp, sensor_fixed.cpp, README.md
  03-call-stack-recursion/    search.cpp, search_fixed.cpp, README.md
```

Start with **[Lesson 01](examples/01-crash-and-stepping/)**.
