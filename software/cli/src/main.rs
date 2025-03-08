use std::{
    fs::File,
    io::{BufReader, BufWriter, copy},
    time::Duration,
};

use anyhow::{Context, Result};
use clap::{Parser, Subcommand};
use serialport::SerialPort;

#[derive(Parser)]
#[command(
    version,
    about,
    long_about = "The Dugmetara Keyboard MIDI controller CLI interface."
)]
#[command(propagate_version = true)]
struct Cli {
    #[command(subcommand)]
    command: Command,

    #[arg(short, long, default_value = "/dev/ttyACM0")]
    serial_port_path: String,
}

#[derive(Subcommand)]
enum Command {
    /// Switches to playing in velocity-sensitive mode (like a piano). Virtually all MIDI instruments support this.
    UseVelocitySensing,

    /// Switches to playing in polytonic-aftertouch mode (press distance of each key controls its volume). Very few MIDI instruments support this.
    UsePressureSensing,

    /// Begins sensor calibration mode. Press each key down and up fully, and then either save the results via complete_calibrating, or discard the results with cancel_calibrating. Typically this only needs to be done once.
    BeginCalibrating,

    /// Saves sensor calibration results to the SD card.
    CompleteCalibrating,

    /// Discards sensor calibration results and restores the old calibration results.
    CancelCalibrating,

    /// Logs the serial output to a file. This can be used for debugging.
    Log {
        /// Where the logs will be written.
        #[arg(short, long, default_value = "serial.log")]
        file_path: String,
    },
}

fn main() -> Result<()> {
    let cli = Cli::parse();

    let serial_port = serialport::new(&cli.serial_port_path, 0)
        .timeout(Duration::from_millis(1000))
        .open()
        .with_context(|| {
            format!(
                "Failed to open serial port at path {}",
                &cli.serial_port_path
            )
        })?;

    match &cli.command {
        Command::BeginCalibrating => send(serial_port, 0),
        Command::CompleteCalibrating => send(serial_port, 1),
        Command::CancelCalibrating => send(serial_port, 2),
        Command::UseVelocitySensing => send(serial_port, 3),
        Command::UsePressureSensing => send(serial_port, 4),
        Command::Log { file_path } => log(serial_port, file_path),
    }
}

fn log(serial_port: Box<dyn SerialPort>, file_path: &str) -> Result<()> {
    let file = File::create(file_path)
        .with_context(|| format!("Failed to open file at path {file_path}"))?;
    let mut reader = BufReader::new(serial_port);
    let mut writer = BufWriter::new(file);

    println!("Writing serial output to log file at {file_path}");
    copy(&mut reader, &mut writer).context("Error occurred while writing data to file.")?;
    Ok(())
}

fn send(mut serial_port: Box<dyn SerialPort>, value: u8) -> Result<()> {
    serial_port
        .write_all(&[value])
        .context("Failed to send data to serial port.")?;
    println!("Sent command.");
    Ok(())
}
