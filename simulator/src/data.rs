use std::collections::HashMap;
use std::path::{Path, PathBuf};

use serde::Deserialize;

use crate::error::SimulatorError;
use crate::models::{Database, Move, PokemonSpecies, Stats};

#[derive(Deserialize)]
struct PokemonRow {
    id: u32,
    identifier: String,
    is_default: u8,
}

#[derive(Deserialize)]
struct StatRow {
    pokemon_id: u32,
    stat_id: u32,
    base_stat: i32,
}

#[derive(Deserialize)]
struct TypeRow {
    pokemon_id: u32,
    type_id: u32,
    slot: u32,
}

#[derive(Deserialize)]
struct MoveRow {
    id: u32,
    identifier: String,
    type_id: u32,
    power: Option<i32>,
    accuracy: Option<i32>,
    priority: i32,
    damage_class_id: u32,
}

#[derive(Deserialize)]
struct TypeNameRow {
    type_id: u32,
    local_language_id: u32,
    name: String,
}

#[derive(Deserialize)]
struct TypeAdvantageRow {
    damage_type_id: u32,
    target_type_id: u32,
    damage_factor: u32,
}

pub fn normalize_name(input: &str) -> String {
    input
        .trim()
        .to_lowercase()
        .split_whitespace()
        .collect::<Vec<_>>()
        .join("-")
}

pub fn load_database(csv_dir: &Path) -> Result<Database, SimulatorError> {
    let pokemon_rows = read_csv::<PokemonRow>(csv_dir.join("pokemon.csv"))?;
    let stat_rows = read_csv::<StatRow>(csv_dir.join("pokemon_stats.csv"))?;
    let type_rows = read_csv::<TypeRow>(csv_dir.join("pokemon_types.csv"))?;
    let move_rows = read_csv::<MoveRow>(csv_dir.join("moves.csv"))?;
    let type_name_rows = read_csv::<TypeNameRow>(csv_dir.join("type_names.csv"))?;
    let type_advantage_rows = read_csv::<TypeAdvantageRow>(csv_dir.join("type_advantage.csv"))?;

    let mut type_names = HashMap::new();
    for row in type_name_rows {
        if row.local_language_id == 9 {
            type_names.insert(row.type_id, row.name);
        }
    }

    let mut type_advantage = HashMap::new();
    for row in type_advantage_rows {
        let factor = row.damage_factor as f32 / 100.0;
        type_advantage.insert((row.damage_type_id, row.target_type_id), factor);
    }

    let mut stats_map: HashMap<u32, Stats> = HashMap::new();
    for row in stat_rows {
        let entry = stats_map.entry(row.pokemon_id).or_insert_with(Stats::empty);
        match row.stat_id {
            1 => entry.hp = row.base_stat,
            2 => entry.attack = row.base_stat,
            3 => entry.defense = row.base_stat,
            4 => entry.sp_attack = row.base_stat,
            5 => entry.sp_defense = row.base_stat,
            6 => entry.speed = row.base_stat,
            _ => {}
        }
    }

    let mut type_map: HashMap<u32, Vec<(u32, u32)>> = HashMap::new();
    for row in type_rows {
        type_map
            .entry(row.pokemon_id)
            .or_default()
            .push((row.slot, row.type_id));
    }

    let mut pokemon = HashMap::new();
    for row in pokemon_rows {
        if row.is_default != 1 {
            continue;
        }
        let stats = stats_map
            .get(&row.id)
            .ok_or_else(|| SimulatorError::MissingData(format!("stats for pokemon {}", row.id)))?
            .clone();
        let mut types = type_map
            .get(&row.id)
            .ok_or_else(|| SimulatorError::MissingData(format!("types for pokemon {}", row.id)))?
            .clone();
        types.sort_by_key(|(slot, _)| *slot);
        let types = types.into_iter().map(|(_, type_id)| type_id).collect();
        let name = normalize_name(&row.identifier);
        pokemon.insert(
            name.clone(),
            PokemonSpecies {
                id: row.id,
                name,
                stats,
                types,
            },
        );
    }

    let mut moves = HashMap::new();
    for row in move_rows {
        let name = normalize_name(&row.identifier);
        moves.insert(
            name.clone(),
            Move {
                id: row.id,
                name,
                type_id: row.type_id,
                power: row.power,
                accuracy: row.accuracy,
                priority: row.priority,
                damage_class_id: row.damage_class_id,
            },
        );
    }

    Ok(Database {
        pokemon,
        moves,
        type_names,
        type_advantage,
    })
}

fn read_csv<T: for<'de> Deserialize<'de>>(path: PathBuf) -> Result<Vec<T>, SimulatorError> {
    let mut reader = csv::Reader::from_path(path)?;
    let mut rows = Vec::new();
    for result in reader.deserialize() {
        rows.push(result?);
    }
    Ok(rows)
}

