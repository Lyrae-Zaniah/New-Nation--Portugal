"""Gera as texturas de interface/bandeira de Portugal a partir das existentes."""
import sys, struct, os
sys.path.insert(0, '/home/claude/tools')
import numpy as np
from PIL import Image, ImageDraw, ImageFilter
import txtool
from portugal_arms import arms, WHITE

U = '/mnt/user-data/uploads/Caribbean Legend Age of Pirates/RESOURCE/Textures/'
O = '/mnt/user-data/outputs/mod/RESOURCE/Textures/'


def V(rgba):
    return rgba[..., :3].max(-1).astype(np.float32)


def blur(a, r):
    im = Image.fromarray(np.clip(a, 0, 255).astype(np.uint8))
    return np.asarray(im.filter(ImageFilter.GaussianBlur(r)), np.float32)


def shade_from(cell, r):
    v = blur(V(cell), r)
    al = cell[..., 3] > 128
    ref = np.percentile(v[al], 90) if al.any() else 255
    return np.clip(v / max(ref, 1), 0.55, 1.08)


def fblur(a, r):
    """Desfoque gaussiano aproximado (3 passadas de caixa) em float."""
    a = a.astype(np.float64)
    k = max(1, int(r))
    for _ in range(3):
        for ax in (0, 1):
            c = np.cumsum(np.pad(a, [(k + 1, k) if i == ax else (0, 0) for i in range(2)], mode='edge'), axis=ax)
            if ax == 0:
                a = (c[2 * k + 1:] - c[:-2 * k - 1]) / (2 * k + 1)
            else:
                a = (c[:, 2 * k + 1:] - c[:, :-2 * k - 1]) / (2 * k + 1)
    return a.astype(np.float32)


def white_mask(cell):
    f = cell[..., :3].astype(np.float32)
    mx = f.max(-1); mn = f.min(-1)
    sat = (mx - mn) / np.maximum(mx, 1)
    return (sat < 0.18) & (mx > 150) & (cell[..., 3] > 128)


def clean_shade(cell, r):
    """Dobras do pano estimadas so pelos pixels brancos (normalized convolution)."""
    m = white_mask(cell).astype(np.float32)
    v = V(cell) * m
    num = fblur(v, r)
    den = fblur(m, r)
    est = np.where(den > 0.02, num / np.maximum(den, 1e-3), np.nan)
    ref = np.nanpercentile(est[cell[..., 3] > 128], 90)
    sh = np.clip(est / max(ref, 1), 0.6, 1.06)
    w = np.clip(den / 0.25, 0, 1)
    return np.where(np.isnan(sh), 0.97, sh * w + 0.97 * (1 - w))


