from pathlib import Path
from copy import deepcopy
from zipfile import ZipFile, ZIP_DEFLATED
from math import sin, cos, pi
from docx import Document
from docx.text.paragraph import Paragraph
from docx.shared import Cm, Pt
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from PIL import Image, ImageDraw, ImageFont

B=Path(__file__).resolve().parent
REF=B.parent/'ЛБ2/Отчёт.docx'
OUT=B/'Отчёт.docx'
d=Document(REF)
patterns=[deepcopy(p._p) for p in d.paragraphs]
body=d.element.body
for el in list(body)[:-1]:body.remove(el)

def add(text='',index=27):
    node=deepcopy(patterns[index])
    rp=node.find('.//'+qn('w:rPr'))
    # Use the first actual run's properties, not the paragraph mark.
    run=node.find(qn('w:r'))
    rp=deepcopy(run.find(qn('w:rPr'))) if run is not None and run.find(qn('w:rPr')) is not None else None
    for el in list(node):
        if el.tag!=qn('w:pPr'):node.remove(el)
    body.insert(len(body)-1,node)
    p=Paragraph(node,d._body)
    r=p.add_run(text)
    if rp is not None:r._r.insert(0,rp)
    return p
def h(t,page=False):
    p=add(t,26)
    if page:p.paragraph_format.page_break_before=True
    return p
def sub(t):return add(t,30)
def code(t,index=121):
    for s in t.splitlines():add(s,index)
def picture(path,width,caption):
    p=add('',43);p.add_run().add_picture(str(path),width=Cm(width))
    add(caption,44)

for i in range(26):
    node=deepcopy(patterns[i]);body.insert(len(body)-1,node)
    if i in [8,9]:
        p=Paragraph(node,d._body)
        text='Лабораторная работа 3 · Вариант 12' if i==8 else '«РАЗРАБОТКА АЛГОРИТМОВ НАХОЖДЕНИЯ ЭЙЛЕРОВЫХ И ГАМИЛЬТОНОВЫХ ГРАФОВ И ИХ ПУТЕЙ»'
        first=True
        for r in p.runs:
            r.text=text if first else '';first=False

# A clear redraw of the exact variant, retaining the source vertex positions.
im=Image.new('RGB',(1800,920),'white');dr=ImageDraw.Draw(im)
font=ImageFont.truetype('/usr/share/fonts/google-noto-vf/NotoSerif[wght].ttf',42)
small=ImageFont.truetype('/usr/share/fonts/google-noto-vf/NotoSerif[wght].ttf',34)
edges=[(1,2),(1,6),(2,3),(2,5),(2,8),(3,4),(3,6),(4,5),(5,6),(5,8),(6,7),(7,8)]
for panel,cx in enumerate([450,1350]):
    coords={i+1:(cx+300*sin(i*pi/4),430-300*cos(i*pi/4)) for i in range(8)}
    for u,v in edges:dr.line([coords[u],coords[v]],fill='black',width=5)
    if panel:
        a,b=coords[3],coords[8]
        for k in range(0,40,2):
            points=[(a[0]+(b[0]-a[0])*j/40,a[1]+(b[1]-a[1])*j/40) for j in [k,k+1]]
            dr.line(points,fill='black',width=7)
    for u,(x,y) in coords.items():
        dr.ellipse((x-38,y-38,x+38,y+38),fill='white',outline='black',width=4)
        dr.text((x,y),str(u),font=font,anchor='mm',fill='black')
    dr.text((cx,810),'Исходный граф' if not panel else 'Добавлено ребро (3, 8)',font=small,anchor='mm',fill='black')
im.save(B/'graphs.png')

h('1. Постановка задачи')
add('Цель работы: изучить методы нахождения эйлеровых и гамильтоновых путей и циклов и реализовать их построение для графа варианта 12.')
add('Нужно определить, является ли граф эйлеровым и гамильтоновым, вывести найденные маршруты и при необходимости изменить граф. Задача решается последовательно для одного графа: сначала строится эйлеров цикл, затем в полученном графе ищется гамильтонов цикл.')
add('Граф простой, неориентированный, содержит 8 вершин и 12 рёбер. Пересечения линий на рисунке задания не образуют дополнительных вершин. Рёбра задаются следующими парами:')
code('(1,2), (1,6), (2,3), (2,5), (2,8), (3,4),\n(3,6), (4,5), (5,6), (5,8), (6,7), (7,8)',32)
picture(B/'graphs.png',16,'Рисунок 1 — Граф варианта 12 до и после изменения')
add('Для хранения графа используется симметричная матрица смежности: 1 означает наличие ребра, 0 — его отсутствие. Петли и кратные рёбра не используются. Программа написана на языке C++17, исходный файл — graph.cpp.')

