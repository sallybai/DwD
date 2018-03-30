
# coding: utf-8

# In[1]:


###
# API attributes
###
user_api_key = "EEAOT77192IH0PCK" # https://thingspeak.com/account/profile
channel_1_id = 446610
read_api_key_channel_1 = "P9IDADYLB2RWRPLQ"

channel_2_id = 446611
read_api_key_channel_2 = "3OP2ICQHJMVNCJG7"

channel_3_id = 446613
read_api_key_channel_3 = "RVC7U90T0ST1R3AN"

channel_4_id = 446614
read_api_key_channel_4 = "V404I0OK1NF2DUKO"

channel_5_id = 446615
read_api_key_channel_5 = "PAYSRRKGOM27VCST"

region_id_dict = {446610: 0, 446611: 1, 446613: 2, 446614: 3, 446615: 4}
region_dict = {446610: "Buckinghamshire", 446611: "Lancashire", 
               446613: "Fermanagh", 446614: "Kinross", 446615: "Cardiff"}
gps_dict = {446610: (51.83333333, 0.83333333), 446611: (53.8, 2.6), 
            446613: (54.48861111, -7.73611111), 446614: (56.25, 3.41666667), 446615: (51.483333, -3.183333)}
plant_dict = {446610: "Hyacinthoides hispanica", 446611: "Hedera helix", 
              446613: "Hyacinthoides non-scripta", 446614: "Impatiens glandulifera", 446615: "Bellis perennis"}


# In[4]:


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


# In[3]:


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
    data.append(to_json(channel_1_id, sum(parse_data(get_data(channel_1_id, read_api_key_channel_1)))))
    data.append(to_json(channel_2_id, sum(parse_data(get_data(channel_2_id, read_api_key_channel_2)))))
    data.append(to_json(channel_3_id, sum(parse_data(get_data(channel_3_id, read_api_key_channel_3)))))
    data.append(to_json(channel_4_id, sum(parse_data(get_data(channel_4_id, read_api_key_channel_4)))))
    data.append(to_json(channel_5_id, sum(parse_data(get_data(channel_5_id, read_api_key_channel_5)))))
    print(data)
    import json
    return render_template("scentmap.html", data=json.dumps(data))

# clear the data in different channels, and redirect to home page
@app.route("/cleardata")
def cleardata():
    clear_channel(channel_1_id, user_api_key)
    clear_channel(channel_2_id, user_api_key)
    clear_channel(channel_3_id, user_api_key)
    clear_channel(channel_4_id, user_api_key)
    clear_channel(channel_5_id, user_api_key)
    return render_template("cleardata.html")

if __name__ == "__main__":
    app.run(debug=True)

