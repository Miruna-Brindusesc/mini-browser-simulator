#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 200
#define MAX_PAGES 50

// Structure that describes a web page
typedef struct page {
    int id;
    char url[50];
    char *description;
} page;

// Function to create a new page
page *create_page(int id, const char *url, const char *description) {
    page *p = malloc(sizeof(page));
    p->id = id;
    strncpy(p->url, url, 50);
    p->url[49] = '\0';
    p->description = malloc(strlen(description) + 1);
    strcpy(p->description, description);
    return p;
}

// Frees the memory associated with a page
void free_page(page *p) {
    if (p) {
        free(p->description);
        free(p);
    }
}

// Structures for the stack (back/forward history)
typedef struct stackNode {
    struct page *pg;
    struct stackNode *next;
} stackNode;

typedef struct stack {
    stackNode *top;
} stack;

// Creates an empty stack
stack *create_stack() {
    stack *s = malloc(sizeof(stack));
    s->top = NULL;
    return s;
}

// Adds a page to the stack (push)
void push(stack *s, page *p) {
    stackNode *node = malloc(sizeof(stackNode));
    node->pg = p;
    node->next = s->top;
    s->top = node;
}

// Removes a page from the stack (pop)
page *pop(stack *s) {
    if (s->top == NULL) return NULL;
    stackNode *tmp = s->top;
    page *p = tmp->pg;
    s->top = tmp->next;
    free(tmp);
    return p;
}

// Checks if the stack is empty
int is_empty(stack *s) {
    return s->top == NULL;
}

// Frees all memory associated with a stack
void free_stack(stack *s) {
    while (!is_empty(s)) {
        pop(s);
    }
    free(s);
}

// Tab structure
typedef struct tab {
    int id;
    page *currentPage;
    stack *backwardStack;
    stack *forwardStack;
    struct tab *next, *prev;
} tab;

// Browser structure
typedef struct browser {
    tab *sentinel;
    tab *current;
    int lastID;
} browser;

// Creates a new tab
tab *create_tab(int id, page *defaultPage) {
    tab *t = malloc(sizeof(tab));
    t->id = id;
    t->currentPage = defaultPage;
    t->backwardStack = create_stack();
    t->forwardStack = create_stack();
    t->next = t->prev = NULL;
    return t;
}

// Initializes the browser with a default tab and a sentinel
browser *init_browser(page *defaultPage) {
    browser *b = malloc(sizeof(browser));

    tab *sentinel = malloc(sizeof(tab));
    sentinel->id = -1;  // sentinel - not a real tab
    b->sentinel = sentinel;

    tab *first = create_tab(0, defaultPage);

    // Initial links: sentinel <-> first <-> sentinel (circular)
    sentinel->next = first;
    sentinel->prev = first;
    first->next = sentinel;
    first->prev = sentinel;

    b->current = first;
    b->lastID = 0;
    return b;
}

// Creates and adds a new tab before the sentinel
void new_tab(browser *b, page *defaultPage) {
    tab *t = create_tab(++b->lastID, defaultPage);
    tab *last = b->sentinel->prev;

    // New links: last <-> t <-> sentinel
    t->next = b->sentinel;
    t->prev = last;
    last->next = t;
    b->sentinel->prev = t;

    b->current = t;
}

// Deletes the current tab and reconnects the neighbors
void close_current_tab(browser *b, FILE *out) {
    tab *curr = b->current;

    // The first tab cannot be closed
    if (curr->id == 0) {
        fprintf(out, "403 Forbidden\n");
        return;
    }

    tab *left = curr->prev;
    tab *right = curr->next;

    // Reconnect links: left <-> right
    left->next = right;
    right->prev = left;

    b->current = left;

    free_stack(curr->backwardStack);
    free_stack(curr->forwardStack);
    free(curr);
}

// Displays the tabs and the description of the current page
void print_tabs(browser *b, FILE *out) {
    if (!b || !b->current) return;

    tab *it = b->current;
    do {
        if (it->id != -1) {
            fprintf(out, "%d ", it->id);
        }
        it = it->next;
    } while (it != b->current);
    fprintf(out, "\n");

    if (b->current && b->current->currentPage) {
        fprintf(out, "%s\n", b->current->currentPage->description);
    }
}

// Searches for a tab by id and makes it the current tab
void open_tab_by_id(browser *b, int tab_id, FILE *out) {
    tab *it = b->sentinel->next;
    while (it != b->sentinel) {
        if (it->id == tab_id) {
            b->current = it;
            return;
        }
        it = it->next;
    }
    fprintf(out, "403 Forbidden\n");
}

// Switches to the next tab (skipping the sentinel)
void next_tab(browser *b) {
    if (!b || !b->current) return;

    b->current = b->current->next;
    if (b->current->id == -1) {
        b->current = b->current->next;
    }
}