h('2. Алгоритмы решения задачи',True)
sub('2.1. Проверка эйлеровости и алгоритм Флёри')
add('Эйлерова цепь проходит по каждому ребру ровно один раз. Если начало и конец совпадают, цепь является эйлеровым циклом. Вершины могут повторяться. В связном неориентированном графе цикл существует тогда и только тогда, когда степени всех вершин чётные; открытая цепь — при ровно двух вершинах нечётной степени.')
add('Сначала обходом в глубину проверяется связность и подсчитываются степени вершин. Для открытой цепи начальной выбирается первая нечётная вершина, для цикла — вершина 1. Далее применяется алгоритм Флёри: мост выбирается только тогда, когда других рёбер из текущей вершины нет.')
add('Мост проверяется временным удалением ребра: если число достижимых из текущей вершины вершин уменьшилось, ребро является мостом. После проверки ребро восстанавливается. Выбранное ребро удаляется из рабочей копии графа, а его конец добавляется в маршрут.')
code('ФЛЁРИ(G):\n  если G несвязен: вернуть «цепи нет»\n  odd := вершины нечётной степени\n  если |odd| не равно 0 и 2: вернуть «цепи нет»\n  u := первая нечётная вершина либо вершина 1\n  путь := [u]; H := копия G\n  пока степень u в H больше 0:\n    выбрать ребро (u,v), которое не является мостом,\n      либо единственное оставшееся ребро\n    удалить (u,v) из H\n    u := v; добавить u в путь\n  вернуть путь',32)
sub('2.2. Изменение графа для эйлерова цикла')
add('Отсутствующие рёбра перебираются наборами по возрастанию их размера. Первый набор, после добавления которого граф связен и все степени чётные, сохраняется. Такой перебор даёт минимальное число добавлений и подходит для небольшого графа лабораторной работы.')
add('В варианте 12 нечётные степени имеют вершины 3 и 8. Ребро (3,8) отсутствует, поэтому достаточно добавить его. Число рёбер увеличивается до 13, а степени обеих вершин становятся равными 4.')

sub('2.3. Гамильтонов путь и цикл')
add('Гамильтонов путь посещает каждую вершину ровно один раз. Гамильтонов цикл дополнительно возвращается в начальную вершину. Для их поиска используется перебор с возвратом: добавляется непосещённый сосед, а если продолжения нет, выполняется возврат к предыдущей вершине.')
code('ПОИСК(путь, посещены, нужен_цикл):\n  если в пути уже n вершин:\n    вернуть НЕ нужен_цикл ИЛИ\n      наличие ребра от конца пути к его началу\n  для каждого соседа v последней вершины:\n    если v не посещена:\n      добавить v в путь; отметить v\n      если ПОИСК успешен: вернуть ИСТИНА\n      удалить v из пути; снять отметку\n  вернуть ЛОЖЬ',32)
add('Для цикла начало фиксируется в вершине 1. Для открытого пути последовательно проверяются все начальные вершины. Соседи перебираются по возрастанию номеров. Если цикл не найден, добавляются недостающие рёбра маршрута 1–2–…–n–1. Это гарантирует цикл при n ≥ 3, но не минимальность числа добавлений.')
sub('2.4. Оценка сложности')
add('Матрица смежности занимает O(n²) памяти. Проверка достижимости по матрице требует O(n²) времени. Верхняя оценка данной реализации Флёри с перебором соседей и проверкой мостов — O(mn³), где m — число рёбер. Поиск гамильтонова пути имеет факториальный рост числа вариантов; для перебора всех начальных вершин верхняя оценка — O(n · n!).')
add('Исправление эйлерова графа перебирает до 2^q наборов, где q — число отсутствующих рёбер. Этот способ предназначен для малых графов. В рассматриваемом варианте решение находится уже при проверке одиночных рёбер.')

