"""Контрольные сценарии консольной игры; запуск: make test."""
from pathlib import Path
import subprocess

program = str(Path('histogram').resolve())

def game(moves):
    return subprocess.run([program, '--game'], input=moves, text=True,
                          capture_output=True, check=True).stdout

result = game('1\nabc\n11 1\n1 1\n1 1\n0 0\n')
assert 'Нужно ввести два целых числа.' in result
assert result.count('Недопустимый ход') == 2
assert result.count('Ход O.') == 2
assert 'Компьютер O:' in game('2\n1 1\n0 0\n')
assert 'Победил X: пять подряд!' in game(Path('report/game_input.txt').read_text())
x, o = [], []
for row in range(10):
    for col in range(15):
        (x if (row + col // 2) % 2 else o).append(f'{row + 1} {col + 1}')
moves = ['1']
for a, b in zip(x, o):
    moves.extend([a, b])
assert 'Ничья: свободных клеток нет.' in game('\n'.join(moves) + '\n')
print('PASS: interactive validation, turn preserved, computer turn, scripted X victory, full drawn game')
