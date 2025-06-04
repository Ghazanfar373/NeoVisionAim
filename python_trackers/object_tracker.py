#!/usr/bin/env python3
"""
NeoVisionAim - AI-Powered Object Tracking System

This module provides real-time object tracking with serial communication
for controlling a stepper motor gimbal.
"""

import argparse
import logging
import os
import sys
import time
from dataclasses import dataclass
from enum import Enum
from struct import pack
from typing import Dict, Optional, Tuple, Union, Any

import cv2
import serial
from imutils.video import FPS, VideoStream

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[logging.StreamHandler(sys.stdout)]
)
logger = logging.getLogger(__name__)

# Constants
DEFAULT_BAUDRATE = 115200
SERIAL_DEVICE = '/dev/ttyUSB0'
DEFAULT_FRAME_WIDTH = 640
DEFAULT_FRAME_HEIGHT = 480
DEFAULT_FLIP_METHOD = 2
SERIAL_HEADER = 223
SERIAL_XOR = 233

class TrackingState(Enum):
    """Enumeration of tracking states."""
    IDLE = 0
    TRACKING = 1
    LOST = 2

class ObjectTracker:
    """Main class for object tracking with stepper motor control."""
    
    # Available OpenCV trackers
    TRACKER_TYPES = {
        "csrt": cv2.TrackerCSRT_create,
        "kcf": cv2.TrackerKCF_create,
        "boosting": cv2.TrackerBoosting_create,
        "mil": cv2.TrackerMIL_create,
        "tld": cv2.TrackerTLD_create,
        "medianflow": cv2.TrackerMedianFlow_create,
        "mosse": cv2.TrackerMOSSE_create
    }
    
    def __init__(self, video_source: Optional[str] = None, 
                 tracker_type: str = "kcf",
                 frame_width: int = DEFAULT_FRAME_WIDTH,
                 frame_height: int = DEFAULT_FRAME_HEIGHT):
        """Initialize the object tracker.
        
        Args:
            video_source: Path to video file or None for camera
            tracker_type: Type of tracker to use (default: kcf)
            frame_width: Width of the camera frame
            frame_height: Height of the camera frame
        """
        self.tracker_type = tracker_type
        self.frame_width = frame_width
        self.frame_height = frame_height
        self.tracker = self._create_tracker()
        self.bounding_box = None
        self.fps = FPS()
        self.running = False
        self.disable_tracking = False
        self.serial_conn = None
        self.video_source = video_source
        self.cap = None
        self.serial_thread = None
    
    def _create_tracker(self):
        """Create and return a new tracker instance."""
        if self.tracker_type not in self.TRACKER_TYPES:
            logger.warning(f"Tracker {self.tracker_type} not found. Using KCF.")
            self.tracker_type = "kcf"
        return self.TRACKER_TYPES[self.tracker_type]()
    
    def init_serial_connection(self) -> bool:
        """Initialize serial connection to Arduino.
        
        Returns:
            bool: True if connection was successful, False otherwise
        """
        try:
            # Set system permissions for serial device
            if os.name == 'posix':
                os.system(f"sudo chmod 777 {SERIAL_DEVICE}")
                os.system("sudo systemctl restart nvargus-daemon")
            
            self.serial_conn = serial.Serial(SERIAL_DEVICE, DEFAULT_BAUDRATE, timeout=1)
            logger.info(f"Connected to Arduino on {SERIAL_DEVICE}")
            
            # Start serial read thread
            self.running = True
            self.serial_thread = threading.Thread(target=self._read_serial_data)
            self.serial_thread.daemon = True
            self.serial_thread.start()
            
            time.sleep(3)  # Wait for Arduino to initialize
            return True
            
        except serial.SerialException as e:
            logger.error(f"Failed to connect to Arduino: {e}")
            return False
    
    def _read_serial_data(self):
        """Background thread to read data from Arduino."""
        while self.running and self.serial_conn and self.serial_conn.is_open:
            try:
                data = self.serial_conn.read()
                if data == b'\xa5':
                    self.disable_tracking = True
                    logger.info("Tracking disabled via serial command")
            except Exception as e:
                logger.error(f"Serial read error: {e}")
                break
    
    def init_video_capture(self) -> bool:
        """Initialize video capture from file or camera.
        
        Returns:
            bool: True if video source was initialized successfully
        """
        try:
            if self.video_source:
                logger.info(f"Opening video file: {self.video_source}")
                self.cap = cv2.VideoCapture(self.video_source)
            else:
                # For Raspberry Pi camera
                # camera_config = (
                #     f'nvarguscamerasrc sensor-id=0 ! video/x-raw(memory:NVMM) '
                #     f'width={self.frame_width}, height={self.frame_height} ! '
                #     f'nvvidconv flip-method={DEFAULT_FLIP_METHOD} ! video/x-raw, '
                #     'format=BGRx ! videoconvert ! video/x-raw, format=BGR ! appsink'
                # )
                # self.cap = cv2.VideoCapture(camera_config, cv2.CAP_GSTREAMER)
                
                # For standard USB camera
                self.cap = VideoStream(src=0).start()
                time.sleep(2.0)  # Camera warm-up time
                
            return True
            
        except Exception as e:
            logger.error(f"Failed to initialize video source: {e}")
            return False
    
    def _send_motor_commands(self, x: int, y: int) -> None:
        """Send motor control commands to Arduino.
        
        Args:
            x: X coordinate of the target
            y: Y coordinate of the target
        """
        if not self.serial_conn or not self.serial_conn.is_open:
            return
        
        try:
            # Calculate normalized coordinates (-1.0 to 1.0)
            width_center = self.frame_width / 2.0
            height_center = self.frame_height / 2.0
            
            dx = int(((x - width_center) / width_center) * 127)
            dy = int(((height_center - y) / height_center) * 127)
            
            # Pack data: header (1B), dx (2B), dy (2B), xor (1B)
            data = pack('<BhhB', SERIAL_HEADER, dx, dy, SERIAL_XOR)
            self.serial_conn.write(data)
            
        except Exception as e:
            logger.error(f"Error sending motor commands: {e}")
    
    def run(self) -> None:
        """Main tracking loop."""
        if not self.init_serial_connection():
            logger.error("Failed to initialize serial connection")
            return
            
        if not self.init_video_capture():
            logger.error("Failed to initialize video source")
            return
            
        self.fps.start()
        
        try:
            while self.running:
                # Read frame from video source
                if isinstance(self.cap, VideoStream):
                    frame = self.cap.read()
                else:
                    ret, frame = self.cap.read()
                    if not ret:
                        logger.warning("Failed to grab frame")
                        break
                
                if frame is None:
                    logger.warning("Empty frame received")
                    continue
                
                # Resize frame if needed
                frame = cv2.resize(frame, (self.frame_width, self.frame_height))
                
                # Process frame based on tracking state
                if self.bounding_box is not None and not self.disable_tracking:
                    success, bbox = self.tracker.update(frame)
                    
                    if success:
                        x, y, w, h = [int(v) for v in bbox]
                        center_x, center_y = x + w//2, y + h//2
                        
                        # Draw bounding box and center point
                        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
                        cv2.circle(frame, (center_x, center_y), 5, (0, 255, 0), -1)
                        
                        # Send motor commands
                        self._send_motor_commands(center_x, center_y)
                        
                        # Draw status text
                        status = f"Tracking | FPS: {self.fps.fps():.1f}"
                        cv2.putText(frame, status, (10, 30), 
                                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
                    else:
                        status = "Tracking lost"
                        cv2.putText(frame, status, (10, 30), 
                                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 255), 2)
                
                # Show the frame
                cv2.imshow("Object Tracker", frame)
                
                # Handle key presses
                key = cv2.waitKey(1) & 0xFF
                
                # 'q' to quit
                if key == ord('q'):
                    break
                # 's' to select ROI
                elif key == ord('s'):
                    self._select_roi(frame)
                # 'd' to reset tracker
                elif key == ord('d'):
                    self._reset_tracker()
                
                # Update FPS counter
                self.fps.update()
        
        except KeyboardInterrupt:
            logger.info("Tracking stopped by user")
        finally:
            self.cleanup()
    
    def _select_roi(self, frame: np.ndarray) -> None:
        """Select region of interest for tracking.
        
        Args:
            frame: Frame to select ROI from
        """
        roi = cv2.selectROI("Select Object to Track", frame, fromCenter=False, showCrosshair=True)
        if roi != (0, 0, 0, 0):  # Check if a valid ROI was selected
            self.bounding_box = roi
            self.tracker = self._create_tracker()
            self.tracker.init(frame, self.bounding_box)
            self.disable_tracking = False
            logger.info(f"Tracking initialized with ROI: {roi}")
        cv2.destroyWindow("Select Object to Track")
    
    def _reset_tracker(self) -> None:
        """Reset the tracker to idle state."""
        self.tracker = self._create_tracker()
        self.bounding_box = None
        logger.info("Tracker reset")
    
    def cleanup(self) -> None:
        """Clean up resources."""
        self.running = False
        self.fps.stop()
        
        # Stop video capture
        if hasattr(self, 'cap'):
            if isinstance(self.cap, VideoStream):
                self.cap.stop()
            else:
                self.cap.release()
        
        # Close serial connection
        if hasattr(self, 'serial_conn') and self.serial_conn and self.serial_conn.is_open:
            self.serial_conn.close()
        
        # Wait for serial thread to finish
        if hasattr(self, 'serial_thread') and self.serial_thread.is_alive():
            self.serial_thread.join(timeout=1.0)
        
        cv2.destroyAllWindows()
        
        # Log FPS information
        logger.info(f"Elapsed time: {self.fps.elapsed():.2f} seconds")
        logger.info(f"Approx. FPS: {self.fps.fps():.2f}")
        logger.info("Cleanup complete")

