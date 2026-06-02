#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

#define MAX_FOODS 100

// ─── Color pairs ────────────────────────────────────────────────────────────
#define CLR_BORDER   1
#define CLR_TITLE    2
#define CLR_SELECT   3
#define CLR_SUCCESS  4
#define CLR_WARN     5
#define CLR_ERROR    6
#define CLR_DIM      7
#define CLR_INPUT    8
#define CLR_HEADER   9

struct FoodEntry {
    char food[30];
    int  calories;
    char mealtype[15];
    char date[15];
};

// ─── Helpers ─────────────────────────────────────────────────────────────────

void draw_border(WINDOW *w) {
    wattron(w, COLOR_PAIR(CLR_BORDER));
    box(w, 0, 0);
    wattroff(w, COLOR_PAIR(CLR_BORDER));
}

void draw_hline(WINDOW *w, int y, int width) {
    wattron(w, COLOR_PAIR(CLR_BORDER));
    mvwhline(w, y, 1, ACS_HLINE, width - 2);
    wattroff(w, COLOR_PAIR(CLR_BORDER));
}

void draw_header(WINDOW *w, int width, const char *subtitle) {
    wattron(w, COLOR_PAIR(CLR_TITLE) | A_BOLD);
    mvwprintw(w, 1, (width - 20) / 2, "  FOOD TRACKER v1.0  ");
    wattroff(w, COLOR_PAIR(CLR_TITLE) | A_BOLD);
    draw_hline(w, 2, width);
    if (subtitle && subtitle[0]) {
        wattron(w, COLOR_PAIR(CLR_DIM));
        mvwprintw(w, 3, (width - (int)strlen(subtitle)) / 2, "%s", subtitle);
        wattroff(w, COLOR_PAIR(CLR_DIM));
    }
}

// Read a string from the window
void read_str(WINDOW *w, int y, int x, char *buf, int maxlen) {
    echo();
    curs_set(1);
    wattron(w, COLOR_PAIR(CLR_INPUT) | A_UNDERLINE);
    mvwgetnstr(w, y, x, buf, maxlen - 1);
    wattroff(w, COLOR_PAIR(CLR_INPUT) | A_UNDERLINE);
    noecho();
    curs_set(0);
}

// Read an int
int read_int(WINDOW *w, int y, int x) {
    char buf[16] = {0};
    read_str(w, y, x, buf, sizeof(buf));
    return atoi(buf);
}

// Centered status message at bottom
void status_msg(WINDOW *w, int height, int width, const char *msg, int color) {
    draw_hline(w, height - 3, width);
    wattron(w, COLOR_PAIR(color) | A_BOLD);
    mvwprintw(w, height - 2, (width - (int)strlen(msg)) / 2, "%s", msg);
    wattroff(w, COLOR_PAIR(color) | A_BOLD);
}

void footer_hint(WINDOW *w, int height, int width, const char *hint) {
    draw_hline(w, height - 3, width);
    wattron(w, COLOR_PAIR(CLR_DIM));
    mvwprintw(w, height - 2, (width - (int)strlen(hint)) / 2, "%s", hint);
    wattroff(w, COLOR_PAIR(CLR_DIM));
}

// ─── API / Built-ins (unchanged logic) ───────────────────────────────────────



// ─── File I/O (unchanged logic) ───────────────────────────────────────────────
int fetchCaloriesFromAPI(const char *foodName) {
    char command[512];
    char result[64];

    // call your shell script
    snprintf(command, sizeof(command),
             "bash ./calorie.sh \"%s\"", foodName);

    // DEBUG
    printf("Running command: %s\n", command);

    FILE *pipe = popen(command, "r");

    if (!pipe) {
        perror("popen failed");
        return -1;
    }

    // read result from script
    if (fgets(result, sizeof(result), pipe) == NULL) {
        pclose(pipe);
        return -1;
    }

    pclose(pipe);

    // remove newline
    result[strcspn(result, "\n")] = '\0';

    // DEBUG
    printf("Script returned: %s\n", result);

    int cal = atoi(result);

    if (cal <= 0)
        return -1;

    return cal;
}
void saveToFile(struct FoodEntry foods[], int count) {
    FILE *fp = fopen("tracker.txt", "w");
    if (!fp) return;
    for (int i = 0; i < count; i++)
        fprintf(fp, "%s %d %s %s\n",
                foods[i].food, foods[i].calories,
                foods[i].mealtype, foods[i].date);
    fclose(fp);
}