// Switches to the previous tab (skipping the sentinel)
void prev_tab(browser *b) {
    if (!b || !b->current) return;

    b->current = b->current->prev;
    while (b->current->id == -1) {
        b->current = b->current->prev;
    }
}

// Opens a page in a tab and updates the history
void open_page(browser *b, page **pages, int n, int pid) {
    page *newPage = NULL;
    for (int i = 0; i < n; i++) {
        if (pages[i]->id == pid) {
            newPage = pages[i];
            break;
        }
    }

    if (!newPage) {
        printf("403 Forbidden\n");
        return;
    }

    if (b->current->currentPage)
        push(b->current->backwardStack, b->current->currentPage);

    free_stack(b->current->forwardStack);
    b->current->forwardStack = create_stack();
    b->current->currentPage = newPage;
}

// Displays the full history of a tab
void print_history(browser *b, int tabID, FILE *out) {
    if (!b) return;

    tab *t = b->sentinel->next;
    while (t != b->sentinel) {
        if (t->id == tabID) break;
        t = t->next;
    }

    if (t == b->sentinel) {
        fprintf(out, "403 Forbidden\n");
        return;
    }

    stackNode *fStack = t->forwardStack->top;
    if (!fStack && !t->currentPage && !t->backwardStack->top) {
        fprintf(out, "403 Forbidden\n");
        return;
    }

    stack *temp = create_stack();
    while (fStack) {
        push(temp, fStack->pg);
        fStack = fStack->next;
    }

    while (!is_empty(temp)) {
        fprintf(out, "%s\n", pop(temp)->url);
    }
    free(temp);

    if (t->currentPage)
        fprintf(out, "%s\n", t->currentPage->url);

    stackNode *bStack = t->backwardStack->top;
    while (bStack) {
        fprintf(out, "%s\n", bStack->pg->url);
        bStack = bStack->next;
    }
}

// Go backward in history
void go_backward(browser *b, FILE *out) {
    if (is_empty(b->current->backwardStack)) {
        fprintf(out, "403 Forbidden\n");
        return;
    }

    push(b->current->forwardStack, b->current->currentPage);
    b->current->currentPage = pop(b->current->backwardStack);
}

// Go forward in history
void go_forward(browser *b, FILE *out) {
    if (is_empty(b->current->forwardStack)) {
        fprintf(out, "403 Forbidden\n");
        return;
    }

    push(b->current->backwardStack, b->current->currentPage);
    b->current->currentPage = pop(b->current->forwardStack);
}

// Frees all allocated memory
void free_browser(browser *b) {
    tab *it = b->sentinel->next;
    while (it != b->sentinel) {
        tab *next = it->next;
        free_stack(it->backwardStack);
        free_stack(it->forwardStack);
        free(it);
        it = next;
    }
    free(b->sentinel);
    free(b);
}

// Main: reads commands and calls the corresponding functionality
int main() {
    FILE *in = fopen("tema1.in", "r");
    FILE *out = fopen("tema1.out", "w");
    if (!in || !out) return 1;

    int N;
    fscanf(in, "%d\n", &N);

    page **pages = malloc(N * sizeof(page *));
    for (int i = 0; i < N; i++) {
        int id;
        char url[1000], desc[1000];
        fscanf(in, "%d %s %[^\n]\n", &id, url, desc);
        pages[i] = create_page(id, url, desc);
    }

    page *defaultPage = create_page(0, "https://acs.pub.ro/", "Computer Science");
    browser *b = init_browser(defaultPage);

    char line[MAX_LINE];
    while (fgets(line, MAX_LINE, in)) {
        if (strncmp(line, "NEW_TAB", 7) == 0) new_tab(b, defaultPage);
        else if (strncmp(line, "PRINT", 5) == 0 && strncmp(line, "PRINT_HISTORY", 13) != 0) print_tabs(b, out);
        else if (strncmp(line, "PAGE", 4) == 0) {
            int pid; sscanf(line, "PAGE %d", &pid); open_page(b, pages, N, pid);
        } else if (strncmp(line, "CLOSE", 5) == 0) close_current_tab(b, out);
        else if (strncmp(line, "OPEN", 4) == 0) {
            int tab_id; sscanf(line, "OPEN %d", &tab_id); open_tab_by_id(b, tab_id, out);
        } else if (strncmp(line, "NEXT", 4) == 0) next_tab(b);
        else if (strncmp(line, "PREV", 4) == 0) prev_tab(b);
        else if (strncmp(line, "PRINT_HISTORY", 13) == 0) {
            int id; sscanf(line, "PRINT_HISTORY %d", &id); print_history(b, id, out);
        } else if (strncmp(line, "BACKWARD", 8) == 0) go_backward(b, out);
        else if (strncmp(line, "FORWARD", 7) == 0) go_forward(b, out);
    }

    free_browser(b);
    for (int i = 0; i < N; i++) free_page(pages[i]);
    free_page(defaultPage);
    free(pages);

    fclose(in);
    fclose(out);
    return 0;
}
