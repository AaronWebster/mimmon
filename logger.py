import serial
import time
import os
import sys
import mysql.connector

cnx = mysql.connector.connect(user='',
                              password='',
                              host='127.0.0.1',
                              database='canyon')
cursor = cnx.cursor()
add_water_measurement = ('INSERT INTO water VALUES (CURRENT_TIMESTAMP, %s, %s)')
add_battery_measurement = ('INSERT INTO battery VALUES (CURRENT_TIMESTAMP, %s, %s)')


with serial.Serial('/dev/ttyACM0') as s:
  while True:
    buf = ''
    while not buf.endswith('\r\r\n'):
      buf += s.read()
    sensor_id, measurement = buf.split('\t')
    if measurement.startswith('R'):
      cursor.execute(add_water_measurement, (int(sensor_id), int(measurement[1:-1])))
    if measurement.startswith('B'):
      cursor.execute(add_battery_measurement, (int(sensor_id), int(measurement[1:-1])))
    cnx.commit()

cursor.close()
cnx.close()
