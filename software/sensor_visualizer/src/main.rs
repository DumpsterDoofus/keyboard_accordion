use std::collections::VecDeque;
use std::io::BufRead;
use std::io::BufReader;
use std::sync::Arc;
use std::sync::atomic::AtomicBool;
use std::sync::atomic::Ordering;
use std::sync::mpsc::Sender;
use std::sync::mpsc::channel;
use std::thread;
use std::thread::JoinHandle;
use std::time::Duration;

use macroquad::color;
use macroquad::prelude::*;

#[macroquad::main(window_conf)]
async fn main() {
    let shutdown = Arc::new(AtomicBool::new(false));
    let (sender, receiver) = channel();
    let handle = create_receive_thread(sender, shutdown.clone());

    let mut positions = VecDeque::new();
    loop {
        if is_key_pressed(KeyCode::Escape) {
            break;
        }

        for s in receiver.try_iter() {
            positions.push_back(s);
        }

        while positions.len() > 1920 / 2 {
            positions.pop_front();
        }

        clear_background(BLACK);

        let mut colors = Vec::<Color>::new();
        for i in 0..192 {
            colors.push(color::hsl_to_rgb(i as f32 / 192.0, 1.0, 0.5));
        }

        for (pos, numbers) in positions.iter().enumerate() {
            for (number, color) in numbers.iter().zip(colors.iter()) {
                draw_rectangle(2.0f32 * pos as f32, 3.7f32 * (number - 280.0f32), 2.0, 2.0, *color);
            }
        }

        next_frame().await;
    }

    shutdown.store(true, Ordering::Relaxed);
    handle.join().unwrap();
}

fn window_conf() -> Conf {
    Conf {
        fullscreen: true,
        ..Default::default()
    }
}

fn create_receive_thread(sender: Sender<Vec<f32>>, shutdown: Arc<AtomicBool>) -> JoinHandle<()> {
    return thread::spawn(move || {
        let serial_port = serialport::new("/dev/ttyACM0", 0)
            .timeout(Duration::from_millis(1000))
            .open()
            .expect("Failed to open serial port");
        let mut reader = BufReader::new(serial_port);
        let mut line = String::new();

        while !shutdown.load(Ordering::Relaxed) {
            reader
                .read_line(&mut line)
                .expect("Failed to receive data from serial port");

            if line.starts_with("Sensor readings: ") {
                let numbers = line[17..]
                    .split(',')
                    .map(|s| s.parse::<f32>().unwrap())
                    // .take(32) // 3-key
                    .take(192) // 192-key
                    .collect();
                sender.send(numbers).unwrap();
            }

            line.clear();
        }
    });
}
