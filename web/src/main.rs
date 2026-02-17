use actix_files::Files;
use actix_web::{get, App, HttpResponse, HttpServer, Responder, web};
use simulator::{Database, load_gen1_database};
use std::path::Path;

struct AppState {
    db: Database,
}

#[get("/api/pokemon")]
async fn get_pokemon(data: web::Data<AppState>) -> impl Responder {
    let mut list: Vec<_> = data.db.pokemon.values().collect();
    list.sort_by_key(|p| p.id);
    HttpResponse::Ok().json(list)
}

#[get("/api/moves")]
async fn get_moves(data: web::Data<AppState>) -> impl Responder {
    let mut list: Vec<_> = data.db.moves.values().collect();
    list.sort_by_key(|m| m.id);
    HttpResponse::Ok().json(list)
}

#[get("/")]
async fn index() -> impl Responder {
    HttpResponse::Ok()
        .content_type("text/html")
        .body(include_str!("../../static/index.html"))
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    env_logger::init();
    
    let db_path = Path::new("static/gen1");
    // Ensure the path exists
    if !db_path.exists() {
        eprintln!("Database path not found: {:?}", db_path);
    }
    
    let db = load_gen1_database(db_path).expect("Failed to load database");
    
    let state = web::Data::new(AppState {
        db,
    });

    println!("Starting server at http://127.0.0.1:8080");

    HttpServer::new(move || {
        App::new()
            .app_data(state.clone())
            .service(index)
            .service(get_pokemon)
            .service(get_moves)
            .service(Files::new("/static", "static").show_files_listing())
    })
    .bind(("127.0.0.1", 8080))?
    .run()
    .await
}
