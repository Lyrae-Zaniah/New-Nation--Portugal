"""Retratos dos oficiais navais portugueses: copia dos ingleses (face_511..516),
casaca vermelha -> branca e bandeira britanica -> bandeira portuguesa."""
import sys, os, struct
sys.path.insert(0, '/home/claude/tools')
import numpy as np, cv2
from PIL import Image
from scipy import ndimage as ndi
from txtool import read_tx, write_tx
from recolor import to_white
from portugal_arms import arms
from build_portugal_ui import fblur

P = '/mnt/user-data/uploads/Caribbean Legend Age of Pirates/RESOURCE/Textures/INTERFACES/PORTRAITS/'
O = '/mnt/user-data/outputs/mod/RESOURCE/Textures/INTERFACES/PORTRAITS/'
NEW = 5000   # face_5511.. = versao portuguesa de face_511..
# area da bandeira (x0,y0,x1,y1) e centro/tamanho do brasao, em pixels do retrato 512
FLAGS = {   # contorno da bandeira (poligono), brasao (cx, cy, tamanho) e o que proteger (chapeu/dourado), em pixels do retrato 512
    511: [([(12,190),(30,150),(60,146),(82,163),(106,190),(118,230),(115,280),(110,330),(108,378),(95,388),(72,376),(50,350),(35,300),(26,250),(14,215)], (68, 262, 62), '')],
    512: [([(38,148),(70,143),(92,158),(121,236),(106,272),(75,264),(38,242)], (78, 205, 46), 'hat')],
    514: [([(0,0),(130,0),(110,30),(88,60),(74,100),(72,150),(90,190),(160,195),(176,226),(150,252),(100,264),(40,274),(0,282)], (40, 150, 70), 'hat,gold')],
    515: [([(55,257),(89,257),(89,283),(55,283)], (71, 270, 16), '')],
    516: [([(8,12),(70,4),(146,22),(145,62),(137,96),(129,128),(121,160),(109,192),(101,216),(60,204),(10,182)], (64, 100, 72), '')],
}
COAT_Y = 320   # casaca so abaixo do queixo


def hsv(rgb):
    f = rgb.astype(np.float32) / 255
    mx = f.max(-1); mn = f.min(-1); d = np.maximum(mx - mn, 1e-6)
    s = np.where(mx > 1e-6, d / np.maximum(mx, 1e-6), 0)
    r, g, b = f[..., 0], f[..., 1], f[..., 2]
    h = np.where(mx == r, ((g - b) / d) % 6, np.where(mx == g, (b - r) / d + 2, (r - g) / d + 4)) * 60
    return h, s, mx


