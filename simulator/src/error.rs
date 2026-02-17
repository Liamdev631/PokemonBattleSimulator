use thiserror::Error;

#[derive(Debug, Error)]
pub enum SimulatorError {
    #[error("io error: {0}")]
    Io(#[from] std::io::Error),
    #[error("csv error: {0}")]
    Csv(#[from] csv::Error),
    #[error("missing data: {0}")]
    MissingData(String),
    #[error("invalid data: {0}")]
    InvalidData(String),
}
