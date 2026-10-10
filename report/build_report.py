"""Создаёт HTML и DOCX отчёта из исходника и результатов контрольного запуска."""
from pathlib import Path
from html import escape
from xml.sax.saxutils import escape as xml
import base64
import struct
import zipfile

folder = Path(__file__).resolve().parent
root = folder.parent
blocks = []
def p(text): blocks.append(('p', text))
def h(text): blocks.append(('h', text))
def code(text): blocks.append(('code', text))
def table(rows): blocks.append(('table', rows))
def image(name, caption): blocks.append(('image', (name, caption)))
def page(): blocks.append(('page', ''))

blocks.append(('title', 'ОБРАЗОВАТЕЛЬНАЯ ОРГАНИЗАЦИЯ: ____________________'))
blocks.append(('title', 'Кафедра / отделение: ____________________'))
p('\n\n')
blocks.append(('title', 'ОТЧЁТ ПО ЛАБОРАТОРНОЙ РАБОТЕ'))
blocks.append(('title', 'РАЗРАБОТКА АЛГОРИТМОВ ГЕНЕРАЦИИ ПСЕВДОСЛУЧАЙНЫХ ПОСЛЕДОВАТЕЛЬНОСТЕЙ'))
blocks.append(('title', 'Вариант 12'))
p('\n')
p('Дисциплина: ____________________')
p('Выполнил(а): ____________________')
p('Группа: ____________________')
p('Проверил(а): ____________________')
p('\n')
blocks.append(('title', 'Город: ____________________     2026 г.'))
page()
h('1. Постановка задачи')
p('Цель работы — изучить генерацию псевдослучайных последовательностей с помощью srand() и rand(), оценку равномерности по частотам и применение генератора в игровой программе.')
p('Задача 1, вариант 12. Сформировать массив из N = 750 чисел в интервале [100; 110]. Разбить его на 10 равных интервалов длиной 1, подсчитать частоты попаданий и построить горизонтальную гистограмму. В программе используется графическая библиотека Cairo; окно выводится через X11, график также сохраняется в PNG.')
p('Задача 2.4. Реализовать «Крестики-нолики» на прямоугольном поле. Побеждает игрок, первым выстроивший пять одинаковых символов подряд по горизонтали, вертикали или диагонали. Выбрано поле 10 строк × 15 столбцов. Реализованы два режима: два человека и человек против компьютера. X ходит первым; компьютер играет за O и выбирает случайную свободную клетку. Заполненное поле без победителя означает ничью.')
p('Среда реализации: язык C, GCC, Linux, Cairo и X11. Функции srand(), rand() и time() доступны в C; перевод программы на C++ для выполнения условия не требуется.')
h('2. Алгоритм решения задачи')
h('2.1. Генерация массива и построение гистограммы')
p('1. Инициализировать генератор вызовом srand((unsigned int)time(NULL)). Для воспроизводимого контрольного запуска передать --seed 12.')
p('2. Для каждого из 750 элементов вычислить x = 100 + 10 · rand() / RAND_MAX, выполняя деление в типе double. Возможные значения включают обе границы.')
p('3. Обнулить 10 счётчиков. Для каждого x вычислить j = floor((x − 100) / 10 · 10). Если j = 10, заменить его на 9, чтобы значение 110 попало в последний интервал. Увеличить соответствующий счётчик.')
p('4. Интервалы имеют вид [100; 101), [101; 102), …, [109; 110]. Каждое число учитывается ровно один раз. Контроль: сумма частот должна равняться 750.')
p('5. Построить горизонтальные столбцы: их длина пропорциональна частоте. Подписать границы интервалов и количества чисел. Отметить красной пунктирной линией ожидаемую частоту N / 10 = 75. Сохранить histogram.png и открыть окно графика.')
p('6. Сравнить частоты с 75. Небольшие различия естественны для конечной случайной выборки. Гистограмма даёт визуальную оценку, но сама по себе не доказывает равномерность или независимость последовательности.')
h('2.2. Игра «Крестики-нолики»')
p('1. Создать пустой массив поля 10 × 15; запросить режим игры, назначить X первым игроком.')
p('2. Для человека прочитать строку и столбец с нумерацией от 1. Проверить формат ввода, границы поля и свободна ли клетка. При ошибке повторить запрос, сохранив очередь игрока. Команда 0 0 завершает игру.')
p('3. Для компьютера составить список свободных клеток, выбрать элемент по индексу rand() % count и поставить O. Такой компьютер делает допустимые случайные ходы, но не использует выигрышную стратегию.')
p('4. После хода вывести поле. Проверить четыре направления: (0, 1), (1, 0), (1, 1), (1, −1). В каждом направлении считать непрерывную цепочку одинаковых символов в обе стороны от последнего хода, включая сам ход.')
p('5. Если длина цепочки не меньше пяти, объявить победителя. Иначе при заполненном поле объявить ничью. Если игра продолжается, сменить игрока и повторить шаг 2 или 3.')
h('2.3. Сборка и запуск')
code('make\n./histogram')
p('В меню выбрать 1 для гистограммы или 2 для игры. При выборе игры указать 1 для двух игроков или 2 для игры против компьютера.')
p('Прямые команды запуска:')
code('./histogram --histogram\n./histogram --png\n./histogram --game\nmake test')
p('Команда --png создаёт изображение без открытия окна. Закрыть окно гистограммы можно кнопкой закрытия или клавишей Escape. Альтернативная сборка без Makefile:')
code('gcc main.c -o histogram $(pkg-config --cflags --libs cairo x11)')
p('Обычной команды gcc main.c недостаточно: требуется указать заголовки и библиотеки Cairo/X11. Они уже доступны в использованной среде.')
page()
h('3. Контрольный тест')
h('3.1. Гистограмма')
code('./histogram --png --seed 12')
p('Начальное значение генератора — 12. В одной и той же реализации rand() такой запуск воспроизводится. В другой стандартной библиотеке конкретная последовательность может отличаться.')
counts = [90,78,76,75,71,65,81,68,75,71]
table([['Интервал','Ожидается','Получено']] + [[f'[{100+i}; {101+i}{"]" if i==9 else ")"}', '75', str(c)] for i,c in enumerate(counts)] + [['Итого','750','750']])
p('Результат: минимум частоты — 65, максимум — 90. Максимальное абсолютное отклонение от 75 составляет 15 чисел, или 20% ожидаемой частоты. Все значения лежат в [100; 110], пропущенных и повторно учтённых элементов нет. На гистограмме нет резко выделяющихся пустых или переполненных интервалов; результат согласуется с приблизительно равномерным распределением этой выборки.')
h('3.2. Контрольная партия')
p('Режим: два игрока. Поочерёдно вводятся следующие координаты (строка, столбец):')
table([['Ход','Игрок','Координаты']] + [[str(i+1), 'X' if i%2==0 else 'O', pair] for i,pair in enumerate(['1 1','2 1','1 2','2 2','1 3','2 3','1 4','2 4','1 5'])])
p('Ожидаемый результат: после девятого хода в первой строке стоят пять X подряд, программа объявляет победу X. Фактический результат: «Победил X: пять подряд!». Результаты совпали.')
code('./histogram --game < report/game_input.txt')
h('3.3. Дополнительные проверки')
table([['Проверка','Ожидаемый и полученный результат'],
['Границы интервалов','100 → первый; 101 → второй; 110 → последний'],
['Четыре символа подряд','Победы нет'],
['Пять в четырёх направлениях','Победа распознаётся в каждом случае'],
['Шесть подряд у края','Победа распознаётся'],
['Занятая клетка и выход за поле','Ход отклонён, очередь сохранена'],
['Неверный формат ввода','Повторный запрос координат'],
['Единственная свободная клетка','Компьютер занимает её'],
['Полное поле без пяти подряд','Ничья'],
['Контрольная партия из 9 ходов','Победа X']])
p('Проверки выполнены программами tests.c и tests_integration.py. Команда make test завершилась успешно. Сборка выполнена с -Wall -Wextra -pedantic; для модульных тестов включён -Werror.')
page()
h('4. Скриншоты работы программы')
image('histogram_screenshot.png', 'Рисунок 1 — окно гистограммы, N = 750, [100; 110], seed = 12.')
image('game_screenshot.png', 'Рисунок 2 — итог контрольной партии: пять X в первой строке и сообщение о победе.')
p('Изображения получены захватом окон запущенной программы. Для второго скриншота в терминале показаны последние 14 строк фактического вывода контрольной партии. Полный вывод сохранён в report/game_test.txt.')
page()
h('5. Выводы по работе')
p('Разработана программа для варианта 12, создающая массив из 750 псевдослучайных вещественных чисел в интервале [100; 110]. Построена графическая горизонтальная гистограмма по 10 равным интервалам. Сумма частот в контрольном запуске составила 750, значения частот — от 65 до 90 при ожидаемой частоте 75.')
p('Изучено влияние начального значения генератора: фиксированное значение позволяет повторить опыт в той же среде, а значение таймера меняет последовательность между запусками, начатыми в разные секунды. Использование таймера не превращает детерминированный rand() в источник истинной случайности.')
p('Реализована игра с пятью символами подряд на поле 10 × 15. Проверены горизонтальные, вертикальные и диагональные победы, корректность ходов и ничья. Случайный выбор свободной клетки демонстрирует применение ГПСЧ в поведении компьютера.')
p('Визуальное сравнение частот удобно для предварительной оценки равномерности. Для более строгого исследования качества генератора потребовались бы дополнительные статистические испытания и проверка зависимости между соседними значениями.')
page()
h('6. Листинг программы')
p('Файл main.c:')
code((root/'main.c').read_text())
h('6.1. Файл сборки Makefile')
code((root/'Makefile').read_text())

