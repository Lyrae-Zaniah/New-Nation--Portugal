import os, re, sys
sys.path.insert(0,'/home/claude/tools')
from porline_texts import GOV, DLG, QB, QB_TITLE
from porline_code import GOV_C, DLG_C, LINE_C
W='/home/claude/work/'
LANGS=['chinese','english','french','german','polish','portuguese','russian','spanish']
# numeracao das chaves
gkey={k:i+5 for i,(k,e,p) in enumerate(GOV)}
dkey={k:i+1 for i,(k,e,p) in enumerate(DLG)}
assert dkey['promoted']==3
def sub(code):
    def r(m):
        t,k=m.group(1),m.group(2)
        if t=='N': return str(dkey[k])
        if t=='G': return f'StringFromKey("por_Governor_{gkey[k]}")'
        return f'StringFromKey("POR_Line_dialog_{dkey[k]}")'
    out=re.sub(r'\{([GDN]):(\w+)\}',r,code)
    assert '{G:' not in out and '{D:' not in out and '{N:' not in out
    return out
def wr(p,s):
    os.makedirs(os.path.dirname(p),exist_ok=True)
    open(p,'wb').write(s.replace('\r\n','\n').replace('\n','\r\n').encode('utf-8'))
wr(W+'PROGRAM/dialogs/Governor/por_Governor.c', sub(GOV_C))
wr(W+'PROGRAM/dialogs/Quest/PortugalLine/POR_Line_dialog.c', sub(DLG_C))
wr(W+'PROGRAM/quests/PortugalLine/POR_Line.c', sub(LINE_C))
open('/tmp/dkey.txt','w').write(repr(dkey))
# textos
base={'english':1,'portuguese':2}
for l in LANGS:
    ix=2 if l=='portuguese' else 1
    # governador: mantem as 4 primeiras chaves ja existentes
    p=W+f'RESOURCE/INI/texts/{l}/dialogs/Governor/por_Governor.txt'
    old=open(p,'rb').read().decode('utf-8')
    head=old[:old.index('por_Governor_5 {')] if 'por_Governor_5 {' in old else old
    if not head.endswith('\r\n'): head+='\r\n'
    body=''.join(f'por_Governor_{gkey[k]} {{\r\n{t[ix]}\r\n}}\r\n' for t in GOV for k in [t[0]])
    wr(p, head+body)
    body=''.join(f'POR_Line_dialog_{dkey[t[0]]} {{\r\n{t[ix]}\r\n}}\r\n' for t in DLG)
    wr(W+f'RESOURCE/INI/texts/{l}/dialogs/Quest/PortugalLine/POR_Line_dialog.txt', body)
# quest book: bloco para acrescentar ao QuestBook_New.txt
for l in LANGS:
    ix=2 if l=='portuguese' else 1
    blk='\r\n\r\n// Nova Nacao: linha de missoes de Portugal\r\nPOR_Line\r\n{\r\n'+QB_TITLE[ix-1]+'\r\n}\r\n'
    for t in QB: blk+=f'\r\nPOR_Line.{t[0]}\r\n{{\r\n{t[ix]}\r\n}}\r\n'
    open(f'/tmp/qb_{l}.txt','wb').write(blk.encode('utf-8'))
print('ok', len(GOV), len(DLG), len(QB))
