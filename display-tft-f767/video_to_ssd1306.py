#!/usr/bin/env python3
"""
video_to_ssd1306.py
-------------------
Converte arquivos de vídeo (MP4, GIF, AVI, etc.) ou sequências de imagens
em arrays C otimizados para o display OLED SSD1306 (I2C / SPI) no STM32.

Requisitos:
- Python 3.7+
- Pillow (`pip install pillow`)
- ffmpeg instalado no sistema (para vídeos mp4/avi/etc)

Exemplo de uso:
    # Gerar demo embutida
    python3 video_to_ssd1306.py --demo -o Core/Inc/animation_frames.h

    # Converter um vídeo MP4 a 15 FPS com dithering
    python3 video_to_ssd1306.py -i meu_video.mp4 -o Core/Inc/animation_frames.h --fps 15

    # Converter GIF limitando a 30 frames
    python3 video_to_ssd1306.py -i meme.gif -o Core/Inc/animation_frames.h --fps 12 --max-frames 30
"""

import argparse
import math
import os
import shutil
import subprocess
import sys
import tempfile
from PIL import Image, ImageDraw, ImageOps, ImageSequence


def image_to_ssd1306_buffer(img: Image.Image, width: int = 128, height: int = 64) -> bytes:
    """
    Converte uma imagem PIL monocromática (modo '1') no formato de buffer GDDRAM do SSD1306.
    Estrutura: Páginas horizontais de 8 pixels de altura, LSB no topo.
    """
    img_1bit = img.convert("1")
    pixels = img_1bit.load()
    buf = bytearray((width * height) // 8)
    pages = height // 8

    for page in range(pages):
        for x in range(width):
            byte = 0
            for bit in range(8):
                y = page * 8 + bit
                if y < height and x < width:
                    # No modo '1', 255/1 é branco (pixel aceso)
                    if pixels[x, y]:
                        byte |= (1 << bit)
            buf[x + page * width] = byte

    return bytes(buf)


def process_frame(
    img: Image.Image,
    target_w: int,
    target_h: int,
    dither: bool = True,
    threshold: int = 128,
    invert: bool = False,
    fit_mode: str = "contain",
) -> Image.Image:
    """
    Redimensiona, ajusta proporção, aplica escala de cinza, dithering/thresholding e inversão.
    """
    img = img.convert("RGB")

    # Redimensionamento mantendo ou ajustando proporção
    if fit_mode == "stretch":
        resized = img.resize((target_w, target_h), Image.Resampling.LANCZOS)
    elif fit_mode == "cover":
        # Preenche cortando sobras
        scale = max(target_w / img.width, target_h / img.height)
        nw, nh = int(img.width * scale), int(img.height * scale)
        resized_full = img.resize((nw, nh), Image.Resampling.LANCZOS)
        left = (nw - target_w) // 2
        top = (nh - target_h) // 2
        resized = resized_full.crop((left, top, left + target_w, top + target_h))
    else:  # contain (letterbox preto nas bordas)
        scale = min(target_w / img.width, target_h / img.height)
        nw, nh = max(1, int(img.width * scale)), max(1, int(img.height * scale))
        scaled = img.resize((nw, nh), Image.Resampling.LANCZOS)
        resized = Image.new("RGB", (target_w, target_h), (0, 0, 0))
        offset_x = (target_w - nw) // 2
        offset_y = (target_h - nh) // 2
        resized.paste(scaled, (offset_x, offset_y))

    # Converte para tons de cinza
    gray = ImageOps.grayscale(resized)

    if invert:
        gray = ImageOps.invert(gray)

    # Binarização (Monocromático)
    if dither:
        # Floyd-Steinberg dithering
        bw = gray.convert("1", dither=Image.Dither.FLOYDSTEINBERG)
    else:
        # Limiarização direta (Threshold)
        bw = gray.point(lambda p: 255 if p >= threshold else 0, mode="1")

    return bw


def extract_frames_from_video(video_path: str, fps: float, max_frames: int = 0) -> list:
    """
    Extrai frames do vídeo usando ffmpeg em diretório temporário.
    """
    if not shutil.which("ffmpeg"):
        raise RuntimeError("ffmpeg não encontrado no PATH. Instale o ffmpeg ou utilize arquivos GIF/imagens.")

    temp_dir = tempfile.mkdtemp(prefix="ssd1306_frames_")
    try:
        out_pattern = os.path.join(temp_dir, "frame_%05d.png")
        cmd = [
            "ffmpeg",
            "-y",
            "-i", video_path,
            "-vf", f"fps={fps}",
            out_pattern,
        ]
        res = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if res.returncode != 0:
            raise RuntimeError(f"Erro ao executar ffmpeg: {res.stderr}")

        frame_files = sorted([os.path.join(temp_dir, f) for f in os.listdir(temp_dir) if f.endswith(".png")])
        if max_frames > 0:
            frame_files = frame_files[:max_frames]

        images = [Image.open(f).copy() for f in frame_files]
        return images
    finally:
        shutil.rmtree(temp_dir, ignore_errors=True)


def extract_frames_from_gif(gif_path: str, max_frames: int = 0) -> list:
    """Extrai frames de GIF animado usando Pillow."""
    img = Image.open(gif_path)
    frames = []
    for frame in ImageSequence.Iterator(img):
        frames.append(frame.copy())
        if max_frames > 0 and len(frames) >= max_frames:
            break
    return frames


def generate_demo_frames(count: int = 24, target_w: int = 128, target_h: int = 64) -> list:
    """
    Gera uma animação geométrica 3D / Radar de demonstração.
    """
    frames = []
    for i in range(count):
        img = Image.new("RGB", (target_w, target_h), (0, 0, 0))
        draw = ImageDraw.Draw(img)
        t = (i / count) * 2.0 * math.pi

        # Círculo externo pulsante
        center_x, center_y = target_w // 2, target_h // 2
        r_pulse = 22 + int(4 * math.sin(t * 2))
        draw.ellipse([center_x - r_pulse, center_y - r_pulse, center_x + r_pulse, center_y + r_pulse], outline=(255, 255, 255))

        # Cubo giratório simples no centro
        size = 14
        angle = t
        cos_a, sin_a = math.cos(angle), math.sin(angle)
        
        # 4 vértices 2D rotacionados
        corners = [
            (-size, -size * 0.7),
            (size, -size * 0.7),
            (size, size * 0.7),
            (-size, size * 0.7),
        ]
        rot_corners = []
        for x, y in corners:
            rx = center_x + int(x * cos_a - y * sin_a)
            ry = center_y + int(x * sin_a + y * cos_a)
            rot_corners.append((rx, ry))

        # Desenha polígono
        for j in range(4):
            draw.line([rot_corners[j], rot_corners[(j + 1) % 4]], fill=(255, 255, 255), width=1)

        # Barra de carregamento / status inferior
        bar_w = int((i + 1) / count * (target_w - 20))
        draw.rectangle([10, target_h - 6, 10 + bar_w, target_h - 3], fill=(255, 255, 255))
        draw.rectangle([8, target_h - 8, target_w - 8, target_h - 1], outline=(255, 255, 255))

        # Texto indicativo
        draw.text((8, 2), "SSD1306 ANIMATION", fill=(255, 255, 255))

        frames.append(img)
    return frames


def export_to_c_header(
    buffers: list,
    output_path: str,
    fps: int,
    width: int = 128,
    height: int = 64,
    var_name: str = "animation_frames",
):
    """
    Exporta a lista de buffers de frames para um arquivo de cabeçalho C (.h).
    """
    total_frames = len(buffers)
    frame_size = (width * height) // 8
    total_bytes = total_frames * frame_size

    header_guard = os.path.basename(output_path).upper().replace(".", "_").replace("-", "_")

    os.makedirs(os.path.dirname(os.path.abspath(output_path)), exist_ok=True)

    with open(output_path, "w", encoding="utf-8") as f:
        f.write("/**\n")
        f.write(" * Arquivo gerado automaticamente por video_to_ssd1306.py\n")
        f.write(f" * Total de quadros: {total_frames}\n")
        f.write(f" * Resolução: {width}x{height}\n")
        f.write(f" * FPS sugerido: {fps}\n")
        f.write(f" * Tamanho por quadro: {frame_size} bytes\n")
        f.write(f" * Memória Flash total utilizada: {total_bytes} bytes ({total_bytes / 1024:.2f} KB)\n")
        f.write(" */\n\n")
        f.write(f"#ifndef __{header_guard}__\n")
        f.write(f"#define __{header_guard}__\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write(f"#define ANIMATION_FRAME_COUNT {total_frames}\n")
        f.write(f"#define ANIMATION_FPS         {fps}\n")
        f.write(f"#define ANIMATION_WIDTH       {width}\n")
        f.write(f"#define ANIMATION_HEIGHT      {height}\n")
        f.write(f"#define ANIMATION_FRAME_SIZE  {frame_size}\n\n")
        f.write(f"static const uint8_t {var_name}[{total_frames}][{frame_size}] = {{\n")

        for idx, buf in enumerate(buffers):
            f.write(f"    // Frame {idx + 1}/{total_frames}\n")
            f.write("    {\n        ")
            for b_idx, byte in enumerate(buf):
                f.write(f"0x{byte:02X}, ")
                if (b_idx + 1) % 16 == 0 and (b_idx + 1) < len(buf):
                    f.write("\n        ")
            f.write("\n    },\n")

        f.write("};\n\n")
        f.write(f"#endif /* __{header_guard}__ */\n")

    print(f"[OK] Arquivo C gerado com sucesso: {output_path}")
    print(f"     Quadros: {total_frames}")
    print(f"     FPS: {fps}")
    print(f"     Flash consumida: {total_bytes / 1024:.2f} KB")


def main():
    parser = argparse.ArgumentParser(
        description="Converte vídeos e GIFs em matrizes C para OLED SSD1306 (STM32)"
    )
    parser.add_argument("-i", "--input", help="Caminho do arquivo de vídeo (mp4, gif, avi, etc.)")
    parser.add_argument("-o", "--output", default="Core/Inc/animation_frames.h", help="Arquivo de saída .h (padrão: Core/Inc/animation_frames.h)")
    parser.add_argument("--demo", action="store_true", help="Gera uma animação de demonstração geométrica")
    parser.add_argument("--fps", type=int, default=15, help="Taxa de quadros por segundo (FPS) desejada (padrão: 15)")
    parser.add_argument("--width", type=int, default=128, help="Largura do display em pixels (padrão: 128)")
    parser.add_argument("--height", type=int, default=64, help="Altura do display em pixels (padrão: 64)")
    parser.add_argument("--max-frames", type=int, default=60, help="Limite máximo de quadros para poupar Flash (padrão: 60, 0 para sem limite)")
    parser.add_argument("--no-dither", action="store_true", help="Desativa dithering e usa threshold direto")
    parser.add_argument("--threshold", type=int, default=128, help="Limiar de preto/branco de 0 a 255 se dither estiver desativado (padrão: 128)")
    parser.add_argument("--invert", action="store_true", help="Inverte as cores (fundo branco / traço preto)")
    parser.add_argument("--fit", choices=["contain", "cover", "stretch"], default="contain", help="Modo de enquadramento (contain, cover, stretch)")

    args = parser.parse_args()

    if args.demo:
        print("[*] Gerando animação demo de 24 quadros...")
        raw_frames = generate_demo_frames(count=24, target_w=args.width, target_h=args.height)
    elif args.input:
        if not os.path.exists(args.input):
            print(f"[ERRO] Arquivo não encontrado: {args.input}", file=sys.stderr)
            sys.exit(1)

        ext = os.path.splitext(args.input)[1].lower()
        if ext == ".gif":
            print(f"[*] Extraindo quadros de GIF: {args.input}")
            raw_frames = extract_frames_from_gif(args.input, max_frames=args.max_frames)
        else:
            print(f"[*] Extraindo quadros do vídeo via ffmpeg a {args.fps} FPS: {args.input}")
            raw_frames = extract_frames_from_video(args.input, fps=args.fps, max_frames=args.max_frames)
    else:
        print("[!] Nenhum arquivo especificado. Gerando animação demo por padrão...")
        raw_frames = generate_demo_frames(count=24, target_w=args.width, target_h=args.height)

    if not raw_frames:
        print("[ERRO] Nenhum quadro foi extraído.", file=sys.stderr)
        sys.exit(1)

    print(f"[*] Processando {len(raw_frames)} quadros para resolução {args.width}x{args.height}...")
    processed_buffers = []
    for idx, f in enumerate(raw_frames):
        bw_img = process_frame(
            f,
            target_w=args.width,
            target_h=args.height,
            dither=not args.no_dither,
            threshold=args.threshold,
            invert=args.invert,
            fit_mode=args.fit,
        )
        buf = image_to_ssd1306_buffer(bw_img, width=args.width, height=args.height)
        processed_buffers.append(buf)

    export_to_c_header(
        processed_buffers,
        output_path=args.output,
        fps=args.fps,
        width=args.width,
        height=args.height,
    )


if __name__ == "__main__":
    main()
