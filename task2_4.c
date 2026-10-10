#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 20
#define WIN_LENGTH 5

/* Чтение ровно двух целых чисел. Конец ввода завершает игру. */
static int read_pair(int *first, int *second)
{
    char line[128];
    for (;;) {
        if (fgets(line, sizeof(line), stdin) == NULL) {
            return 0;
        }
        if (strchr(line, '\n') == NULL && !feof(stdin)) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
            printf("Слишком длинная строка. Повторите ввод: ");
            continue;
        }
        char *end;
        char *start = line;
        long x = strtol(start, &end, 10);
        if (end != start && x >= 1 && x <= MAX_SIZE) {
            start = end;
            long y = strtol(start, &end, 10);
            if (end != start && y >= 1 && y <= MAX_SIZE) {
                end += strspn(end, " \t\r\n");
                if (*end == '\0') {
                    *first = (int)x;
                    *second = (int)y;
                    return 1;
                }
            }
        }
        printf("Введите два целых числа от 1 до %d: ", MAX_SIZE);
    }
}

static void print_board(char board[MAX_SIZE][MAX_SIZE], int rows, int cols)
{
    printf("\n    ");
    for (int c = 0; c < cols; c++) {
        printf("%2d ", c + 1);
    }
    putchar('\n');
    for (int r = 0; r < rows; r++) {
        printf("%3d ", r + 1);
        for (int c = 0; c < cols; c++) {
            printf(" %c ", board[r][c]);
        }
        putchar('\n');
    }
    putchar('\n');
}

/* Проверяем четыре направления через последнюю поставленную фигуру. */
static int has_won(char board[MAX_SIZE][MAX_SIZE], int rows, int cols,
                   int row, int col)
{
    const int directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
    for (int d = 0; d < 4; d++) {
        int count = 1;
        for (int sign = -1; sign <= 1; sign += 2) {
            int dr = directions[d][0] * sign;
            int dc = directions[d][1] * sign;
            int r = row + dr;
            int c = col + dc;
            while (r >= 0 && r < rows && c >= 0 && c < cols &&
                   board[r][c] == board[row][col]) {
                count++;
                r += dr;
                c += dc;
            }
        }
        if (count >= WIN_LENGTH) {
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    char board[MAX_SIZE][MAX_SIZE];
    int rows, cols;

    printf("Крестики-нолики для двух игроков. Побеждают пять фигур подряд.\n");
    printf("Введите количество строк и столбцов (от 5 до %d): ", MAX_SIZE);
    for (;;) {
        if (!read_pair(&rows, &cols)) {
            return 0;
        }
        if (rows >= WIN_LENGTH && cols >= WIN_LENGTH) {
            break;
        }
        printf("Оба размера должны быть от 5 до %d. Повторите: ", MAX_SIZE);
    }

    memset(board, '.', sizeof(board));
    char player = 'X';
    int moves = 0;
    print_board(board, rows, cols);

    while (moves < rows * cols) {
        int row, col;
        printf("Ход %c. Введите строку и столбец: ", player);
        if (!read_pair(&row, &col)) {
            printf("\nИгра завершена: конец ввода.\n");
            return 0;
        }
        row--;
        col--;
        if (row >= rows || col >= cols) {
            printf("Клетка находится за пределами поля.\n");
            continue;
        }
        if (board[row][col] != '.') {
            printf("Клетка занята. Выберите другую.\n");
            continue;
        }

        board[row][col] = player;
        moves++;
        print_board(board, rows, cols);
        if (has_won(board, rows, cols, row, col)) {
            printf("Победил игрок %c!\n", player);
            return 0;
        }
        player = player == 'X' ? 'O' : 'X';
    }

    printf("Ничья! Поле заполнено.\n");
    return 0;
}
