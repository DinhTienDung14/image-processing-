from PIL import Image
import os

WIDTH = 64
HEIGHT = 64

os.makedirs("gray_jpg", exist_ok=True)

for i in range(1, 11):
    with open(f"Gray_raw/gray{i}.raw", "rb") as f:
        data = f.read()

    if len(data) != WIDTH * HEIGHT:
        continue

    img = Image.frombytes(
        "L",
        (WIDTH, HEIGHT),
        data
    )

    img.save(f"gray_jpg/gray{i}.jpg", quality=100)
