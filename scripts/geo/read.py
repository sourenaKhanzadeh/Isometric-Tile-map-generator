import json

with open("map_data.json", "r") as file:
    data = json.load(file)

# Print the data to understand its structure
print(data)  # Add this line to inspect the structure

# Assuming data is a list of dictionaries
for item in data:
    if "features" in item:
        for feature in item["features"]:
            print(feature["geometry"]["coordinates"])