# Самодостаточный HTML с изображениями внутри файла.
html = ['<!doctype html><html lang="ru"><meta charset="utf-8"><title>Отчёт — вариант 12</title><style>body{max-width:900px;margin:40px auto;color:#111;font:16px/1.5 Georgia,serif}h2{font-size:21px}p{text-align:justify}.title{text-align:center;font-weight:bold}table{border-collapse:collapse;width:100%;margin:16px 0}td{border:1px solid #777;padding:5px 9px}tr:first-child{font-weight:bold;background:#eee}pre{font:11px/1.3 monospace;white-space:pre-wrap;overflow-wrap:anywhere}img{max-width:100%}figure{margin:20px 0;break-inside:avoid}figcaption{font-style:italic}.page{break-before:page}@media print{@page{size:A4;margin:20mm}body{margin:0;font-size:12pt}h2{break-after:avoid}}</style>']
for kind, data in blocks:
    if kind=='page': html.append('<div class="page"></div>')
    elif kind=='table': html.append('<table>'+''.join('<tr>'+''.join('<td>'+escape(x)+'</td>' for x in row)+'</tr>' for row in data)+'</table>')
    elif kind=='image':
        name, caption = data
        b64 = base64.b64encode((folder/name).read_bytes()).decode()
        html.append(f'<figure><img src="data:image/png;base64,{b64}"><figcaption>{escape(caption)}</figcaption></figure>')
    else:
        tag = {'title':'p class="title"','h':'h2','p':'p','code':'pre'}[kind]
        html.append('<'+tag+'>'+escape(data)+'</'+tag.split()[0]+'>')
