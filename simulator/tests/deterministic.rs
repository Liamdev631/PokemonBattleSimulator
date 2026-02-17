use std::path::PathBuf;

use simulator::{calculate_damage, load_database, normalize_name, type_multiplier, BattleState};

fn csv_dir() -> PathBuf {
    PathBuf::from(env!("CARGO_MANIFEST_DIR"))
        .join("..")
        .join("static")
        .join("csv")
}

#[test]
fn loads_pokemon_and_moves() {
    let db = load_database(&csv_dir()).expect("load database");
    let bulbasaur = db
        .get_pokemon(&normalize_name("bulbasaur"))
        .expect("bulbasaur exists");
    let pound = db.get_move(&normalize_name("pound")).expect("pound exists");
    assert_eq!(bulbasaur.stats.hp, 45);
    assert_eq!(pound.power, Some(40));
}

#[test]
fn type_multiplier_matches_chart() {
    let db = load_database(&csv_dir()).expect("load database");
    let multiplier = type_multiplier(1, &[8], &db);
    assert_eq!(multiplier, 0.0);
}

#[test]
fn damage_calculation_is_deterministic() {
    let db = load_database(&csv_dir()).expect("load database");
    let bulbasaur = db
        .get_pokemon(&normalize_name("bulbasaur"))
        .expect("bulbasaur exists")
        .clone();
    let charmander = db
        .get_pokemon(&normalize_name("charmander"))
        .expect("charmander exists")
        .clone();
    let pound = db.get_move(&normalize_name("pound")).expect("pound exists");

    let state = BattleState::new(bulbasaur, charmander);
    let damage = calculate_damage(&state.player, &state.opponent, pound, &db);
    assert_eq!(damage, 46);
}

#[test]
fn turn_order_uses_speed() {
    let db = load_database(&csv_dir()).expect("load database");
    let bulbasaur = db
        .get_pokemon(&normalize_name("bulbasaur"))
        .expect("bulbasaur exists")
        .clone();
    let charmander = db
        .get_pokemon(&normalize_name("charmander"))
        .expect("charmander exists")
        .clone();
    let pound = db.get_move(&normalize_name("pound")).expect("pound exists");
    let mut state = BattleState::new(bulbasaur, charmander);
    let result = simulator::simulate_turn(&mut state, pound, pound, &db);
    assert_eq!(
        result.events.first(),
        Some(&simulator::BattleEvent::MoveUsed {
            side: simulator::Side::Opponent,
            move_name: "pound".to_string(),
            damage: 42,
            target_hp: 3,
        })
    );
}
