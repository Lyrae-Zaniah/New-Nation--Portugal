"""Fase 4: estandarte de Portugal na criacao de personagem (recon_sc_frame) e
icone da habilidade 'Bandeira de Portugal' (perk_enable / perk_disable)."""
import sys, os
sys.path.insert(0, '/home/claude/tools')
import numpy as np, cv2
from PIL import Image
from scipy import ndimage as ndi
import txtool
from portugal_arms import arms
from build_portugal_ui import fblur

U = '/mnt/user-data/uploads/Caribbean Legend Age of Pirates/RESOURCE/Textures/INTERFACES/'
O = '/mnt/user-data/outputs/mod/RESOURCE/Textures/INTERFACES/'


def de_red(cell, grey=False, box=None, dil=3):
    """troca a cruz vermelha (Borgonha) por pano branco com a mesma luz/dobras."""
    rgb = cell[..., :3].astype(np.float32)
    R, G, B = rgb[..., 0], rgb[..., 1], rgb[..., 2]
    red = (R > G * 1.25) & (R > B * 1.2) & (R - G > 18) & (cell[..., 3] > 30)
    if box is not None:
        bm = np.zeros(red.shape, bool); bm[box[1]:box[3], box[0]:box[2]] = True; red &= bm
    red = ndi.binary_dilation(red, iterations=dil) & (cell[..., 3] > 30)
    if box is not None: red &= bm
    v = rgb.max(-1)
    lum = rgb.mean(-1)
    sat = (rgb.max(-1) - rgb.min(-1)) / np.maximum(rgb.max(-1), 1)
    cloth = (sat < 0.22) & (lum > 120) & (cell[..., 3] > 200) & ~ndi.binary_dilation(red, iterations=2)
    if box is not None: cloth &= bm
    out = rgb.copy()
    for r in (2, 4, 8, 16):
        num = fblur(rgb * cloth[..., None] if False else 0, 1) if False else None
    # cor media local do pano branco (convolucao normalizada, raio crescente)
    fill = np.zeros_like(rgb); have = np.zeros(red.shape, bool)
    for r in (2, 3, 5, 8, 12, 20):
        den = fblur(cloth.astype(np.float32), r)
        ok = (den > 0.05) & ~have
        for c in range(3):
            fill[..., c] = np.where(ok, fblur(rgb[..., c] * cloth, r) / np.maximum(den, 1e-3), fill[..., c])
        have |= ok
    # dobras: luz relativa do proprio vermelho
    rv = np.where(red, v, 0)
    rm = fblur(rv, 4) / np.maximum(fblur(red.astype(np.float32), 4), 1e-3)
    fold = np.clip(np.where(red, v / np.maximum(rm, 1), 1), 0.85, 1.12) ** 0.6
    out[red] = np.clip(fill[red] * fold[red, None], 0, 255)
    # corrige o brilho medio do X para casar com o pano em volta
    lo = out.mean(-1)
    ring = ndi.binary_dilation(red, iterations=6) & ~red & cloth
    for r in (6, 10):
        tgt = fblur(lo * ring, r) / np.maximum(fblur(ring.astype(np.float32), r), 1e-3)
        cur = fblur(lo * red, r) / np.maximum(fblur(red.astype(np.float32), r), 1e-3)
        k = np.clip(tgt / np.maximum(cur, 1), 0.7, 1.1)
        out[red] = np.clip(out[red] * k[red, None], 0, 255)
        lo = out.mean(-1)
    res = cell.copy(); res[..., :3] = out.astype(np.uint8)
    return res, red


def paste_arms(cell, cx, cy, size, shade_from, skew=0.0, grey=False):
    a = np.asarray(arms(size), np.float32)
    if skew:
        M = np.float32([[1, skew, -skew * size / 2], [0, 1, 0]])
        a = cv2.warpAffine(a, M, (size, size), flags=cv2.INTER_LINEAR)
    if grey:
        g = a[..., :3].mean(-1, keepdims=True)
        a[..., :3] = a[..., :3] * 0.45 + g * 0.55
    x0, y0 = int(cx - size / 2), int(cy - size / 2)
    reg = cell[y0:y0 + size, x0:x0 + size].astype(np.float32)
    src = shade_from[y0:y0 + size, x0:x0 + size, :3].astype(np.float32).max(-1)
    sh = np.clip(src / max(np.percentile(src, 90), 1), 0.55, 1.05)[..., None]
    al = a[..., 3:4] / 255
    reg[..., :3] = reg[..., :3] * (1 - al) + a[..., :3] * sh * al
    cell[y0:y0 + size, x0:x0 + size] = np.clip(reg, 0, 255).astype(np.uint8)