int loadFromFile(struct FoodEntry foods[]) {
    int count = 0;
    FILE *fp = fopen("tracker.txt", "r");
    if (!fp) return 0;
    while (count < MAX_FOODS &&
           fscanf(fp, "%29s %d %14s %14s",
                  foods[count].food, &foods[count].calories,
                  foods[count].mealtype, foods[count].date) == 4)
        count++;
    fclose(fp);
    return count;
}

// ─── Screens ──────────────────────────────────────────────────────────────────

void screen_add_food(WINDOW *w, int h, int wd, struct FoodEntry foods[], int *count) {
    if (*count >= MAX_FOODS) return;

    struct FoodEntry *e = &foods[*count];

    wclear(w); draw_border(w);
    draw_header(w, wd, "Add a new food entry");

    mvwprintw(w, 5,  3, "Food name    : ");
    wrefresh(w);
    read_str(w, 5, 18, e->food, 30);

    // Calorie lookup
    int cal = -1;
    if (strcmp(e->food, "rice") == 0)        { cal = 130; }
    else if (strcmp(e->food, "banana") == 0) { cal = 89;  }
    else if (strcmp(e->food, "roti") == 0)   { cal = 120; }
    if (strcmp(e->food, "rice") == 0)         cal = 130;
    else if (strcmp(e->food, "banana") == 0) cal = 89;
    else if (strcmp(e->food, "chapati") == 0)   cal = 120;
    else if (strcmp(e->food, "potato") == 0) cal = 77;
    else if (strcmp(e->food, "egg") == 0)    cal = 155;
    else if (strcmp(e->food, "milk") == 0)   cal = 42;
    else if (strcmp(e->food, "apple") == 0)  cal = 52;
    else if (strcmp(e->food, "bread") == 0)  cal = 265;
    else if (strcmp(e->food, "paneer") == 0) cal = 265;
    else {
        wclear(w); draw_border(w);
        draw_header(w, wd, "Add a new food entry");
        mvwprintw(w, 5, 3, "Food name    : %s", e->food);
        wattron(w, COLOR_PAIR(CLR_WARN));
        mvwprintw(w, 7, 3, "Searching online for \"%s\"...", e->food);
        wattroff(w, COLOR_PAIR(CLR_WARN));
        wrefresh(w);
        cal = fetchCaloriesFromAPI(e->food);
    }

    wclear(w); draw_border(w);
    draw_header(w, wd, "Add a new food entry");
    mvwprintw(w, 5, 3, "Food name    : %s", e->food);

    if (cal > 0) {
        e->calories = cal;
        wattron(w, COLOR_PAIR(CLR_SUCCESS));
        mvwprintw(w, 7, 3, "Calories     : %d kcal", cal);
        wattroff(w, COLOR_PAIR(CLR_SUCCESS));
    } else {
        wattron(w, COLOR_PAIR(CLR_WARN));
        mvwprintw(w, 7, 3, "Not found. Enter calories : ");
        wattroff(w, COLOR_PAIR(CLR_WARN));
        wrefresh(w);
        e->calories = read_int(w, 7, 31);
    }

    wattron(w, COLOR_PAIR(CLR_DIM));
    mvwprintw(w, 9,  3, "Meal type (breakfast/lunch/dinner/snack):");
    wattroff(w, COLOR_PAIR(CLR_DIM));
    mvwprintw(w, 10, 3, "> ");
    wrefresh(w);
    read_str(w, 10, 5, e->mealtype, 15);

    wattron(w, COLOR_PAIR(CLR_DIM));
    mvwprintw(w, 12, 3, "Date (dd-mm-yyyy):");
    wattroff(w, COLOR_PAIR(CLR_DIM));
    mvwprintw(w, 13, 3, "> ");
    wrefresh(w);
    read_str(w, 13, 5, e->date, 15);

    (*count)++;
    saveToFile(foods, *count);

    status_msg(w, h, wd, "  Food added!  Press any key...", CLR_SUCCESS);
    wrefresh(w);
    wgetch(w);
}

