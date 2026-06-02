#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <string.h>

// Color pair IDs
#define CLR_TITLE    1
#define CLR_MENU     2
#define CLR_SELECT   3
#define CLR_SUCCESS  4
#define CLR_WARN     5
#define CLR_INPUT    6
#define CLR_BORDER   7

struct user {
    float bmi;
    int dailygoal;
};

// ─── Helpers ────────────────────────────────────────────────────────────────

void draw_border(WINDOW *win) {
    wattron(win, COLOR_PAIR(CLR_BORDER));
    box(win, 0, 0);
    wattroff(win, COLOR_PAIR(CLR_BORDER));
}

void draw_header(WINDOW *win, int width) {
    wattron(win, COLOR_PAIR(CLR_TITLE) | A_BOLD);
    mvwprintw(win, 1, (width - 22) / 2, "  CALORIE TRACKER v1.0  ");
    wattroff(win, COLOR_PAIR(CLR_TITLE) | A_BOLD);

    wattron(win, COLOR_PAIR(CLR_BORDER));
    mvwhline(win, 2, 1, ACS_HLINE, width - 2);
    wattroff(win, COLOR_PAIR(CLR_BORDER));
}

// Read a float from user inside the ncurses window
float read_float(WINDOW *win, int y, int x) {
    char buf[32] = {0};
    echo();
    curs_set(1);
    wattron(win, COLOR_PAIR(CLR_INPUT));
    mvwgetnstr(win, y, x, buf, 10);
    wattroff(win, COLOR_PAIR(CLR_INPUT));
    noecho();
    curs_set(0);
    return atof(buf);
}

int read_int(WINDOW *win, int y, int x) {
    char buf[32] = {0};
    echo();
    curs_set(1);
    wattron(win, COLOR_PAIR(CLR_INPUT));
    mvwgetnstr(win, y, x, buf, 10);
    wattroff(win, COLOR_PAIR(CLR_INPUT));
    noecho();
    curs_set(0);
    return atoi(buf);
}

// Show a result screen with BMI + goal info
void show_result(WINDOW *win, int width, struct user *u, const char *method) {
    wclear(win);
    draw_border(win);
    draw_header(win, width);

    wattron(win, COLOR_PAIR(CLR_SUCCESS) | A_BOLD);
    mvwprintw(win, 4, (width - 16) / 2, "[ SETUP COMPLETE ]");
    wattroff(win, COLOR_PAIR(CLR_SUCCESS) | A_BOLD);

    mvwprintw(win, 6,  4, "Method  : %s", method);
    mvwprintw(win, 7,  4, "BMI     : %.2f", u->bmi);

    // BMI category
    const char *category;
    int cat_color;
    if (u->bmi < 18.5) {
        category = "Underweight";
        cat_color = CLR_WARN;
    } else if (u->bmi <= 24.9) {
        category = "Normal";
        cat_color = CLR_SUCCESS;
    } else {
        category = "Overweight";
        cat_color = CLR_WARN;
    }

    mvwprintw(win, 8,  4, "Status  : ");
    wattron(win, COLOR_PAIR(cat_color) | A_BOLD);
    wprintw(win, "%s", category);
    wattroff(win, COLOR_PAIR(cat_color) | A_BOLD);

    mvwprintw(win, 9,  4, "Goal    : ");
    wattron(win, COLOR_PAIR(CLR_SUCCESS) | A_BOLD);
    wprintw(win, "%d kcal/day", u->dailygoal);
    wattroff(win, COLOR_PAIR(CLR_SUCCESS) | A_BOLD);

    wattron(win, COLOR_PAIR(CLR_BORDER));
    mvwhline(win, 11, 1, ACS_HLINE, width - 2);
    wattroff(win, COLOR_PAIR(CLR_BORDER));

    wattron(win, A_DIM);
    mvwprintw(win, 12, 4, "Press any key to exit...");
    wattroff(win, A_DIM);

    wrefresh(win);
    wgetch(win);
}

// ─── Screens ────────────────────────────────────────────────────────────────

void screen_calculate_bmi(WINDOW *win, int width, struct user *u) {
    wclear(win);
    draw_border(win);
    draw_header(win, width);

    wattron(win, COLOR_PAIR(CLR_TITLE) | A_BOLD);
    mvwprintw(win, 4, 4, "Calculate BMI");
    wattroff(win, COLOR_PAIR(CLR_TITLE) | A_BOLD);

    mvwprintw(win, 6, 4, "Weight (kg) : ");
    wrefresh(win);
    float weight = read_float(win, 6, 18);

    mvwprintw(win, 7, 4, "Height (cm) : ");
    wrefresh(win);
    float height = read_float(win, 7, 18);

    float bmi = weight / ((height / 100) * (height / 100));
    u->bmi = bmi;

    if (bmi < 18.5)       u->dailygoal = 2500;
    else if (bmi <= 24.9) u->dailygoal = 2000;
    else                  u->dailygoal = 1800;

    show_result(win, width, u, "BMI Calculated");
}

