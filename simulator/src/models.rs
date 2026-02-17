use std::collections::HashMap;
use serde::{Deserialize, Serialize};

#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct Stats {
    pub hp: i32,
    pub attack: i32,
    pub defense: i32,
    pub sp_attack: i32,
    pub sp_defense: i32,
    pub speed: i32,
}

impl Stats {
    pub fn empty() -> Self {
        Self {
            hp: 0,
            attack: 0,
            defense: 0,
            sp_attack: 0,
            sp_defense: 0,
            speed: 0,
        }
    }
}

#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct PokemonSpecies {
    pub id: u32,
    pub name: String,
    pub stats: Stats,
    pub types: Vec<u32>,
    #[serde(default)]
    pub learnset: Vec<u32>,
}

#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct Move {
    pub id: u32,
    pub name: String,
    pub type_id: u32,
    pub power: Option<i32>,
    pub accuracy: Option<i32>,
    pub priority: i32,
    pub damage_class_id: u32,
}

#[derive(Debug, Clone)]
pub struct Database {
    pub pokemon: HashMap<String, PokemonSpecies>,
    pub moves: HashMap<String, Move>,
    pub type_names: HashMap<u32, String>,
    pub type_advantage: HashMap<(u32, u32), f32>,
}

impl Database {
    pub fn get_pokemon(&self, name: &str) -> Option<&PokemonSpecies> {
        self.pokemon.get(name)
    }

    pub fn get_move(&self, name: &str) -> Option<&Move> {
        self.moves.get(name)
    }
}
