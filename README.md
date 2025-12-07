# MatrixController

MatrixController is a versatile firmware solution designed for controlling WS2812B LED matrices of all sizes. It provides a self-hosted web interface for seamless interaction, allowing users to manage and customize their LED matrices with ease.

## Features

- **Supports WS2812B LEDs**: Compatible with matrices of all sizes.
- **Web Interface**: A responsive, self-hosted interface for real-time control.
- **Custom Animations**: Create and modify animation patterns.
- **Platform Agnostic**: Built using modular tools to cater to developers and DIY enthusiasts.

## Getting Started

Follow these steps to set up and run the MatrixController:

### Prerequisites

- WS2812B LED matrix
- Compatible hardware (e.g., an ESP32 microcontroller)
- A computer with internet connectivity

### Installation

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/IliaSalaur/MatrixController.git
   cd MatrixController
   ```

2. **Build the Firmware**:
   Make sure you have the necessary development environment (e.g., ESP-IDF for ESP32). Follow the instructions in the [ESP-IDF documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/) to set up the environment.

   Then build and flash the firmware using:
   ```bash
   idf.py build
   idf.py flash
   ```

3. **Setup the Web Interface**:
   The web interface is located in the `web` directory. To preview the interface locally:
   ```bash
   cd web
   npm install
   npm start
   ```
   For production, use:
   ```bash
   npm run build
   ```

4. **Configure and Connect**:
   Power your hardware and access the web interface through the provided IP address (shown in the serial monitor after flashing).

## Usage

Once connected to the web interface, you can:
- Upload or select preconfigured animations.
- Adjust brightness and matrix dimensions.
- Configure custom settings for your matrix.

### Example Use Cases:
- Dynamic lighting for events or parties.
- LED signage and displays.
- Ambient lighting and home décor.

## Development

MatrixController is actively developed to ensure compatibility and introduce new features. Feel free to contribute or fork the project.

### File Details
- **`main/`**: Main firmware codebase.
- **`web/`**: React-based web interface code.
- **`CMakeLists.txt`**: Build setup for the firmware.
- **`diagram.json`**: System diagram (if provided).

## Screenshots

*(Add screenshots of the web interface here to demonstrate its features and usability)*

## License

MatrixController is licensed under the Apache License 2.0. See the [LICENSE](LICENSE) file for more details.

## Contributions

Contributions, issues, and feature requests are welcome! Please create an issue or make a pull request to help grow the project.