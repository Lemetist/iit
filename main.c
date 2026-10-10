#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xutil.h>

/* Сборка: gcc main.c -o histogram $(pkg-config --cflags --libs cairo x11) */
#define WINDOW_WIDTH 1000
#define N 750
#define BINS 10
#define A 100.0
#define B 110.0
#define ROWS 10
#define COLS 15

static void label(cairo_t *cr, double x, double y, const char *text) {
  cairo_move_to(cr, x, y);
  cairo_show_text(cr, text);
}

static void draw_histogram(cairo_t *cr, int width, int height,
                           const int *counts, int n, int k, double a, double b) {
  char text[256];
  double expected = (double)n / k;
  int maximum = 1;
  for (int i = 0; i < k; i++) {
    if (counts[i] > maximum) maximum = counts[i];
  }

  /* Используем одинаковые координаты для окна и PNG. */
  cairo_save(cr);
  cairo_scale(cr, (double)width / WINDOW_WIDTH,
              (double)height / (160 + 26 * k));
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_paint(cr);
  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                        CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_source_rgb(cr, 0.12, 0.16, 0.22);
  cairo_set_font_size(cr, 22);
  label(cr, 24, 32, "Распределение случайных чисел");
  cairo_set_font_size(cr, 14);
  snprintf(text, sizeof(text), "N = %d    Интервалы: %d    Ожидается N / k = %.2f", n, k, expected);
  label(cr, 24, 59, text);

  const double left = 360, top = 90, plot_width = 540;
  double scale = plot_width / maximum;
  for (int tick = 0; tick <= 5; tick++) {
    double x = left + plot_width * tick / 5;
    cairo_set_source_rgb(cr, 0.88, 0.90, 0.93);
    cairo_move_to(cr, x, top);
    cairo_line_to(cr, x, top + 26 * k);
    cairo_stroke(cr);
    cairo_set_source_rgb(cr, 0.30, 0.33, 0.38);
    snprintf(text, sizeof(text), "%.1f", (double)maximum * tick / 5);
    label(cr, x - 8, top + 26 * k + 20, text);
  }

  for (int i = 0; i < k; i++) {
    double lo = a + (b - a) * ((double)i / k);
    double hi = i == k - 1 ? b : a + (b - a) * ((double)(i + 1) / k);
    double y = top + 26 * i;
    cairo_set_source_rgb(cr, 0.12, 0.16, 0.22);
    snprintf(text, sizeof(text), "[%.7g; %.7g%s", lo, hi, i == k - 1 ? "]" : ")");
    label(cr, 24, y + 16, text);
    cairo_set_source_rgb(cr, 0.20, 0.48, 0.82);
    cairo_rectangle(cr, left, y + 2, counts[i] * scale, 19);
    cairo_fill(cr);
    cairo_set_source_rgb(cr, 0.12, 0.16, 0.22);
    snprintf(text, sizeof(text), "%d", counts[i]);
    label(cr, left + counts[i] * scale + 8, y + 16, text);
  }

  /* Красная пунктирная линия — ожидаемое число попаданий. */
  double dash[] = {5, 4};
  cairo_set_source_rgb(cr, 0.82, 0.22, 0.18);
  cairo_set_line_width(cr, 2);
  cairo_set_dash(cr, dash, 2, 0);
  cairo_move_to(cr, left + expected * scale, top);
  cairo_line_to(cr, left + expected * scale, top + 26 * k);
  cairo_stroke(cr);
  label(cr, 24, top + 26 * k + 48, "Красная линия: N / k. Близкие длины столбцов указывают на равномерность.");
  cairo_restore(cr);
}

