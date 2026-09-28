from PIL import Image

WIDTH = 64
HEIGHT = 64

for i in range(1,11):
    with open(f"Edge_raw/edge{i}_H.raw", "rb") as f:
        data = f.read()

    img = Image.frombytes("L", (WIDTH, HEIGHT), data)

    img = img.point(lambda p: 255 if p else 0)

    img = img.resize((512, 512), Image.Resampling.NEAREST)

    img.save(f"edge_H/edge{i}_H.png")
for i in range(1,11):
    with open(f"Edge_raw/edge{i}_P.raw", "rb") as f:
        data = f.read()
    
    img = Image.frombytes("L", (WIDTH, HEIGHT), data)

    img = img.point(lambda p: 255 if p else 0)

    img = img.resize((512, 512), Image.Resampling.NEAREST)

    img.save(f"edge_P/edge{i}_P.png")
for i in range(1,11):
    with open(f"Edge_raw/edge{i}_V.raw", "rb") as f:
        data = f.read()
    
    img = Image.frombytes("L", (WIDTH, HEIGHT), data)

    img = img.point(lambda p: 255 if p else 0)

    img = img.resize((512, 512), Image.Resampling.NEAREST)

    img.save(f"edge_V/edge{i}_V.png")

for i in range(1,11):
    with open(f"Edge_raw/edge{i}_M.raw", "rb") as f:
        data = f.read()
    
    img = Image.frombytes("L", (WIDTH, HEIGHT), data)

    img = img.point(lambda p: 255 if p else 0)

    img = img.resize((512, 512), Image.Resampling.NEAREST)

    img.save(f"edge_M/edge{i}_M.png")