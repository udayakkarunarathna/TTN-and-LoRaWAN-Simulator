import sys
import datetime
import base64
import paho.mqtt.client as mqtt
import json

# This PAHO MQTT Code is migrated to v2.x: https://eclipse.dev/paho/files/paho.mqtt.python/html/migrations.html

THE_BROKER = "eu1.cloud.thethings.network"
MY_TOPICS = 'v3/+/devices/+/up'
APP_ID = 'my-lab2-ttn-appid-1'
API_KEY = "NNSXS.KY4AJ3OSDI3FBPWSJYYEFL2CRAHPSF643CYHWNY.SLMMMLSPWGNZXBEY6WMO533RL2SLC2K6SQRU5YOBJTYHK37VEJ6Q"

def format_time():
    tm = datetime.datetime.now()
    sf = tm.strftime('%Y-%m-%d %H:%M:%S.%f')
    return sf[:-4]

def on_connect(client, userdata, flags, reason_code, properties):
    print(f'[+] Connected to: {client._host}, port: {client._port}')
    print(f'Flags: {flags},  return code: {reason_code}')
    client.subscribe(MY_TOPICS, qos=0)        # Subscribe to all topics

def on_subscribe(client, userdata, mid, reason_codes, properties):
    print(f'Subscribed to topics: {MY_TOPICS}')
    print('Waiting for messages...')

def on_message(client, userdata, message):
    themsg = str(message.payload)
    print(f'\nReceived topic: {str(message.topic)} with payload: {themsg}, at subscribers local time: {format_time()}')
    print("")
    
    # Raw JSON from TTN
    payload_str = message.payload.decode()
    print("Valid JSON:\n", payload_str)
    
    data = json.loads(payload_str)    
    
    # Base64-encoded payload from TTN
    frm_payload_b64 = data["uplink_message"]["frm_payload"]
    decoded = base64.b64decode(frm_payload_b64).decode(errors="ignore")
    
    device_id = data["end_device_ids"]["device_id"]
    
    print("")
    print(f"Device ID      : {device_id}")
    print(f"Decoded string : {decoded}")

def on_disconnect(client, userdata, flags, reason_code, properties):
    print("disconnected")

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)

client.on_connect = on_connect
client.on_message = on_message
client.on_subscribe = on_subscribe
client.on_disconnect = on_disconnect

print(f'Connecting to TTN V3: {THE_BROKER}')
# Setup authentication from settings above
client.username_pw_set(APP_ID, API_KEY)

try:
    # IMPORTANT - this enables the encryption of messages
    client.tls_set()	# default certification authority of the local system
    client.connect(THE_BROKER, port=8883, keepalive=60)

except BaseException as ex:
    print(f'Cannot connect to TTN V3: {THE_BROKER}')
    print(f"TTN V3 error: {ex}")
    sys.exit()

client.loop_forever()

