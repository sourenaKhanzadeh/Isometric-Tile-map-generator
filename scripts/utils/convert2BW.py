import cv2
import numpy as np
import json

image = cv2.imread("res/Earth-Small.png")

image = cv2.resize(image, (1920, 1080))

# Convert to grayscale
gray_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

# Apply Gaussian smoothing (optional)
blurred_image = cv2.GaussianBlur(gray_image, (3, 3), 0)

# Sobel operators
Gx = cv2.Sobel(blurred_image, cv2.CV_64F, 1, 0, ksize=3)
Gy = cv2.Sobel(blurred_image, cv2.CV_64F, 0, 1, ksize=3)

# Gradient magnitude
G = np.sqrt(Gx**2 + Gy**2)

# Normalize to range 0-255
G = np.uint8(255 * G / np.max(G))

# Threshold to create a binary image
_, binary_image = cv2.threshold(G, 50, 255, cv2.THRESH_BINARY)

# Find contours (polygons outlining the map)
contours, _ = cv2.findContours(binary_image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

# Save contours as JSON
contours_list = [contour.flatten().tolist() for contour in contours]
with open("contours.json", "w") as json_file:
    json.dump(contours_list, json_file)

# Draw contours on the original image
contour_image = cv2.cvtColor(binary_image, cv2.COLOR_GRAY2BGR)  # Convert binary image to BGR for color drawing
cv2.drawContours(contour_image, contours, -1, (0, 255, 0), 2)  # Draw contours in green

cv2.imshow("Contours", contour_image)
cv2.waitKey(0)
cv2.destroyAllWindows()