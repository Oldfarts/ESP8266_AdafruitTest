import time 
import RPi.GPIO as GPIO 
import paho.mqtt.client as mqtt 
# Configuration: 
LED_PIN        = 0 
# Initialize GPIO for LED and button. 
GPIO.setmode(GPIO.BCM) 
GPIO.setwarnings(False) 
GPIO.setup(LED_PIN, GPIO.OUT) 
GPIO.output(LED_PIN, GPIO.LOW) 
# Setup callback functions that are called when MQTT events happen like 
# connecting to the server or receiving data from a subscribed feed. 
def on_connect(client, userdata, flags, rc): 
   print("Connected with result code " + str(rc)) 
   # Subscribing in on_connect() means that if we lose the connection and 
   # reconnect then subscriptions will be renewed. 
   client.subscribe("/leds/pi") 
# The callback for when a PUBLISH message is received from the server. 
def on_message(client, userdata, msg): 
   print(msg.topic+" "+str( msg.payload)) 
   # Check if this is a message for the Pi LED. 
   if msg.topic == '/leds/pi': 
       # Look at the message data and perform the appropriate action. 
       if msg.payload == b'ON' :
           GPIO.output(LED_PIN, GPIO.HIGH) 
       elif msg.payload == b'1': 
           GPIO.output(LED_PIN, GPIO.HIGH) 
       elif msg.payload == b'OFF' :
           GPIO.output(LED_PIN, GPIO.LOW) 
       elif msg.payload == b'0':    
           GPIO.output(LED_PIN, GPIO.LOW)

# Create MQTT client and connect to localhost, i.e. the Raspberry Pi running 
# this script and the MQTT server. 
client = mqtt.Client() 
client.on_connect = on_connect 
client.on_message = on_message 
client.connect('localhost', 1883, 60) 
# Connect to the MQTT server and process messages in a background thread. 
client.loop_start() 
# Main loop to listen for button presses. 
print('Script is running, press Ctrl-C to quit...') 
while True: 
   # Look for a change from high to low value on the button input to 
   # signal a button press. 
   time.sleep(3)
   client.publish('/leds/esp8266', 'ON')
   #client.publish('/leds/esp8266', '0')
   time.sleep(3)
   client.publish('/leds/esp8266', 'OFF') 
   #client.publish('/leds/esp8266', '1') 