static int show_window(const int *counts, int n, int k, double a, double b) {
  Display *display = XOpenDisplay(NULL);
  if (display == NULL) {
    fprintf(stderr, "Не удалось открыть окно X11. График сохранён в histogram.png.\n");
    return 1;
  }
  int screen = DefaultScreen(display);
  int width = WINDOW_WIDTH, height = 160 + 26 * k;
  Window window = XCreateSimpleWindow(display, RootWindow(display, screen),
      0, 0, width, height, 0, BlackPixel(display, screen), WhitePixel(display, screen));
  XStoreName(display, window, "Random distribution histogram");
  XSelectInput(display, window, ExposureMask | StructureNotifyMask | KeyPressMask);
  Atom close_message = XInternAtom(display, "WM_DELETE_WINDOW", False);
  Atom protocols = XInternAtom(display, "WM_PROTOCOLS", False);
  XSetWMProtocols(display, window, &close_message, 1);
  XMapWindow(display, window);
  cairo_surface_t *surface = cairo_xlib_surface_create(
      display, window, DefaultVisual(display, screen), width, height);
  cairo_t *cr = cairo_create(surface);

  for (;;) {
    XEvent event;
    XNextEvent(display, &event);
    if ((event.type == ClientMessage && event.xclient.message_type == protocols &&
         (Atom)event.xclient.data.l[0] == close_message) ||
        (event.type == KeyPress && XLookupKeysym(&event.xkey, 0) == XK_Escape)) break;
    if (event.type == ConfigureNotify) {
      width = event.xconfigure.width;
      height = event.xconfigure.height;
      cairo_xlib_surface_set_size(surface, width, height);
    }
    if (event.type == Expose || event.type == ConfigureNotify) {
      draw_histogram(cr, width, height, counts, n, k, a, b);
      cairo_surface_flush(surface);
      XFlush(display);
    }
  }
  int failed = cairo_status(cr) != CAIRO_STATUS_SUCCESS;
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
  XDestroyWindow(display, window);
  XCloseDisplay(display);
  return failed;
}

/* b включается только в последний интервал. */
static int interval_index(double value) {
  int index = (int)((value - A) / (B - A) * BINS);
  return index >= BINS ? BINS - 1 : index;
}

static void generate_numbers(double numbers[N], int counts[BINS]) {
  for (int i = 0; i < BINS; i++) counts[i] = 0;
  for (int i = 0; i < N; i++) {
    numbers[i] = A + (B - A) * ((double)rand() / RAND_MAX);
    counts[interval_index(numbers[i])]++;
  }
}

static int run_histogram(int png_only) {
  double numbers[N];
  int counts[BINS];
  generate_numbers(numbers, counts);
  printf("Вариант 12: N = %d, [%.0f; %.0f], интервалов: %d\n", N, A, B, BINS);
  for (int i = 0; i < BINS; i++) {
    printf("[%.0f; %.0f%s %d\n", A + i, A + i + 1,
           i == BINS - 1 ? "]" : ")", counts[i]);
  }
  printf("Ожидаемое количество в каждом интервале: %.0f\n", (double)N / BINS);

  cairo_surface_t *png = cairo_image_surface_create(
      CAIRO_FORMAT_ARGB32, WINDOW_WIDTH, 160 + 26 * BINS);
  cairo_t *cr = cairo_create(png);
  draw_histogram(cr, WINDOW_WIDTH, 160 + 26 * BINS, counts, N, BINS, A, B);
  cairo_status_t status = cairo_status(cr);
  if (status == CAIRO_STATUS_SUCCESS)
    status = cairo_surface_write_to_png(png, "histogram.png");
  cairo_destroy(cr);
  cairo_surface_destroy(png);
  if (status != CAIRO_STATUS_SUCCESS) {
    fprintf(stderr, "Ошибка сохранения: %s\n", cairo_status_to_string(status));
    return 1;
  }
  printf("График сохранён в histogram.png.\n");
  return png_only ? 0 : show_window(counts, N, BINS, A, B);
}

/* Нулевой элемент означает свободную клетку. */
static int put_mark(char board[ROWS][COLS], int row, int col, char mark) {
  if (row < 0 || row >= ROWS || col < 0 || col >= COLS || board[row][col])
    return 0;
  board[row][col] = mark;
  return 1;
}

static int has_five(char board[ROWS][COLS], int row, int col) {
  const int directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
  char mark = board[row][col];
  if (!mark) return 0;
  for (int d = 0; d < 4; d++) {
    int count = 1;
    for (int sign = -1; sign <= 1; sign += 2) {
      int r = row + sign * directions[d][0];
      int c = col + sign * directions[d][1];
      while (r >= 0 && r < ROWS && c >= 0 && c < COLS && board[r][c] == mark) {
        count++;
        r += sign * directions[d][0];
        c += sign * directions[d][1];
      }
    }
    if (count >= 5) return 1;
  }
  return 0;
}