void screen_show_foods(WINDOW *w, int h, int wd, struct FoodEntry foods[], int count) {
    int scroll = 0;
    int visible = h - 8; // rows available for entries

    while (1) {
        wclear(w); draw_border(w);
        draw_header(w, wd, "All food entries");

        if (count == 0) {
            wattron(w, COLOR_PAIR(CLR_WARN));
            mvwprintw(w, h / 2, (wd - 20) / 2, "No food entries yet.");
            wattroff(w, COLOR_PAIR(CLR_WARN));
        } else {
            // Column headers
            wattron(w, COLOR_PAIR(CLR_HEADER) | A_BOLD);
            mvwprintw(w, 4, 2,  "%-3s %-16s %6s  %-10s %-12s",
                      "#", "Food", "kcal", "Meal", "Date");
            wattroff(w, COLOR_PAIR(CLR_HEADER) | A_BOLD);
            draw_hline(w, 5, wd);

            int shown = 0;
            for (int i = scroll; i < count && shown < visible; i++, shown++) {
                int row = 6 + shown;
                // Color by meal type
                int color = CLR_DIM;
                if (strcmp(foods[i].mealtype, "breakfast") == 0) color = CLR_SUCCESS;
                else if (strcmp(foods[i].mealtype, "lunch")  == 0) color = CLR_TITLE;
                else if (strcmp(foods[i].mealtype, "dinner") == 0) color = CLR_WARN;
                else if (strcmp(foods[i].mealtype, "snack")  == 0) color = CLR_ERROR;

                wattron(w, COLOR_PAIR(color));
                mvwprintw(w, row, 2, "%-3d %-16s %6d  %-10s %-12s",
                          i + 1, foods[i].food, foods[i].calories,
                          foods[i].mealtype, foods[i].date);
                wattroff(w, COLOR_PAIR(color));
            }

            // Scroll indicator
            if (count > visible) {
                wattron(w, COLOR_PAIR(CLR_DIM));
                mvwprintw(w, h - 4, wd - 18, "[%d-%d of %d]",
                          scroll + 1,
                          (scroll + visible < count) ? scroll + visible : count,
                          count);
                wattroff(w, COLOR_PAIR(CLR_DIM));
            }
        }

        footer_hint(w, h, wd, "UP/DOWN: scroll   Q: back");
        wrefresh(w);

        int ch = wgetch(w);
        if (ch == KEY_UP   && scroll > 0) scroll--;
        if (ch == KEY_DOWN && scroll + visible < count) scroll++;
        if (ch == 'q' || ch == 'Q') break;
    }
}

