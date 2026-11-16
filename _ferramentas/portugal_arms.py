"""Desenha as armas reais de Portugal (c. 1640-1667) e a bandeira (campo branco + armas coroadas)."""
from PIL import Image, ImageDraw, ImageFilter
import numpy as np

RED = (196, 30, 36, 255)
GOLD = (232, 186, 52, 255)
GOLD_D = (150, 105, 20, 255)
BLUE = (28, 58, 150, 255)
WHITE = (250, 250, 246, 255)
SILVER = (238, 238, 238, 255)
OUT = (40, 30, 20, 255)


def shield_poly(x0, y0, w, h, n=40):
    """Escudo iberico: lados retos e base arredondada."""
    pts = [(x0, y0), (x0 + w, y0), (x0 + w, y0 + h * 0.55)]
    cx, cy, r = x0 + w / 2, y0 + h * 0.55, w / 2
    for i in range(n + 1):
        a = np.pi * i / n
        pts.append((cx + r * np.cos(a), cy + (h - h * 0.55) * np.sin(a)))
    pts.append((x0, y0 + h * 0.55))
    return pts


def castle(d, cx, cy, s):
    """Castelo dourado com tres torres."""
    w = s; h = s
    d.rectangle([cx - w * 0.45, cy - h * 0.05, cx + w * 0.45, cy + h * 0.45], fill=GOLD, outline=GOLD_D)
    for dx in (-0.32, 0, 0.32):
        tw = w * 0.2; th = h * (0.55 if dx == 0 else 0.4)
        d.rectangle([cx + dx * w - tw / 2, cy - th, cx + dx * w + tw / 2, cy], fill=GOLD, outline=GOLD_D)
    d.rectangle([cx - w * 0.08, cy + h * 0.15, cx + w * 0.08, cy + h * 0.45], fill=GOLD_D)


def quina(d, cx, cy, s):
    """Escudete azul com cinco besantes de prata em aspa."""
    w = s * 0.62; h = s * 0.8
    d.polygon(shield_poly(cx - w / 2, cy - h / 2, w, h, 16), fill=BLUE)
    r = s * 0.065
    for ox, oy in ((-0.17, -0.2), (0.17, -0.2), (0, 0), (-0.17, 0.2), (0.17, 0.2)):
        x = cx + ox * s; y = cy + oy * s - s * 0.03
        d.ellipse([x - r, y - r, x + r, y + r], fill=SILVER)


def crown(d, cx, top, w):
    """Coroa real fechada (arcos) dourada."""
    h = w * 0.55
    base_y = top + h
    d.rectangle([cx - w / 2, base_y - h * 0.22, cx + w / 2, base_y], fill=GOLD, outline=GOLD_D)
    # arcos
    d.chord([cx - w / 2, top + h * 0.15, cx + w / 2, base_y + h * 0.6], 180, 360, fill=GOLD, outline=GOLD_D)
    d.line([cx, top + h * 0.15, cx, base_y - h * 0.22], fill=GOLD_D, width=max(1, int(w * 0.03)))
    # globo e cruz
    r = w * 0.07
    d.ellipse([cx - r, top + h * 0.02, cx + r, top + h * 0.02 + 2 * r], fill=GOLD, outline=GOLD_D)
    d.line([cx, top - h * 0.18, cx, top + h * 0.05], fill=GOLD_D, width=max(1, int(w * 0.04)))
    d.line([cx - w * 0.06, top - h * 0.08, cx + w * 0.06, top - h * 0.08], fill=GOLD_D, width=max(1, int(w * 0.04)))
    # joias
    for dx in (-0.3, 0, 0.3):
        jr = w * 0.035
        d.ellipse([cx + dx * w - jr, base_y - h * 0.15 - jr, cx + dx * w + jr, base_y - h * 0.15 + jr], fill=RED)


def arms(size, with_crown=True):
    """Imagem RGBA (size x size) com o escudo (e coroa)."""
    S = size * 4  # superamostragem
    im = Image.new('RGBA', (S, S), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    if with_crown:
        sw, sh = S * 0.60, S * 0.66
        x0, y0 = (S - sw) / 2, S * 0.30
        crown(d, S / 2, S * 0.04, S * 0.44)
    else:
        sw, sh = S * 0.80, S * 0.92
        x0, y0 = (S - sw) / 2, S * 0.04
    # bordadura vermelha
    d.polygon(shield_poly(x0, y0, sw, sh), fill=RED, outline=OUT)
    # campo de prata interno
    b = sw * 0.17
    d.polygon(shield_poly(x0 + b, y0 + b, sw - 2 * b, sh - 2 * b), fill=WHITE)
    # 7 castelos: 3 no chefe, 2 em cada flanco
    cs = b * 0.75
    for fx in (0.2, 0.5, 0.8):
        castle(d, x0 + sw * fx, y0 + b * 0.55, cs)
    for fy in (0.42, 0.68):
        castle(d, x0 + b * 0.5, y0 + sh * fy, cs)
        castle(d, x0 + sw - b * 0.5, y0 + sh * fy, cs)
    # 5 quinas em cruz
    ix, iw = x0 + b, sw - 2 * b
    iy, ih = y0 + b, sh - 2 * b
    q = iw * 0.27
    cx, cy = ix + iw / 2, iy + ih * 0.44
    for ox, oy in ((0, -1), (-1, 0), (0, 0), (1, 0), (0, 1)):
        quina(d, cx + ox * q * 0.95, cy + oy * q * 1.05, q)
    return im.resize((size, size), Image.LANCZOS)


def flag(w, h):
    """Bandeira: campo branco com as armas coroadas no centro."""
    im = Image.new('RGBA', (w, h), WHITE)
    a = min(w, h) * 0.86
    ar = arms(int(a))
    im.alpha_composite(ar, (int(w / 2 - a / 2), int(h / 2 - a / 2)))
    return im


if __name__ == '__main__':
    arms(256).save('/tmp/arms.png')
    arms(256, False).save('/tmp/arms_nocrown.png')
    flag(512, 256).save('/tmp/flag.png')
