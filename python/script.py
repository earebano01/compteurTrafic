# import serial
# import psycopg2
# from datetime import date, datetime

# ser = serial.Serial('COM3', 9600)

# conn = psycopg2.connect(host='localhost', port='5432', database='test1', user='postgres', password='admin')

# cursor = conn.cursor()

# cursor.execute('''
#     CREATE TABLE IF NOT EXISTS bike_test (
#         ID SERIAL PRIMARY KEY,
#         bike_count INT,
#         db_level INT,
#         distance_cm INT,
#         date DATE,
#         time TIME
#     )
# ''')

# cursor.execute('''
#     CREATE TABLE IF NOT EXISTS vehicle_test (
#         ID SERIAL PRIMARY KEY,
#         vehicle_count INT,
#         db_level INT,
#         distance_cm INT,
#         date DATE,
#         time TIME
#     )
# ''')

# conn.commit()

# try:
#     while True:
#         line = ser.readline().decode().strip()

#         if line == '':
#             continue

#         try:
#             values = line.split(',')
#             bike_count = 1
#             vehicle_count = 1
#             val = float(values[0])
#             cm = float(values[1])
#         except (ValueError, IndexError):
#             print('Invalid values:', line)
#             continue

#         current_time = datetime.now().strftime('%H:%M:%S')

#         if val > 60:
#             cursor.execute('''
#                 INSERT INTO vehicle_test (vehicle_count, db_level, distance_cm, date, time)
#                 VALUES (%s, %s, %s, %s, %s)
#             ''', (vehicle_count, val, cm, date.today(), datetime.now().time()))
#         elif 15 <= val <= 55:
#             cursor.execute('''
#                 INSERT INTO bike_test (bike_count, db_level, distance_cm, date, time)
#                 VALUES (%s, %s, %s, %s, %s)
#             ''', (bike_count, val, cm, date.today(), datetime.now().time()))

#         conn.commit()

# except KeyboardInterrupt:
#     pass

# ser.close()
# cursor.close()
# conn.close()


import serial
import psycopg2
from datetime import date, datetime

ser = serial.Serial('COM3', 9600)

conn = psycopg2.connect(host='localhost', port='5432', database='test1', user='postgres', password='admin')

cursor = conn.cursor()

cursor.execute('''
    CREATE TABLE IF NOT EXISTS bike_test (
        ID SERIAL PRIMARY KEY,
        bike_count INT,
        db_level INT,
        distance_cm INT,
        date DATE,
        time TIME
    )
''')

cursor.execute('''
    CREATE TABLE IF NOT EXISTS vehicle_test (
        ID SERIAL PRIMARY KEY,
        vehicle_count INT,
        db_level INT,
        distance_cm INT,
        date DATE,
        time TIME
    )
''')

conn.commit()

try:
    while True:
        line = ser.readline().decode().strip()

        if line == '':
            continue

        try:
            values = line.split(',')
            bike_count = 1
            vehicle_count = 1
            val = float(values[0])
            cm = float(values[1])
        except (ValueError, IndexError):
            print('Invalid values:', line)
            continue

        current_time = datetime.now().strftime('%H:%M:%S')

        # if val > 150:
        if val > 65:
            cursor.execute('''
                INSERT INTO vehicle_test (vehicle_count, db_level, distance_cm, date, time)
                VALUES (%s, %s, %s, %s, %s)
            ''', (vehicle_count, val, cm, date.today(), datetime.now().time()))
        # elif val < 150:
        elif val < 50:
            cursor.execute('''
                INSERT INTO bike_test (bike_count, db_level, distance_cm, date, time)
                VALUES (%s, %s, %s, %s, %s)
            ''', (bike_count, val, cm, date.today(), datetime.now().time()))

        conn.commit()

except KeyboardInterrupt:
    pass

ser.close()
cursor.close()
conn.close()

