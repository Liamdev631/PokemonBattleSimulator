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
