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

        for (pos, numbers) in positions.iter().enumerate() {
            // let start_index = 0; // 3-key
            let start_index = 112; // 192-key
            let s1 = numbers[start_index + 0];
            let s2 = numbers[start_index + 1];
            let s3 = numbers[start_index + 2];
            draw_circle(2.0f32 * pos as f32, 3.7f32 * (s1 - 280.0f32), 2.0, RED);
            draw_circle(2.0f32 * pos as f32, 3.7f32 * (s2 - 280.0f32), 2.0, GREEN);
            draw_circle(2.0f32 * pos as f32, 3.7f32 * (s3 - 280.0f32), 2.0, BLUE);
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
                    .map(|s| match s.parse::<f32>() {
                        Ok(num) => num,
                        Err(v) => {
                            println!("Error parsing number with text \"{}\": {}", s, v);
                            0.0
                        }
                    })
                    // .take(32) // 3-key
                    .take(192) // 192-key
                    .collect();
                sender.send(numbers).unwrap();
            }

            line.clear();
        }
    });
}
