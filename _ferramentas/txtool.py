"""Leitor/escritor do formato de textura .tga.tx do Storm Engine (Caribbean Legend).

Cabecalho (24 bytes, little-endian, int32):
  flags, width, height, nmips, format (D3DFORMAT ou FourCC), mip0_size
Depois: niveis de mip em sequencia (do maior para o menor).

Formatos suportados na leitura: A8R8G8B8 (21), X8R8G8B8 (22), A1R5G5B5 (25),
R5G6B5 (23), A4R4G4B4 (26), DXT1, DXT3, DXT5.
Escrita: sempre A8R8G8B8 (21), com a mesma quantidade de mips (gerados por reducao).
"""
import struct
import numpy as np
from PIL import Image

FOURCC = {b'DXT1': 'DXT1', b'DXT3': 'DXT3', b'DXT5': 'DXT5'}


def read_header(b):
    flags, w, h, nm, fmt, size0 = struct.unpack_from('<6i', b, 0)
    fb = struct.pack('<i', fmt)
    fmtname = FOURCC.get(fb, fmt)
    return dict(flags=flags, w=w, h=h, nmips=nm, fmt=fmtname, size0=size0)


def _565(c):
    r = ((c >> 11) & 31) * 255 // 31
    g = ((c >> 5) & 63) * 255 // 63
    b = (c & 31) * 255 // 31
    return np.stack([r, g, b], -1).astype(np.int32)


