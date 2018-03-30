# coding: utf-8

###
# API attributes
###
user_api_key = "EEAOT77192IH0PCK" # https://thingspeak.com/account/profile
channel_id = 440934
read_api_key = "XV205NRZNX2NXNMY"

region_id_dict = {440934: 0}
region_dict = {440934: "Cardiff"}
gps_dict = {440934: (51.483333, -3.183333)}
plant_dict = {440934: "Bellis perennis"}

###
# helper functions
###
import requests

def get_data(channel_id: int, read_api_key: str) -> str:
    resp = requests.get("https://api.thingspeak.com/channels/{}/fields/1.json?api_key={}".format(channel_id, read_api_key))
    return resp    
    
def parse_data(resp: str) -> list:
    return [int(entry["field1"]) for entry in resp.json()["feeds"]]

def to_json(channel_id: int, channel_time_data: int) -> dict:
    return {"region_id": region_id_dict[channel_id], "region_name": region_dict[channel_id], "lat": gps_dict[channel_id][0], "lon": gps_dict[channel_id][1], "plant": plant_dict[channel_id], "time": channel_time_data}

def clear_channel(channel_id: int, user_api_key: str):
    payload = {"api_key": user_api_key}
    requests.delete("https://api.thingspeak.com/channels/{}/feeds.json".format(channel_id), params=payload)

###
# main part of the app
###

from flask import Flask, render_template 


app = Flask(__name__)

# main page
@app.route("/")
def index():
    return render_template("index.html")

# store the data to local, and pass data to the front end, in json form
@app.route("/scentmap")
def scentmap():
    data = []
    data.append(to_json(channel_id, sum(parse_data(get_data(channel_id, read_api_key)))))
    print(data)
    import json
    return render_template("scentmap.html", data=json.dumps(data))

# clear the data in different channels, and redirect to home page
@app.route("/cleardata")
def cleardata():
    clear_channel(channel_id, user_api_key)
    return render_template("cleardata.html")

if __name__ == "__main__":
    app.run(debug=True)
