use std::io::{self, Write};

use simulator::{normalize_name, Database, Move, PokemonSpecies, SimulatorError};

pub fn read_input(prompt: &str) -> String {
    print!("{prompt}");
    let _ = io::stdout().flush();
    let mut input = String::new();
    let _ = io::stdin().read_line(&mut input);
    input.trim().to_string()
}

pub fn select_pokemon(db: &Database, name: &str) -> Option<PokemonSpecies> {
    db.get_pokemon(name).cloned()
}

pub fn select_move<'a>(db: &'a Database, input: &str, default_move: &'a Move) -> &'a Move {
    let name = normalize_name(input);
    db.get_move(&name).unwrap_or(default_move)
}

pub fn resolve_default_move<'a>(
    db: &'a Database,
    name: &str,
) -> Result<&'a Move, SimulatorError> {
    let name = normalize_name(name);
    db.get_move(&name)
        .ok_or_else(|| SimulatorError::MissingData(format!("move {name}")))
}

#[cfg(test)]
mod tests {
    use std::path::PathBuf;

    use simulator::load_database;

    use super::*;

    fn csv_dir() -> PathBuf {
        PathBuf::from(env!("CARGO_MANIFEST_DIR"))
            .join("..")
            .join("static")
            .join("csv")
    }

    #[test]
    fn select_move_falls_back_to_default() {
        let db = load_database(&csv_dir()).expect("load database");
        let default_move = resolve_default_move(&db, "pound").expect("default move");
        let selected = select_move(&db, "not-a-move", default_move);
        assert_eq!(selected.name, "pound");
    }

    #[test]
    fn select_pokemon_uses_normalized_name() {
        let db = load_database(&csv_dir()).expect("load database");
        let chosen = select_pokemon(&db, &normalize_name("Bulbasaur"));
        assert!(chosen.is_some());
    }
}
