import os
from PIL import Image

FONT_DIR = '.'
OUTFILE = 'arrays.c'
FIXED_HEIGHT = 29

def rgb_to_565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

def make_array_name(label):
    return f"font_outfit24_light300_{label}"

def to_c_array(name, data, width, height):
    hex_values = [f"0x{val:04x}" for val in data]
    lines = [", ".join(hex_values[i:i + width]) for i in range(0, len(hex_values), width)]
    return f"""#define OUTFIT_24PX_LIGHT_300_E3E3E3_{name.upper()}_HEIGHT {height}
#define OUTFIT_24PX_LIGHT_300_E3E3E3_{name.upper()}_WIDTH {width}

// array size is {len(data)}
const uint16_t {make_array_name(name)}[] = {{
  {",\n  ".join(lines)}
}};
"""

def detect_char_from_filename(filename):
    for prefix in ['Letter-lower-', 'Letter-', 'Number-', 'Char-']:
        if prefix in filename:
            return prefix, filename.split(prefix)[-1].split('.bmp')[0]
    return None, None

header = '#include "font.h"\n'
c_definitions = []
glyph_entries = []
seen_array_names = set()
seen_keys = set()

char_mapping = {
    "Auml": 'Ä', "Ouml": 'Ö', "Uuml": 'Ü',
    "auml": 'ä', "ouml": 'ö', "uuml": 'ü',
    "szlig": 'ß', "DoubleQuote": '"', "Slash": '/',
    "Dash": '-', "Dot": '.', "Colon": ':', "Comma": ',',
    "Exclamation": '!', "Question": '?', "Plus": '+',
    "Equals": '=', "Hash": '#', "Percent": '%', "At": '@',
    "EUR": '€', "LeftParent": '(', "RightParent": ')',
    "Degree": '°', "Dollar": '$'
}

for fname in sorted(os.listdir(FONT_DIR)):
    if not fname.lower().endswith('.bmp'):
        continue

    prefix, char_name = detect_char_from_filename(fname)
    if not char_name:
        continue

    label = char_name.replace('szlig', 'ss') \
                     .replace('Auml', 'Ae').replace('Ouml', 'Oe').replace('Uuml', 'Ue') \
                     .replace('auml', 'ae').replace('ouml', 'oe').replace('uuml', 'ue') \
                     .replace('DoubleQuote', 'quote') \
                     .replace('LeftParent', 'lp').replace('RightParent', 'rp') \
                     .replace('-', '_').replace(' ', '_')

    # prefix einbauen für Eindeutigkeit
    if prefix == 'Letter-lower-':
        label = f"lower_{label}"
    elif prefix == 'Letter-':
        label = f"upper_{label}"

    array_name = make_array_name(label)

    if prefix == 'Letter-lower-':
        key_char = char_mapping.get(char_name, char_name.lower())
    elif prefix == 'Letter-':
        key_char = char_mapping.get(char_name, char_name.upper())
    else:
        key_char = char_mapping.get(char_name, char_name)

    key_ascii = ord(key_char) if isinstance(key_char, str) and len(key_char) == 1 and ord(key_char) < 128 else None

    if array_name in seen_array_names or (key_ascii is not None and key_ascii in seen_keys):
        print(f"⚠️  Duplikat übersprungen: {label}")
        continue

    seen_array_names.add(array_name)
    if key_ascii is not None:
        seen_keys.add(key_ascii)

    path = os.path.join(FONT_DIR, fname)
    img = Image.open(path).convert('RGB')

    width, real_height = img.size
    height = FIXED_HEIGHT

    pixels = []
    for y in range(height):
        for x in range(width):
            if y < real_height:
                pixels.append(rgb_to_565(*img.getpixel((x, y))))
            else:
                pixels.append(0x0000)

    c_definitions.append(to_c_array(label, pixels, width, height))

    if key_ascii is not None:
        glyph_entries.append(f"""    [{key_ascii}] = {{
        .data = {array_name},
        .width = OUTFIT_24PX_LIGHT_300_E3E3E3_{label.upper()}_WIDTH,
        .height = OUTFIT_24PX_LIGHT_300_E3E3E3_{label.upper()}_HEIGHT
    }}""")

# Endausgabe schreiben
full_output = header + '\n\n'.join(c_definitions) + f"""\n
const FontGlyph outfit24_glyphs[128] = {{
{',\n'.join(glyph_entries)}
}};
"""

with open(OUTFILE, 'w') as f:
    f.write(full_output)

print(f"✅ Datei '{OUTFILE}' erfolgreich erstellt – ohne Redefinitionen und mit Zebra-Fix.")
