import os
from PIL import Image
import re

# Konfiguration
FONT_NAME = "Outfit-13px-Light-100-e3e3e3"
BMP_DIR = "./"  # Verzeichnis mit BMP-Dateien
OUTFILE = "font_generated.c"

# Mapping für Sonderzeichen
CHAR_MAP = {
    "Exclamation": "!",
    "Doublequote": "\"",
    "Hash": "#",
    "Dollar": "$",
    "Percent": "%",
    "Ampersand": "&",
    "Singlequote": "'",
    "Left_Parent": "(",
    "Right_Parent": ")",
    "Star": "*",
    "Plus": "+",
    "Comma": ",",
    "Dash": "-",
    "Dot": ".",
    "Slash": "/",
    "Colon": ":",
    "Semicolon": ";",
    "Less": "<",
    "Equals": "=",
    "Greater": ">",
    "Question": "?",
    "At": "@",
    "Left_Square": "[",
    "Backslash": "\\",
    "Right_Square": "]",
    "Caret": "^",
    "Underscore": "_",
    "Backtick": "`",
    "Left_Curly": "{",
    "Pipe": "|",
    "Right_Curly": "}",
    "Tilde": "~",
    "Deg": "°",
    "Euro": "€",
}

def char_name_to_ascii(name):
    if name.startswith("Lowercase-"):
        return name[-1].lower()
    elif name.startswith("Uppercase-"):
        return name[-1].upper()
    elif name.startswith("Letter-"):
        return name.split("-")[-1]
    elif name.startswith("Number-"):
        return name.split("-")[-1]
    elif name in CHAR_MAP:
        return CHAR_MAP[name]
    return None

def to_macro(name):
    return name.upper().replace("-", "_")

bmp_files = sorted(f for f in os.listdir(BMP_DIR) if f.endswith(".bmp") and FONT_NAME in f)

arrays = []
macros = []
mapping_entries = []

for bmp in bmp_files:
    match = re.match(rf"{FONT_NAME}-(.+?)\.bmp", bmp)
    if not match:
        continue
    cname = match.group(1)
    ascii_char = char_name_to_ascii(cname)
    if ascii_char is None or len(ascii_char) != 1:
        continue

    varname = f"{FONT_NAME.replace('-', '_')}_{cname.replace('-', '_')}"
    macro_w = f"{to_macro(varname)}_WIDTH"
    macro_h = f"{to_macro(varname)}_HEIGHT"

    img = Image.open(os.path.join(BMP_DIR, bmp)).convert("L")
    width, height = img.size
    pixels = list(img.getdata())

    rgb565_data = []
    for p in pixels:
        r = g = b = p
        r5 = (r >> 3) & 0x1F
        g6 = (g >> 2) & 0x3F
        b5 = (b >> 3) & 0x1F
        rgb565 = (r5 << 11) | (g6 << 5) | b5
        rgb565_data.append(rgb565)

    array = f"const uint16_t {varname}[] = {{\n"
    for y in range(height):
        line = rgb565_data[y*width:(y+1)*width]
        array += "    " + ", ".join(f"0x{val:04X}" for val in line) + ",\n"
    array += "};\n"
    arrays.append(array)

    macros.append(f"#define {macro_w} {width}")
    macros.append(f"#define {macro_h} {height}")

    mapping = f"""    ['{ascii_char}'] = {{
        .data = {varname},
        .width = {macro_w},
        .height = {macro_h}
    }}"""
    mapping_entries.append(mapping)

with open(OUTFILE, "w") as f:
    f.write("#include <stdint.h>\n\n")
    f.write("// Bitmap Arrays\n\n")
    f.writelines(arrays)
    f.write("\n// Dimensions\n\n")
    f.writelines(line + "\n" for line in macros)
    f.write("\n\n// Mapping\n")
    f.write("const FontGlyph outfit13_glyphs[128] = {\n")
    f.write(",\n".join(mapping_entries))
    f.write("\n};\n")
