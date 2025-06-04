# NeoVisionAim - AI-Powered Vision Tracking System

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Python](https://img.shields.io/badge/Python-3.8%2B-blue.svg)](https://www.python.org/)
[![OpenCV](https://img.shields.io/badge/OpenCV-4.5%2B-orange.svg)](https://opencv.org/)

NeoVisionAim is an advanced computer vision tracking system that combines AI-powered object detection with precision stepper motor control to create a highly accurate target tracking and aiming solution. The system is designed to detect, track, and follow moving targets with sub-degree accuracy.

![NeoVisionAim Demo](demo.gif)

## 🚀 Features

- **AI-Powered Tracking**: Utilizes state-of-the-art object detection and tracking algorithms
- **High Precision Control**: Stepper motor gimbal for smooth and accurate movement
- **Real-time Processing**: Low-latency processing for responsive tracking
- **Customizable**: Easily configurable for different tracking scenarios and targets
- **Cross-Platform**: Works on Windows and Linux (Raspberry Pi compatible)
- **Web Interface**: Intuitive web-based control panel for monitoring and configuration

## 🛠 Hardware Requirements

- Stepper motor gimbal (2-axis or 3-axis)
- Stepper motor drivers (e.g., DRV8825, TMC2209)
- Raspberry Pi 4 or similar single-board computer
- High-quality USB camera or Raspberry Pi Camera Module
- 12V Power Supply
- Jumper wires and connectors

## 📦 Installation

### Prerequisites
- Python 3.8 or higher
- OpenCV 4.5 or higher
- NumPy
- Flask (for web interface)

### Setup

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/NeoVisionAim.git
   cd NeoVisionAim
   ```

2. Install the required packages:
   ```bash
   pip install -r requirements.txt
   ```

3. Configure your hardware settings in `config.py`:
   ```python
   # Stepper motor configuration
   STEPPER_PINS = {
       'x_axis': {'step': 17, 'dir': 18, 'enable': 27},
       'y_axis': {'step': 22, 'dir': 23, 'enable': 24}
   }
   
   # Camera settings
   CAMERA_INDEX = 0  # Change this if using multiple cameras
   ```

## 🚀 Usage

1. Start the tracking system:
   ```bash
   python main.py
   ```

2. Access the web interface at `http://localhost:5000`

3. Use the web interface to:
   - Start/stop tracking
   - Adjust tracking sensitivity
   - Calibrate the gimbal
   - View the camera feed with tracking overlay

## 🤖 How It Works

1. The system captures video from the camera
2. AI algorithms detect and track the target
3. The position data is processed to calculate required gimbal movements
4. Stepper motors adjust the camera position to keep the target centered
5. The web interface provides real-time feedback and control

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👥 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## 📧 Contact

For questions or feedback, please open an issue or contact the project maintainers.