pub fn load_gen1_database(json_dir: &Path) -> Result<Database, SimulatorError> {
    // Load Types
    let type_file = std::fs::File::open(json_dir.join("types.json"))
        .map_err(|e| SimulatorError::MissingData(format!("types.json: {}", e)))?;
    let type_reader = std::io::BufReader::new(type_file);
    let type_names: HashMap<u32, String> = serde_json::from_reader(type_reader)
        .map_err(|e| SimulatorError::MissingData(format!("types.json parse: {}", e)))?;

    // Load Moves
    #[derive(Deserialize)]
    struct MoveJson {
        id: u32,
        name: String,
        identifier: String,
        type_id: u32,
        power: Option<i32>,
        accuracy: Option<i32>,
        priority: i32,
        damage_class_id: u32,
    }

    let moves_file = std::fs::File::open(json_dir.join("moves.json"))
        .map_err(|e| SimulatorError::MissingData(format!("moves.json: {}", e)))?;
    let moves_reader = std::io::BufReader::new(moves_file);
    let moves_list: Vec<MoveJson> = serde_json::from_reader(moves_reader)
        .map_err(|e| SimulatorError::MissingData(format!("moves.json parse: {}", e)))?;
    
    let mut moves = HashMap::new();
    for m in moves_list {
        let move_struct = Move {
            id: m.id,
            name: m.name,
            type_id: m.type_id,
            power: m.power,
            accuracy: m.accuracy,
            priority: m.priority,
            damage_class_id: m.damage_class_id,
        };
        moves.insert(m.identifier, move_struct);
    }

    // Load Pokemon
    // Note: pokemon.json has "identifier" field but PokemonSpecies struct uses "name"
    // We need to match what the struct expects.
    // Let's check PokemonSpecies struct in models.rs again.
    // It has `pub name: String`.
    // My pokemon.json has "identifier": "bulbasaur", "name": "Bulbasaur".
    // I should use "name" for display, "identifier" for lookup?
    // The Database uses HashMap<String, PokemonSpecies> where key is normalized name.
    
    let pokemon_file = std::fs::File::open(json_dir.join("pokemon.json"))
        .map_err(|e| SimulatorError::MissingData(format!("pokemon.json: {}", e)))?;
    let pokemon_reader = std::io::BufReader::new(pokemon_file);
    
    // Define a temporary struct matching JSON if needed, or use PokemonSpecies directly if it matches.
    // PokemonSpecies has `pub name: String`, `pub stats: Stats`, `pub types: Vec<u32>`.
    // My JSON has `identifier`, `name`, `stats`, `types`.
    // `PokemonSpecies` derives Deserialize, so it should work if fields match.
    // Extra fields in JSON are ignored by default in serde_json? No, unless `#[serde(deny_unknown_fields)]` is used.
    // But `PokemonSpecies` struct doesn't have `identifier`.
    // I should probably add `identifier` to `PokemonSpecies` or use a temporary struct.
    // Let's check `models.rs` again.
    
    #[derive(Deserialize)]
    struct PokemonJson {
        id: u32,
        name: String,
        identifier: String,
        types: Vec<u32>,
        stats: Stats,
    }

    let pokemon_list: Vec<PokemonJson> = serde_json::from_reader(pokemon_reader)
        .map_err(|e| SimulatorError::MissingData(format!("pokemon.json parse: {}", e)))?;

    let mut pokemon = HashMap::new();
    for p in pokemon_list {
        let species = PokemonSpecies {
            id: p.id,
            name: p.name.clone(),
            stats: p.stats,
            types: p.types,
        };
        // Use identifier as key for lookup? Or name?
        // load_database uses normalize_name(&row.identifier).
        // Let's use p.identifier.
        pokemon.insert(p.identifier, species);
    }

    // Load Learnsets
    let learnset_path = json_dir.join("learnsets.csv");
    if learnset_path.exists() {
        let mut reader = csv::ReaderBuilder::new()
            .has_headers(false)
            .flexible(true)
            .from_path(learnset_path)
            .map_err(|e| SimulatorError::MissingData(format!("learnsets.csv: {}", e)))?;
        
        for result in reader.records() {
            let record = result.map_err(|e| SimulatorError::MissingData(format!("learnsets.csv record: {}", e)))?;
            if let Some(id_str) = record.get(0) {
                if let Ok(pokemon_id) = id_str.parse::<u32>() {
                    // Find the pokemon with this ID
                    // Since we stored pokemon by identifier (string), we need to iterate or change storage.
                    // Wait, finding by ID in HashMap<String, Pokemon> is slow (O(N)).
                    // But we only do this once at startup. 151 items is fine.
                    // Or we can build a temporary map ID -> Identifier.
                    
                    let mut move_ids = Vec::new();
                    for i in 1..record.len() {
                        if let Some(move_id_str) = record.get(i) {
                            if !move_id_str.is_empty() {
                                if let Ok(mid) = move_id_str.parse::<u32>() {
                                    move_ids.push(mid);
                                }
                            }
                        }
                    }
                    
                    // Update pokemon
                    for species in pokemon.values_mut() {
                        if species.id == pokemon_id {
                            species.learnset = move_ids;
                            break;
                        }
                    }
                }
            }
        }
    }

    // Type Advantage - Placeholder for now
    let type_advantage = HashMap::new();

    Ok(Database {
        pokemon,
        moves,
        type_names,
        type_advantage,
    })
}
