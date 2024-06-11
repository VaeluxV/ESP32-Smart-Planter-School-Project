import paho.mqtt.client as mqtt
from influxdb import InfluxDBClient
import json

# InfluxDB settings
INFLUXDB_ADDRESS = 'localhost'
INFLUXDB_USER = 'subze'
INFLUXDB_PASSWORD = 'pass'
INFLUXDB_DATABASE = 'planter_readings'

# MQTT settings
MQTT_ADDRESS = 'localhost'
MQTT_USER = 'test'
MQTT_PASSWORD = 'test'
MQTT_CLIENT_ID = 'MQTTInfluxDBBridge'
MQTT_TOPIC = 'planter/#'

def on_connect(client, userdata, flags, rc):
    print('Connected with result code ' + str(rc))
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    print(f"Received message: {msg.topic} {str(msg.payload)}")
    save_to_influxdb(msg.topic, msg.payload)

def save_to_influxdb(topic, payload):
    try:
        # Decode the payload
        payload_str = payload.decode('utf-8')
        print(f"Decoded payload: {payload_str}")

        # Determine the data type and process accordingly
        if topic in ['planter/temperature', 'planter/soil_temperature', 'planter/ldr', 'planter/soil_humidity', 'planter/reservoir_level']:
            value = float(payload_str)
        elif topic in ['planter/fan_status', 'planter/pump_status']:
            value = int(payload_str)
        elif topic in ['planter/target']:
            value = str(payload_str)
        else:
            print(f"Unknown topic: {topic}")
            return

        # Construct the data to be written to InfluxDB
        data = [
            {
                "measurement": topic.replace('/', '_'),
                "tags": {
                    "host": "ESP32"
                },
                "fields": {
                    "value": value
                }
            }
        ]
        
        # Debug output before writing to InfluxDB
        print(f"Writing to InfluxDB: {data}")

        influxdb_client.write_points(data)
    except Exception as e:
        print(f"Error saving to InfluxDB: {e}")

def _init_influxdb_database():
    databases = influxdb_client.get_list_database()
    if len(list(filter(lambda x: x['name'] == INFLUXDB_DATABASE, databases))) == 0:
        influxdb_client.create_database(INFLUXDB_DATABASE)
    influxdb_client.switch_database(INFLUXDB_DATABASE)

def main():
    global influxdb_client
    influxdb_client = InfluxDBClient(INFLUXDB_ADDRESS, 8086, INFLUXDB_USER, INFLUXDB_PASSWORD, None)
    _init_influxdb_database()

    mqtt_client = mqtt.Client(client_id=MQTT_CLIENT_ID, protocol=mqtt.MQTTv311)
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message

    mqtt_client.connect(MQTT_ADDRESS, 1883)
    mqtt_client.loop_forever()

if __name__ == '__main__':
    print('MQTT to InfluxDB bridge')
    main()