def compose_flag(cell, centered, hoist_keep=True):
    """cell: RGBA da bandeira espanhola (forma). Retorna bandeira portuguesa com mesma forma/dobras."""
    h, w = cell.shape[:2]
    al = cell[..., 3] > 128
    ys, xs = np.where(al)
    x0, x1, y0, y1 = xs.min(), xs.max(), ys.min(), ys.max()
    bw, bh = x1 - x0 + 1, y1 - y0 + 1
    base = Image.new('RGBA', (w, h), WHITE)
    a = int(bh * 0.78)
    cx = x0 + (bw / 2 if centered else bw * 0.34)
    cy = y0 + bh / 2
    base.alpha_composite(arms(a), (int(cx - a / 2), int(cy - a / 2)))
    out = np.asarray(base, np.float32).copy()
    sh = clean_shade(cell, max(2, h // 45))
    out[..., :3] *= sh[..., None]
    if hoist_keep:
        # mantem a amarracao junto ao mastro (faixa esquerda escura) do original
        hoist = int(bw * 0.035)
        dark = V(cell[:, x0:x0 + hoist]) < 120
        seg = out[:, x0:x0 + hoist, :3]
        seg[dark] = cell[:, x0:x0 + hoist, :3][dark]
    out[..., 3] = cell[..., 3]
    return np.clip(out, 0, 255).astype(np.uint8)


def write_tx_a1r5g5b5(path, rgba, nmips):
    h, w = rgba.shape[:2]
    out = [struct.pack('<6i', 0, w, h, nmips, 25, w * h * 2)]
    im = Image.fromarray(rgba, 'RGBA'); cw, ch = w, h
    for i in range(nmips):
        lvl = im if i == 0 else im.resize((cw, ch), Image.LANCZOS)
        a = np.asarray(lvl, np.uint16)
        v = ((a[..., 3] >= 128).astype(np.uint16) << 15) | ((a[..., 0] >> 3) << 10) | ((a[..., 1] >> 3) << 5) | (a[..., 2] >> 3)
        out.append(v.astype('<u2').tobytes())
        cw, ch = max(1, cw // 2), max(1, ch // 2)
    open(path, 'wb').write(b''.join(out))


def ship_flags():
    hd, img = txtool.read_tx(U + 'flags/flagall_ship_b.tga.tx')
    H, W = img.shape[:2]          # 2048 x 2048, 4 colunas x 8 linhas
    cw, rh = W // 4, H // 8
    new = np.zeros((H, cw * 8, 4), np.uint8)
    new[:, :W] = img
    for r in range(8):
        sp = img[r * rh:(r + 1) * rh, 3 * cw:4 * cw]
        if r == 7:
            cell = sp.copy()     # linha de bandeiras piratas: nao usada por Portugal
        else:
            # molde: a bandeira (de qualquer nacao) desta linha com mais area branca
            cands = [img[r * rh:(r + 1) * rh, c * cw:(c + 1) * cw] for c in range(4)]
            score = [white_mask(c).sum() / max(1, (c[..., 3] > 128).sum()) for c in cands]
            best = cands[int(np.argmax(score))]
            cov = (best[..., 3] > 128).mean()
            cell = compose_flag(best, centered=cov > 0.80)
            print('linha', r, 'molde coluna', int(np.argmax(score)), 'branco %.0f%%' % (100 * max(score)))
        new[r * rh:(r + 1) * rh, 4 * cw:5 * cw] = cell
    # colunas 5-7 sem uso: copias das colunas 0-2 (so para manter a grade 8 x 8)
    new[:, 5 * cw:8 * cw] = img[:, 0:3 * cw]
    os.makedirs(O + 'flags', exist_ok=True)
    write_tx_a1r5g5b5(O + 'flags/flagall_ship_b.tga.tx', new, hd['nmips'])
    return new


def nation_icon():
    hd, img = txtool.read_tx(U + 'INTERFACES/nations.tga.tx')   # 1024x128, 5 icones de 128
    spa = img[:, 0:128].astype(np.float32)
    # interior: retangulo arredondado dentro da moldura
    m = Image.new('L', (128, 128), 0)
    ImageDraw.Draw(m).rounded_rectangle([18, 18, 109, 109], radius=20, fill=255)
    m = np.asarray(m.filter(ImageFilter.GaussianBlur(1.2)), np.float32) / 255
    inner = Image.new('RGBA', (128, 128), WHITE)
    a = 84
    inner.alpha_composite(arms(a), ((128 - a) // 2, (128 - a) // 2 + 1))
    inn = np.asarray(inner, np.float32).copy()
    # papel envelhecido + vinheta (como o icone espanhol)
    yy, xx = np.mgrid[0:128, 0:128]
    d = np.hypot(xx - 63.5, yy - 63.5) / 64
    vig = np.clip(1.02 - 0.22 * d ** 2, 0.75, 1.0)
    rng = np.random.default_rng(7)
    grain = 1 + blur(rng.normal(0, 18, (128, 128)) + 128, 0.8) / 255 * 0.08 - 0.04
    inn[..., :3] *= (vig * grain)[..., None]
    out = spa * (1 - m[..., None]) + inn * m[..., None]
    out[..., 3] = spa[..., 3]
    new = img.copy()
    new[:, 640:768] = np.clip(out, 0, 255).astype(np.uint8)
    os.makedirs(O + 'INTERFACES', exist_ok=True)
    txtool.write_tx(O + 'INTERFACES/nations.tga.tx', new, hd['nmips'])
    return new


def battle_icon():
    hd, img = txtool.read_tx(U + 'BATTLE_INTERFACE/small_nations.tga.tx')   # 1024x64, 16 x 64
    spa = img[:, 0:64]
    cell = compose_flag(spa, centered=True, hoist_keep=False)
    new = img.copy(); new[:, 64:128] = cell
    os.makedirs(O + 'BATTLE_INTERFACE', exist_ok=True)
    txtool.write_tx(O + 'BATTLE_INTERFACE/small_nations.tga.tx', new, hd['nmips'])
    return new


def small_shield():
    hd, img = txtool.read_tx(U + 'INTERFACES/nations_small_all.tga.tx')    # 256x64, icones 32x32
    spa = img[0:32, 0:32].astype(np.float32)
    al = spa[..., 3] > 128
    ys, xs = np.where(al)
    x0, x1, y0, y1 = xs.min(), xs.max(), ys.min(), ys.max()
    sh = Image.new('RGBA', (32, 32), (0, 0, 0, 0))
    ar = arms(256, with_crown=False).resize((x1 - x0 + 1, y1 - y0 + 1), Image.LANCZOS)
    sh.alpha_composite(ar, (int(x0), int(y0)))
    s = np.asarray(sh, np.float32).copy()
    shade = shade_from(img[0:32, 0:32], 1.5)
    s[..., :3] *= shade[..., None]
    # contorno escuro do escudo original
    edge = (spa[..., :3].max(-1) < 70) & al
    s[edge, :3] = spa[edge, :3]
    s[..., 3] = spa[..., 3]
    new = img.copy(); new[32:64, 224:256] = np.clip(s, 0, 255).astype(np.uint8)
    txtool.write_tx(O + 'INTERFACES/nations_small_all.tga.tx', new, hd['nmips'])
    return new


if __name__ == '__main__':
    f = ship_flags(); n = nation_icon(); b = battle_icon(); s = small_shield()
    def show(a, path, scale):
        im = Image.fromarray(a); bg = Image.new('RGBA', im.size, (90, 90, 90, 255)); bg.alpha_composite(im)
        bg.convert('RGB').resize((int(im.width * scale), int(im.height * scale)), Image.LANCZOS).save(path)
    show(f[:, :2560 + 512], '/tmp/q_flags.png', 0.3)
    show(n[:, 512:768], '/tmp/q_icon.png', 2)
    show(b[:, 0:192], '/tmp/q_battle.png', 3)
    show(s, '/tmp/q_small.png', 4)
    print('ok')
