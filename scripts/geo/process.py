import geopandas as gpd
from shapely.geometry import mapping
import json
import pathlib

file_path = pathlib.Path(__file__).parent / "land_lines"
# Load the shapefile with GeoPandas
gdf = gpd.read_file(file_path / "countries.shp")

# Optional: Filter for specific countries or regions
# gdf = gdf[gdf['CONTINENT'] == 'Africa']  # Example: Filter to Africa

# Convert to a projection suitable for a 2D map (e.g., Mercator)
gdf = gdf.to_crs("EPSG:3395")  # Projected Mercator

# Simplify each country polygon and export as JSON
map_data = []
for _, row in gdf.iterrows():
    country = {
        "name": row["NAME"],
        "geometry": mapping(row["geometry"].simplify(0.1)),  # Tolerance parameter for simplification
    }
    map_data.append(country)

# Save to a JSON file for loading in C++
with open("map_data.json", "w") as f:
    json.dump(map_data, f)