h('3. Контрольный тест',True)
sub('3.1. Исходные данные и ожидаемый результат')
add('Степени исходных вершин: 2, 4, 3, 2, 4, 4, 2, 3. Их сумма равна 24 = 2 · 12. Граф связен, нечётные степени имеют ровно две вершины — 3 и 8. Ожидается открытая эйлерова цепь от 3 к 8 и отсутствие эйлерова цикла. Гамильтонов цикл существует.')
sub('Матрица смежности исходного графа')
matrix=[[0]*8 for _ in range(8)]
for u,v in edges:matrix[u-1][v-1]=matrix[v-1][u-1]=1
table=d.add_table(rows=9,cols=9);table.autofit=False
for j,col in enumerate(table.columns):col.width=Cm(1.55)
for i,row in enumerate(table.rows):
    for j,cell in enumerate(row.cells):
        cell.text=('' if not i and not j else str(j) if not i else str(i) if not j else str(matrix[i-1][j-1]))
        for p in cell.paragraphs:
            p.alignment=WD_ALIGN_PARAGRAPH.CENTER
            p.paragraph_format.first_line_indent=Cm(0)
            p.paragraph_format.space_before=p.paragraph_format.space_after=Pt(2)
            p.paragraph_format.line_spacing=1
            for r in p.runs:r.font.name='Times New Roman';r.font.size=Pt(12);r.bold=not i or not j
        pr=cell._tc.get_or_add_tcPr();b=OxmlElement('w:tcBorders')
        for edge in ['top','bottom','left','right']:
            el=OxmlElement('w:'+edge);el.set(qn('w:val'),'single');el.set(qn('w:sz'),'4');el.set(qn('w:color'),'BFBFBF');b.append(el)
        pr.append(b)
        if not i or not j:
            shade=OxmlElement('w:shd');shade.set(qn('w:fill'),'F2F2F2');pr.append(shade)
sub('3.2. Проверка корректности')
add('Для эйлеровой цепи проверяются m + 1 вершин и использование каждого ребра ровно один раз. Для цикла дополнительно совпадают первая и последняя вершины. Для гамильтонова маршрута проверяются смежность всех последовательных вершин, однократное посещение каждой вершины и замыкание цикла.')
add('Проверены все 1099 простых неориентированных графов с 1–5 вершинами. Гамильтонов поиск сопоставлен с независимым перебором перестановок. Отдельно проверены вариант 12, исправление несвязного графа из двух треугольников и построение гамильтонова цикла для звезды. Все проверки пройдены. Эйлеров поиск требует связности всех вершин, включая изолированные.')

h('4. Результаты работы программы',True)
add('Приведён фактический вывод программы. Сначала исследуется исходный граф, затем в него добавляется ребро (3,8). Гамильтонов этап работает уже с изменённым графом.')
code((B/'result.txt').read_text().strip(),121)
sub('4.1. Сборка и запуск')
code('g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic \\\n    graph.cpp -o graph\n./graph',32)

h('5. Выводы по работе')
add('Реализованы алгоритм Флёри и перебор с возвратом. В исходном графе варианта 12 найдены открытая эйлерова цепь и гамильтонов цикл. Эйлеров цикл отсутствует из-за нечётных степеней вершин 3 и 8.')
add('Добавление одного ребра (3,8) делает граф эйлеровым. Это минимальное изменение: без добавления ребра две нечётные степени сохраняются. Гамильтонов цикл остаётся доступным, поэтому на втором этапе дополнительные изменения не требуются. Итоговый граф содержит 8 вершин и 13 рёбер и является одновременно эйлеровым и гамильтоновым.')

h('6. Листинг программы',True)
add('Файл graph.cpp. Программа последовательно исследует исходный граф, строит эйлеров цикл после исправления и находит гамильтонов цикл в текущем графе.',120)
code((B/'graph.cpp').read_text(),121)

tmp=B/'_report_work.docx';d.save(tmp)
# Retain original package parts exactly, replacing only authored content,
# its drawing relationships, and any newly inserted media.
with ZipFile(REF) as a,ZipFile(tmp) as b:
    parts={n:a.read(n) for n in a.namelist()}
    for n in b.namelist():
        if n in ['word/document.xml','word/_rels/document.xml.rels'] or n not in parts:parts[n]=b.read(n)
    with ZipFile(OUT,'w',ZIP_DEFLATED) as out:
        for n,data in parts.items():out.writestr(n,data)
tmp.unlink()
(B/'artifact.md').write_text('Reference: '+str(REF)+'\nCover paragraphs 0–25 retained; only laboratory number and topic changed. Section geometry, styles and all headers/footers retained. Body cloned from reference paragraph 27, headings 26/30, caption 44, image paragraph 43, code 121 and pseudocode 32. New content: graph figure and adjacency table. Original package parts preserved except document.xml, drawing relationships and new image. Render every page before delivery.\n')
print(OUT)
