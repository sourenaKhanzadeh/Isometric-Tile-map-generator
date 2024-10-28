import geopandas as gpd
from shapely.geometry import mapping
import json
import pathlib

file_path = pathlib.Path(__file__).parent / "countries"
# Load the shapefile with GeoPandas
gdf = gpd.read_file(file_path / "ne_10m_admin_0_countries.shp")

# Optional: Filter for specific countries or regions
# gdf = gdf[gdf['CONTINENT'] == 'Africa']  # Example: Filter to Africa

# Convert to a projection suitable for a 2D map (e.g., Mercator)
gdf = gdf.to_crs("EPSG:3395")  # Projected Mercator

# Create GeoJSON structure
geojson = {
    "type": "FeatureCollection",
    "features": []
}

for _, row in gdf.iterrows():
    feature = {
        "type": "Feature",
        "properties": {
            "name": row["NAME"]
        },
        "geometry": mapping(row["geometry"].simplify(0.1))  # Tolerance parameter for simplification
    }
    geojson["features"].append(feature)

# Save to a GeoJSON file for loading in C++
with open("map_data.json", "w") as f:
    json.dump(geojson, f)
