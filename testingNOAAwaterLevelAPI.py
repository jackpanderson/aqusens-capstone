import requests

def queryForWaterLevel():
    url = "https://api.tidesandcurrents.noaa.gov/api/prod/datagetter?date=latest&station=9412110&product=water_level&datum=STND&time_zone=lst&units=metric&format=json"

    response = requests.get(url)

    if response.status_code == 200:
        data = response.json()

        water_level = data['data'][0]['v']
        return water_level
    else:
        return -1


print("Water level is ", queryForWaterLevel(), "meters")
