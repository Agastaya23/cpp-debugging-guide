# Lesson 02: Loops and conditions

**File:** [`sensor.cpp`](sensor.cpp) · **Answer:** [`sensor_fixed.cpp`](sensor_fixed.cpp)

**You'll learn:** conditional breakpoints, hit counts, logpoints, the Watch panel, Run to Cursor, and changing a variable while paused.

## The program

A weather station records one temperature per minute for a day: 1,440 readings. When the sensor goes offline it records `-999` instead, and those readings should be skipped. `summarize` loops over the readings and works out the average, minimum and maximum.

Run it:
```
Readings: 1440
Skipped:  0   (should be 5)
Minimum:  -999   (should be 15)
Maximum:  23   (should be 23)
Average:  15.4688
```

The offline readings aren't being skipped. But the loop runs 1,440 times, and pressing F10 1,440 times isn't an option. This lesson is about getting the debugger to stop **only at the moment you care about**.

The loop is in `summarize`:
```cpp
34    for (std::size_t i = 0; i < readings.size(); i++) {
35        int r = readings[i];
36        if (r == -99) {  // skip offline readings
37            result.skipped++;
38            continue;
39        }
40        sum += r;
41        count++;
42        if (r < result.minimum) result.minimum = r;
43        if (r > result.maximum) result.maximum = r;
44    }
```

---

## Part 1: Logpoint, to find *when* it goes wrong

A **logpoint** prints a message every time a line runs, *without pausing* and without editing your code. It's like adding a `cout` line, except you don't recompile and don't have to remember to delete it.

1. Right-click the gutter at **line 42** → **Add Logpoint…**
2. Type this message (expressions go in `{braces}`):
   ```
   i={i} r={r} minimum so far={result.minimum}
   ```
3. Press Enter. A diamond ◆ marks the logpoint.
4. Press **F5** and open the **Debug Console** (Cmd+Shift+Y).

You get one line per reading. Scroll (or use the filter box) and you'll see:
```
i=599 r=18 minimum so far=15
i=600 r=-999 minimum so far=15     ← an offline reading got past the check
...
```
Now you know **when** the bad value gets in: at `i = 600`. Remove the logpoint (right-click it → Remove Breakpoint).

> Logpoints are the right tool when you want the *history* of a value rather than a snapshot.

---

## Part 2: Conditional breakpoint, to stop *only* on the bad case

1. Click the gutter at **line 36** to add a breakpoint.
2. Right-click the red dot → **Edit Breakpoint…** → choose **Expression** in the dropdown.
3. Type `r < 0` and press Enter. The dot gets a small `=` mark.
4. Press **F5**.

The program skips the first 600 normal readings and pauses on the first negative one:
```
i     = 600
r     = -999
sum   = 9840
count = 600
```
Line 36, `if (r == -99)`, hasn't run yet. Press **F10** once. Execution goes to **line 40**, not line 37. The `if` was false, so the reading wasn't skipped.

Check it in the Debug Console:
```
r == -99        → false
r == OFFLINE    → true
OFFLINE         → -999
```
That's the bug. The check compares against a hand-typed `-99`, but offline readings are `-999`. The program already has a constant, `OFFLINE`, for exactly this value, and line 36 should use it.

> **Conditional breakpoints** can use any expression the debugger can evaluate: `i == 600`, `r < 0 && count > 10`, `student == 0`. Keep to plain variables and arithmetic. Calling functions such as `readings.size()` may fail (see the gotcha in Lesson 01).

---

## Part 3: Hit count, to stop on the Nth time

Sometimes you know *which iteration* is bad, not which value. Change the breakpoint on line 36:

1. Right-click the dot → **Edit Breakpoint…** → choose **Hit Count**.
2. Type `601` and press Enter. (Delete the `r < 0` expression first if it's still there.)
3. Restart with **Cmd+Shift+F5**.

The debugger pauses the 601st time line 36 runs, which is when `i == 600`. The first hit is `i == 0`, so hit number N is `i == N - 1`.

Use hit counts for problems like "it breaks on the 500th request".

---

## Part 4: Watch panel, to pin what you care about

While paused in the loop, go to the **Watch** panel in the Run sidebar, click **+**, and add each of these:
```
r
r == OFFLINE
sum
count
result.minimum
```
Now press **F10** a few times. The Watch panel updates on every step, and you don't have to dig through Variables or retype things in the console. Expand `result` in Variables too: structs open up with ▸.

---

## Part 5: Change a variable live

While paused at line 36 with `r = -999`:

1. In the **Variables** panel, double-click `r` and change it to `-99`. Press Enter.
2. Press **F10**.

This time execution goes to line **37**, `result.skipped++`. You've tested your theory ("if the values matched, the skip would work") *without editing or recompiling*. Changing a variable while paused is a quick way to ask "what if?"

---

## Part 6: Run to Cursor

You don't always need a breakpoint to jump ahead. While paused anywhere in `summarize`, **right-click line 45** (`result.average = …`) → **Run to Cursor**. The program runs the rest of the loop and pauses there, as if you'd placed a one-time breakpoint. Check `sum`, `count` and `result` to see the final totals.

---

## Fix and verify

Change line 36 to:
```cpp
if (r == OFFLINE) {  // skip offline readings
```
Then verify with the debugger. Put a breakpoint on line 37 (`result.skipped++;`) and press F5. It should pause with `i = 600`. Press F5 four more times: `i` should be 601, 602, 603, 604. Remove the breakpoint and let it finish:
```
Skipped:  5   (should be 5)
Minimum:  15   (should be 15)
```

Compare your fix with [`sensor_fixed.cpp`](sensor_fixed.cpp).

> **Lesson:** don't step through a loop by hand. Use a **logpoint** to find *when* a value goes bad, a **conditional breakpoint** or **hit count** to jump to that moment, then step from there.

**Next: [Lesson 03, Call stack and recursion →](../03-call-stack-recursion/)**
