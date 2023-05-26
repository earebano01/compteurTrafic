#on importe les bibliothèques/modules nécessaires.
import serial
import psycopg2
from datetime import date, datetime

#on met en place la connexion série avec le port et le débit (baud rate) spécifiés
ser = serial.Serial('COM3', 9600)

#on établit une connexion à la base de données PostgreSQL en utilisant les paramètres ci-dessous
conn = psycopg2.connect(host='localhost', port='5432', database='test1', user='postgres', password='admin')

#on crée un objet curseur pour exécuter les requêtes SQL sur la base de données.
cursor = conn.cursor()

#on exécute une requête SQL pour créer une table si elle n'existe pas déjà
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

#on valide les modifications apportées aux tables de la base de données
conn.commit()

#on commence une boucle infinie pour lire les données depuis la connexion série
try:
    while True:
        line = ser.readline().decode().strip()

        if line == '':
            continue

#on lit une ligne de données depuis le port série et on essaie de diviser la ligne 
# de données en valeurs en utilisant la virgule comme séparateur
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

#on insère les données collectées dans la table appropriée en fonction du seuil de niveau de dB défini 
# pour chaque catégorie
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

#on ferme la connection
ser.close()
cursor.close()
conn.close()

