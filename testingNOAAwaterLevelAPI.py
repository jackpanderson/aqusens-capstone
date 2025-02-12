import requests

def queryForWaterLevel():
    url = "https://api.tidesandcurrents.noaa.gov/api/prod/datagetter?date=latest&station=9412110&product=water_level&datum=STND&time_zone=lst&units=metric&format=json"
    # Queries for the current water level from Port San Luis pier, updates every 6 minutes.

    # Tries to get NOAA data from url
    # If connectivity issues or data not found, fallback to SD card information (return -1)
    try:
        response = requests.get(url)
        if response.status_code == 200:
            data = response.json()

            water_level = data['data'][0]['v']
            return water_level
        else:
            return -1
    except requests.exceptions.RequestException as e:
        return -1
