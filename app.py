
from flask import Flask, request, jsonify, render_template
import sqlite3
from datetime import datetime
import logging
import json

app = Flask(__name__, template_folder='templates')

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

# Database configuration
DB_NAME = 'sensor_data.db'

def init_db():
    """Initialize the SQLite database and create the sensors table if it doesn't exist."""
    try:
        with sqlite3.connect(DB_NAME) as conn:
            cursor = conn.cursor()
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS sensor_readings (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    temperature REAL NOT NULL,
                    humidity REAL NOT NULL,
                    pressure REAL NOT NULL,
                    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
                )
            ''')
            conn.commit()
        logging.info("Database initialized successfully")
    except sqlite3.Error as e:
        logging.error(f"Database initialization error: {e}")
        raise

def insert_reading(temperature, humidity, pressure):
    """Insert a new sensor reading into the database."""
    try:
        with sqlite3.connect(DB_NAME) as conn:
            cursor = conn.cursor()
            cursor.execute('''
                INSERT INTO sensor_readings (temperature, humidity, pressure)
                VALUES (?, ?, ?)
            ''', (temperature, humidity, pressure))
            conn.commit()
        logging.info("Sensor reading stored successfully")
        return True
    except sqlite3.Error as e:
        logging.error(f"Error inserting data: {e}")
        return False

@app.route('/', methods=['GET'])
def index():
    """Render the dashboard page."""
    return render_template('dashboard.html')

@app.route('/', methods=['POST'])
def receive_data():
    """Handle incoming sensor data."""
    try:
        data = request.get_json()
        
        # Validate required fields
        required_fields = ['temperature', 'humidity', 'pressure']
        if not all(field in data for field in required_fields):
            return jsonify({'error': 'Missing required fields'}), 400
        
        # Extract and validate data
        temperature = float(data['temperature'])
        humidity = float(data['humidity'])
        pressure = float(data['pressure'])
        
        # Store the data
        if insert_reading(temperature, humidity, pressure):
            logging.info(f"Received and stored: Temp={temperature}°C, Humidity={humidity}%, Pressure={pressure}hPa")
            return jsonify({'status': 'success'}), 200
        else:
            return jsonify({'error': 'Failed to store data'}), 500
            
    except (ValueError, TypeError) as e:
        logging.error(f"Data validation error: {e}")
        return jsonify({'error': 'Invalid data format'}), 400
    except Exception as e:
        logging.error(f"Unexpected error: {e}")
        return jsonify({'error': 'Server error'}), 500

@app.route('/readings', methods=['GET'])
def get_readings():
    """Retrieve all sensor readings."""
    try:
        with sqlite3.connect(DB_NAME) as conn:
            cursor = conn.cursor()
            cursor.execute('''
                SELECT 
                    temperature, 
                    humidity, 
                    pressure, 
                    datetime(timestamp, 'localtime') as timestamp 
                FROM sensor_readings 
                ORDER BY timestamp DESC 
                LIMIT 100
            ''')
            readings = cursor.fetchall()
            
            # Format data for plotting
            data = {
                'timestamps': [row[3] for row in readings][::-1],
                'temperature': [row[0] for row in readings][::-1],
                'humidity': [row[1] for row in readings][::-1],
                'pressure': [row[2] for row in readings][::-1]
            }
            
            return jsonify(data), 200
    except sqlite3.Error as e:
        logging.error(f"Database error: {e}")
        return jsonify({'error': 'Database error'}), 500

if __name__ == '__main__':
    # Initialize the database before starting the server
    init_db()
    
    # Run the Flask app
    app.run(host='0.0.0.0', port=9999, debug=True)