html.append('</html>')
(folder/'report_variant12.html').write_text('\n'.join(html))

# DOCX (OOXML) без сторонних Python-пакетов.
parts = []
relations = ['<Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles" Target="styles.xml"/>']
media = []
def para(text, style='Normal'):
    font = 'Liberation Mono' if style == 'Code' else 'Times New Roman'
    size = {'Code':18,'TableText':22,'Caption':22,'Title':28,'Heading1':28}.get(style,26)
    after = 0 if style == 'Code' else 100
    props = f'<w:spacing w:after="{after}" w:line="{220 if style == "Code" else 280}" w:lineRule="auto"/>'
    if style == 'Title': props += '<w:jc w:val="center"/>'
    if style == 'Heading1': props += '<w:keepNext/><w:spacing w:before="200" w:after="140"/>'
    run = f'<w:rFonts w:ascii="{font}" w:hAnsi="{font}"/><w:sz w:val="{size}"/>'
    if style in ('Title','Heading1'): run += '<w:b/>'
    if style == 'Caption': run += '<w:i/>'
    return f'<w:p><w:pPr>{props}</w:pPr><w:r><w:rPr>{run}</w:rPr><w:t xml:space="preserve">{xml(text)}</w:t></w:r></w:p>' 
for kind, data in blocks:
    if kind=='page': parts.append('<w:p><w:r><w:br w:type="page"/></w:r></w:p>')
    elif kind=='table':
        cell_width = 9600 // len(data[0])
        grid = '<w:tblGrid>' + ''.join(f'<w:gridCol w:w="{cell_width}"/>' for _ in data[0]) + '</w:tblGrid>'
        rows = ''.join('<w:tr><w:trPr><w:cantSplit/></w:trPr>'+''.join(f'<w:tc><w:tcPr><w:tcW w:w="{cell_width}" w:type="dxa"/></w:tcPr>'+para(x,'TableText')+'</w:tc>' for x in row)+'</w:tr>' for row in data)
        parts.append('<w:tbl><w:tblPr><w:tblW w:w="9600" w:type="dxa"/><w:tblLayout w:type="fixed"/><w:tblBorders>'+''.join(f'<w:{side} w:val="single" w:sz="4" w:color="888888"/>' for side in ['top','left','bottom','right','insideH','insideV'])+'</w:tblBorders></w:tblPr>'+grid+rows+'</w:tbl>')
    elif kind=='image':
        name, caption = data
        raw = (folder/name).read_bytes()
        width,height = struct.unpack('>II', raw[16:24])
        cx = 5943600; cy = int(cx*height/width)
        idx = len(media)+2; rid = f'rId{idx}'
        relations.append(f'<Relationship Id="{rid}" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/image" Target="media/{name}"/>')
        media.append((name,raw))
        parts.append(f'''<w:p><w:r><w:drawing><wp:inline distT="0" distB="0" distL="0" distR="0"><wp:extent cx="{cx}" cy="{cy}"/><wp:docPr id="{idx}" name="{name}"/><a:graphic><a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:pic><pic:nvPicPr><pic:cNvPr id="{idx}" name="{name}"/><pic:cNvPicPr/></pic:nvPicPr><pic:blipFill><a:blip r:embed="{rid}"/><a:stretch><a:fillRect/></a:stretch></pic:blipFill><pic:spPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="{cx}" cy="{cy}"/></a:xfrm><a:prstGeom prst="rect"><a:avLst/></a:prstGeom></pic:spPr></pic:pic></a:graphicData></a:graphic></wp:inline></w:drawing></w:r></w:p>''')
        parts.append(para(caption,'Caption'))
    elif kind=='code':
        parts.extend(para(line,'Code') for line in data.splitlines())
    else:
        style = {'p':'Normal','title':'Title','h':'Heading1'}[kind]
        parts.extend(para(line,style) for line in data.split('\n'))
