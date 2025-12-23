use std::io::Write;

extern crate hidapi;

fn main() {
    env_logger::init();
    let foot_pedal = get_foot_pedal();
    let mut midi_output = get_midi_output();
    let pedal_on: [u8; 3] = [0xB0, 0x40, 0x7F];
    let pedal_off: [u8; 3] = [0xB0, 0x40, 0x00];

    let mut buffer = [0u8; 64];
    loop {
        let size = foot_pedal
            .read(&mut buffer)
            .expect("Failed to read data from device.");
        let data = &buffer[..size];
        log::debug!("Received {} bytes: {:?}", size, data);

        if size == 9 {
            // When pressed, I get this data: [1, 0, 0, 5, 0, 0, 0, 0, 0]. The first byte is the report ID, and the remaining 8 bytes are a USB HID keyboard report, in this case sending the letter "b". When released, I get this data: [1, 0, 0, 0, 0, 0, 0, 0, 0]. This means no key is pressed.
            if data[3] > 0 {
                log::info!("Pedal pressed");
                midi_output
                    .send(&pedal_on)
                    .expect("Failed to send pedal on.");
            } else {
                log::info!("Pedal released");
                midi_output
                    .send(&pedal_off)
                    .expect("Failed to send pedal off.");
            }
        } else {
            log::warn!("Expected 9 bytes, so ignoring data.");
        }
    }
}
fn get_foot_pedal() -> hidapi::HidDevice {
    let hid_api = hidapi::HidApi::new().expect("Failed to initialize HID API.");

    log::debug!("Printing all available USB HID devices:");
    for device_info in hid_api.device_list() {
        print_device_info(device_info);
    }

    // "pcsensor", see https://usb-ids.gowdy.us/read/UD/3553
    let vendor_id = 0x3553;

    // "footswitch", see https://usb-ids.gowdy.us/read/UD/3553/b001
    let product_id = 0xb001;

    // 0 is the data reading interface, 1 is the configuration writing interface
    let interface_number = 0;

    // (1, 6) is a keyboard. It seems to send "b" when pressed.
    let usage_page = 1;
    let usage = 6;

    log::debug!(
        "Searching for a device with the following criteria:\nvendor_id: {vendor_id}\nproduct_id: {product_id}\ninterface_number: {interface_number}\nusage_page: {usage_page}\nusage: {usage}"
    );

    let device_info = hid_api
        .device_list()
        .find(|d| {
            d.vendor_id() == vendor_id
                && d.product_id() == product_id
                && d.interface_number() == interface_number
                && d.usage_page() == usage_page
                && d.usage() == usage
        })
        .expect("Device not found.");

    log::debug!("Connecting to device at path: {:?}", device_info.path());

    return device_info
        .open_device(&hid_api)
        .expect("Failed to open device.");
}

fn print_device_info(device: &hidapi::DeviceInfo) {
    let path = device.path();
    let vendor_id = device.vendor_id();
    let product_id = device.product_id();
    let serial_number = device.serial_number();
    let release_number = device.release_number();
    let manufacturer_string = device.manufacturer_string();
    let product_string = device.product_string();
    let usage_page = device.usage_page();
    let usage = device.usage();
    let interface_number = device.interface_number();
    let bus_type = device.bus_type();

    log::debug!(
        "Device details:\npath: {path:?}\nvendor_id: {vendor_id:?}\nproduct_id: {product_id:?}\nserial_number: {serial_number:?}\nrelease_number: {release_number:?}\nmanufacturer_string: {manufacturer_string:?}\nproduct_string: {product_string:?}\nusage_page: {usage_page:?}\nusage: {usage:?}\ninterface_number: {interface_number:?}\nbus_type: {bus_type:?}"
    );
}

fn get_midi_output() -> midir::MidiOutputConnection {
    let midi_out =
        midir::MidiOutput::new("USB Foot Pedal").expect("Failed to initialize MIDI output.");

    // Get an output port (read from console if multiple are available)
    let out_ports = midi_out.ports();
    let out_port: &midir::MidiOutputPort = match out_ports.len() {
        0 => panic!("no output port found"),
        1 => {
            log::info!(
                "Choosing the only available output port: {}",
                midi_out.port_name(&out_ports[0]).unwrap()
            );
            &out_ports[0]
        }
        _ => {
            println!("\nAvailable output ports:");
            for (i, p) in out_ports.iter().enumerate() {
                println!("{}: {}", i, midi_out.port_name(p).unwrap());
            }
            print!("Please select output port: ");
            std::io::stdout().flush().expect("Failed to flush stdout.");
            let mut input = String::new();
            std::io::stdin()
                .read_line(&mut input)
                .expect("Failed to read stdin.");
            out_ports
                .get(
                    input
                        .trim()
                        .parse::<usize>()
                        .expect("Unparseable output port number"),
                )
                .expect("Nonexistent output port number")
        }
    };

    return midi_out
        .connect(out_port, "usb-sustain-pedal")
        .expect("Failed to connect to MIDI output port.");
}
