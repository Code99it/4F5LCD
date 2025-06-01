import os
from PIL import Image

bmp_dir = "."

name_to_char = {
    "quote": "\"", "dash": "-", "doubledot": ":", "dot": ".", "comma": ",",
    "semicolon": ";", "plus": "+", "percent": "%", "star": "*", "slash": "/",
    "underscore": "_", 
    "bracket-open": "(", "bracket-close": ")",
    "degree": "°", "szlig": "ß",
    "Auml": "Ä", "Ouml": "Ö", "Uuml": "Ü",
    "auml": "ä", "ouml": "ö", "uuml": "ü"
}

def rgb888_to_rgb565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

def make_array_name(filename):
    name = os.path.splitext(filename)[0]
    return f"char_{name.replace('-', '_')}"

def resolve_char(name):
    if name.startswith("lower-"):
        return name[-1]
    elif name.startswith("number-"):
        return name.split("-")[-1]
    elif name in name_to_char:
        return name_to_char[name]
    elif len(name) == 1:
        return name
    else:
        return "?"  # Fallback bei unbekanntem Symbol

def escape_c_char(char):
    if char == "\\":
        return "'\\\\'"
    elif char == "'":
        return "'\\''"
    elif char == "\"":
        return "'\\\"'"
    elif len(char) == 1:
        return f"'{char}'"
    else:
        return "'?'"

with open("font_output.c", "w") as out_file:
    out_file.write("#include <stdint.h>\n\n")
    metadata = []

    for file_name in sorted(os.listdir(bmp_dir)):
        if not file_name.lower().endswith(".bmp"):
            continue

        base_name = os.path.splitext(file_name)[0].replace("PT-Sans-24-white-", "")
        symbol = resolve_char(base_name)
        symbol_c = escape_c_char(symbol)

        image_path = os.path.join(bmp_dir, file_name)
        image = Image.open(image_path).convert("RGB")
        width, height = image.size
        array_name = make_array_name(file_name)

        out_file.write(f"// {file_name} → {symbol}\n")
        out_file.write(f"const uint16_t {array_name}[{width * height}] = {{\n")

        for y in range(height):
            out_file.write("    ")
            for x in range(width):
                r, g, b = image.getpixel((x, y))
                rgb565 = rgb888_to_rgb565(r, g, b)
                out_file.write(f"0x{rgb565:04X}, ")
            out_file.write("\n")

        out_file.write("};\n\n")

        metadata.append((symbol_c, array_name, width, height))

    # Zeichentabelle
    out_file.write("// Zeichentabelle\n")
    out_file.write("typedef struct {\n")
    out_file.write("    char symbol;\n")
    out_file.write("    const uint16_t *bitmap;\n")
    out_file.write("    uint8_t width;\n")
    out_file.write("    uint8_t height;\n")
    out_file.write("} CharDef;\n\n")

    out_file.write("const CharDef font[] = {\n")
    for symbol_c, array_name, width, height in metadata:
        out_file.write(f"    {{ {symbol_c}, {array_name}, {width}, {height} }},\n")
    out_file.write("};\n")

# === Header-Datei erzeugen ===
with open("font_output.h", "w") as header_file:
    header_file.write("#ifndef FONT_OUTPUT_H\n")
    header_file.write("#define FONT_OUTPUT_H\n\n")
    header_file.write("#pragma once\n")
    header_file.write("#include <stdint.h>\n\n")

    header_file.write("typedef struct {\n")
    header_file.write("    char symbol;\n")
    header_file.write("    const uint16_t *bitmap;\n")
    header_file.write("    uint8_t width;\n")
    header_file.write("    uint8_t height;\n")
    header_file.write("} CharDef;\n\n")

    header_file.write("extern const CharDef font[];\n")
    header_file.write(f"extern const size_t font_len;\n\n")

    for symbol_c, array_name, width, height in metadata:
        header_file.write(f"extern const uint16_t {array_name}[{width * height}];\n")

    header_file.write("\n#endif // FONT_OUTPUT_H\n")