void screen_known_bmi(WINDOW *win, int width, struct user *u) {
    wclear(win);
    draw_border(win);
    draw_header(win, width);

    wattron(win, COLOR_PAIR(CLR_TITLE) | A_BOLD);
    mvwprintw(win, 4, 4, "Enter Your BMI");
    wattroff(win, COLOR_PAIR(CLR_TITLE) | A_BOLD);

    mvwprintw(win, 6, 4, "BMI value : ");
    wrefresh(win);
    float bmi = read_float(win, 6, 16);
    u->bmi = bmi;

    if (bmi < 18.5)       u->dailygoal = 2500;
    else if (bmi <= 24.9) u->dailygoal = 2000;
    else                  u->dailygoal = 1800;

    show_result(win, width, u, "BMI Provided");
}

void screen_set_calories(WINDOW *win, int width, struct user *u) {
    wclear(win);
    draw_border(win);
    draw_header(win, width);

    wattron(win, COLOR_PAIR(CLR_TITLE) | A_BOLD);
    mvwprintw(win, 4, 4, "Set Calorie Goal");
    wattroff(win, COLOR_PAIR(CLR_TITLE) | A_BOLD);

    mvwprintw(win, 6, 4, "Daily goal (kcal) : ");
    wrefresh(win);
    int goal = read_int(win, 6, 24);
    u->dailygoal = goal;
    u->bmi = 0;

    show_result(win, width, u, "Manual Goal");
}

// ─── Main Menu ──────────────────────────────────────────────────────────────

int main() {
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);

    // Init colors
    start_color();
    use_default_colors();
    init_pair(CLR_TITLE,   COLOR_CYAN,    -1);
    init_pair(CLR_MENU,    COLOR_WHITE,   -1);
    init_pair(CLR_SELECT,  COLOR_BLACK,   COLOR_CYAN);
    init_pair(CLR_SUCCESS, COLOR_GREEN,   -1);
    init_pair(CLR_WARN,    COLOR_YELLOW,  -1);
    init_pair(CLR_INPUT,   COLOR_WHITE,   -1);
    init_pair(CLR_BORDER,  COLOR_CYAN,    -1);

    int height = 20, width = 50;
    int starty = (LINES - height) / 2;
    int startx = (COLS  - width)  / 2;

    WINDOW *win = newwin(height, width, starty, startx);
    keypad(win, TRUE);

    const char *menu_items[] = {
        "  Calculate BMI from weight & height",
        "  I already know my BMI",
        "  I know my calorie goal",
        "  Edit daily calorie goal",
        "  Exit",
    };
    int n_items = 5;
    int selected = 0;

    struct user user = {0, 0};

    while (1) {
        wclear(win);
        draw_border(win);
        draw_header(win, width);

        // Subtitle
        wattron(win, A_DIM);
        mvwprintw(win, 3, (width - 28) / 2, "Let's start our journey today!");
        wattroff(win, A_DIM);

        // Menu items
        for (int i = 0; i < n_items; i++) {
            if (i == selected) {
                wattron(win, COLOR_PAIR(CLR_SELECT) | A_BOLD);
                mvwprintw(win, 5 + i * 2, 2, "%-46s", menu_items[i]);
                wattroff(win, COLOR_PAIR(CLR_SELECT) | A_BOLD);
            } else {
                wattron(win, COLOR_PAIR(CLR_MENU));
                mvwprintw(win, 5 + i * 2, 2, "%-46s", menu_items[i]);
                wattroff(win, COLOR_PAIR(CLR_MENU));
            }
        }

        // Footer hint
        wattron(win, COLOR_PAIR(CLR_BORDER));
        mvwhline(win, height - 3, 1, ACS_HLINE, width - 2);
        wattroff(win, COLOR_PAIR(CLR_BORDER));
        wattron(win, A_DIM);
        mvwprintw(win, height - 2, 3, "UP/DOWN: navigate   ENTER: select");
        wattroff(win, A_DIM);

        wrefresh(win);

        int ch = wgetch(win);
        if (ch == KEY_UP)   selected = (selected - 1 + n_items) % n_items;
        if (ch == KEY_DOWN) selected = (selected + 1) % n_items;
        if (ch == '\n' || ch == KEY_ENTER) {
            switch (selected) {
                case 0: screen_calculate_bmi(win, width, &user);  break;
                case 1: screen_known_bmi(win, width, &user);      break;
                case 2: screen_set_calories(win, width, &user);   break;
                case 3: screen_set_calories(win, width, &user);   break;
                case 4: goto done;
            }
        }
        if (ch == 'q' || ch == 'Q') goto done;
    }

done:
    delwin(win);
    endwin();
    return 0;
}