/* Выбираем случайную свободную клетку. */
static int computer_move(char board[ROWS][COLS], int *row, int *col) {
  int free_cells[ROWS * COLS], count = 0;
  for (int r = 0; r < ROWS; r++)
    for (int c = 0; c < COLS; c++)
      if (!board[r][c]) free_cells[count++] = r * COLS + c;
  if (count == 0) return 0;
  int cell = free_cells[rand() % count];
  *row = cell / COLS;
  *col = cell % COLS;
  return put_mark(board, *row, *col, 'O');
}

static void print_board(char board[ROWS][COLS]) {
  printf("\n    ");
  for (int c = 1; c <= COLS; c++) printf("%3d", c);
  putchar('\n');
  for (int r = 0; r < ROWS; r++) {
    printf("%3d ", r + 1);
    for (int c = 0; c < COLS; c++)
      printf("%3c", board[r][c] ? board[r][c] : '.');
    putchar('\n');
  }
}

static int run_game(void) {
  char board[ROWS][COLS] = {{0}};
  char line[128], extra;
  int mode;
  printf("Крестики-нолики: пять подряд. Поле %d x %d.\n", ROWS, COLS);
  for (;;) {
    printf("Режим: 1 — два игрока, 2 — против компьютера: ");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) return 0;
    if (sscanf(line, "%d %c", &mode, &extra) == 1 && (mode == 1 || mode == 2)) break;
    printf("Введите 1 или 2.\n");
  }
  char turn = 'X';
  int moves = 0;
  print_board(board);
  while (moves < ROWS * COLS) {
    int row, col;
    if (mode == 2 && turn == 'O') {
      if (!computer_move(board, &row, &col)) break;
      printf("Компьютер O: %d %d\n", row + 1, col + 1);
    } else {
      printf("Ход %c. Введите строку и столбец (0 0 — выход): ", turn);
      fflush(stdout);
      if (!fgets(line, sizeof(line), stdin)) return 0;
      if (sscanf(line, "%d %d %c", &row, &col, &extra) != 2) {
        printf("Нужно ввести два целых числа.\n");
        continue;
      }
      if (row == 0 && col == 0) return 0;
      if (row < 1 || row > ROWS || col < 1 || col > COLS ||
          !put_mark(board, row - 1, col - 1, turn)) {
        printf("Недопустимый ход: клетка занята или находится вне поля.\n");
        continue;
      }
      row--;
      col--;
    }
    moves++;
    print_board(board);
    if (has_five(board, row, col)) {
      printf("Победил %c: пять подряд!\n", turn);
      return 0;
    }
    turn = turn == 'X' ? 'O' : 'X';
  }
  printf("Ничья: свободных клеток нет.\n");
  return 0;
}

int main(int argc, char **argv) {
  int task = 0, png_only = 0;
  unsigned int seed = (unsigned int)time(NULL);
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--png") == 0) { task = 1; png_only = 1; }
    else if (strcmp(argv[i], "--histogram") == 0) task = 1;
    else if (strcmp(argv[i], "--game") == 0) task = 2;
    else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
      char extra;
      if (sscanf(argv[++i], "%u %c", &seed, &extra) != 1) return 1;
    } else {
      fprintf(stderr, "Использование: %s [--histogram | --png | --game] [--seed число]\n", argv[0]);
      return 1;
    }
  }
  srand(seed);
  if (task == 0) {
    char line[128], extra;
    printf("Вариант 12\n1 — гистограмма\n2 — крестики-нолики\nВыберите задачу: ");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin) ||
        sscanf(line, "%d %c", &task, &extra) != 1 || (task != 1 && task != 2)) {
      fprintf(stderr, "Нужно выбрать задачу 1 или 2.\n");
      return 1;
    }
  }
  return task == 1 ? run_histogram(png_only) : run_game();
}
