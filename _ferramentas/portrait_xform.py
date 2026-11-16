import sys; sys.path.insert(0,'/home/claude/tools')
from txtool import read_tx
import numpy as np, cv2
P='/mnt/user-data/uploads/Caribbean Legend Age of Pirates/RESOURCE/Textures/INTERFACES/PORTRAITS/'
def find(big, small):
    """small ~= big[oy:oy+n*s, ox:ox+n*s] resized. retorna (s, ox, oy, score)"""
    B=cv2.cvtColor(big[...,:3],cv2.COLOR_RGB2GRAY).astype(np.float32)
    S=cv2.cvtColor(small[...,:3],cv2.COLOR_RGB2GRAY).astype(np.float32)
    n=S.shape[0]; m=int(n*0.06)
    T=S[m:n-m,m:n-m]   # sem moldura
    best=None
    for s in np.arange(512/n*0.5, 512/n*1.001, 0.01):
        Bs=cv2.resize(B,(int(round(512/s)),)*2,interpolation=cv2.INTER_AREA)
        if Bs.shape[0]<T.shape[0]: continue
        r=cv2.matchTemplate(Bs,T,cv2.TM_CCOEFF_NORMED)
        _,v,_,loc=cv2.minMaxLoc(r)
        if best is None or v>best[3]:
            k=512/Bs.shape[0]
            best=(s*1.0,(loc[0]-m)*k,(loc[1]-m)*k,v)
    return best
if __name__=='__main__':
    for i in (511,514,516):
        _,big=read_tx(P+f'256/face_{i}.tga.tx')
        for sz in ('128','64'):
            _,sm=read_tx(P+f'{sz}/face_{i}.tga.tx'); print(i,sz,find(big,sm))