void screen_total(WINDOW *w, int h, int wd,
                  struct FoodEntry foods[], int count, int goal) {
    wclear(w); draw_border(w);
    draw_header(w, wd, "Calorie Summary");

    if (count == 0) {
        wattron(w, COLOR_PAIR(CLR_WARN));
        mvwprintw(w, h / 2, (wd - 22) / 2, "No entries to calculate.");
        wattroff(w, COLOR_PAIR(CLR_WARN));
        footer_hint(w, h, wd, "Press any key...");
        wrefresh(w); wgetch(w);
        return;
    }

    // Breakdown by meal
    int totals[4] = {0};   // breakfast, lunch, dinner, snack
    const char *labels[4] = {"breakfast", "lunch", "dinner", "snack"};
    int grand = 0;
    for (int i = 0; i < count; i++) {
        grand += foods[i].calories;
        for (int m = 0; m < 4; m++)
            if (strcmp(foods[i].mealtype, labels[m]) == 0)
                totals[m] += foods[i].calories;
    }

    int colors[4] = {CLR_SUCCESS, CLR_TITLE, CLR_WARN, CLR_ERROR};
    for (int m = 0; m < 4; m++) {
        wattron(w, COLOR_PAIR(colors[m]));
        mvwprintw(w, 5 + m * 2, 4, "%-12s : %d kcal", labels[m], totals[m]);
        wattroff(w, COLOR_PAIR(colors[m]));
    }

    draw_hline(w, 13, wd);

    wattron(w, A_BOLD);
    mvwprintw(w, 14, 4, "Total consumed : %d kcal", grand);
    wattroff(w, A_BOLD);

    if (goal > 0) {
        int diff = goal - grand;
        mvwprintw(w, 15, 4, "Daily goal     : %d kcal", goal);
        if (diff >= 0) {
            wattron(w, COLOR_PAIR(CLR_SUCCESS) | A_BOLD);
            mvwprintw(w, 16, 4, "Remaining      : %d kcal  ", diff);
            wattroff(w, COLOR_PAIR(CLR_SUCCESS) | A_BOLD);
        } else {
            wattron(w, COLOR_PAIR(CLR_ERROR) | A_BOLD);
            mvwprintw(w, 16, 4, "Over by        : %d kcal! ", -diff);
            wattroff(w, COLOR_PAIR(CLR_ERROR) | A_BOLD);
        }

        // Progress bar
        int bar_w = wd - 10;
        float pct = (float)grand / goal;
        if (pct > 1.0f) pct = 1.0f;
        int filled = (int)(pct * bar_w);
        mvwprintw(w, 18, 4, "[");
        wattron(w, COLOR_PAIR(grand > goal ? CLR_ERROR : CLR_SUCCESS) | A_BOLD);
        for (int i = 0; i < bar_w; i++)
            wprintw(w, i < filled ? "#" : "-");
        wattroff(w, COLOR_PAIR(grand > goal ? CLR_ERROR : CLR_SUCCESS) | A_BOLD);
        wprintw(w, "] %d%%", (int)(pct * 100));
    }

    footer_hint(w, h, wd, "Press any key...");
    wrefresh(w);
    wgetch(w);
}

void screen_delete(WINDOW *w, int h, int wd,
                   struct FoodEntry foods[], int *count) {
    wclear(w); draw_border(w);
    draw_header(w, wd, "Delete a food entry");

    if (*count == 0) {
        wattron(w, COLOR_PAIR(CLR_WARN));
        mvwprintw(w, h / 2, (wd - 18) / 2, "Nothing to delete.");
        wattroff(w, COLOR_PAIR(CLR_WARN));
        footer_hint(w, h, wd, "Press any key...");
        wrefresh(w); wgetch(w);
        return;
    }

    mvwprintw(w, 5, 3, "Food name to delete : ");
    wrefresh(w);
    char target[30] = {0};
    read_str(w, 5, 25, target, 30);

    int found = 0;
    for (int i = 0; i < *count; i++) {
        if (strcmp(foods[i].food, target) == 0) {
            for (int j = i; j < *count - 1; j++)
                foods[j] = foods[j + 1];
            (*count)--;
            saveToFile(foods, *count);
            found = 1;
            break;
        }
    }

    if (found)
        status_msg(w, h, wd, "  Deleted! Press any key...", CLR_SUCCESS);
    else
        status_msg(w, h, wd, "  Not found. Press any key...", CLR_ERROR);

    wrefresh(w);
    wgetch(w);
}

// ─── Main Menu ────────────────────────────────────────────────────────────────

