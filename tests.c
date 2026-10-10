#define main application_main
#include "main.c"
#undef main
#include <assert.h>

int main(void) {
  double numbers[N];
  int counts[BINS], sum = 0;
  srand(12);
  generate_numbers(numbers, counts);
  for (int i = 0; i < N; i++) assert(numbers[i] >= A && numbers[i] <= B);
  for (int i = 0; i < BINS; i++) sum += counts[i];
  assert(sum == N);
  assert(interval_index(A) == 0);
  assert(interval_index(B) == BINS - 1);
  assert(interval_index(101.0) == 1);
  assert(interval_index(100.999999) == 0);
  puts("PASS: 750 values, range, bin boundaries, total frequency");

  char board[ROWS][COLS] = {{0}};
  assert(!put_mark(board, -1, 0, 'X'));
  assert(!put_mark(board, ROWS, 0, 'X'));
  assert(!put_mark(board, 0, COLS, 'X'));
  assert(put_mark(board, 0, 0, 'X'));
  assert(!put_mark(board, 0, 0, 'O'));
  assert(board[0][0] == 'X');
  puts("PASS: invalid and occupied cells rejected");

  const int dr[] = {0, 1, 1, 1}, dc[] = {1, 0, 1, -1};
  for (int d = 0; d < 4; d++) {
    memset(board, 0, sizeof(board));
    int start_col = d == 3 ? COLS - 1 : 0;
    for (int i = 0; i < 4; i++) board[i * dr[d]][start_col + i * dc[d]] = 'X';
    assert(!has_five(board, 0, start_col));
    board[4 * dr[d]][start_col + 4 * dc[d]] = 'X';
    assert(has_five(board, 2 * dr[d], start_col + 2 * dc[d]));
  }
  memset(board, 0, sizeof(board));
  for (int i = 0; i < 6; i++) board[ROWS - 1][COLS - 1 - i] = 'O';
  assert(has_five(board, ROWS - 1, COLS - 3));
  puts("PASS: four is not a win; five in four directions; six at edge");

  /* Полное поле без пяти подряд — ничья. */
  for (int r = 0; r < ROWS; r++)
    for (int c = 0; c < COLS; c++) board[r][c] = ((r + c / 2) % 2) ? 'X' : 'O';
  for (int r = 0; r < ROWS; r++)
    for (int c = 0; c < COLS; c++) assert(!has_five(board, r, c));
  int row, col;
  assert(!computer_move(board, &row, &col));
  board[3][4] = 0;
  assert(computer_move(board, &row, &col));
  assert(row == 3 && col == 4 && board[3][4] == 'O');
  puts("PASS: drawn board; computer fills the only free cell");
  return 0;
}
