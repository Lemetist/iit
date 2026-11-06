from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED
from copy import deepcopy
from lxml import etree as E
import re

base=Path(__file__).resolve().parent
ref=base.parent/'ЛБ2/Задание 2.docx'
target=base/'Отчёт.docx'
ns={'w':'http://schemas.openxmlformats.org/wordprocessingml/2006/main'}
def w(n):return '{'+ns['w']+'}'+n
with ZipFile(target) as z: old=E.fromstring(z.read('word/document.xml'))
with ZipFile(ref) as z:
    parts={i.filename:z.read(i.filename) for i in z.infolist()}
root=E.fromstring(parts['word/document.xml'])
b=root.find('w:body',ns)
source=b.findall('w:p',ns)
sect=deepcopy(b.find('w:sectPr',ns))
for el in list(b):b.remove(el)

def text(p):
    return ''.join('\n' if el.tag==w('br') else (el.text or '') for el in p.iter() if el.tag in [w('t'),w('br')])

def clone(index,content=None):
    p=deepcopy(source[index])
    if content is not None:
        rp=p.find('w:r/w:rPr',ns)
        if rp is None:rp=p.find('w:pPr/w:rPr',ns)
        rp=deepcopy(rp) if rp is not None else None
        for el in list(p):
            if el.tag!=w('pPr'):p.remove(el)
        r=E.SubElement(p,w('r'))
        if rp is not None:r.append(rp)
        for i,line in enumerate(content.split('\n')):
            if i:E.SubElement(r,w('br'))
            t=E.SubElement(r,w('t'));t.set('{http://www.w3.org/XML/1998/namespace}space','preserve');t.text=line
    b.append(p)
    return p

clone(0,'Задание № 3')
clone(1)
clone(2,'РАЗРАБОТКА АЛГОРИТМОВ НАХОЖДЕНИЯ ЭЙЛЕРОВЫХ И ГАМИЛЬТОНОВЫХ ГРАФОВ И ИХ ПУТЕЙ')
clone(3)
aim=clone(4)
runs=aim.findall('w:r',ns)
for r in runs:
    for el in list(r):
        if el.tag!=w('rPr'):r.remove(el)
for r,t in [(runs[0],'Цели и задачи: '),(runs[1],'Изучить методы нахождения эйлеровых и гамильтоновых путей и циклов. Реализовать их построение для графа варианта 12, проверить свойства исходного графа и изменить его при необходимости.')]:
    E.SubElement(r,w('t')).text=t
clone(5);clone(6)
started=False
for p in old.findall('w:body/w:p',ns):
    s=text(p)
    if s=='1. Постановка задачи':started=True
    if not started:continue
    if s.startswith('Цель работы:'):continue
    if s.startswith('Исходный код программы  продолжение'):continue
    is_heading=p.find('w:r/w:rPr/w:b',ns) is not None
    is_code=any(x.get(w('ascii'))=='Liberation Mono' for x in p.findall('.//w:rFonts',ns))
    if is_heading:
        if s!='1. Постановка задачи':clone(13)
        clone(7 if s=='1. Постановка задачи' else 11,re.sub(r'^\d+\.\s*','',s))
    elif is_code:
        # Literal content uses the same 14 pt Times New Roman run as
        # the reference's own numerical example. Each code line is a
        # separate paragraph so ordinary pagination can split the listing.
        for line in s.split('\n'):
            clone(8,line)
    else:clone(12,s)
b.append(sect)
parts['word/document.xml']=E.tostring(root,xml_declaration=True,encoding='UTF-8',standalone=True)
with ZipFile(target,'w',ZIP_DEFLATED) as z:
    for name,data in parts.items():z.writestr(name,data)
with ZipFile(ref) as z,ZipFile(target) as out:
    assert all(z.read(n)==out.read(n) for n in z.namelist() if n!='word/document.xml')
(base/'artifact.md').write_text('Reference: '+str(ref)+'\nDirect paragraph clones preserve exact pPr and rPr. Title: source 0 and 2, blank title spacing: 1 and 3, objective: 4, objective spacing: 5 and 6, first heading: 7, subheading: 11, body: 12, heading spacing: 13. Code and numerical text: 8 (Times New Roman 14, same as reference). No cover, no section page breaks, no monospace font. Section properties and every package part except document.xml unchanged.\n')
print(target)