def replace_flag(img, poly, emb, prot=''):
    from PIL import ImageDraw
    H, W = img.shape[:2]
    pm = Image.new('L', (W, H), 0); ImageDraw.Draw(pm).polygon(poly, fill=255)
    pm = np.asarray(pm) > 0
    ys, xs = np.where(pm); x0, x1, y0, y1 = xs.min(), xs.max() + 1, ys.min(), ys.max() + 1
    sub = img[y0:y1, x0:x1, :3].astype(np.float32)
    h, s, v = hsv(sub)
    red = ((h < 14) | (h > 330)) & (s > 0.35)
    bluish = sub[..., 2] > sub[..., 0] * 1.12
    hat = (v < 0.22) & ~bluish & ~red
    hair = (h >= 14) & (h <= 60) & (s > 0.25) & (v > 0.15)
    gold = (h >= 30) & (h <= 60) & (s > 0.3)
    m = pm[y0:y1, x0:x1].copy()
    if 'hat' in prot: m &= ~hat
    if 'gold' in prot: m &= ~(gold & (s > 0.4))
    m = ndi.binary_opening(m, iterations=1)
    m = ndi.binary_closing(m, iterations=1) & pm[y0:y1, x0:x1]
    R_, G_, B_ = sub[..., 0], sub[..., 1], sub[..., 2]
    redc = (R_ > G_ * 1.25) & (R_ > B_ * 1.15) & m
    bluec = (B_ > R_ * 1.06) & (B_ >= G_) & m & ~redc
    whitec = m & ~redc & ~bluec
    r = max(3, (x1 - x0) // 10)
    fold = np.ones_like(v)
    for c in (redc, bluec, whitec):
        if c.sum() < 5: continue
        lm = fblur(v * c, r) / np.maximum(fblur(c.astype(np.float32), r), 1e-3)
        fold = np.where(c, v / np.maximum(lm, 1e-3), fold)
    fold = np.clip(fold, 0.55, 1.35).astype(np.float32)
    fold = cv2.medianBlur(fold, 5)
    fold = cv2.GaussianBlur(fold, (0, 0), 1.0) ** 0.7
    R = max(6, (x1 - x0) // 4)
    den = fblur(whitec.astype(np.float32), R)
    illum = fblur(v * whitec, R) / np.maximum(den, 1e-3)
    gw = np.median(v[whitec]) if whitec.sum() > 20 else 0.8
    illum = np.where(den > 0.08, illum, gw)
    shade = np.clip(illum * fold / 0.97, 0.25, 1.0)
    flag = np.ones_like(sub) * np.array([246, 245, 238], np.float32)
    cx, cy, sz = emb
    a = np.asarray(arms(sz), np.float32)
    ax, ay = cx - x0 - sz // 2, cy - y0 - sz // 2
    ys_, xs_ = slice(max(0, ay), min(sub.shape[0], ay + sz)), slice(max(0, ax), min(sub.shape[1], ax + sz))
    aa = a[ys_.start - ay:ys_.stop - ay, xs_.start - ax:xs_.stop - ax]
    al = aa[..., 3:4] / 255
    flag[ys_, xs_] = flag[ys_, xs_] * (1 - al) + aa[..., :3] * al
    flag *= shade[..., None]
    soft = cv2.GaussianBlur(m.astype(np.float32), (0, 0), 0.8)[..., None]
    img[y0:y1, x0:x1, :3] = np.clip(sub * (1 - soft) + flag * soft, 0, 255).astype(np.uint8)


def process(i):
    hd, big = read_tx(P + f'256/face_{i}.tga.tx')
    out = big.copy()
    low = out[COAT_Y:].copy()
    w, frac = to_white(low)
    out[COAT_Y:] = w
    for box, emb, prot in FLAGS.get(i, []):
        replace_flag(out, box, emb, prot)
    return big, out


# transformacao retrato 512 -> versoes menores (medida por template matching)
XF = {'128': (1.6, 51.2, 51.2), '64': (2.87, 71.91, 0.0)}


def small_version(big, new, size):
    _, sm = read_tx(P + f'{size}/face_{I}.tga.tx')
    n = sm.shape[0]
    s, ox, oy = XF[size]
    M = np.float32([[1 / s, 0, -ox / s], [0, 1 / s, -oy / s]])
    delta = new[..., :3].astype(np.float32) - big[..., :3].astype(np.float32)
    # reduz o delta para o tamanho pequeno (media por area)
    k = 1 / s
    dsm = cv2.warpAffine(cv2.GaussianBlur(delta, (0, 0), max(0.5, s / 2.5)), M, (n, n), flags=cv2.INTER_LINEAR)
    res = sm.copy()
    res[..., :3] = np.clip(sm[..., :3].astype(np.float32) + dsm, 0, 255).astype(np.uint8)
    return res, sm


def save(path, rgba, nmips):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    write_tx(path, rgba, nmips)


if __name__ == '__main__':
    sheet = []
    for I in range(511, 517):
        big, new = process(I)
        hd, _ = read_tx(P + f'256/face_{I}.tga.tx')
        save(O + f'256/face_{NEW + I}.tga.tx', new, hd['nmips'])
        row = [Image.fromarray(new[..., :3]).resize((256, 256))]
        for sz in ('128', '64'):
            hd2, _ = read_tx(P + f'{sz}/face_{I}.tga.tx')
            res, sm = small_version(big, new, sz)
            save(O + f'{sz}/face_{NEW + I}.tga.tx', res, hd2['nmips'])
            row.append(Image.fromarray(res[..., :3]).resize((128, 128)))
        sheet.append(row)
    W = Image.new('RGB', (6 * 256, 256 + 128))
    for k, row in enumerate(sheet):
        W.paste(row[0], (k * 256, 0)); W.paste(row[1], (k * 256, 256)); W.paste(row[2], (k * 256 + 128, 256))
    W.save('/tmp/q_port.png')
    print('ok')
