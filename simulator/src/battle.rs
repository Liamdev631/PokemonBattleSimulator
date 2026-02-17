use crate::models::{Database, Move, PokemonSpecies};

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Side {
    Player,
    Opponent,
}

#[derive(Debug, Clone)]
pub struct BattlePokemon {
    pub species: PokemonSpecies,
    pub current_hp: i32,
}

impl BattlePokemon {
    pub fn new(species: PokemonSpecies) -> Self {
        let hp = species.stats.hp.max(1);
        Self {
            species,
            current_hp: hp,
        }
    }

    pub fn is_fainted(&self) -> bool {
        self.current_hp <= 0
    }
}

#[derive(Debug, Clone)]
pub struct BattleState {
    pub player: BattlePokemon,
    pub opponent: BattlePokemon,
    pub turn: u32,
}

impl BattleState {
    pub fn new(player: PokemonSpecies, opponent: PokemonSpecies) -> Self {
        Self {
            player: BattlePokemon::new(player),
            opponent: BattlePokemon::new(opponent),
            turn: 1,
        }
    }

    pub fn is_over(&self) -> bool {
        self.player.is_fainted() || self.opponent.is_fainted()
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum BattleEvent {
    MoveUsed {
        side: Side,
        move_name: String,
        damage: i32,
        target_hp: i32,
    },
    Fainted {
        side: Side,
    },
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct TurnResult {
    pub events: Vec<BattleEvent>,
    pub outcome: Option<Side>,
}

pub fn simulate_turn(
    state: &mut BattleState,
    player_move: &Move,
    opponent_move: &Move,
    db: &Database,
) -> TurnResult {
    let mut events = Vec::new();
    let player_speed = state.player.species.stats.speed;
    let opponent_speed = state.opponent.species.stats.speed;
    let order = if player_speed >= opponent_speed {
        vec![(Side::Player, player_move), (Side::Opponent, opponent_move)]
    } else {
        vec![(Side::Opponent, opponent_move), (Side::Player, player_move)]
    };

    for (side, mv) in order {
        if state.is_over() {
            break;
        }
        let (attacker, defender, target_side) = match side {
            Side::Player => (&mut state.player, &mut state.opponent, Side::Opponent),
            Side::Opponent => (&mut state.opponent, &mut state.player, Side::Player),
        };
        let damage = calculate_damage(attacker, defender, mv, db);
        if damage > 0 {
            defender.current_hp = (defender.current_hp - damage).max(0);
        }
        events.push(BattleEvent::MoveUsed {
            side,
            move_name: mv.name.clone(),
            damage,
            target_hp: defender.current_hp,
        });
        if defender.is_fainted() {
            events.push(BattleEvent::Fainted { side: target_side });
        }
    }

    let outcome = if state.player.is_fainted() {
        Some(Side::Opponent)
    } else if state.opponent.is_fainted() {
        Some(Side::Player)
    } else {
        None
    };
    if outcome.is_none() {
        state.turn += 1;
    }
    TurnResult { events, outcome }
}

pub fn calculate_damage(
    attacker: &BattlePokemon,
    defender: &BattlePokemon,
    mv: &Move,
    db: &Database,
) -> i32 {
    let power = match mv.power {
        Some(power) if power > 0 => power,
        _ => return 0,
    };
    if mv.damage_class_id == 1 {
        return 0;
    }
    let (attack, defense) = if mv.damage_class_id == 3 {
        (attacker.species.stats.sp_attack, defender.species.stats.sp_defense)
    } else {
        (attacker.species.stats.attack, defender.species.stats.defense)
    };
    let base = power as f32 * (attack as f32 / defense.max(1) as f32);
    let multiplier = type_multiplier(mv.type_id, &defender.species.types, db);
    let damage = (base * multiplier).round().max(1.0) as i32;
    damage
}

pub fn type_multiplier(move_type_id: u32, target_types: &[u32], db: &Database) -> f32 {
    let mut multiplier = 1.0;
    for target_type in target_types {
        let factor = db
            .type_advantage
            .get(&(move_type_id, *target_type))
            .copied()
            .unwrap_or(1.0);
        multiplier *= factor;
    }
    multiplier
}
