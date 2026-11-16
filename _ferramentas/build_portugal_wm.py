"""Mapa-mundi: bandeira portuguesa nas colonias (icons.tga linha 12), quadro do jogador
(WorldMapEnsigns.tga, posicao 5) e bandeira dos navios (worldmap_por_nat.tga + portuguese_*.gm)."""
import sys, os
sys.path.insert(0, '/home/claude/tools')
import numpy as np
from PIL import Image
import txtool
from portugal_arms import arms
from build_portugal_ui import clean_shade, white_mask, V, fblur

U = '/mnt/user-data/uploads/Caribbean Legend Age of Pirates/RESOURCE/'
O = '/mnt/user-data/outputs/mod/RESOURCE/'
WH = np.array([246, 245, 238], np.float32)


def put_arms(base, cx, cy, size, shade=None):
    a = np.asarray(arms(size), np.float32)
    x0, y0 = int(round(cx - size / 2)), int(round(cy - size / 2))
    reg = base[y0:y0 + size, x0:x0 + size]
    al = a[..., 3:4] / 255
    reg[..., :3] = reg[..., :3] * (1 - al) + a[..., :3] * al * (shade[y0:y0 + size, x0:x0 + size, None] if shade is not None else 1)
    return base


def fold_shade(cell, mask, r):
    """dobras a partir da luminancia, removendo o desenho original (normaliza por cor)."""
    sub = cell[..., :3].astype(np.float32); v = sub.max(-1) / 255
    R, G, B = sub[..., 0], sub[..., 1], sub[..., 2]
    red = (R > G * 1.25) & (R > B * 1.15) & mask
    blue = (B > R * 1.06) & (B >= G) & mask & ~red
    gold = (R > B * 1.4) & (G > B * 1.2) & mask & ~red
    other = mask & ~red & ~blue & ~gold
    fold = np.ones_like(v)
    for c in (red, blue, gold, other):
        if c.sum() < 3: continue
        lm = fblur(v * c, r) / np.maximum(fblur(c.astype(np.float32), r), 1e-3)
        fold = np.where(c, v / np.maximum(lm, 1e-3), fold)
    wv = v[other & (v > np.percentile(v[mask], 50))] if other.any() else v[mask]
    level = min(0.98, np.percentile(wv, 80) if wv.size else 0.9)
    return np.clip(fold, 0.6, 1.25) ** 0.8 * level


def icons():
    hd, img = txtool.read_tx(U + 'Textures/INTERFACES/Worldmap/icons.tga.tx')   # 256x512: 8 quadros x 16 linhas (32px)
    out = img.copy()
    for f in range(8):
        cell = img[2 * 32:3 * 32, f * 32:(f + 1) * 32].astype(np.float32)     # linha 2 = Espanha (mesma forma/ondulacao)
        al = cell[..., 3] > 100
        ys, xs = np.where(al)
        sh = fold_shade(img[2 * 32:3 * 32, f * 32:(f + 1) * 32], al, 3)
        new = np.zeros_like(cell); new[..., :3] = WH; new[..., 3] = cell[..., 3]
        # mantem a borda escura (contorno) do icone original
        put_arms(new, (xs.min() + xs.max()) / 2 + 0.5, (ys.min() + ys.max()) / 2 + 0.5, 18, None)
        new[..., :3] *= sh[..., None]
        edge = (V(img[2 * 32:3 * 32, f * 32:(f + 1) * 32]) < 70) & al
        new[edge, :3] = cell[edge, :3]
        out[12 * 32:13 * 32, f * 32:(f + 1) * 32] = np.clip(new, 0, 255).astype(np.uint8)
    os.makedirs(O + 'Textures/INTERFACES/Worldmap', exist_ok=True)
    txtool.write_tx(O + 'Textures/INTERFACES/Worldmap/icons.tga.tx', out, hd['nmips'])
    return out


def ensigns():
    hd, img = txtool.read_tx(U + 'Textures/INTERFACES/Worldmap/WorldMapEnsigns.tga.tx')  # 512x64, quadros de 64
    out = img.copy()
    cell = img[:, 3 * 64:4 * 64].astype(np.float32)          # Espanha
    # interior = tecido (dentro da moldura de madeira)
    other = img[:, 1 * 64:2 * 64].astype(np.float32)        # Inglaterra: mesma moldura, tecido diferente
    diff = np.abs(cell[..., :3] - other[..., :3]).max(-1) > 40
    from scipy import ndimage as ndi
    inner = ndi.binary_fill_holes(ndi.binary_closing(diff, iterations=3))
    box = np.zeros_like(inner); box[4:60, 4:60] = True
    inner = (inner | ndi.binary_erosion(inner, iterations=0) ) & box
    inner = ndi.binary_dilation(inner, iterations=1) & box
    sh = fold_shade(img[:, 3 * 64:4 * 64], inner, 4)
    new = cell.copy()
    new[inner, :3] = WH
    tmp = new.copy(); put_arms(tmp, 32, 32, 40)
    new[inner, :3] = tmp[inner, :3] * sh[inner, None]
    out[:, 5 * 64:6 * 64] = np.clip(new, 0, 255).astype(np.uint8)
    txtool.write_tx(O + 'Textures/INTERFACES/Worldmap/WorldMapEnsigns.tga.tx', out, hd['nmips'])
    return out


def ship_flag_atlas():
    hd, img = txtool.read_tx(U + 'Textures/WorldMap/geometry/worldmap_nations.tga.tx')   # 256x128, Espanha em (0,0) 64x64
    out = img.copy()
    # base: o pano branco liso (quadro 2,1) com o brasao por cima
    base = img[64:128, 128:192].astype(np.float32)
    v = base[..., :3].max(-1)
    sh = v / np.percentile(v, 90)
    new = base.copy(); new[..., :3] = WH
    put_arms(new, 32, 32, 40)
    new[..., :3] *= np.clip(sh, 0.3, 1.05)[..., None]
    out[0:64, 0:64] = np.clip(new, 0, 255).astype(np.uint8)
    out[..., 3] = 255
    os.makedirs(O + 'Textures/WorldMap/geometry', exist_ok=True)
    txtool.write_tx(O + 'Textures/WorldMap/geometry/worldmap_por_nat.tga.tx', out, hd['nmips'])
    return out


def models():
    src = U + 'Models/WorldMap/'
    os.makedirs(O + 'Models/WorldMap', exist_ok=True)
    for t in ('bark', 'barrel', 'boat', 'frigate', 'galleon', 'manowar', 'sloop'):
        b = open(src + f'spanish_{t}.gm', 'rb').read()
        n = b.count(b'worldmap_nations.tga')
        b = b.replace(b'worldmap_nations.tga', b'worldmap_por_nat.tga')
        open(O + f'Models/WorldMap/portuguese_{t}.gm', 'wb').write(b)
        print(t, 'trocas', n)


if __name__ == '__main__':
    a = icons(); e = ensigns(); s = ship_flag_atlas(); models()
    def show(x, k):
        im = Image.fromarray(x, 'RGBA'); bg = Image.new('RGBA', im.size, (90, 90, 90, 255)); bg.alpha_composite(im)
        return bg.resize((im.width * k, im.height * k), Image.NEAREST)
    show(a[11 * 32:14 * 32], 3).save('/tmp/q_icons.png')
    show(e, 2).save('/tmp/q_ens.png')
    show(s, 3).save('/tmp/q_wmn.png')