int main() {
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);

    start_color();
    use_default_colors();
    init_pair(CLR_BORDER,  COLOR_CYAN,    -1);
    init_pair(CLR_TITLE,   COLOR_CYAN,    -1);
    init_pair(CLR_SELECT,  COLOR_BLACK,   COLOR_CYAN);
    init_pair(CLR_SUCCESS, COLOR_GREEN,   -1);
    init_pair(CLR_WARN,    COLOR_YELLOW,  -1);
    init_pair(CLR_ERROR,   COLOR_RED,     -1);
    init_pair(CLR_DIM,     COLOR_WHITE,   -1);
    init_pair(CLR_INPUT,   COLOR_WHITE,   -1);
    init_pair(CLR_HEADER,  COLOR_MAGENTA, -1);

    int height = 24, width = 58;

    // Guard: terminal too small
    if (LINES < height || COLS < width) {
        endwin();
        fprintf(stderr, "Terminal too small! Need at least %dx%d. Currently %dx%d\n",
                width, height, COLS, LINES);
        return 1;
    }

    int sy = (LINES - height) / 2;
    int sx = (COLS  - width)  / 2;
    WINDOW *win = newwin(height, width, sy, sx);
    keypad(win, TRUE);

    struct FoodEntry foods[MAX_FOODS];
    int count = loadFromFile(foods);
    int goal  = 0; // can be set from calorie.c flow if integrated

    const char *menu_items[] = {
        "  Add Food",
        "  Show All Foods",
        "  Calorie Summary",
        "  Delete Food",
        "  Exit & Save",
    };
    int n_items = 5;
    int selected = 0;

    while (1) {
        // Total for live display
        int total = 0;
        for (int i = 0; i < count; i++) total += foods[i].calories;

        wclear(win);
        draw_border(win);
        draw_header(win, width, NULL);

        // Live stat bar
        wattron(win, COLOR_PAIR(CLR_DIM));
        mvwprintw(win, 3, 3, "Entries: %-3d", count);
        wattroff(win, COLOR_PAIR(CLR_DIM));
        if (total > 0) {
            int col = total > 2000 ? CLR_ERROR : CLR_SUCCESS;
            wattron(win, COLOR_PAIR(col) | A_BOLD);
            mvwprintw(win, 3, width - 18, "Today: %d kcal", total);
            wattroff(win, COLOR_PAIR(col) | A_BOLD);
        }
        draw_hline(win, 4, width);

        // Menu
        for (int i = 0; i < n_items; i++) {
            if (i == selected) {
                wattron(win, COLOR_PAIR(CLR_SELECT) | A_BOLD);
                mvwprintw(win, 6 + i * 2, 2, "%-54s", menu_items[i]);
                wattroff(win, COLOR_PAIR(CLR_SELECT) | A_BOLD);
            } else {
                mvwprintw(win, 6 + i * 2, 2, "%-54s", menu_items[i]);
            }
        }

        footer_hint(win, height, width, "UP/DOWN: navigate   ENTER: select   Q: quit");
        wrefresh(win);

        int ch = wgetch(win);
        if (ch == KEY_UP)   selected = (selected - 1 + n_items) % n_items;
        if (ch == KEY_DOWN) selected = (selected + 1) % n_items;
        if (ch == '\n' || ch == KEY_ENTER) {
            switch (selected) {
                case 0: screen_add_food(win, height, width, foods, &count);         break;
                case 1: screen_show_foods(win, height, width, foods, count);        break;
                case 2: screen_total(win, height, width, foods, count, goal);       break;
                case 3: screen_delete(win, height, width, foods, &count);           break;
                case 4: goto done;
            }
        }
        if (ch == 'q' || ch == 'Q') goto done;
    }

done:
    saveToFile(foods, count);
    delwin(win);
    endwin();

    printf("alvidaaaaaa dostooooo\n"); // keeping your exit message :)
    return 0;
}
