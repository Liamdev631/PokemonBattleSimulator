import json
import csv
import sys

def load_json(path):
    with open(path, "r") as f:
        return json.load(f)

def load_learnsets(path):
    learnsets = {}
    with open(path, "r") as f:
        reader = csv.reader(f)
        for row in reader:
            if not row: continue
            pid = int(row[0])
            moves = [int(m) for m in row[1:]]
            learnsets[pid] = set(moves)
    return learnsets

print("Loading data...")
pokemon = load_json("static/gen1/pokemon.json")
moves = load_json("static/gen1/moves.json")
learnsets = load_learnsets("static/gen1/learnsets.csv")

# Create maps for easier lookup
pokemon_map = {p["id"]: p for p in pokemon}
moves_map = {m["id"]: m for m in moves}
moves_name_map = {m["identifier"]: m["id"] for m in moves}

errors = []

# 1. Check all 151 Pokemon exist
if len(pokemon) != 151:
    errors.append(f"Expected 151 Pokemon, found {len(pokemon)}")

# 2. Check Learnsets coverage
if len(learnsets) != 151:
    errors.append(f"Expected 151 learnsets, found {len(learnsets)}")

# 3. Specific Checks (Sanity Checks)
# Charizard (6) should learn Flamethrower
charizard_id = 6
flamethrower_id = moves_name_map.get("flamethrower")
if not flamethrower_id:
    errors.append("Flamethrower not found in moves db")
elif flamethrower_id not in learnsets.get(charizard_id, []):
    errors.append(f"Charizard (6) does not learn Flamethrower ({flamethrower_id})")

# Pikachu (25) should learn Thunderbolt
pikachu_id = 25
thunderbolt_id = moves_name_map.get("thunderbolt")
if not thunderbolt_id:
    errors.append("Thunderbolt not found in moves db")
elif thunderbolt_id not in learnsets.get(pikachu_id, []):
    errors.append(f"Pikachu (25) does not learn Thunderbolt ({thunderbolt_id})")

# Magikarp (129) should learn Splash
magikarp_id = 129
splash_id = moves_name_map.get("splash")
if not splash_id:
    errors.append("Splash not found in moves db")
elif splash_id not in learnsets.get(magikarp_id, []):
    errors.append(f"Magikarp (129) does not learn Splash ({splash_id})")

# Magikarp should NOT learn Hyper Beam
hyper_beam_id = moves_name_map.get("hyper-beam")
if hyper_beam_id and hyper_beam_id in learnsets.get(magikarp_id, []):
    errors.append(f"Magikarp (129) SHOULD NOT learn Hyper Beam ({hyper_beam_id})")

# Mew (151) should learn Transform (via Level up in Gen 1? Actually Mew learns Transform at level 10)
mew_id = 151
transform_id = moves_name_map.get("transform")
if not transform_id:
    errors.append("Transform not found in moves db")
elif transform_id not in learnsets.get(mew_id, []):
    errors.append(f"Mew (151) does not learn Transform ({transform_id})")

# Ditto (132) should learn Transform
ditto_id = 132
if transform_id not in learnsets.get(ditto_id, []):
    errors.append(f"Ditto (132) does not learn Transform ({transform_id})")

# Check Type consistency
# Charizard is Fire/Flying (10, 3)
charizard = pokemon_map.get(6)
if charizard:
    types = charizard["types"]
    if 10 not in types or 3 not in types:
        errors.append(f"Charizard types incorrect: {types}. Expected Fire(10) and Flying(3)")

# Check Stats consistency
# Mew has 100 base stats across board
mew = pokemon_map.get(151)
if mew:
    stats = mew["stats"]
    if not all(v == 100 for v in stats.values()):
        errors.append(f"Mew stats incorrect: {stats}")

if errors:
    print("Verification FAILED with errors:")
    for e in errors:
        print(f"- {e}")
    sys.exit(1)
else:
    print("Verification PASSED!")
    print(f"Checked {len(pokemon)} Pokemon and {len(moves)} Moves.")
