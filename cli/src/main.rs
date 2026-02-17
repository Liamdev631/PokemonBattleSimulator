mod game;

use std::path::PathBuf;

use simulator::{load_database, normalize_name, BattleState, Side};

use crate::game::{read_input, resolve_default_move, select_move, select_pokemon};

fn main() {
    let csv_dir = PathBuf::from(env!("CARGO_MANIFEST_DIR"))
        .join("..")
        .join("static")
        .join("csv");
    let db = match load_database(&csv_dir) {
        Ok(db) => db,
        Err(error) => {
            eprintln!("Failed to load data: {error}");
            return;
        }
    };

    let default_move = match resolve_default_move(&db, "pound") {
        Ok(mv) => mv,
        Err(error) => {
            eprintln!("Failed to load default move: {error}");
            return;
        }
    };

    println!("Pokemon Battle Simulator");
    println!("Enter pokemon names using their identifiers like bulbasaur or charmander.");

    let player_species = loop {
        let name = read_input("Player pokemon: ");
        let name = normalize_name(&name);
        if let Some(species) = select_pokemon(&db, &name) {
            break species;
        }
        println!("Pokemon not found. Try again.");
    };

    let opponent_species = loop {
        let name = read_input("Opponent pokemon: ");
        let name = normalize_name(&name);
        if let Some(species) = select_pokemon(&db, &name) {
            break species;
        }
        println!("Pokemon not found. Try again.");
    };

    let mut state = BattleState::new(player_species, opponent_species);
    println!("Battle start!");

    while !state.is_over() {
        println!(
            "Turn {} - Player HP: {} | Opponent HP: {}",
            state.turn, state.player.current_hp, state.opponent.current_hp
        );
        let input = read_input("Move name: ");
        let player_move = select_move(&db, &input, default_move);
        let opponent_move = default_move;
        let turn_result = simulator::simulate_turn(&mut state, player_move, opponent_move, &db);
        for event in turn_result.events {
            match event {
                simulator::BattleEvent::MoveUsed {
                    side,
                    move_name,
                    damage,
                    target_hp,
                } => {
                    let side_name = match side {
                        Side::Player => "Player",
                        Side::Opponent => "Opponent",
                    };
                    println!("{side_name} used {move_name} for {damage} damage. Target HP: {target_hp}");
                }
                simulator::BattleEvent::Fainted { side } => {
                    let side_name = match side {
                        Side::Player => "Player",
                        Side::Opponent => "Opponent",
                    };
                    println!("{side_name} fainted!");
                }
            }
        }
    }
}
