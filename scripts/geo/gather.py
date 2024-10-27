# Example: Downloading Natural Earth country boundaries
import urllib.request

url = "https://www.naturalearthdata.com/http//www.naturalearthdata.com/download/50m/cultural/ne_50m_admin_0_countries.zip"
urllib.request.urlretrieve(url, "countries.zip")