def _dxt_color_blocks(blk, w, h, dxt1):
    # blk: (nblocks, 8) bytes of color part
    c0 = blk[:, 0].astype(np.uint32) | (blk[:, 1].astype(np.uint32) << 8)
    c1 = blk[:, 2].astype(np.uint32) | (blk[:, 3].astype(np.uint32) << 8)
    idx = (blk[:, 4].astype(np.uint32) | (blk[:, 5].astype(np.uint32) << 8) |
           (blk[:, 6].astype(np.uint32) << 16) | (blk[:, 7].astype(np.uint32) << 24))
    p0 = _565(c0); p1 = _565(c1)
    n = len(c0)
    pal = np.zeros((n, 4, 4), np.int32)
    pal[:, 0, :3] = p0; pal[:, 1, :3] = p1; pal[:, :, 3] = 255
    four = (c0 > c1) | (not dxt1)
    four = np.broadcast_to(four, (n,)) if np.ndim(four) == 0 else four
    pal[:, 2, :3] = np.where(four[:, None], (2 * p0 + p1) // 3, (p0 + p1) // 2)
    pal[:, 3, :3] = np.where(four[:, None], (p0 + 2 * p1) // 3, 0)
    if dxt1:
        pal[:, 3, 3] = np.where(four, 255, 0)
    sel = np.stack([(idx >> (2 * i)) & 3 for i in range(16)], -1)  # (n,16)
    px = np.take_along_axis(pal, sel[:, :, None].astype(np.int64), 1)  # (n,16,4)
    return px


def _blocks_to_image(px, w, h):
    bw, bh = max(1, (w + 3) // 4), max(1, (h + 3) // 4)
    img = px.reshape(bh, bw, 4, 4, 4).transpose(0, 2, 1, 3, 4).reshape(bh * 4, bw * 4, 4)
    return img[:h, :w]


def decode_level(b, off, w, h, fmt):
    """Retorna (rgba uint8 HxWx4, bytes_consumidos)."""
    if fmt in (21, 22):
        n = w * h * 4
        a = np.frombuffer(b, np.uint8, n, off).reshape(h, w, 4)  # B G R A
        rgba = a[:, :, [2, 1, 0, 3]].copy()
        if fmt == 22:
            rgba[:, :, 3] = 255
        return rgba, n
    if fmt in (23, 25, 26):
        n = w * h * 2
        c = np.frombuffer(b, '<u2', w * h, off).astype(np.uint32).reshape(h, w)
        if fmt == 23:
            r = (c >> 11) & 31; g = (c >> 5) & 63; bb = c & 31
            rgba = np.stack([r * 255 // 31, g * 255 // 63, bb * 255 // 31, np.full_like(c, 255)], -1)
        elif fmt == 25:
            a = (c >> 15) & 1; r = (c >> 10) & 31; g = (c >> 5) & 31; bb = c & 31
            rgba = np.stack([r * 255 // 31, g * 255 // 31, bb * 255 // 31, a * 255], -1)
        else:
            a = (c >> 12) & 15; r = (c >> 8) & 15; g = (c >> 4) & 15; bb = c & 15
            rgba = np.stack([r * 17, g * 17, bb * 17, a * 17], -1)
        return rgba.astype(np.uint8), n
    if fmt in ('DXT1', 'DXT3', 'DXT5'):
        bw, bh = max(1, (w + 3) // 4), max(1, (h + 3) // 4)
        bs = 8 if fmt == 'DXT1' else 16
        n = bw * bh * bs
        raw = np.frombuffer(b, np.uint8, n, off).reshape(-1, bs)
        if fmt == 'DXT1':
            px = _dxt_color_blocks(raw, w, h, True)
        else:
            px = _dxt_color_blocks(raw[:, 8:], w, h, False)
            if fmt == 'DXT3':
                ab = raw[:, :8]
                al = np.stack([(ab[:, i // 2] >> (4 * (i % 2))) & 15 for i in range(16)], -1) * 17
            else:
                a0 = raw[:, 0].astype(np.int32); a1 = raw[:, 1].astype(np.int32)
                bits = np.zeros(len(raw), np.uint64)
                for i in range(6):
                    bits |= raw[:, 2 + i].astype(np.uint64) << np.uint64(8 * i)
                sel = np.stack([((bits >> np.uint64(3 * i)) & np.uint64(7)).astype(np.int32) for i in range(16)], -1)
                pal = np.zeros((len(raw), 8), np.int32)
                pal[:, 0] = a0; pal[:, 1] = a1
                big = a0 > a1
                for i in range(1, 7):
                    pal[:, i + 1] = np.where(big, ((7 - i) * a0 + i * a1) // 7, 0)
                for i in range(1, 5):
                    pal[:, i + 1] = np.where(big, pal[:, i + 1], ((5 - i) * a0 + i * a1) // 5)
                pal[:, 6] = np.where(big, pal[:, 6], 0)
                pal[:, 7] = np.where(big, pal[:, 7], 255)
                al = np.take_along_axis(pal, sel, 1)
            px[:, :, 3] = al
        return _blocks_to_image(px, w, h).astype(np.uint8), n
    raise ValueError('formato nao suportado: %r' % (fmt,))


def read_tx(path):
    b = open(path, 'rb').read()
    hd = read_header(b)
    img, _ = decode_level(b, 24, hd['w'], hd['h'], hd['fmt'])
    return hd, img


def write_tx(path, rgba, nmips, flags=0):
    """Grava em A8R8G8B8 com nmips niveis."""
    h, w = rgba.shape[:2]
    out = [struct.pack('<6i', flags, w, h, nmips, 21, w * h * 4)]
    im = Image.fromarray(rgba, 'RGBA')
    cw, ch = w, h
    for i in range(nmips):
        lvl = im if i == 0 else im.resize((cw, ch), Image.LANCZOS)
        a = np.asarray(lvl, np.uint8)
        out.append(a[:, :, [2, 1, 0, 3]].tobytes())
        cw, ch = max(1, cw // 2), max(1, ch // 2)
    open(path, 'wb').write(b''.join(out))


if __name__ == '__main__':
    import sys
    for p in sys.argv[1:]:
        hd, img = read_tx(p)
        print(p, hd, img.shape)


def _to565(rgb):
    r = (rgb[..., 0].astype(np.uint32) * 31 + 127) // 255
    g = (rgb[..., 1].astype(np.uint32) * 63 + 127) // 255
    b = (rgb[..., 2].astype(np.uint32) * 31 + 127) // 255
    return (r << 11) | (g << 5) | b


def encode_dxt1(rgba):
    """Compressor DXT1 simples (caixa delimitadora por bloco, 4 cores)."""
    h, w = rgba.shape[:2]
    bw, bh = max(1, w // 4), max(1, h // 4)
    blk = rgba[:bh * 4, :bw * 4, :3].reshape(bh, 4, bw, 4, 3).transpose(0, 2, 1, 3, 4).reshape(-1, 16, 3).astype(np.int32)
    # eixo principal aproximado: maior variancia entre canais -> usa min/max por luminancia
    lum = blk @ np.array([299, 587, 114])
    imin = lum.argmin(1); imax = lum.argmax(1)
    cmax = blk[np.arange(len(blk)), imax]; cmin = blk[np.arange(len(blk)), imin]
    c0 = _to565(cmax); c1 = _to565(cmin)
    swap = c0 < c1
    c0, c1 = np.where(swap, c1, c0), np.where(swap, c0, c1)
    eq = c0 == c1
    c0 = np.where(eq & (c0 < 0xFFFF), c0 + 1, c0)
    p0 = _565(c0); p1 = _565(c1)
    pal = np.stack([p0, p1, (2 * p0 + p1) // 3, (p0 + 2 * p1) // 3], 1)  # (n,4,3)
    d = ((blk[:, :, None, :] - pal[:, None, :, :]) ** 2).sum(-1)  # (n,16,4)
    sel = d.argmin(-1).astype(np.uint32)
    idx = np.zeros(len(blk), np.uint32)
    for i in range(16):
        idx |= sel[:, i] << np.uint32(2 * i)
    out = np.zeros((len(blk), 8), np.uint8)
    out[:, 0] = c0 & 255; out[:, 1] = c0 >> 8; out[:, 2] = c1 & 255; out[:, 3] = c1 >> 8
    for i in range(4):
        out[:, 4 + i] = (idx >> np.uint32(8 * i)) & 255
    return out.tobytes()


def write_tx_dxt1(path, rgba, nmips, flags=0):
    h, w = rgba.shape[:2]
    fourcc = struct.unpack('<i', b'DXT1')[0]
    out = [struct.pack('<6i', flags, w, h, nmips, fourcc, max(1, w // 4) * max(1, h // 4) * 8)]
    im = Image.fromarray(rgba, 'RGBA'); cw, ch = w, h
    for i in range(nmips):
        lvl = im if i == 0 else im.resize((cw, ch), Image.LANCZOS)
        a = np.asarray(lvl, np.uint8)
        if cw < 4 or ch < 4:
            pad = np.zeros((max(4, ch), max(4, cw), 4), np.uint8); pad[:ch, :cw] = a; a = pad
        out.append(encode_dxt1(a))
        cw, ch = max(1, cw // 2), max(1, ch // 2)
    open(path, 'wb').write(b''.join(out))