ns = 'xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships" xmlns:wp="http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing" xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main" xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture"'
document = f'<?xml version="1.0" encoding="UTF-8"?><w:document {ns}><w:body>'+''.join(parts)+'<w:sectPr><w:pgSz w:w="11906" w:h="16838"/><w:pgMar w:top="1134" w:right="850" w:bottom="1134" w:left="1134"/></w:sectPr></w:body></w:document>'
styles = '''<w:styles xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"><w:style w:type="paragraph" w:default="1" w:styleId="Normal"><w:name w:val="Normal"/><w:pPr><w:spacing w:after="120" w:line="300" w:lineRule="auto"/></w:pPr><w:rPr><w:rFonts w:ascii="Times New Roman" w:hAnsi="Times New Roman"/><w:sz w:val="26"/><w:lang w:val="ru-RU"/></w:rPr></w:style><w:style w:type="paragraph" w:styleId="Title"><w:basedOn w:val="Normal"/><w:pPr><w:jc w:val="center"/><w:spacing w:after="240"/></w:pPr><w:rPr><w:b/><w:sz w:val="28"/></w:rPr></w:style><w:style w:type="paragraph" w:styleId="Heading1"><w:name w:val="heading 1"/><w:basedOn w:val="Normal"/><w:pPr><w:keepNext/><w:spacing w:before="240" w:after="160"/></w:pPr><w:rPr><w:b/><w:sz w:val="28"/></w:rPr></w:style><w:style w:type="paragraph" w:styleId="Code"><w:basedOn w:val="Normal"/><w:pPr><w:spacing w:before="0" w:after="0" w:line="200" w:lineRule="auto"/></w:pPr><w:rPr><w:rFonts w:ascii="Liberation Mono" w:hAnsi="Liberation Mono"/><w:sz w:val="15"/></w:rPr></w:style><w:style w:type="paragraph" w:styleId="TableText"><w:basedOn w:val="Normal"/><w:pPr><w:spacing w:after="60"/></w:pPr><w:rPr><w:sz w:val="22"/></w:rPr></w:style><w:style w:type="paragraph" w:styleId="Caption"><w:basedOn w:val="Normal"/><w:rPr><w:i/><w:sz w:val="22"/></w:rPr></w:style></w:styles>'''
with zipfile.ZipFile(folder/'report_variant12.docx','w',zipfile.ZIP_DEFLATED) as z:
    z.writestr('[Content_Types].xml','<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types"><Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/><Default Extension="xml" ContentType="application/xml"/><Default Extension="png" ContentType="image/png"/><Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/><Override PartName="/word/styles.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml"/></Types>')
    z.writestr('_rels/.rels','<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships"><Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/></Relationships>')
    z.writestr('word/document.xml',document)
    z.writestr('word/styles.xml',styles)
    z.writestr('word/_rels/document.xml.rels','<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">'+''.join(relations)+'</Relationships>')
    for name,raw in media: z.writestr('word/media/'+name,raw)
print('Created report_variant12.html and report_variant12.docx')
