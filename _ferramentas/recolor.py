"""Troca o vermelho dos uniformes ingleses por branco (uniforme portugues)."""
import numpy as np


def red_mask(rgb):
    f = rgb.astype(np.float32) / 255.0
    r, g, b = f[..., 0], f[..., 1], f[..., 2]
    mx = f.max(-1); mn = f.min(-1)
    v = mx
    s = np.where(mx > 1e-6, (mx - mn) / np.maximum(mx, 1e-6), 0)
    # matiz em graus
    d = np.maximum(mx - mn, 1e-6)
    h = np.where(mx == r, ((g - b) / d) % 6, np.where(mx == g, (b - r) / d + 2, (r - g) / d + 4)) * 60.0
    hd = np.minimum(h, 360 - h)            # distancia ao vermelho puro (0 grau)
    # pertencimento suave: matiz ate ~12 graus forte, some ate 20; saturacao > 0.45
    mh = np.clip((20.0 - hd) / 8.0, 0, 1)
    ms = np.clip((s - 0.40) / 0.15, 0, 1)
    mv = np.clip((v - 0.06) / 0.08, 0, 1)
    # vermelho dominante sobre o verde (exclui pele/laranja)
    mr = np.clip(((r / np.maximum(g, 1e-3)) - 2.0) / 0.8, 0, 1)
    return mh * ms * mv * mr, v


def skin_density(rgb):
    """Densidade local de pele (para nao mexer em labios/narinas/olhos)."""
    from PIL import Image, ImageFilter
    f = rgb.astype(np.float32) / 255.0
    r, g, b = f[..., 0], f[..., 1], f[..., 2]
    mx = f.max(-1); mn = f.min(-1); d = np.maximum(mx - mn, 1e-6)
    s = np.where(mx > 1e-6, d / np.maximum(mx, 1e-6), 0)
    h = np.where(mx == r, ((g - b) / d) % 6, np.where(mx == g, (b - r) / d + 2, (r - g) / d + 4)) * 60.0
    skin = ((h > 10) & (h < 40) & (s > 0.18) & (s < 0.75) & (mx > 0.25) & (r > g) & (g > b)).astype(np.uint8) * 255
    rad = max(2, rgb.shape[1] // 64)
    blur = Image.fromarray(skin).filter(ImageFilter.GaussianBlur(rad))
    return np.asarray(blur, np.float32) / 255.0


def to_white(rgba):
    rgb = rgba[..., :3]
    m, v = red_mask(rgb)
    sk = skin_density(rgb)
    m = m * np.clip((0.30 - sk) / 0.15, 0, 1)   # perto de pele: nao recolore
    # brilho do tecido branco preservando as dobras
    lum = np.clip(0.28 + 1.05 * v, 0, 0.97)
    white = np.stack([lum, lum * 0.985, lum * 0.95], -1) * 255.0   # branco levemente "cru"
    out = rgb.astype(np.float32) * (1 - m[..., None]) + white * m[..., None]
    res = rgba.copy()
    res[..., :3] = np.clip(out, 0, 255).astype(np.uint8)
    return res, float(m.mean())
