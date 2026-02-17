import requests
from bs4 import BeautifulSoup
import csv
import os
import re
import json
import time

# Create directories
os.makedirs("static/csv", exist_ok=True)
os.makedirs("static/gen1", exist_ok=True)
os.makedirs("static/gen1/images", exist_ok=True)

# Helper to normalize names
def normalize(name):
    name = name.lower()
    name = name.replace("♀", "-f").replace("♂", "-m")
    name = name.replace(". ", "-").replace(" ", "-")
    name = re.sub(r"[^a-z0-9-]", "", name)
    return name

# Map type names to IDs (based on PokeAPI)
TYPE_MAP = {
    "normal": 1, "fighting": 2, "flying": 3, "poison": 4, "ground": 5, "rock": 6, "bug": 7,
    "ghost": 8, "steel": 9, "fire": 10, "water": 11, "grass": 12, "electric": 13, "psychic": 14,
    "ice": 15, "dragon": 16, "dark": 17, "fairy": 18
}

# Data Storage
pokemon_list = []
moves_map = {} # name -> id
learnsets = {} # pid -> set(move_id)

# 1. Scrape Moves Data (Gen 1)
print("Scraping Gen 1 Moves...")
url = "https://pokemondb.net/move/generation/1"
response = requests.get(url)
soup = BeautifulSoup(response.content, "html.parser")
table = soup.find("table")
rows = table.find("tbody").find_all("tr")

moves_data = []

for i, row in enumerate(rows):
    cols = row.find_all("td")
    name = cols[0].find("a").text
    type_name = cols[1].find("a").text.lower()
    category = cols[2].get("data-sort-value")
    power = cols[3].text
    accuracy = cols[4].text
    pp = cols[5].text
    
    power = int(power) if power.isdigit() else 0
    accuracy = int(accuracy) if accuracy.isdigit() else 0
    pp = int(pp) if pp.isdigit() else 0
    
    type_id = TYPE_MAP.get(type_name, 0)
    
    damage_class_id = 0
    if category == "physical": damage_class_id = 2
    elif category == "special": damage_class_id = 3
    elif category == "status": damage_class_id = 1
        
    identifier = normalize(name)
    move_id = i + 1
    moves_map[identifier] = move_id
    
    # Save for CSV/JSON
    moves_data.append({
        "id": move_id,
        "name": name,
        "identifier": identifier,
        "type_id": type_id,
        "power": power,
        "accuracy": accuracy,
        "pp": pp,
        "priority": 0,
        "damage_class_id": damage_class_id
    })

# Write Moves JSON
with open("static/gen1/moves.json", "w") as f:
    json.dump(moves_data, f, indent=None)

print(f"Scraped {len(moves_data)} Moves.")

# 2. Scrape Pokemon Data (Gen 1)
print("Scraping Gen 1 Pokemon...")
url = "https://pokemondb.net/pokedex/all"
response = requests.get(url)
soup = BeautifulSoup(response.content, "html.parser")
table = soup.find("table", id="pokedex")
rows = table.find("tbody").find_all("tr")

pokemon_json_data = []

count = 0
for row in rows:
    cols = row.find_all("td")
    id_text = cols[0].find("span", class_="infocard-cell-data").text
    id = int(id_text)
    
    if id > 151:
        break
        
    name_tag = cols[1].find("a")
    name = name_tag.text
    subname = cols[1].find("small")
    if subname:
        continue
        
    identifier = normalize(name)
    
    # Types
    types = [TYPE_MAP.get(t.text.lower(), 0) for t in cols[2].find_all("a")]
    
    # Stats
    hp = int(cols[4].text)
    attack = int(cols[5].text)
    defense = int(cols[6].text)
    sp_atk = int(cols[7].text)
    sp_def = int(cols[8].text)
    speed = int(cols[9].text)
    
    pokemon_entry = {
        "id": id,
        "name": name,
        "identifier": identifier,
        "types": types,
        "stats": {
            "hp": hp,
            "attack": attack,
            "defense": defense,
            "sp_attack": sp_atk,
            "sp_defense": sp_def,
            "speed": speed
        }
    }
    pokemon_json_data.append(pokemon_entry)
    pokemon_list.append((id, identifier, name))
    count += 1

# Write Pokemon JSON
with open("static/gen1/pokemon.json", "w") as f:
    json.dump(pokemon_json_data, f, indent=None)

print(f"Scraped {count} Pokemon.")

# 3. Scrape Learnsets
print("Scraping Gen 1 Learnsets...")
learnset_rows = []

for pid, identifier, name in pokemon_list:
    # URL for Gen 1 moves
    # Special cases for Nidoran
    url_name = identifier
    if identifier == "nidoran-f": url_name = "nidoran-f"
    elif identifier == "nidoran-m": url_name = "nidoran-m"
    elif identifier == "mr-mime": url_name = "mr-mime"
    
    # Pokemondb uses specific slugs. Usually name.lower() with hyphens.
    # Check for anomalies if needed.
    
    url = f"https://pokemondb.net/pokedex/{url_name}/moves/1"
    try:
        response = requests.get(url)
        if response.status_code != 200:
            print(f"Failed to fetch moves for {name} ({url})")
            continue
            
        soup = BeautifulSoup(response.content, "html.parser")
        
        # There are multiple tables: Level up, TM/HM.
        # We want all moves.
        tables = soup.find_all("table", class_="data-table")
        
        known_moves = set()
        
        for table in tables:
            # Check headers to ensure it's moves
            headers = [th.text for th in table.find_all("th")]
            if "Move" not in headers:
                continue
                
            move_rows = table.find("tbody").find_all("tr")
            for row in move_rows:
                cols = row.find_all("td")
                # Move name is usually in the first column for these tables?
                # Let's check the header index.
                try:
                    move_idx = headers.index("Move")
                    move_name = cols[move_idx].text.strip()
                    move_id = moves_map.get(normalize(move_name))
                    if move_id:
                        known_moves.add(move_id)
                except (ValueError, IndexError):
                    continue

        # Add to learnset
        row = [pid] + sorted(list(known_moves))
        learnset_rows.append(row)
        
        print(f"Scraped {len(known_moves)} moves for {name}")
        
    except Exception as e:
        print(f"Error scraping {name}: {e}")
        
    # Be nice to the server
    # time.sleep(0.1)

# Write Learnsets CSV
with open("static/gen1/learnsets.csv", "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerows(learnset_rows)

print("Learnsets scraped.")

# 4. Scrape Images
print("Scraping Gen 1 Images (FireRed/LeafGreen)...")
for pid, identifier, name in pokemon_list:
    # URL for FireRed/LeafGreen sprites
    # https://img.pokemondb.net/sprites/firered-leafgreen/normal/bulbasaur.png
    
    # Check for special cases
    url_name = identifier
    if identifier == "nidoran-f": url_name = "nidoran-f"
    elif identifier == "nidoran-m": url_name = "nidoran-m"
    elif identifier == "mr-mime": url_name = "mr-mime"
    
    img_url = f"https://img.pokemondb.net/sprites/firered-leafgreen/normal/{url_name}.png"
    img_path = f"static/gen1/images/{pid}.png"
    
    try:
        r = requests.get(img_url)
        if r.status_code == 200:
            with open(img_path, "wb") as f:
                f.write(r.content)
            # print(f"Downloaded image for {name}")
        else:
            print(f"Failed to download image for {name}: {r.status_code}")
    except Exception as e:
        print(f"Error downloading image for {name}: {e}")
    
    # Be nice to the server
    # time.sleep(0.1)

print("Done.")
