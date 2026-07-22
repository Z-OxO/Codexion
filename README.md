*This project has been created as part of the 42 curriculum by jbenhass.*

# Codexion

A little concurrency playground built around the classic dining philosophers
problem, but re-skinned: instead of philosophers eating with forks, you get
**coders** compiling code with **USB dongles**. Same headaches (deadlocks,
starvation, races), different story.

## What's going on

Picture a round table. A bunch of coders sit around it, and between every pair
of neighbours there's a shared USB dongle. To compile, a coder needs to grab
**two** dongles at once — the one on their left and the one on their right.
Once they're done compiling they drop both dongles, go debug for a bit, then
refactor, then get back in line to compile again.

The catch: if a coder waits too long between two compiles, they **burn out**,
and the whole simulation stops. So the real job here is handing out the shared
dongles *fairly* so nobody starves, while respecting a cooldown on each dongle
and catching a burnout the instant it happens.

There are exactly as many dongles as coders. With `n` coders you get `n`
dongles, one between each pair.

## Build & run

```
make          # builds ./codexion
make clean    # removes object files
make fclean   # removes objects + binary
make re       # rebuild from scratch
```

Compilation is `cc -Wall -Wextra -Werror -pthread`.

Then run it like this:

```
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All 8 arguments are required. Every time is in **milliseconds**. `scheduler` is
either `fifo` or `edf` (nothing else). Bad input — wrong count, negative,
non-numeric, overflow, unknown scheduler — gets rejected before any thread even
starts.

- `number_of_coders` — how many coders (and dongles) sit at the table.
- `time_to_burnout` — max time a coder can go without starting a new compile
  before burning out.
- `time_to_compile` — how long a compile takes (dongles are held the whole time).
- `time_to_debug` / `time_to_refactor` — the two phases between compiles.
- `number_of_compiles_required` — once every coder has compiled at least this
  many times, we stop and call it a win.
- `dongle_cooldown` — after a dongle is dropped, it's unusable for this long.
- `scheduler` — `fifo` or `edf`, the policy for who gets a contested dongle.

A couple of examples:

```
./codexion 5 800 200 100 100 3 50 fifo     # everyone finishes, no burnout
./codexion 4 250 100 50 50 999 30 edf      # tight timing, someone burns out
./codexion 1 300 100 100 100 5 50 fifo     # one coder, one dongle -> burns out
```

Every state change is printed as `timestamp_in_ms coder_number state`, for
example:

```
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
200 1 is debugging
300 1 is refactoring
```

The run ends when either everyone hit their compile target, or somebody burned
out — whichever comes first.

## How it's built

I didn't let each coder grab dongle mutexes on their own. Instead there's **one
dedicated scheduler thread** that owns the arbitration. When a coder wants to
compile, it drops a request into a shared **priority queue** and goes to sleep
on its own condition variable. The scheduler wakes up, looks at who's waiting,
hands dongles to whoever it can (both of their dongles free and off cooldown),
and puts the rest back in the queue keeping their original priority. That's what
kills head-of-line blocking — a coder that can't be served right now doesn't
block the ones behind it.

The priority queue is a **binary min-heap written by hand** (no stdlib priority
queue). The key is:

- **FIFO** → the arrival timestamp, so first come first served.
- **EDF** → the burnout deadline (`last_compile_start + time_to_burnout`), so
  the coder closest to burning out jumps the line. Ties break on coder id to
  stay fully deterministic.

Each dongle is just a single timestamp: `available_at`. When a compile is
granted, both dongles get `available_at = now + time_to_compile + cooldown` in
one shot. That reserves them for the whole compile *and* the cooldown at once,
so a neighbour can never sneak in and grab a dongle mid-compile. No separate
"in use" flag needed.

A separate **monitor thread** watches the deadlines. It sleeps with
`pthread_cond_timedwait` until the earliest deadline across all coders, wakes
up, re-checks, and if someone's past their deadline it prints the burnout right
away. It also gets nudged every time a compile finishes so it can recompute.

## Blocking cases handled

- **Deadlock.** The four Coffman conditions can't all hold because a coder never
  grabs one dongle and then sits there waiting for the second. Grabbing is
  all-or-nothing, done by the scheduler under a single lock: you only get
  granted when *both* your dongles are free. No partial hold, no circular wait.
- **Starvation / liveness.** In FIFO a waiting request keeps its original
  arrival key when it's re-queued, so it never gets overtaken by newer requests.
  In EDF the key is the deadline, which only gets more urgent the longer you
  wait, so a coder in danger naturally climbs to the top. A greedy neighbour
  can't hog the dongles forever.
- **Cooldown.** After release a dongle's `available_at` is pushed
  `dongle_cooldown` ms into the future, and the scheduler treats it as busy
  until then, so it genuinely can't be re-taken during the cooldown.
- **Precise burnout.** The monitor thread sleeps exactly until the next deadline
  and re-checks on wake, so the burnout line lands within the required 10 ms.
- **Log serialization.** Every line goes out under a dedicated output mutex with
  a single `printf`, so two messages can't get scrambled onto the same line.
  Once the sim is stopped, nothing else prints — so nothing ever shows up after
  `burned out`.
- **The `n == 1` edge case.** One coder, one dongle. They can't ever hold two,
  so they take their single dongle and burn out. Handled on its own instead of
  aliasing left and right onto the same dongle.

## Thread synchronization mechanisms

Here are the primitives and what each one is for:

- **`pthread_mutex_t lock`** — one global lock over all the shared state: the
  priority queue, every dongle's `available_at`, each coder's `compile_count`
  and `last_compile_start`, and the `started` / `stop` flags. Since everything
  shared lives under one lock, there's no lock ordering to get wrong, so no
  lock-ordering deadlock.
- **`pthread_mutex_t log_mutex`** — guards stdout so log lines stay atomic.
- **`pthread_cond_t start_cond`** — a start barrier. All threads are created
  first, then wait here until `main` sets `sim_start` and flips `started`.
  Released with `broadcast` so *all* of them wake (a `signal` would only wake
  one and hang the rest).
- **`pthread_cond_t sched_wake`** — the scheduler sleeps on this (or on a timed
  wait until the next dongle frees up), and gets woken when a coder posts a
  request or releases dongles.
- **`pthread_cond_t coder_wake[i]`** — one per coder, so the scheduler can wake
  the exact coder it just granted dongles to.
- **`pthread_cond_t mon_cond`** — lets a finishing compile poke the monitor to
  recompute the earliest deadline.

Every condition variable is used with a proper predicate loop
(`while (!condition) pthread_cond_wait(...)`) under its mutex, so no wakeup gets
lost.

**A couple of concrete race examples.** Two neighbours both wanting the dongle
between them are serialized by the scheduler under `lock` — only one grant ever
writes `available_at`, so the dongle can't be duplicated. The monitor reads
`last_compile_start` under the same `lock` a grant uses to write it, so it never
reads a half-updated deadline. And the `stop` flag being written and read under
the lock means shutdown is clean: every thread sees it, exits, and joins.

## Resources

- POSIX threads man pages: `pthread_create`, `pthread_mutex_lock`,
  `pthread_cond_wait`, `pthread_cond_timedwait`, `gettimeofday`.
- The Dining Philosophers problem (Dijkstra) for the general
  resource-sharing / deadlock-avoidance background.
- Earliest Deadline First (EDF) scheduling for the real-time policy.
- Binary heap / priority queue basics for the scheduler's request queue.

**How AI was used.** I used an AI assistant for two things: helping write and
polish this README, and giving me a second pair of eyes while debugging race
conditions at a few tricky points in the threading logic (chasing down lost
wakeups and ordering issues between the coders and the monitor). Everything it
suggested I read, tested, and rewrote in my own words — I can explain every line
of the code and why it's there.