def parse_arguments():
    """Parse command line arguments.
    
    Returns:
        argparse.Namespace: Parsed command line arguments
    """
    parser = argparse.ArgumentParser(description="Object Tracker with Stepper Motor Control")
    parser.add_argument(
        "-v", "--video", 
        type=str, 
        default=None,
        help="path to input video file (optional)"
    )
    parser.add_argument(
        "-t", "--tracker", 
        type=str, 
        default="kcf",
        choices=["csrt", "kcf", "boosting", "mil", "tld", "medianflow", "mosse"],
        help="OpenCV object tracker type (default: kcf)"
    )
    parser.add_argument(
        "--width", 
        type=int, 
        default=DEFAULT_FRAME_WIDTH,
        help=f"frame width (default: {DEFAULT_FRAME_WIDTH})"
    )
    parser.add_argument(
        "--height", 
        type=int, 
        default=DEFAULT_FRAME_HEIGHT,
        help=f"frame height (default: {DEFAULT_FRAME_HEIGHT})"
    )
    return parser.parse_args()

def main():
    """Main function to run the object tracker."""
    args = parse_arguments()
    
    logger.info("Starting Object Tracker")
    logger.info(f"Tracker type: {args.tracker}")
    logger.info(f"Frame size: {args.width}x{args.height}")
    
    tracker = ObjectTracker(
        video_source=args.video,
        tracker_type=args.tracker,
        frame_width=args.width,
        frame_height=args.height
    )
    
    try:
        tracker.run()
    except Exception as e:
        logger.error(f"An error occurred: {e}", exc_info=True)
    finally:
        tracker.cleanup()

if __name__ == "__main__":
    main()
