# Mini Browser Simulator

## General Description
This C program simulates a simplified browser with multi-tab functionality. The user can open web pages, navigate forward and backward through history, open and close tabs, switch between tabs, and view browsing history.

The browser is controlled through an input file (`tema1.in`), and the output is written to a file (`tema1.out`).

## Program Structure

### 1. Data Structures

#### page
Represents a web page.

```c
typedef struct page {
    int id;
    char url[50];
    char *description;
} page;
```

#### stack & stackNode
Stack implementation (LIFO), used for Back and Forward navigation.

```C
typedef struct stackNode {
    struct page *pg;
    struct stackNode *next;
} stackNode;
```

```C
typedef struct stack {
    stackNode *top;
} stack;
```

#### tab
Represents a browser tab, containing the current page and history stacks.

```C
typedef struct tab {
    int id;
    page *currentPage;
    stack *backwardStack;
    stack *forwardStack;
    struct tab *next, *prev;
} tab;
```

#### browser
Represents the browser itself. Keeps track of the current tab and all tabs.

```C
typedef struct browser {
    tab *sentinel;
    tab *current;
    int lastID;
} browser;
```

###  2. Functionality

#### Web Pages
Web pages are created using the create_page() function and freed from memory using the free_page() function. Each page contains an identifier (id), a url, and a description.

#### Stacks
Browsing history is managed using stacks. Stack operations are performed through the push(), pop(), is_empty(), and free_stack() functions, which allow adding, removing, checking, and freeing elements from the stack.

#### Tabs
A new tab is created using the create_tab() function. Each tab maintains its own browsing history, both for previously visited pages (backward) and for pages that can be revisited (forward).

#### Browser
The browser is initialized through the init_browser() function, which creates an initial tab and opens a default page.

The new_tab() function allows opening a new tab, and close_current_tab() closes the current tab.

Navigation between tabs is done with next_tab() and prev_tab().

The open_tab_by_id() function sets the tab with a specific identifier as active.

Opening a new page in a tab is done with open_page(), simultaneously updating the browsing history.

The print_tabs() function displays all tabs and the current page, while print_history() displays the browsing history for a specific tab.

History navigation is performed through the go_backward() and go_forward() functions.

Finally, the free_browser() function frees the memory allocated for the browser structures.

### Error Handling
The program handles the following edge cases:
- Attempting to navigate back when no history exists
- Attempting to navigate forward when no forward history exists
- Closing the last remaining tab
- Switching to a non-existent tab ID
- Opening invalid URLs

### Example
**Input (tema1.in):** <br>
0 <br>
9 <br>
NEW_TAB <br>
NEW_TAB <br>
OPEN 1 <br>
NEW_TAB <br>
OPEN 2 <br>
PRINT <br>
NEW_TAB <br>
OPEN 4 <br>
PRINT <br>

**Output (tema1.out):** <br>
2 3 0 1 &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;  # First PRINT output <br>
Computer Science <br>
4 0 1 2 3 &nbsp; &nbsp; &nbsp; &nbsp; # Second PRINT output <br>
Computer Science <br>

### Compilation and Usage

#### Compile:
```bash
make build
```
#### Run:

```bash
make run
```

### Tests
The program includes a comprehensive testing framework with the following components: 

```bash
tests/
├── in/              # Input test files
├── ref/             # Reference output files
└── checker.sh       # Validation script
```

#### Run all tests

```bash
./tests/checker/checker.sh
```

#### Run specific test

```bash
./tests/checker/checker.sh test1
``` 

#### Run with verbose output

```bash
./tests/checker/checker.sh -v all
``` 

#### Requirements for the checker

To run the tests and checker script, you need:

- **GCC** - C compiler
- **Valgrind** - Memory leak detection (optional but recommended)
- **dos2unix** - Line ending conversion (for cross-platform compatibility)

#### Installation
**Ubuntu/Debian:**

```bash
sudo apt-get update
sudo apt-get install gcc valgrind dos2unix
```

**Windows (WSL):**
```bash
sudo apt-get update
sudo apt-get install gcc valgrind dos2unix build-essential
```