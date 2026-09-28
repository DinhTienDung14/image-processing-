from PIL import Image

import os

os.makedirs("raw", exist_ok=True)
for i in range(1,11):
    img = Image.open(f"anhgoc/{i}.jpg")
    img = img.convert("RGB")
    img = img.resize((64,64))
    with open(f"RGB_raw/{i}.raw", "wb") as f:
        f.write(img.tobytes())

    

    