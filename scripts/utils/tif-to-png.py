from PIL import Image
import os

Image.MAX_IMAGE_PIXELS = None  # Or set to a specific number if you want a controlled limit

input_folder = "res/Earth-Large.tif"
output_folder = "res/Earth-Large.png"

img = Image.open(input_folder)
img.save(output_folder)