def banner():
    p = U + 'Recon/recon_sc_frame.tga.tx'
    hd, img = txtool.read_tx(p)
    out = img.copy()
    cell = img[1812:2028, 524:754].copy()          # estandarte espanhol
    new, red = de_red(cell, box=(38, 48, 196, 214), dil=6)
    ys, xs = np.where(red)
    cx, cy = (xs.min() + xs.max()) / 2, (ys.min() + ys.max()) / 2 + 4
    paste_arms(new, cx, cy, 70, new)
    out[1812:2028, 1292:1522] = new
    os.makedirs(O + 'Recon', exist_ok=True)
    txtool.write_tx(O + 'Recon/recon_sc_frame.tga.tx', out, hd['nmips'])
    return new


def perks():
    res = []
    for f, grey in (('perk_enable.tga', False), ('perk_disable.tga', True)):
        im = np.asarray(Image.open(U + 'PERKS/' + f).convert('RGBA')).copy()
        cell = im[640:768, 640:768].copy()           # FlagSpa
        new, red = de_red(cell, grey, box=(12, 22, 112, 112), dil=3)
        ys, xs = np.where(red)
        cx, cy = (xs.min() + xs.max()) / 2 - 2, (ys.min() + ys.max()) / 2
        paste_arms(new, cx, cy, 40, new, skew=-0.12, grey=grey)
        im[1536:1664, 0:128] = new                   # slot livre (0,1536)
        os.makedirs(O + 'PERKS', exist_ok=True)
        Image.fromarray(im, 'RGBA').save(O + 'PERKS/' + f, compression=None)
        res.append(new)
    return res




def whiten_blue(cell, box, grey=False, fill=True, thr=25, k=1.15):
    """pano azul (Franca) -> pano branco com as mesmas dobras; flores-de-lis removidas."""
    rgb = cell[..., :3].astype(np.float32)
    R, G, B = rgb[..., 0], rgb[..., 1], rgb[..., 2]
    bm = np.zeros(R.shape, bool); bm[box[1]:box[3], box[0]:box[2]] = True
    a = cell[..., 3] > 20
    blue = (B > R * k) & (B > G * 1.0) & (B - R > thr) & a & bm
    gold = (R > B * 1.15) & (G > B * 1.0) & (R - B > 50) & a & bm & ~blue
    gold &= ndi.binary_fill_holes(ndi.binary_dilation(blue, iterations=4))
    gold = ndi.binary_dilation(gold, iterations=2) & bm & a
    region = ndi.binary_closing(blue | gold, iterations=2)
    if fill: region = ndi.binary_fill_holes(region)
    region &= bm & a
    v = rgb.max(-1)
    v8 = np.clip(v, 0, 255).astype(np.uint8)
    vi = cv2.inpaint(v8, gold.astype(np.uint8) * 255, 5, cv2.INPAINT_TELEA).astype(np.float32)
    ref = np.percentile(vi[region & ~gold], 97)
    shade = np.clip(vi / max(ref, 1), 0.3, 1.04)
    tint = np.array([238, 236, 228], np.float32)
    if grey: tint = np.array([232, 232, 232], np.float32)
    white = tint * shade[..., None]
    soft = cv2.GaussianBlur(region.astype(np.float32), (0, 0), 0.6)[..., None]
    out = cell.copy()
    out[..., :3] = np.clip(rgb * (1 - soft) + white * soft, 0, 255).astype(np.uint8)
    return out, region


def banner2():
    hd, img = txtool.read_tx(U + 'Recon/recon_sc_frame.tga.tx')
    out = img.copy()
    cell = img[1812:2028, 268:498].copy()            # estandarte frances
    new, reg = whiten_blue(cell, (30, 26, 200, 216))
    ys, xs = np.where(reg)
    paste_arms(new, (xs.min() + xs.max()) / 2, (ys.min() + ys.max()) / 2 + 2, 72, new)
    out[1812:2028, 1292:1522] = new
    txtool.write_tx(O + 'Recon/recon_sc_frame.tga.tx', out, hd['nmips'])
    return new


def perks2():
    res = []
    for f, grey in (('perk_enable.tga', False), ('perk_disable.tga', True)):
        im = np.asarray(Image.open(U + 'PERKS/' + f).convert('RGBA')).copy()
        cell = im[640:768, 512:640].copy()           # FlagFra
        new, reg = whiten_blue(cell, (10, 18, 118, 118), grey, fill=False, thr=(4 if grey else 8), k=(1.04 if grey else 1.08))
        ys, xs = np.where(reg)
        paste_arms(new, 52, 58, 40, new, skew=-0.12, grey=grey)
        im[1536:1664, 0:128] = new
        Image.fromarray(im, 'RGBA').save(O + 'PERKS/' + f, compression=None)
        res.append(new)
    return res


if __name__ == '__main__':
    b = banner2(); e, d = perks2()
    W = Image.new('RGBA', (230 + 256 + 20, 216), (255, 0, 255, 255))
    W.alpha_composite(Image.fromarray(b, 'RGBA'), (0, 0))
    W.alpha_composite(Image.fromarray(e, 'RGBA'), (240, 0))
    W.alpha_composite(Image.fromarray(d, 'RGBA'), (240 + 133, 0))
    W.resize((W.width * 2, W.height * 2)).save('/tmp/q_f4.png')
    print('ok')

